#!/usr/bin/python3

# For 3.8+ only!
import os
import time
from pathlib import Path
import shutil
from watchdog.observers import Observer
from watchdog.events import PatternMatchingEventHandler


SYSROOT = r'C:\Windows\System32'
CURROOTP = Path(__file__).parent  # Win11 is x64 only!
SYSROOTP = Path(SYSROOT)
DOWNLOAD_DIR = r'C:\Windows\SoftwareDistribution\Download'

def do_hook(targetDir):
    print(f"Hooking {targetDir}")
    targetDir = Path(targetDir)
    (targetDir / "VERSION.dll").symlink_to(CURROOTP / "AppraiserPatcher.dll")


def poll_once():
    for entry in Path(DOWNLOAD_DIR).iterdir():
        if not entry.is_dir():
            continue
        if not (entry / 'WindowsUpdateBox.exe').exists():
            continue
        if (entry / 'VERSION.dll').exists():
            continue
        do_hook(entry)


def polling():
    while True:
        poll_once()
        time.sleep(30.0)

def main():
    if not (SYSROOTP / "VERSION_.dll").exists():
        (SYSROOTP / "VERSION_.dll").symlink_to(SYSROOTP / "VERSION.dll")

    polling()
    
if __name__ == "__main__":
    main()