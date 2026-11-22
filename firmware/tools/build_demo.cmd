@echo off
pwsh -NoProfile -File "%~dp0build_demo.ps1"
exit /b %ERRORLEVEL%
