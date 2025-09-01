# PowerShell script to build all demo projects
# This script should be run from within VS Code with VS 2022 Developer environment

param(
    [switch]$Verbose,
    [switch]$Clean
)

# Function to write verbose output
function Write-VerboseOutput {
    param([string]$Message)
    if ($Verbose) {
        Write-Host $Message -ForegroundColor Green
    }
}

# Function to write section headers
function Write-SectionHeader {
    param([string]$Section)
    Write-Host ""
    Write-Host "=" * 60 -ForegroundColor Cyan
    Write-Host "Building $Section" -ForegroundColor Cyan
    Write-Host "=" * 60 -ForegroundColor Cyan
}

# Function to check if we're in VS Developer environment
function Test-VSEnvironment {
    # Check if VCINSTALLDIR is set (indicates VS environment is loaded)
    if ($env:VCINSTALLDIR) {
        return $true
    }
    
    # Check if cmake is available
    try {
        $null = Get-Command cmake -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

# Function to build hermes-cli
function Build-HermesCli {
    Write-SectionHeader "Hermes CLI"
    
    if (-not (Test-Path "hermes-cli")) {
        Write-Error "hermes-cli directory not found"
        return $false
    }
    
    Set-Location "hermes-cli"
    try {
        $buildArgs = @()
        if ($Verbose) { $buildArgs += "-Verbose" }
        if ($Clean) { $buildArgs += "-Clean" }
        
        & ".\build-hermes-cli.ps1" @buildArgs
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to build hermes-cli"
            return $false
        }
        Write-Host "✅ Hermes CLI built successfully" -ForegroundColor Green
        return $true
    } finally {
        Set-Location ".."
    }
}

# Function to build Node-API modules (C and C++)
function Build-NodeApiModule {
    param([string]$ModulePath, [string]$ModuleName)
    
    Write-SectionHeader $ModuleName
    
    if (-not (Test-Path $ModulePath)) {
        Write-Error "$ModulePath directory not found"
        return $false
    }
    
    Set-Location $ModulePath
    try {
        Write-VerboseOutput "Running npm install in $ModulePath"
        
        # Clean if requested
        if ($Clean -and (Test-Path "build")) {
            Write-Host "Cleaning previous build..." -ForegroundColor Yellow
            Remove-Item "build" -Recurse -Force -ErrorAction SilentlyContinue
            Write-VerboseOutput "Removed build directory"
        }
        
        # Clean node_modules if clean requested
        if ($Clean -and (Test-Path "node_modules")) {
            Write-Host "Cleaning node_modules..." -ForegroundColor Yellow
            Remove-Item "node_modules" -Recurse -Force -ErrorAction SilentlyContinue
            Write-VerboseOutput "Removed node_modules directory"
        }
        
        & npm install
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to build $ModuleName"
            return $false
        }
        Write-Host "✅ $ModuleName built successfully" -ForegroundColor Green
        return $true
    } finally {
        Set-Location ".."
    }
}

# Function to build C# module
function Build-CSharpModule {
    Write-SectionHeader "C# API"
    
    if (-not (Test-Path "cs-api")) {
        Write-Error "cs-api directory not found"
        return $false
    }
    
    Set-Location "cs-api"
    try {
        Write-VerboseOutput "Running npm install in cs-api"
        
        # Clean if requested
        if ($Clean) {
            Write-Host "Cleaning previous build..." -ForegroundColor Yellow
            if (Test-Path "build") {
                Remove-Item "build" -Recurse -Force -ErrorAction SilentlyContinue
            }
            if (Test-Path "bin") {
                Remove-Item "bin" -Recurse -Force -ErrorAction SilentlyContinue
            }
            if (Test-Path "obj") {
                Remove-Item "obj" -Recurse -Force -ErrorAction SilentlyContinue
            }
            if (Test-Path "node_modules") {
                Remove-Item "node_modules" -Recurse -Force -ErrorAction SilentlyContinue
            }
            Write-VerboseOutput "Cleaned build directories"
        }
        
        # Install npm dependencies
        & npm install
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to run npm install for C# API"
            return $false
        }
        
        Write-VerboseOutput "Running dotnet publish"
        
        # Build and publish .NET module
        & dotnet publish
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to build C# API with dotnet publish"
            return $false
        }
        
        Write-Host "✅ C# API built successfully" -ForegroundColor Green
        return $true
    } finally {
        Set-Location ".."
    }
}

# Main script execution
try {
    Write-Host "Building all demo projects..." -ForegroundColor Yellow
    
    # Get the script directory
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    Set-Location $scriptDir
    Write-VerboseOutput "Script directory: $scriptDir"
    
    # Check if we have VS environment for native builds
    $hasVSEnvironment = Test-VSEnvironment
    if (-not $hasVSEnvironment) {
        Write-Warning "Visual Studio Developer environment not detected!"
        Write-Host "Some builds may fail. Please run this script from within VS Code launched by start-vs-code.ps1" -ForegroundColor Yellow
    } else {
        Write-VerboseOutput "Visual Studio Developer environment detected"
    }
    
    $buildResults = @()
    $startTime = Get-Date
    
    # Build all projects
    $buildResults += @{ Name = "Hermes CLI"; Success = (Build-HermesCli) }
    $buildResults += @{ Name = "C API"; Success = (Build-NodeApiModule "c-api" "C API") }
    $buildResults += @{ Name = "C++ API"; Success = (Build-NodeApiModule "cpp-api" "C++ API") }
    $buildResults += @{ Name = "C# API"; Success = (Build-CSharpModule) }
    
    # Report results
    $endTime = Get-Date
    $duration = $endTime - $startTime
    
    Write-Host ""
    Write-Host "=" * 60 -ForegroundColor Magenta
    Write-Host "Build Summary" -ForegroundColor Magenta
    Write-Host "=" * 60 -ForegroundColor Magenta
    
    $successCount = 0
    foreach ($result in $buildResults) {
        $status = if ($result.Success) { "✅ SUCCESS"; $successCount++ } else { "❌ FAILED" }
        $color = if ($result.Success) { "Green" } else { "Red" }
        Write-Host "$($result.Name): $status" -ForegroundColor $color
    }
    
    Write-Host ""
    Write-Host "Total time: $($duration.TotalSeconds.ToString('F1')) seconds" -ForegroundColor Cyan
    Write-Host "Successful builds: $successCount / $($buildResults.Count)" -ForegroundColor Cyan
    
    if ($successCount -eq $buildResults.Count) {
        Write-Host ""
        Write-Host "🎉 All projects built successfully!" -ForegroundColor Green
        Write-Host "You can now test the demos with different JavaScript runtimes:" -ForegroundColor Yellow
        Write-Host "  • node .\hello.cjs" -ForegroundColor White
        Write-Host "  • hermes-cli.exe .\hello.cjs" -ForegroundColor White
        Write-Host "  • deno --allow-read --allow-env --allow-ffi .\hello.cjs" -ForegroundColor White
        Write-Host "  • bun .\hello.cjs" -ForegroundColor White
        exit 0
    } else {
        Write-Host ""
        Write-Error "Some builds failed. Please check the output above for details."
        exit 1
    }
    
} catch {
    Write-Error "An error occurred during build: $($_.Exception.Message)"
    exit 1
} finally {
    # Return to original directory
    Set-Location $scriptDir
}
