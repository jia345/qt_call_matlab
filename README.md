# Qt调用MATLAB命令行演示

这个项目演示了如何从Qt应用程序调用MATLAB命令行，执行MATLAB脚本，并获取执行结果。本应用程序使用QProcess启动MATLAB而非MATLAB引擎API，因此无需链接MATLAB库，使用更加简单灵活。

## 特性

- 简单直观的用户界面
- 自动检测系统中已安装的MATLAB版本
- 支持选择并运行任意MATLAB脚本文件
- 实时显示MATLAB执行结果
- 记住上次使用的MATLAB路径

## 前提条件

- Qt (5.x 或 6.x)
- MATLAB (建议2018a或更高版本)
- CMake (3.14 或更高版本)
- C++编译器支持C++17标准

## 构建项目

1. 克隆项目仓库:
   ```
   git clone [仓库URL]
   cd qt_call_matlab
   ```

2. 使用CMake配置项目:
   ```
   mkdir build
   cd build
   cmake ..
   ```

3. 构建项目:
   ```
   cmake --build .
   ```

## 使用方法

1. 启动应用程序后，界面将显示MATLAB路径（如果能在系统中找到）
2. 如需更改MATLAB路径，点击"Browse..."按钮选择MATLAB可执行文件(matlab.exe)
3. 点击脚本路径旁的"Browse..."按钮选择要执行的MATLAB脚本(.m文件)
4. 点击"Run MATLAB Script"按钮执行脚本
5. 脚本的输出将实时显示在界面下方的输出框中

## 示例脚本

项目中包含了一个示例MATLAB脚本`example.m`，它执行一些简单的数值计算并输出结果

## 故障排除

如果遇到问题:

1. **MATLAB路径错误**: 确保正确设置MATLAB.exe的路径
2. **脚本路径错误**: 确保.m文件路径正确且可访问
3. **字符编码问题**: 如果出现乱码，尝试修改readMatlabOutput函数中的字符编码处理
4. **MATLAB启动失败**: 检查是否有足够的系统权限来启动MATLAB进程
