@echo off

echo ################################################
echo ################################################
echo ################################################
echo 1. for build WinCE port, you should use the command "call cmake.exe -G "Visual Studio 8 2005" -D CMAKE_WINCE_SDK:STRING="STANDARDSDK_500 (ARMV4I)" -D PORT:STRING=WinCE \path\to\source"
echo 2. for build WinCECasqt port, you should replace the PORT:STRING=WinCE to PORT:STRING=WinCECasqt
echo 3. if you want build the different wince platform, you should edit the CMAKE_WINCE_SDK:STRING="..."
echo 4. for the memory limit on WinCE platform, we should debug the Webkit on Window, so you should build the WinCEOnWindows port, while build the WinCEOnWindows port, you should excute the command "call cmake.exe -G "Visual Studio 8 2005" -DCMAKE_SYSTEM_PROCESSOR::STRING=i386 -DCMAKE_SYSTEM_NAME:STRING=Windows -DPORT:STRING=WinCEOnWindows \path\to\source"
echo ################################################
echo ################################################
echo ################################################

call cmake.exe -G "Visual Studio 8 2005" -D CMAKE_WINCE_SDK:STRING="STANDARDSDK_500 (ARMV4I)" -D PORT:STRING=WinCE ..


@echo on