#include "node_api.h"
#include "node_lite.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

// !!! This is a "fake" implementation of TSFN that is good enough to run
// !!! the C# demo.
// !!! Do not use it in production code.

// The TSFN function is based on Node.js code and the latest
// PR https://github.com/nodejs/node/pull/55877

class ThreadSafeFunction {
 public:
  ThreadSafeFunction(napi_value func,
                     napi_value resource,
                     napi_value name,
                     size_t thread_count,
                     void* context,
                     size_t max_queue_size,
                     napi_env env,
                     void* finalize_data,
                     napi_finalize finalize_cb,
                     napi_threadsafe_function_call_js call_js_cb)
      : thread_count_(thread_count),
        context_(context),
        max_queue_size_(max_queue_size),
        env_(env),
        finalize_data_(finalize_data),
        finalize_cb_(finalize_cb),
        call_js_cb_(call_js_cb == nullptr ? CallJs : call_js_cb) {
    if (func != nullptr) {
      napi_create_reference(env, func, 1, &ref_);
    }
    // TODO: node::AddEnvironmentCleanupHook(env->isolate, Cleanup, this);
    // TODO: env->Ref();
  }

  ~ThreadSafeFunction() { ReleaseResources(); }

  // These methods can be called from any thread.

  napi_status Push(void* data, napi_threadsafe_function_call_mode mode) {
    {
      std::unique_lock lock{mutex_};

      if (max_queue_size_ != 0) {
        while (queue_.size() >= max_queue_size_ && state_ == kOpen) {
          if (mode == napi_tsfn_nonblocking) {
            return napi_queue_full;
          }
          cond_.wait(lock);
        }
      }

      if (state_ == kOpen) {
        queue_.push(data);
        Send();
        return napi_ok;
      }

      if (thread_count_ == 0) {
        return napi_invalid_arg;
      }
      thread_count_--;
      if (!(state_ == kClosed && thread_count_ == 0)) {
        return napi_closing;
      }
    }
    // Make sure to release lock before destroying
    delete this;
    return napi_closing;
  }

  napi_status Acquire() {
    std::scoped_lock lock{mutex_};
    if (state_ != kOpen) {
      return napi_closing;
    }

    thread_count_++;
    return napi_ok;
  }

  napi_status Release(napi_threadsafe_function_release_mode mode) {
    {
      std::scoped_lock lock{mutex_};

      if (thread_count_ == 0) {
        return napi_invalid_arg;
      }

      thread_count_--;

      if (thread_count_ == 0 || mode == napi_tsfn_abort) {
        if (state_ == kOpen) {
          if (mode == napi_tsfn_abort) {
            state_ = kClosing;
          }
          if (state_ == kClosing && max_queue_size_ > 0) {
            cond_.notify_all();
          }
          Send();
        }
      }

      if (!(state_ == kClosed && thread_count_ == 0)) {
        return napi_ok;
      }
    }
    // Make sure to release lock before destroying
    delete this;
    return napi_ok;
  }

  void EmptyQueueAndMaybeDelete() {
    {
      std::scoped_lock lock{mutex_};
      for (; !queue_.empty(); queue_.pop()) {
        call_js_cb_(nullptr, nullptr, context_, queue_.front());
      }
      if (thread_count_ > 0) {
        // At this point this TSFN is effectively done, but we need to keep
        // it alive for other threads that still have pointers to it until
        // they release them.
        // But we already release all the resources that we can at this point
        queue_ = {};
        ReleaseResources();
        return;
      }
    }
    // Make sure to release lock before destroying
    delete this;
  }

  // These methods must only be called from the loop thread.

  napi_status Init() {
    ThreadSafeFunction* ts_fn = this;
    uv_loop_t* loop = env->node_env()->event_loop();

    if (uv_async_init(loop, &async, AsyncCb) == 0) {
      if (max_queue_size > 0) {
        cond = std::make_unique<node::ConditionVariable>();
      }
      if (max_queue_size == 0 || cond) {
        return napi_ok;
      }

      env->node_env()->CloseHandle(
          reinterpret_cast<uv_handle_t*>(&async),
          [](uv_handle_t* handle) -> void {
            ThreadSafeFunction* ts_fn =
                node::ContainerOf(&ThreadSafeFunction::async,
                                  reinterpret_cast<uv_async_t*>(handle));
            delete ts_fn;
          });

      // Prevent the thread-safe function from being deleted here, because
      // the callback above will delete it.
      ts_fn = nullptr;
    }

    delete ts_fn;

    return napi_generic_failure;
  }

  napi_status Unref() {
    uv_unref(reinterpret_cast<uv_handle_t*>(&async));

    return napi_ok;
  }

  napi_status Ref() {
    uv_ref(reinterpret_cast<uv_handle_t*>(&async));

    return napi_ok;
  }

  inline void* Context() { return context_; }

 protected:
  void ReleaseResources() {
    if (state_ != kClosed) {
      state_ = kClosed;
      napi_delete_reference(env_, ref_);
      node::RemoveEnvironmentCleanupHook(env->isolate, Cleanup, this);
      env->Unref();
      async_resource.reset();
    }
  }

  void Dispatch() {
    bool has_more = true;

    // Limit maximum synchronous iteration count to prevent event loop
    // starvation. See `src/node_messaging.cc` for an inspiration.
    unsigned int iterations_left = kMaxIterationCount;
    while (has_more && --iterations_left != 0) {
      dispatch_state = kDispatchRunning;
      has_more = DispatchOne();

      // Send() was called while we were executing the JS function
      if (dispatch_state.exchange(kDispatchIdle) != kDispatchRunning) {
        has_more = true;
      }
    }

    if (has_more) {
      Send();
    }
  }

  bool DispatchOne() {
    void* data = nullptr;
    bool popped_value = false;
    bool has_more = false;

    {
      node::Mutex::ScopedLock lock(this->mutex);
      if (state == kOpen) {
        size_t size = queue.size();
        if (size > 0) {
          data = queue.front();
          queue.pop();
          popped_value = true;
          if (size == max_queue_size && max_queue_size > 0) {
            cond->Signal(lock);
          }
          size--;
        }

        if (size == 0) {
          if (thread_count == 0) {
            state = kClosing;
            if (max_queue_size > 0) {
              cond->Signal(lock);
            }
            CloseHandlesAndMaybeDelete();
          }
        } else {
          has_more = true;
        }
      } else {
        CloseHandlesAndMaybeDelete();
      }
    }

    if (popped_value) {
      v8::HandleScope scope(env->isolate);
      AsyncResource::CallbackScope cb_scope(&*async_resource);
      napi_value js_callback = nullptr;
      if (!ref.IsEmpty()) {
        v8::Local<v8::Function> js_cb =
            v8::Local<v8::Function>::New(env->isolate, ref);
        js_callback = v8impl::JsValueFromV8LocalValue(js_cb);
      }
      env->CallbackIntoModule<false>(
          [&](napi_env env) { call_js_cb(env, js_callback, context, data); });
    }

    return has_more;
  }

  void Finalize() {
    v8::HandleScope scope(env->isolate);
    if (finalize_cb) {
      AsyncResource::CallbackScope cb_scope(&*async_resource);
      env->CallFinalizer<false>(finalize_cb, finalize_data, context);
    }
    EmptyQueueAndMaybeDelete();
  }

  void CloseHandlesAndMaybeDelete(bool set_closing = false) {
    v8::HandleScope scope(env->isolate);
    if (set_closing) {
      node::Mutex::ScopedLock lock(this->mutex);
      state = kClosing;
      if (max_queue_size > 0) {
        cond->Signal(lock);
      }
    }
    if (handles_closing) {
      return;
    }
    handles_closing = true;
    env->node_env()->CloseHandle(
        reinterpret_cast<uv_handle_t*>(&async),
        [](uv_handle_t* handle) -> void {
          ThreadSafeFunction* ts_fn =
              node::ContainerOf(&ThreadSafeFunction::async,
                                reinterpret_cast<uv_async_t*>(handle));
          ts_fn->Finalize();
        });
  }

  void Send() {
    // Ask currently running Dispatch() to make one more iteration
    unsigned char current_state = dispatch_state.fetch_or(kDispatchPending);
    if ((current_state & kDispatchRunning) == kDispatchRunning) {
      return;
    }

    CHECK_EQ(0, uv_async_send(&async));
  }

  // Default way of calling into JavaScript. Used when ThreadSafeFunction is
  //  without a call_js_cb_.
  static void CallJs(napi_env env, napi_value cb, void* context, void* data) {
    if (!(env == nullptr || cb == nullptr)) {
      napi_value recv;
      napi_status status;

      status = napi_get_undefined(env, &recv);
      if (status != napi_ok) {
        napi_throw_error(env,
                         "ERR_NAPI_TSFN_GET_UNDEFINED",
                         "Failed to retrieve undefined value");
        return;
      }

      status = napi_call_function(env, recv, cb, 0, nullptr, nullptr);
      if (status != napi_ok && status != napi_pending_exception) {
        napi_throw_error(
            env, "ERR_NAPI_TSFN_CALL_JS", "Failed to call JS callback");
        return;
      }
    }
  }

  static void AsyncCb(uv_async_t* async) {
    ThreadSafeFunction* ts_fn =
        node::ContainerOf(&ThreadSafeFunction::async, async);
    ts_fn->Dispatch();
  }

  static void Cleanup(void* data) {
    reinterpret_cast<ThreadSafeFunction*>(data)->CloseHandlesAndMaybeDelete(
        true);
  }

 private:
  enum State : uint32_t { kOpen, kClosing, kClosed };

  static const uint32_t kDispatchIdle = 0;
  static const uint32_t kDispatchRunning = 1 << 0;
  static const uint32_t kDispatchPending = 1 << 1;

  static const size_t kMaxIterationCount = 1000;

  std::optional<AsyncResource> async_resource_;

  // These are variables protected by the mutex.
  std::mutex mutex_;
  std::optional<std::condition_variable> cond_;
  std::queue<void*> queue_;
  size_t thread_count_;
  State state_{kOpen};
  std::atomic_uint32_t dispatch_state_{kDispatchIdle};

  // These are variables set once, upon creation, and then never again, which
  // means we don't need the mutex to read them.
  void* context_{nullptr};
  const size_t max_queue_size_{0};

  // These are variables accessed only from the loop thread.
  napi_ref ref_{nullptr};
  napi_env env_{nullptr};
  void* finalize_data_{nullptr};
  napi_finalize finalize_cb_{nullptr};
  napi_threadsafe_function_call_js call_js_cb_{nullptr};
  bool handles_closing_{false};
};

NAPI_EXTERN napi_status NAPI_CDECL
napi_create_threadsafe_function(napi_env env,
                                napi_value func,
                                napi_value async_resource,
                                napi_value async_resource_name,
                                size_t max_queue_size,
                                size_t initial_thread_count,
                                void* thread_finalize_data,
                                napi_finalize thread_finalize_cb,
                                void* context,
                                napi_threadsafe_function_call_js call_js_cb,
                                napi_threadsafe_function* result) {
  if (env == nullptr) return napi_invalid_arg;
  if (async_resource_name == nullptr) return napi_invalid_arg;
  if (initial_thread_count == 0) return napi_invalid_arg;
  if (result == 0) return napi_invalid_arg;

  napi_status status = napi_ok;

  if (func == nullptr) {
    if (call_js_cb == nullptr) return napi_invalid_arg;
  } else {
    napi_valuetype value_type;
    napi_typeof(env, func, &value_type);
    if (value_type != napi_function) {
      return napi_invalid_arg;
    }
  }

  if (async_resource == nullptr) {
    napi_create_object(env, &async_resource);
  } else {
    napi_valuetype value_type;
    napi_typeof(env, async_resource, &value_type);
    if (value_type != napi_object) {
      return napi_invalid_arg;
    }
  }

  napi_valuetype value_type;
  napi_typeof(env, async_resource_name, &value_type);
  if (value_type != napi_string) {
    return napi_invalid_arg;
  }

  ThreadSafeFunction* ts_fn = new ThreadSafeFunction(func,
                                                     async_resource,
                                                     async_resource_name,
                                                     initial_thread_count,
                                                     context,
                                                     max_queue_size,
                                                     env,
                                                     thread_finalize_data,
                                                     thread_finalize_cb,
                                                     call_js_cb);

  if (ts_fn == nullptr) {
    return napi_generic_failure;
  }

  // Init deletes ts_fn upon failure.
  status = ts_fn->Init();
  if (status == napi_ok) {
    *result = reinterpret_cast<napi_threadsafe_function>(ts_fn);
  }

  return status;
}

NAPI_EXTERN napi_status NAPI_CDECL napi_get_threadsafe_function_context(
    napi_threadsafe_function func, void** result) {
  if (func == nullptr) return napi_invalid_arg;
  if (result == nullptr) return napi_invalid_arg;
  *result = reinterpret_cast<ThreadSafeFunction*>(func)->Context();
  return napi_ok;
}

NAPI_EXTERN napi_status NAPI_CDECL
napi_call_threadsafe_function(napi_threadsafe_function func,
                              void* data,
                              napi_threadsafe_function_call_mode is_blocking) {
  if (func == nullptr) return napi_invalid_arg;
  return reinterpret_cast<ThreadSafeFunction*>(func)->Push(data, is_blocking);
}

NAPI_EXTERN napi_status NAPI_CDECL
napi_acquire_threadsafe_function(napi_threadsafe_function func) {
  if (func == nullptr) return napi_invalid_arg;
  return reinterpret_cast<ThreadSafeFunction*>(func)->Acquire();
}

NAPI_EXTERN napi_status NAPI_CDECL napi_release_threadsafe_function(
    napi_threadsafe_function func, napi_threadsafe_function_release_mode mode) {
  if (func == nullptr) return napi_invalid_arg;
  return reinterpret_cast<ThreadSafeFunction*>(func)->Release(mode);
}

NAPI_EXTERN napi_status NAPI_CDECL napi_unref_threadsafe_function(
    node_api_basic_env env, napi_threadsafe_function func) {
  if (func == nullptr) return napi_invalid_arg;
  return reinterpret_cast<ThreadSafeFunction*>(func)->Unref();
}

NAPI_EXTERN napi_status NAPI_CDECL napi_ref_threadsafe_function(
    node_api_basic_env env, napi_threadsafe_function func) {
  if (func == nullptr) return napi_invalid_arg;
  return reinterpret_cast<ThreadSafeFunction*>(func)->Ref();
}