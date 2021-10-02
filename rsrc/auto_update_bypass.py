#!/usr/bin/python3

# For 3.8+ only!
import os
import time
from pathlib import Path
import shutil
from watchdog.observers import Observer
from watchdog.events import PatternMatchingEventHandler


SYSROOT = r'C:\Windows\System32'
CURROOTP = Path(__file__).parent.absolute()  # Win11 is x64 only!
SYSROOTP = Path(SYSROOT)
DOWNLOAD_DIR = r'C:\Windows\SoftwareDistribution\Download'

def do_hook(targetDir):
    print(f"Hooking {targetDir}")
    targetDir = Path(targetDir)
    targetDLL = (targetDir / "VERSION.dll")
    if targetDLL.exists():
        targetDLL.unlink()
    targetDLL.symlink_to(CURROOTP / "AppraiserPatcher.dll")
    #shutil.copy(CURROOTP / "AppraiserPatcher.dll", targetDLL)
    oriDll = (targetDir / "VERSION_.dll")
    if oriDll.exists():
        oriDll.unlink()
    #oriDll.symlink_to(SYSROOTP / "VERSION.dll")
    shutil.copy(SYSROOTP / "VERSION.dll", oriDll)


def poll_once():
    for entry in Path(DOWNLOAD_DIR).iterdir():
        if not entry.is_dir():
            continue
        if not (entry / 'WindowsUpdateBox.exe').exists():
            continue
        #if (entry / 'VERSION.dll').exists():
        #    continue
        do_hook(entry)


def polling():
    while True:
        try:
            poll_once()
        except Exception as e:
            import traceback; traceback.print_exc()
        time.sleep(30.0)

def main():
    if not (SYSROOTP / "VERSION_.dll").exists():
        #(SYSROOTP / "VERSION_.dll").symlink_to(SYSROOTP / "VERSION.dll")
        shutil.copy(SYSROOTP / "VERSION.dll", SYSROOTP / "VERSION_.dll")
        
    polling()
    
if __name__ == "__main__":
    main()