# 1.头文件

通常每一个.h对应一个.cc文件

## 自包含
每个头文件应该包含自己所需的所有头文件。

## 防止重复包含
头文件必须有
````
#ifndef FOO_BAR_H
#define FOO_BAR_H
...
#endif // FOO_BAR_H
````

如果要用#pragma once，需保证所有平台都有效。

## 头文件包含顺序
项目的所有头文件都用项目根目录的子级，避免使用UNIX快捷目录.(当前)和..(上一级)。
1. 本cc的.h文件 // 确保该头文件编译失败立刻终止
2. 空行
3. C语言标准库文件
4. 空行
5. C++标准库文件
6. 空行
7. 其他库头文件
8. 空行
9. 本项目头文件
10. 空行
11. 条件编译

# 2.作用域

## 命名空间
````
// In the .h file
namespace mynamespace {

// All declarations are within the namespace scope.
// Notice the lack of indentation.
class MyClass {
 public:
  ...
  void Foo();
};

}  // namespace mynamespace
````
````
// In the .cc file
#include "a.h"

DEFINE_FLAG(bool, someflag, false, "dummy flag");

namespace mynamespace {

// Definition of functions is within scope of the namespace.
void MyClass::Foo() {
  ...
}

}  // namespace mynamespace
````
- 自己的代码应放在namespace里，不要使用using namespace xxx，以防污染全局代码
- 在命名空间的最后注释上名字
