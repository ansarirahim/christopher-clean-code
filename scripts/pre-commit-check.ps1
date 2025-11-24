# Pre-commit verification script for DA7281 HAL
# Run this before every commit to catch common mistakes!
#
# Usage:
#   cd da7281-dwm3001c-hal
#   .\scripts\pre-commit-check.ps1
#
# Author: A. R. Ansari
# Date: 2024-11-24

param(
    [switch]$Verbose = $false
)

# Color output helpers
function Write-Pass { param($msg) Write-Host "  PASS: $msg" -ForegroundColor Green }
function Write-Fail { param($msg) Write-Host "  FAIL: $msg" -ForegroundColor Red }
function Write-Warn { param($msg) Write-Host "  WARN: $msg" -ForegroundColor Yellow }
function Write-Info { param($msg) Write-Host "  INFO: $msg" -ForegroundColor Cyan }
function Write-Step { param($msg) Write-Host "" ; Write-Host "[$msg]" -ForegroundColor Yellow }

# Track overall pass/fail
$script:failCount = 0
$script:warnCount = 0

Write-Host ""
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "     DA7281 HAL Pre-Commit Verification Script             " -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan

# ============================================================================
# CHECK 1: Verify all 4 I2C addresses are documented
# ============================================================================
Write-Step "1/6 Checking I2C addresses in da7281_config.h"

$configFile = "include/da7281_config.h"
if (-not (Test-Path $configFile)) {
    Write-Fail "File not found: $configFile"
    $script:failCount++
} else {
    $config = Get-Content $configFile -Raw
    $addresses = @("0x48", "0x49", "0x4A", "0x4B")
    $missing = @()
    
    foreach ($addr in $addresses) {
        if ($config -notmatch [regex]::Escape($addr)) {
            $missing += $addr
        }
    }
    
    if ($missing.Count -gt 0) {
        Write-Fail "Missing I2C addresses: $($missing -join ', ')"
        Write-Info "All 4 addresses must be defined: 0x48, 0x49, 0x4A, 0x4B"
        $script:failCount++
    } else {
        Write-Pass "All 4 I2C addresses present (0x48, 0x49, 0x4A, 0x4B)"
    }
}

# ============================================================================
# CHECK 2: Verify chip ID is correct
# ============================================================================
Write-Step "2/6 Checking DA7281 chip ID in da7281_registers.h"

$regFile = "include/da7281_registers.h"
if (-not (Test-Path $regFile)) {
    Write-Fail "File not found: $regFile"
    $script:failCount++
} else {
    $registers = Get-Content $regFile -Raw
    if ($registers -match "0xBA") {
        Write-Pass "Chip ID 0xBA found (correct per datasheet)"
    } else {
        Write-Fail "Chip ID 0xBA not found - verify against datasheet"
        $script:failCount++
    }
}

# ============================================================================
# CHECK 3: Check for magic numbers in staged files
# ============================================================================
Write-Step "3/6 Checking for magic numbers in staged files"

try {
    $staged = git diff --cached --name-only 2>$null | Where-Object { $_ -match '\.(c|h)$' }
    
    if ($staged) {
        Write-Warn "Manual review required for staged C/H files:"
        foreach ($file in $staged) {
            Write-Host "    - $file" -ForegroundColor White
        }
        Write-Info "Check for magic numbers - all constants should have #define names"
        $script:warnCount++
    } else {
        Write-Pass "No staged C/H files to review"
    }
} catch {
    Write-Warn "Git not available or not in a git repository"
    $script:warnCount++
}

# ============================================================================
# CHECK 4: Run cppcheck static analysis (if installed)
# ============================================================================
Write-Step "4/6 Running cppcheck static analysis"

if (Get-Command cppcheck -ErrorAction SilentlyContinue) {
    Write-Info "Running cppcheck (this may take a moment)..."
    
    $cppcheckArgs = @(
        "--enable=warning,style,performance,portability"
        "--suppress=missingIncludeSystem"
        "--suppress=unusedFunction"
        "--quiet"
        "--inline-suppr"
        "src/"
        "include/"
    )
    
    $cppcheckOutput = & cppcheck $cppcheckArgs 2>&1
    
    if ($LASTEXITCODE -eq 0 -and -not $cppcheckOutput) {
        Write-Pass "No cppcheck warnings found"
    } elseif ($cppcheckOutput) {
        Write-Warn "cppcheck found potential issues:"
        $cppcheckOutput | ForEach-Object { Write-Host "    $_" -ForegroundColor Yellow }
        $script:warnCount++
    } else {
        Write-Pass "cppcheck completed with no critical issues"
    }
} else {
    Write-Warn "cppcheck not installed - skipping static analysis"
    Write-Info "Install with: choco install cppcheck"
    $script:warnCount++
}

# ============================================================================
# CHECK 5: Verify README examples use valid I2C addresses
# ============================================================================
Write-Step "5/6 Checking README.md for valid I2C addresses"

$readmeFile = "README.md"
if (-not (Test-Path $readmeFile)) {
    Write-Fail "File not found: $readmeFile"
    $script:failCount++
} else {
    $readme = Get-Content $readmeFile -Raw
    
    # Check for invalid addresses (common mistakes)
    $invalidAddresses = @("0x90", "0x92", "0x94", "0x96")  # Left-shifted versions
    $foundInvalid = @()
    
    foreach ($addr in $invalidAddresses) {
        if ($readme -match [regex]::Escape($addr)) {
            $foundInvalid += $addr
        }
    }
    
    if ($foundInvalid.Count -gt 0) {
        Write-Fail "Found left-shifted I2C addresses in README: $($foundInvalid -join ', ')"
        Write-Info "Use 7-bit addresses: 0x48, 0x49, 0x4A, 0x4B (NOT left-shifted)"
        $script:failCount++
    } else {
        Write-Pass "No invalid I2C addresses found in README"
    }
}

# ============================================================================
# CHECK 6: Commit message reminder
# ============================================================================
Write-Step "6/6 Commit message quality reminder"

Write-Info "Your commit message should include:"
Write-Host "    1. WHAT changed (summary of modifications)" -ForegroundColor White
Write-Host "    2. WHY it changed (reason/motivation)" -ForegroundColor White
Write-Host "    3. Datasheet references (if applicable)" -ForegroundColor White
Write-Host ""
Write-Info "Example:"
Write-Host "    Fix: Add all 4 I2C addresses per datasheet Table 16" -ForegroundColor Gray
Write-Host "    " -ForegroundColor Gray
Write-Host "    - Added 0x48 and 0x49 address definitions" -ForegroundColor Gray
Write-Host "    - Updated README with correct pin mappings" -ForegroundColor Gray
Write-Host "    " -ForegroundColor Gray
Write-Host "    Datasheet: DA7281 Rev 3.1, Table 16 (p58)" -ForegroundColor Gray

# ============================================================================
# SUMMARY
# ============================================================================
Write-Host ""
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "                    SUMMARY                                 " -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan

if ($script:failCount -eq 0 -and $script:warnCount -eq 0) {
    Write-Host ""
    Write-Host "  ALL CHECKS PASSED! Safe to commit." -ForegroundColor Green
    exit 0
} elseif ($script:failCount -eq 0) {
    Write-Host ""
    Write-Host "  $script:warnCount warning(s) - review before committing" -ForegroundColor Yellow
    exit 0
} else {
    Write-Host ""
    Write-Host "  $script:failCount critical issue(s) found - DO NOT COMMIT!" -ForegroundColor Red
    Write-Host "  $script:warnCount warning(s)" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "  Fix the issues above before committing." -ForegroundColor Red
    exit 1
}

