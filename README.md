# AppraiserPatcher11: Kill Appraiser Blocker on Windows 11

## What Is It

Appraiser.dll controls whether a device can installs a Windows 11 Update packages, and the policy it's using can be find in Appraiser_Data.ini.

Scripts in this repo tries to patch Appraiser.dll automatically for each Windows 11 update. 

Note: you can use OfflineInsiderEnroll to forcely enroll and upgrade to Windows 11

## Usage

Directly run the Python script (note: 3.8+ required), or add it to task scheduler

## How It Works

When update system using Windows Update, magic happens inside:
- WindowsUpdateBox.exe (located in C:\Windows\SoftwareDistribution\Download\XXXXXXXX) is called by svchost
- WindowsUpdateBox.exe creates C:\$WINDOWS.~BT, checking the hash, and releasing SetupHost.exe installer
- SetupHost.exe invoking Appraiser to check requirements

This tweak does these things:
- watches directory C:\Windows\SoftwareDistribution\Download, and dll hijacks all WindowsUpdateBox.exe with VERSION.dll
- hook CreateProcess, and replace Appraiser.dll in C:\$WINDOWS.~BT

## Disclaimer

The patched appraiser.dll included in this repo is only replacing `DT_ANY_SYS_BlockingSystem` to `DT_ANY_SYS_NonBlockSystem`

If you don't trust me, then go ahead and patch it yourself.