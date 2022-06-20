现阶段代码应基于C++17，不能用C++2x特性。在使用C++14和C++17时，应该考虑可移植性。

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

## 前置声明
尽可能避免使用前置声明。因为：前置声明隐藏了依赖关系，头文件改动时，用户的代码会跳过必要的重新编译过程。

## 内联函数
只有当函数只有10行甚至更少时才将其定义为内联函数。存取函数一般内联在头文件中。

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
原生数据类型(POD)指int char float以及对应的裸指针、数组、结构体，非原生数据类型可能是类（涉及到构造析构）。

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

# 3.类

## 构造函数
- 构造函数不允许调用虚函数，这种调用不会重定向到子类的实现
- 构造函数很难上报错误，如果需要有意义的初始化尽量使用Init()方法

## 禁用隐式类型转换
不要使用隐式类型转换，对于单参数构造函数和转换运算符加上explicit关键字

## 可拷贝和可移动
如果你的类需要就显示定义可拷贝(拷贝构造+拷贝赋值)和可移动(移动构造+移动赋值)函数，否则就把隐式拷贝和移动函数禁用
````
class Copyable {
 public:
  Copyable(const Copyable& other) = default;
  Copyable& operator=(const Copyable& other) = default;

  // The implicit move operations are suppressed by the declarations above.
  // You may explicitly declare move operations to support efficient moves.
};

class MoveOnly {
 public:
  MoveOnly(MoveOnly&& other) = default;
  MoveOnly& operator=(MoveOnly&& other) = default;

  // The copy operations are implicitly deleted, but you can
  // spell that out explicitly if you want:
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;
};

class NotCopyableOrMovable {
 public:
  // Not copyable or movable
  NotCopyableOrMovable(const NotCopyableOrMovable&) = delete;
  NotCopyableOrMovable& operator=(const NotCopyableOrMovable&) = delete;

  // The move operations are implicitly disabled, but you can
  // spell that out explicitly if you want:
  NotCopyableOrMovable(NotCopyableOrMovable&&) = delete;
  NotCopyableOrMovable& operator=(NotCopyableOrMovable&&) = delete;
};
````
不要为基类提供赋值、拷贝构造、移动构造函数。如果基类需要可复制，提供一个public virtual Clone()函数和protected拷贝构造函数以供派生类实现。

## 结构体和类
仅当只有数据成员时使用struct，其他一概使用class。

结构体只提供构造、析构、Initialize()、Reset()、 Validate() 等用于设定数据成员的函数。

类以public开始、protected在中间、private在最后。将类似的声明放在一起，建议以如下的顺序: 类型 (包括typedef, using和嵌套的结构体与类)、常量、工厂函数、构造函数、赋值运算符、析构函数、其它函数、数据成员。

## 继承
如果使用继承的话定义为public继承，所有非静态数据成员必须为private

C++实践中继承应用于两种场合：
- 实现继承。继承基类的代码实现，减少编码工作量。不要过度使用实现继承，只有在“是一个”的情况下使用，否则组合更合适。
- 接口继承。仅继承基类的接口，自己实现代码。如果你的类有虚函数，则析构函数也应该声明为virtual。

对于重载的虚函数或虚析构函数也要声明virtual，并使用override或final标注。
好处：如果写错函数声明会编译报错，并且这些标记起到了文档的作用。
````
class Base
{
public:
	Base() 
	{
	}
	virtual ~Base() 
	{
	}
	virtual void Run() = 0;
};

class Derived1 : public Base
{
public:
	Derived1() : Base()
	{
	}
	virtual ~Derived1() override
	{
	}
	virtual void Run() override
	{
	}
};
````

## 多重继承
用到多重继承的情况少之又少。只有以下情况允许使用多重继承：最多只有一个基类是非抽象基类，其他基类都是纯接口类。

## 接口
一般以Interface为后缀或以I为前缀，纯接口满足以下条件：
- 没有定义任何构造函数，即使有也不带任何参数
- 只有纯虚函数（virtual void fun() = 0）和静态函数
- 必须定义虚析构函数（不能是纯虚函数）
- 没有非静态数据成员

## 运算符重载
除少数特定情况外，不要进行运算符重载，也不要创建用户定义字面量。常用的运算符重载有=、==、>、<、<<

# 4.函数

## 输入输出
- 一般以返回值为输出
- 所有输入参数排在所有输出参数之前，即使加入新参数也要按这个顺序
- 输入参数是值参或const引用(或特殊需要用const指针)，输出参数为指针【google硬性规定】。std::optional表示可选的按值输入
````
void Fun(const std::string &in, std::string *out);
````

## 编写简短函数
一般一个函数不要超过40行代码。如果函数太长就会很复杂，难以阅读和发现bug。

## 函数重载
若要使用函数重载，则应该让调用者一眼就明白对应哪个
````
// Good
class MyClass {
public:
    void Analyze(const string &text);
    void Analyze(const char *text, size_t textlen);
};
````
````
// Bad
class MyClass {
public:
    void Append(int a);
    void Append(char a);
};
// 改成AppendInt() AppendChar()更好
````

## 缺省参数
- 虚函数不允许使用缺省参数，因为难以保证所有重载声明都是同样的缺省
- 缺省参数与函数重载遵循同样的规则，一般建议使用函数重载。因为缺省参数会干扰函数指针, 导致函数签名与调用点的签名不一致。

## 函数返回类型后置语法
C++11新引入的
````
auto foo(int x) -> int;
````
除非必须，否则不用

# 5.智能指针与所有权
使用C++11的std::unique_ptr<>管理动态分配的对象，并用std::move()转移所有权。如果不是必须不要使用std::shared_ptr<>

# 6.其他C++特性

## 右值引用
在定义移动拷贝和移动赋值运算符时使用右值引用

## 合理使用友元
允许合理的使用友元类和友元函数，以有限地访问私有成员

## 禁止使用异常
原因如下：
- 如果添加throw语句，为了异常安全需要检查所有调用点，否则只能眼睁睁看着异常一路往上跑，最终中断掉整个程序
- 异常会彻底扰乱程序的执行流程并难以判断
- 滥用异常会鼓励开发者捕获不合时宜或没法恢复的“伪异常”
使用错误码、断言等代替异常。

## 禁止使用运行时类型识别
RTTI允许程序员在运行时识别C++类对象的类型，它通过使用typeid()或dynamic_cast<>完成。
使用RTTI说明类设计出了问题，虚函数自身可以根据子类类型的不同而执行不同代码。

## 使用C++的类型转换
使用C++的类型转换，不要用C语言风格的类型转换。
- 用static_cast<>代替int y = (int)x或指针转换
- 用const_cast<>去掉const属性
- 用reinterpret_cast<>进行类型强转
- dynamic_cast<>参考上一节RTTI

## 流
流是C++中的标准I/O抽象，如<iostream>所示。主要用于调试日志记录和测试诊断。
