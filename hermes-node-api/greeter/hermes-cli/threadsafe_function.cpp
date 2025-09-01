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

// The TSFN function is based on Node.js code
// The code is reduced to a minimal implementation for demonstration purposes.
// It mostly delegates napi_call_threadsafe_function calls to the task runner.

std::shared_ptr<node_api_tests::NodeLiteTaskRunner> tsfnTaskRunner;

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
      : context_(context),
        env_(env),
        finalize_data_(finalize_data),
        finalize_cb_(finalize_cb),
        call_js_cb_(call_js_cb == nullptr ? CallJs : call_js_cb) {
    if (func != nullptr) {
      napi_create_reference(env, func, 1, &func_ref_);
    }
  }

  napi_status Push(void* data, napi_threadsafe_function_call_mode mode) {
    std::scoped_lock lock{mutex_};
    queue_.push(data);
    Send();
    return napi_ok;
  }

  void* Context() { return context_; }

 private:
  void Dispatch() {
    bool popped_value = false;
    void* data = nullptr;
    {
      std::scoped_lock lock{mutex_};
      if (!queue_.empty()) {
        data = queue_.front();
        queue_.pop();
        popped_value = true;
      }
    }
    if (popped_value) {
      call_js_cb_(env_, nullptr, context_, data);
    }

    std::scoped_lock lock{mutex_};
    if (!queue_.empty()) {
      Send();
    }
  }

  void Send() {
    tsfnTaskRunner->PostTask([this]() { Dispatch(); });
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

 private:
  // These are variables protected by the mutex.
  std::mutex mutex_;
  std::queue<void*> queue_;

  // These are variables set once, upon creation, and then never again, which
  // means we don't need the mutex to read them.
  void* context_{nullptr};

  // These are variables accessed only from the loop thread.
  napi_ref func_ref_{nullptr};
  napi_env env_{nullptr};
  void* finalize_data_{nullptr};
  napi_finalize finalize_cb_{nullptr};
  napi_threadsafe_function_call_js call_js_cb_{nullptr};
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

  *result = reinterpret_cast<napi_threadsafe_function>(ts_fn);

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
  // Do nothing in this fake implementation
  return napi_ok;
}

NAPI_EXTERN napi_status NAPI_CDECL napi_release_threadsafe_function(
    napi_threadsafe_function func, napi_threadsafe_function_release_mode mode) {
  // Do nothing in this fake implementation
  return napi_ok;
}

NAPI_EXTERN napi_status NAPI_CDECL napi_unref_threadsafe_function(
    node_api_basic_env env, napi_threadsafe_function func) {
  // Do nothing in this fake implementation
  return napi_ok;
}

NAPI_EXTERN napi_status NAPI_CDECL napi_ref_threadsafe_function(
    node_api_basic_env env, napi_threadsafe_function func) {
  // Do nothing in this fake implementation
  return napi_ok;
}