# PowerShell script to build hermes-cli
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

# Main script execution
try {
    Write-Host "Building hermes-cli..." -ForegroundColor Yellow
    
    # Get the script directory
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    Write-VerboseOutput "Script directory: $scriptDir"
    
    # Calculate paths
    $hermesCliDir = Join-Path $scriptDir "hermes-cli"
    $buildDir = Join-Path $hermesCliDir "build"
    $releaseDir = Join-Path $buildDir "bin\Release"
    $hermesCliExe = Join-Path $releaseDir "hermes-cli.exe"
    
    Write-VerboseOutput "Hermes CLI directory: $hermesCliDir"
    Write-VerboseOutput "Build directory: $buildDir"
    
    # Check if hermes-cli source directory exists
    if (-not (Test-Path $hermesCliDir)) {
        Write-Error "Hermes CLI source directory not found: $hermesCliDir"
        exit 1
    }
    
    # Check if we have VS environment
    if (-not (Test-VSEnvironment)) {
        Write-Error "Visual Studio Developer environment not detected!"
        Write-Host "Please run this script from within VS Code launched by start-vs-code.ps1" -ForegroundColor Yellow
        Write-Host "Or run it from a VS 2022 Developer Command Prompt" -ForegroundColor Yellow
        exit 1
    }
    
    Write-VerboseOutput "Visual Studio Developer environment detected"
    
    # Navigate to hermes-cli directory
    Set-Location $hermesCliDir
    Write-VerboseOutput "Changed to hermes-cli directory"
    
    # Clean build if requested
    if ($Clean -and (Test-Path $buildDir)) {
        Write-Host "Cleaning previous build..." -ForegroundColor Yellow
        Remove-Item $buildDir -Recurse -Force
        Write-VerboseOutput "Removed build directory"
    }
    
    # Create build directory
    if (-not (Test-Path $buildDir)) {
        Write-Host "Creating build directory..." -ForegroundColor Yellow
        New-Item -ItemType Directory -Path $buildDir | Out-Null
        Write-VerboseOutput "Created build directory"
    }
    
    # Navigate to build directory
    Set-Location $buildDir
    Write-VerboseOutput "Changed to build directory"
    
    # Run CMake configuration
    Write-Host "Running CMake configuration..." -ForegroundColor Yellow
    & cmake .. -G "Visual Studio 17 2022" -A x64
    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake configuration failed with exit code $LASTEXITCODE"
        exit 1
    }
    Write-VerboseOutput "CMake configuration completed successfully"
    
    # Build the project
    Write-Host "Building hermes-cli (Release configuration)..." -ForegroundColor Yellow
    & cmake --build . --config Release
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed with exit code $LASTEXITCODE"
        exit 1
    }
    
    # Check if the executable was created
    if (Test-Path $hermesCliExe) {
        Write-Host "✅ Build completed successfully!" -ForegroundColor Green
        Write-Host "hermes-cli.exe is available at: $hermesCliExe" -ForegroundColor Green
        
        # Test the executable
        Write-Host "Testing hermes-cli..." -ForegroundColor Yellow
        & $hermesCliExe --help
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ hermes-cli is working correctly!" -ForegroundColor Green
        } else {
            Write-Warning "hermes-cli.exe was built but may not be working correctly"
        }
    } else {
        Write-Warning "Build appeared to succeed but hermes-cli.exe was not found at expected location: $hermesCliExe"
        Write-Host "Checking build output directory..." -ForegroundColor Yellow
        
        # List the contents of the build directory to help troubleshoot
        Get-ChildItem $buildDir -Recurse -Name "*.exe" | ForEach-Object {
            Write-Host "Found executable: $_" -ForegroundColor Cyan
        }
    }
    
} catch {
    Write-Error "An error occurred during build: $($_.Exception.Message)"
    exit 1
} finally {
    # Return to original directory
    Set-Location $scriptDir
}
