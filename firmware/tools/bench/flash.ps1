# Bench flash helper. Copy next to a build's flash_args + .bin files (from build-*/) on the PC the board is plugged into.
# Run: .\flash.ps1 [-Port COM17]
param([string]$Port = "COM17")
$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot
if (-not (Get-Command idf.py -ErrorAction SilentlyContinue)) {
    . "D:\Bindu\.espressif\v5.5.6\esp-idf\export.ps1"
}
python -m esptool --chip esp32s3 -p $Port -b 460800 --before default_reset --after hard_reset write_flash "@flash_args"
if ($LASTEXITCODE -ne 0) { throw "Flash failed (close any Serial Monitor using $Port and retry)" }
"Flash OK - board restarted. Leave it plugged in; RDP 'Ports' redirection lets the office PC read COM17."

