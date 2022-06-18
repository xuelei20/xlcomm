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
- 自己的代码应放在namespace里，不要使用using namespace xxx，以防污染全局作用域
- 在命名空间的最后注释上名字
- 不要在头文件使用命名空间别名（可以在命名空间/函数内部使用）

## 源文件内部变量
在源文件内部使用不被外部访问的变量时，可以使用static或匿名命名空间限定作用域，不要用裸的全局变量。
````
// 函数和变量可以经由声明为static具有内部链接性（即使两个源文件声明了一样的标识符，所指向的实体也不同）
static int g_count;
// C static声明的作用域已经被C++标准提倡弃用
````
````
// 置于匿名命名空间的声明也具有内部链接性
namespace {
...
} // namespace
````

## 不要定义全局函数
使用静态成员函数、命名空间内的非成员函数，尽量不要使用裸的全局函数，以免污染全局作用域。
````
namespace xl {
namespace base {
void function1();
void function2();
} // base
} // xl

````

## 局部变量
在离使用局部变量最近的位置声明（除非为了效率），并显示初始化（比隐式初始化再赋值更高效）
````
std::vector<int> vec = { 10, 20 };
// 好于以下代码
std::vector<int> vec;
vec.push_back(10);
vec.push_back(20);
````

## 禁止定义非原生数据类型全局/静态变量
原生数据类型指int char float以及对应的裸指针、数组、结构体，非原生数据类型可能是类（涉及到构造析构）。

同一个编译单元内全局/静态类对象构造析构是明确的，初始化顺序按照声明顺序进行，销毁则逆之；不同的编译单元中初始化和销毁顺序属于不明确行为，这会导致难以发现的bug。既然初始化顺序不确定，那么在异常中断时析构的顺序也不确定，可能一个对象被析构了，其他编译单元的线程还在访问它。
不过constexpr变量除外，毕竟它们不涉及动态初始化或析构。

静态储存周期变量包括：裸全局变量、static全局变量、static成员变量、static函数变量。
````
const int kNum = 10;  // Allowed

struct X { int n; };
const X kX[] = {{1}, {2}, {3}};  // Allowed

void foo() {
  static const char* const kMessages[] = {"hello", "world"};  // Allowed
}

// Allowed: constexpr guarantees trivial destructor.
constexpr std::array<int, 3> kArray = {1, 2, 3};
````
````
// bad: non-trivial destructor
const std::string kFoo = "foo";

// Bad for the same reason, even though kBar is a reference (the
// rule also applies to lifetime-extended temporary objects).
const std::string& kBar = StrCat("a", "b", "c");

void bar() {
  // Bad: non-trivial destructor.
  static std::map<int, int> kData = {{1, 0}, {2, 0}, {3, 0}};
}
````
综上所述，只允许原生数据类型静态储存周期变量，完全禁止vector（使用数组替代）、string（使用const char[]替代）。如果确实需要静态对象，可以用单例（只创建不销毁）。
