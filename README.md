# xlcomm
xuelei's common library
- src/base基本功能封装。包括线程、进程和应用程序、文件系统、日期时间、日志（同步/异步）等
- src/net高性能网络io封装。

# 版本号及兼容性
版本号使用x.y.z三位
- 升级z版本: 二进制(ABI)兼容，使用者只需替换动态库即可
- 升级y版本: 源码级(API)兼容，使用者还得更新头文件重新编译
- 升级x版本: 源码级不兼容(避免出现)，使用者必须改用新接口

能保证ABI的: 
1. 修改函数实现
2. 新增结构体,类,非virtual函数

破坏ABI但能保证API的: 
1. 修改非static成员变量(包括新增 删除 修改类型 非private变换顺序)
2. 修改virtual成员函数(包括新增 删除 修改类型 变换顺序)
3. 修改结构体(新增字段 变换顺序)
4. 修改枚举值

破坏API的:
1. 修改原有函数声明(包括名字 参数 返回值)
2. 删除函数,结构体,结构体字段,枚举字段等

# 各个平台
| 操作系统 | 编辑器/IDE         | 编译规则   |  默认编译器  | 运行时库                                  | 调试器  |
| :-----: | :----------------: | :-------: | :--------:  | :---------------------------------------: | :----: |
| Windows | Visual Studio 201x | .vcxproj  | MSVC(cl)    | Microsoft Visual C++ 201x Redistributable | WinDbg |
| Linux   | vim/VSCode         | Makefile  | gcc/g++     | libc/libstdc++                            | gdb    |
| MacOS   | XCode              | XCode     | LLVM(Clang) | libc++                                    |        |

CMake允许开发者编写一种平台无关的 CMakeList.txt 文件来定制整个编译流程，然后再根据目标用户的平台进一步生成所需的本地化 Makefile 或工程文件
