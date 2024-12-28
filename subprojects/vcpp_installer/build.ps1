Set-Location $PSScriptRoot

pyinstaller --onefile `
    --distpath ./dist_pyinstaller `
    --workpath ./build_pyinstaller `
    ./assets/windows/installer/check_vcpp_version_and_install.py

pyinstaller check_vcpp_version_and_install.py --onefile --distpath . --name check_vcpp_version_and_install.exe -y

Copy-Item check_vcpp_version_and_install.exe ../../assets/check_vcpp_version_and_install.exe
