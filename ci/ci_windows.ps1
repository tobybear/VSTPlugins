#
# Build script for GitHub Actions.
#

$ErrorActionPreference = "Stop"

git clone --recursive https://github.com/steinbergmedia/vst3sdk.git

cd .\vst3sdk\pluginterfaces\base\
git apply ..\..\..\VSTPlugins\ci\windows_patch\funknown.cpp.patch
cd ..\..\..\

mkdir build
mkdir target
$SRC_ROOT = (Get-Item .).FullName

# SMTG_PLUGIN_TARGET_PATH must be set for GitHub Actions. Because cmake can't
# reach the default path which is `C:/Program Files/Common Files/VST3`.
cmake `
  -S vst3sdk `
  -B build `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -DCMAKE_BUILD_TYPE=Release `
  -DSMTG_MYPLUGINS_SRC_PATH="$SRC_ROOT\VSTPlugins" `
  -DSMTG_PLUGIN_TARGET_PATH="$SRC_ROOT\target" `
  -DSMTG_ADD_VST3_HOSTING_SAMPLES=FALSE `
  -DSMTG_ADD_VST3_PLUGINS_SAMPLES=FALSE `
  -DSMTG_CREATE_PLUGIN_LINK=FALSE

cmake --build build -j --config Release

# https://gitlab.com/gitlab-org/gitlab-runner/issues/3194#note_196458158
if (!$?) { Exit $LASTEXITCODE }

$plugin_dir = "$SRC_ROOT\build\VST3\Release"
foreach ($dir in $plugin_dir) {
  attrib.exe -S $dir /D
}
Copy-Item -Path $plugin_dir "$SRC_ROOT\vst_windows" -Recurse
