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
DOWNLOAD_DIR = 'C:\Windows\SoftwareDistribution\Download'

def do_hook(targetExe):
    targetDir = Path(targetExe).parent
    (CURROOTP / "AppraiserPatcher.dll").symlink_to(targetDir / "VERSION.dll")

class UpdateBoxHooker(PatternMatchingEventHandler):
    def __init__(self):
        patterns = ["*"]
        ignore_patterns = None
        ignore_directories = False
        case_sensitive = True
        PatternMatchingEventHandler.__init__(self, patterns, ignore_patterns, ignore_directories, case_sensitive)
    
    def on_created(self, event):
        print(f"hey, {event.src_path} has been created!")
        if event.src_path.lower().endswith(r'windowsupdatebox.exe'):
            do_hook(event.src_path)
    
    def on_deleted(self, event):
        print(f"what the f**k! Someone deleted {event.src_path}!")
    
    def on_modified(self, event):
        print(f"hey buddy, {event.src_path} has been modified")
        if event.src_path.lower().endswith(r'windowsupdatebox.exe'):
            do_hook(event.src_path)
    
    def on_moved(self, event):
        print(f"ok ok ok, someone moved {event.src_path} to {event.dest_path}")


def main():
    if not (SYSROOTP / "VERSION_.dll").exists():
        (SYSROOTP / "VERSION.dll").symlink_to(SYSROOTP / "VERSION_.dll")

    my_observer = Observer()
    my_observer.schedule(UpdateBoxHooker(), DOWNLOAD_DIR, recursive=True)
    my_observer.start()
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        my_observer.stop()
        my_observer.join()

if __name__ == "__main__":
    main()