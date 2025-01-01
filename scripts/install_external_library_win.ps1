cd $PSScriptRoot
cd ../..
$IKURA_DIR = (pwd).Path

# create download directory and fetch shaderc src
if (-not (Test-Path downloads)) {
    mkdir downloads
    cd downloads
    Invoke-WebRequest https://github.com/google/shaderc/archive/refs/tags/v2022.3.tar.gz -OutFile ./v2022.3.tar.gz
    tar -xf v2022.3.tar.gz

    # build shaderc
    cd ./shaderc-2022.3
    python ./utils/git-sync-deps
} else {
    cd downloads/shaderc-2022.3
}

$env:CXXFLAGS = "-D_CRT_SECURE_NO_WARNINGS=1 -DWIN32=1"
cmake -S . -B build -G Ninja
cmake --build build

# place compiled library
cd $IKURA_DIR

if (-not (Test-Path libs/shaderc)) {
    mkdir libs/shaderc
}
cp ./downloads/shaderc-2022.3/build/libshaderc/shaderc_combined.lib ./libs/shaderc/shaderc_combined.lib
