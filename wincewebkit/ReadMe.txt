1) Getting the latest codes
git fetch
git checkout wincegdi

2) Building

mkdir WebKitBuild_xxx
cd WebKitBuild_xxx

WinCE Port Build

cmake -G "Visual Studio 8 2005" -DCMAKE_WINCE_SDK:STRING="STANDARDSDK_500 (ARMV4I)" -DPORT:STRING=WinCE \path\to\source


WinCE Casqt complex Port Build

cmake -G "Visual Studio 8 2005" -DCMAKE_WINCE_SDK:STRING="STANDARDSDK_500 (ARMV4I)" -DPORT:STRING=WinCECasqt \path\to\source

WinCEOnWindows Port Build
cmake -G "Visual Studio 8 2005" -DCMAKE_SYSTEM_PROCESSOR::STRING=i386 -DCMAKE_SYSTEM_NAME:STRING=Windows -DPORT:STRING=WinCEOnWindows \path\to\source


