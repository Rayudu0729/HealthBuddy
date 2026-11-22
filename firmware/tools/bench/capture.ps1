# Bench serial capture without resetting the board (safe with RDP port redirection).
# Run: .\capture.ps1 [-Port COM17] [-Seconds 30] [-Name capture]  -> writes <Name>.log next to the script
param([string]$Port = "COM17", [int]$Seconds = 30, [string]$Name = "capture")
$log = Join-Path $PSScriptRoot "$Name.log"
$lines = New-Object System.Collections.Generic.List[string]
$end = (Get-Date).AddSeconds($Seconds)
$p = $null
"Capturing $Seconds s from $Port - keep the target fixed..."
while ((Get-Date) -lt $end) {
    try {
        if (-not $p -or -not $p.IsOpen) {
            $p = New-Object System.IO.Ports.SerialPort $Port, 115200
            $p.ReadTimeout = 1000
            $p.Open()
        }
        $lines.Add($p.ReadLine().TrimEnd())
    } catch [TimeoutException] {
    } catch {
        # Port vanished (board reset/re-enumerated): close and retry
        if ($p) { try { $p.Close() } catch { } }; $p = $null
        Start-Sleep -Milliseconds 500
    }
}
if ($p -and $p.IsOpen) { $p.Close() }
$lines | Set-Content $log -Encoding UTF8
"Saved $($lines.Count) lines to $log"
if ($lines.Count -eq 0) { "No data: unplug/replug the board, wait 3 s, and run again." }

