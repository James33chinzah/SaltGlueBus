@echo off
setlocal
cd /d "%~dp0"
echo ======================================================
echo Salt Glue Bus - Windows 10/11 VST3 builder
echo ======================================================
echo.
where cmake >nul 2>nul
if errorlevel 1 (
  echo ERROR: CMake is not installed or not on PATH.
  echo Install CMake 3.22+ from https://cmake.org/download/
  pause
  exit /b 1
)
if not exist "C:\Program Files\Microsoft Visual Studio\2022" if not exist "C:\Program Files\Microsoft Visual Studio\18" (
  echo WARNING: Visual Studio 2022 or newer was not found in the usual location.
  echo Install Visual Studio with the "Desktop development with C++" workload.
  echo If you use another installation path, open the appropriate Developer Command Prompt first.
  echo.
)
if not exist "JUCE\CMakeLists.txt" (
  echo Downloading JUCE framework. This requires internet access...
  powershell -NoProfile -ExecutionPolicy Bypass -Command "$ProgressPreference='SilentlyContinue'; Invoke-WebRequest -Uri 'https://github.com/juce-framework/JUCE/archive/refs/heads/master.zip' -OutFile 'JUCE.zip'; Expand-Archive -Path 'JUCE.zip' -DestinationPath '.' -Force; $d=Get-ChildItem -Directory | Where-Object {$_.Name -like 'JUCE-*'} | Sort-Object LastWriteTime -Descending | Select-Object -First 1; if (!$d) { exit 2 }; if (Test-Path 'JUCE') { Remove-Item 'JUCE' -Recurse -Force }; Rename-Item $d.FullName 'JUCE'; Remove-Item 'JUCE.zip' -Force"
  if errorlevel 1 (
    echo ERROR: JUCE download failed. Check your internet connection and try again.
    pause
    exit /b 1
  )
)
echo.
echo Configuring 64-bit Release build...
cmake -S . -B build -DJUCE_DIR="%CD%\JUCE" -A x64
if errorlevel 1 goto failed
echo.
echo Compiling VST3 and Standalone app...
cmake --build build --config Release --parallel
if errorlevel 1 goto failed
echo.
echo BUILD FINISHED. Looking for plugin bundle...
if exist "build\SaltGlueBus_artefacts\Release\VST3\Salt Glue Bus.vst3" (
  echo VST3 created at:
  echo %CD%\build\SaltGlueBus_artefacts\Release\VST3\Salt Glue Bus.vst3
  echo.
  echo Copy the entire .vst3 folder to:
  echo C:\Program Files\Common Files\VST3\
) else (
  echo Build completed, but the expected VST3 path was not found.
  echo Search inside the build\SaltGlueBus_artefacts folder.
)
echo.
echo Open Studio One and rescan plug-ins. Restart Studio One if needed.
pause
exit /b 0
:failed
echo.
echo BUILD FAILED. Read the error messages above. Make sure Visual Studio has the C++ workload and Windows SDK installed.
pause
exit /b 1
