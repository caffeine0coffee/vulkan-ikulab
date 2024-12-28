Set-Location $PSScriptRoot

pyinstaller main.py --onefile --distpath . --name imv_version_checker -y

Copy-Item imv_version_checker.exe ../../assets/imv_version_checker.exe -Force
