import subprocess
import sys
import webbrowser
import packaging.version

from ui import MainUi
from version_check import get_latest_release_version

version_info_dir = ""


def log(message):
    print(f"imv version checker: {message}")


def on_close():
    pass


def on_later():
    pass


def on_skip():
    global version_info_dir

    get_succeeded, latest_version = get_latest_release_version()

    if get_succeeded:
        with open(f"{version_info_dir}/skip_version.txt", "w") as f:
            f.write(latest_version)


def on_open():
    webbrowser.open("https://github.com/ikulab/ikulab-motion-viewer/releases/latest")


def main():
    global version_info_dir
    version_info_dir = sys.argv[1]

    get_succeeded, latest_version = get_latest_release_version()

    if not get_succeeded:
        log("Failed to get latest version.")
        return

    log(f"Detected latest version: {latest_version}")

    with open(f"{version_info_dir}/current_version.txt", "r") as f:
        current_version = f.read()
        log(f"Detected current version: {current_version}")

    with open(f"{version_info_dir}/skip_version.txt", "r") as f:
        skip_version = f.read()
        log(f"Detected skip update version: {skip_version}")

    new_version_available = False
    if packaging.version.parse(current_version) < packaging.version.parse(latest_version):
        log(f"New version is available: {latest_version}")
        new_version_available = True

    skip_update = False
    if latest_version == skip_version:
        log("Skipping update...")
        skip_update = True

    open_update_dialog = False
    if new_version_available and not skip_update:
        log("Opening update dialog...")
        open_update_dialog = True

    if open_update_dialog:
        app = MainUi(latest_version, on_open, on_close, on_later, on_skip)
        app.MainLoop()


if __name__ == "__main__":
    main()
