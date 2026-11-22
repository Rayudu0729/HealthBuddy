Remove-Item Env:MSYSTEM -ErrorAction SilentlyContinue
. C:\Espressif\tools\Microsoft.v5.5.6.PowerShell_profile.ps1 *> $null
Set-Location (Split-Path $PSScriptRoot -Parent)
idf.py -B build-demo build
exit $LASTEXITCODE
