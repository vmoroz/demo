# PowerShell script to set up environment and launch VS Code with VS 2022 Developer tools
# This script adds hermes-cli to PATH and launches VS Code from VS 2022 Developer command prompt

param(
    [switch]$Verbose
)

# Function to write verbose output
function Write-VerboseOutput {
    param([string]$Message)
    if ($Verbose) {
        Write-Host $Message -ForegroundColor Green
    }
}

# Function to find Visual Studio 2022 installation
function Find-VisualStudio2022 {
    Write-VerboseOutput "Searching for Visual Studio 2022 installation..."
    
    # Try using vswhere.exe (recommended method)
    $vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vswherePath) {
        Write-VerboseOutput "Found vswhere.exe at: $vswherePath"
        
        # Find VS 2022 installations
        $vsInstallations = & $vswherePath -version "[17.0,18.0)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        
        if ($vsInstallations -and $vsInstallations.Count -gt 0) {
            # Handle both single string and array results
            if ($vsInstallations -is [array]) {
                $vsPath = $vsInstallations[0].Trim()
            } else {
                $vsPath = $vsInstallations.Trim()
            }
            
            if ($vsPath -and (Test-Path $vsPath)) {
                Write-VerboseOutput "Found Visual Studio 2022 at: $vsPath"
                return $vsPath
            }
        }
    }
    
    # Fallback: Check common installation paths
    $commonPaths = @(
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Professional",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Enterprise",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Community"
    )
    
    foreach ($path in $commonPaths) {
        if (Test-Path $path) {
            Write-VerboseOutput "Found Visual Studio 2022 at: $path"
            return $path
        }
    }
    
    return $null
}

# Function to get vcvarsall.bat path
function Get-VCVarsAllPath {
    param([string]$VSPath)
    
    $vcvarsPath = Join-Path $VSPath "VC\Auxiliary\Build\vcvarsall.bat"
    if (Test-Path $vcvarsPath) {
        return $vcvarsPath
    }
    
    return $null
}

# Main script execution
try {
    Write-Host "Setting up environment for VS Code with VS 2022 Developer tools..." -ForegroundColor Yellow
    
    # Get the script directory
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    Write-VerboseOutput "Script directory: $scriptDir"
    
    # Calculate absolute path to hermes-cli build directory
    $hermesCliBuildPath = Join-Path $scriptDir "hermes-cli\build\bin\Release"
    $hermesCliBuildPath = [System.IO.Path]::GetFullPath($hermesCliBuildPath)
    Write-VerboseOutput "Hermes CLI build path: $hermesCliBuildPath"
    
    # Check if hermes-cli build directory exists
    if (-not (Test-Path $hermesCliBuildPath)) {
        Write-Warning "Hermes CLI build directory does not exist: $hermesCliBuildPath"
        Write-Host "After VS Code launches, you can build hermes-cli using:" -ForegroundColor Yellow
        Write-Host "  cd hermes-cli" -ForegroundColor Cyan
        Write-Host "  mkdir build" -ForegroundColor Cyan
        Write-Host "  cd build" -ForegroundColor Cyan
        Write-Host "  cmake .." -ForegroundColor Cyan
        Write-Host "  cmake --build . --config Release" -ForegroundColor Cyan
    } else {
        Write-VerboseOutput "Hermes CLI build directory exists"
    }
    
    # Find Visual Studio 2022
    $vsPath = Find-VisualStudio2022
    if (-not $vsPath) {
        Write-Error "Visual Studio 2022 not found. Please install Visual Studio 2022 with C++ tools."
        Write-Host "You can download it from: https://visualstudio.microsoft.com/vs/" -ForegroundColor Yellow
        exit 1
    }
    
    Write-VerboseOutput "Using Visual Studio installation: $vsPath"
    
    # Get vcvarsall.bat path
    $vcvarsPath = Get-VCVarsAllPath $vsPath
    if (-not $vcvarsPath) {
        Write-Error "vcvarsall.bat not found in Visual Studio installation: $vsPath"
        Write-Host "Please make sure Visual Studio 2022 is installed with C++ build tools." -ForegroundColor Yellow
        exit 1
    }
    
    Write-VerboseOutput "vcvarsall.bat path: $vcvarsPath"
    
    # Create a temporary batch file that will:
    # 1. Set up VS 2022 environment
    # 2. Add hermes-cli to PATH (even if directory doesn't exist yet)
    # 3. Launch VS Code
    $tempBatchFile = [System.IO.Path]::GetTempFileName() + ".bat"
    
    $batchContent = @"
@echo off
echo Setting up Visual Studio 2022 environment...
call "$vcvarsPath" x64

echo Adding hermes-cli to PATH...
set "PATH=$hermesCliBuildPath;%PATH%"

echo Launching VS Code...
code "$scriptDir"
"@
    
    # Write the batch file
    $batchContent | Out-File -FilePath $tempBatchFile -Encoding ASCII
    Write-VerboseOutput "Created temporary batch file: $tempBatchFile"
    
    # Execute the batch file
    Write-Host "Launching VS Code with VS 2022 environment..." -ForegroundColor Green
    & cmd.exe /c $tempBatchFile
    
    # Clean up the temporary batch file
    Start-Sleep -Seconds 2  # Give it a moment to execute
    Remove-Item $tempBatchFile -ErrorAction SilentlyContinue
    
    Write-Host "VS Code should now be launching with VS 2022 environment and hermes-cli in PATH." -ForegroundColor Green
    
    if (-not (Test-Path $hermesCliBuildPath)) {
        Write-Host ""
        Write-Host "NEXT STEPS:" -ForegroundColor Yellow
        Write-Host "1. VS Code will open with the VS 2022 Developer environment" -ForegroundColor White
        Write-Host "2. Open the integrated terminal in VS Code (Ctrl+`)" -ForegroundColor White
        Write-Host "3. Build hermes-cli with these commands:" -ForegroundColor White
        Write-Host "   cd hermes-cli" -ForegroundColor Cyan
        Write-Host "   mkdir build" -ForegroundColor Cyan
        Write-Host "   cd build" -ForegroundColor Cyan
        Write-Host "   cmake .." -ForegroundColor Cyan
        Write-Host "   cmake --build . --config Release" -ForegroundColor Cyan
        Write-Host "4. After building, hermes-cli.exe will be available in your PATH" -ForegroundColor White
    }
    
} catch {
    Write-Error "An error occurred: $($_.Exception.Message)"
    exit 1
}
