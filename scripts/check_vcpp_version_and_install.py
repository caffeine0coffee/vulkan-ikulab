import sys
import subprocess
from packaging import version
import winreg


def get_vcpp_version():
    """
    WindowsのレジストリからインストールされているVC++ランタイムのバージョンを取得する
    """
    try:
        regkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64")

        inst, _ = winreg.QueryValueEx(regkey, "Installed")
        ver, _ = winreg.QueryValueEx(regkey, "Version")

        winreg.CloseKey(regkey)

    except FileNotFoundError:
        return None, None

    return inst, ver


if __name__ == "__main__":
    installed, installed_version = get_vcpp_version()

    print("installed:", installed)
    print("installed version:", installed_version)

    if installed is None or installed == 0:
        print("Visual C++ 2015 Redistributable is not installed.")
        subprocess.run(["vc_redist.x64.exe"])
        sys.exit(0)

    # get required version from vcpp_version.txt
    required_version = None
    with open("vcpp_version.txt", "r") as f:
        required_version = f.read().strip()

    print("required version:", required_version)
    if required_version is None:
        print("vcpp_version.txt not found.")
        sys.exit(1)

    if version.parse(installed_version) < version.parse(required_version):
        print("Visual C++ 2015 Redistributable is outdated.")
        subprocess.run(["vc_redist.x64.exe"])
        sys.exit(0)
