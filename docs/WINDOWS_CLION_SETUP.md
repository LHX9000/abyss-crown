# Windows + CLion + MinGW 部署说明

本文档用于帮助甲方在 Windows 10/11 x64 电脑上，从 GitHub 克隆源码并使用 CLion 构建运行 `ABYSS CROWN`。

## 1. 需要安装的软件

- Git for Windows
- JetBrains CLion
- CLion 可识别的 MinGW Toolchain
- CMake 3.21 或更高版本
- Ninja

CLion 通常可以管理 CMake、Ninja 和 MinGW。打开 CLion 后，在：

`Settings > Build, Execution, Deployment > Toolchains`

确认存在一个可用的 `MinGW` Toolchain，并且 C Compiler、C++ Compiler、Debugger、CMake、Build Tool 均能被识别。

## 2. 克隆仓库

在 PowerShell 或 Git Bash 中执行：

```powershell
git clone https://github.com/LHX9000/abyss-crown.git
cd abyss-crown
```

## 3. 初始化 vcpkg

在项目根目录双击或运行：

```bat
scripts\setup_windows_clion.bat
```

脚本会：

- 将 vcpkg 克隆到项目内的 `.tools\vcpkg`。
- 执行 vcpkg 初始化。
- 设置当前用户的 `VCPKG_ROOT` 环境变量。

脚本完成后需要重启 CLion，让新的环境变量生效。

## 4. 使用 CLion 打开并构建

1. 在 CLion 中选择 `Open`，打开仓库根目录。
2. CLion 检测到 `CMakeLists.txt` 和 `CMakePresets.json` 后，选择 `Windows MinGW Debug (vcpkg)`。
3. 确认 Toolchain 为 `MinGW`，Generator 为 `Ninja`。
4. 等待 CMake 和 vcpkg 自动安装 `SDL2`、`SDL2_ttf`。
5. 在右上角运行配置中选择 `abyss_crown`。
6. 点击运行按钮。

CMake 构建完成后，会自动把 `assets/`、`config/` 和所需运行时 DLL 复制到可执行文件目录。

## 5. Release 构建

在 CLion 的 CMake Profile 中选择：

`Windows MinGW Release (vcpkg)`

也可以在已配置好 `VCPKG_ROOT` 的终端执行：

```powershell
cmake --preset windows-mingw-release
cmake --build --preset windows-mingw-release
```

## 6. 常见问题

### CMake 提示找不到 vcpkg toolchain

- 确认已经运行 `scripts\setup_windows_clion.bat`。
- 重启 CLion。
- 在终端执行 `echo %VCPKG_ROOT%`，确认变量有值。

### CMake 提示找不到 SDL2 或 SDL2_ttf

- 确认当前 CMake Profile 使用了 `windows-mingw-debug` 或 `windows-mingw-release` preset。
- 删除对应的 `cmake-build-windows-*` 目录后重新加载 CMake。
- 确认电脑可以访问 GitHub 和 vcpkg 下载源。

### 启动后没有中文文字

- 检查构建输出目录中是否存在 `assets/fonts/NotoSansSC-Regular.otf`。
- CMake 正常构建时会自动复制整个 `assets/` 目录。
- 字体缺失时，程序会尝试使用 Windows 系统中文字体。

### 启动后背景图或像素素材缺失

- 检查可执行文件旁是否存在 `assets/images/`。
- 使用 CLion 的 `abyss_crown` CMake Target 运行，不要只复制单独的 EXE。

### llama.cpp 不可用

llama.cpp 是可选功能。默认配置使用规则 AI，不影响游戏运行。

如果需要启用 llama.cpp，在构建输出目录的 `config/game_config.txt` 中配置 Windows 路径：

```text
llama_enabled=true
llama_executable=C:\path\to\llama-cli.exe
llama_model=C:\path\to\model.gguf
```

## 7. 验收建议

- 从全新克隆的仓库打开项目。
- 通过 CLion 完成 CMake 配置和构建。
- 检查中文、背景图、像素素材和配置是否正常加载。
- 完成一次胜利和一次失败流程。
- 确认未配置 llama.cpp 时规则 AI 正常工作。
