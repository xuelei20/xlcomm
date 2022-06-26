现阶段代码应基于C++17，不能用C++2x特性。在使用C++14和C++17时，应该考虑可移植性。

# 1.头文件

通常每一个.h对应一个.cc文件

## 自包含
每个头文件应该包含自己所需的所有头文件。

## 防止重复包含
所有头文件都应该有 #define 保护来防止头文件被多重包含, 命名格式当是: <PROJECT>_<PATH>_<FILE>_H_ .
为保证唯一性, 头文件的命名应该基于所在项目源代码树的全路径. 例如, 项目 foo 中的头文件 foo/src/bar/baz.h 可按如下方式保护:
```
#ifndef FOO_BAR_BAZ_H_
#define FOO_BAR_BAZ_H_
...
#endif // FOO_BAR_BAZ_H_
```

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
```
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
```
```
// In the .cc file
#include "a.h"

DEFINE_FLAG(bool, someflag, false, "dummy flag");

namespace mynamespace {

// Definition of functions is within scope of the namespace.
void MyClass::Foo() {
  ...
}

}  // namespace mynamespace
```
- 自己的代码应放在namespace里，不要使用using namespace xxx，以防污染全局作用域
- 在命名空间的最后注释上名字
- 不要在头文件使用命名空间别名（可以在命名空间/函数内部使用）

## 源文件内部变量
在源文件内部使用不被外部访问的变量时，可以使用static或匿名命名空间限定作用域，不要用裸的全局变量。
```
// 函数和变量可以经由声明为static具有内部链接性（即使两个源文件声明了一样的标识符，所指向的实体也不同）
static int g_count;
// C static声明的作用域已经被C++标准提倡弃用
```
```
// 置于匿名命名空间的声明也具有内部链接性
namespace {
...
} // namespace
```

## 不要定义全局函数
使用静态成员函数、命名空间内的非成员函数，尽量不要使用裸的全局函数，以免污染全局作用域。
```
namespace xl {
namespace base {
void function1();
void function2();
} // base
} // xl

```

## 局部变量
在离使用局部变量最近的位置声明（除非为了效率），并显示初始化（比隐式初始化再赋值更高效）
```
std::vector<int> vec = { 10, 20 };
// 好于以下代码
std::vector<int> vec;
vec.push_back(10);
vec.push_back(20);
```

## 禁止定义非原生数据类型全局/静态变量
原生数据类型(POD)指int char float以及对应的裸指针、数组、结构体，非原生数据类型可能是类（涉及到构造析构）。

同一个编译单元内全局/静态类对象构造析构是明确的，初始化顺序按照声明顺序进行，销毁则逆之；不同的编译单元中初始化和销毁顺序属于不明确行为，这会导致难以发现的bug。既然初始化顺序不确定，那么在异常中断时析构的顺序也不确定，可能一个对象被析构了，其他编译单元的线程还在访问它。
不过constexpr变量除外，毕竟它们不涉及动态初始化或析构。

静态储存周期变量包括：裸全局变量、static全局变量、static成员变量、static函数变量。
```
const int kNum = 10;  // Allowed

struct X { int n; };
const X kX[] = {{1}, {2}, {3}};  // Allowed

void foo() {
  static const char* const kMessages[] = {"hello", "world"};  // Allowed
}

// Allowed: constexpr guarantees trivial destructor.
constexpr std::array<int, 3> kArray = {1, 2, 3};
```
```
// bad: non-trivial destructor
const std::string kFoo = "foo";

// Bad for the same reason, even though kBar is a reference (the
// rule also applies to lifetime-extended temporary objects).
const std::string& kBar = StrCat("a", "b", "c");

void bar() {
  // Bad: non-trivial destructor.
  static std::map<int, int> kData = {{1, 0}, {2, 0}, {3, 0}};
}
```
综上所述，只允许原生数据类型静态储存周期变量，完全禁止vector（使用数组替代）、string（使用const char[]替代）。如果确实需要静态对象，可以用单例（只创建不销毁）。

# 3.类

## 构造函数
- 构造函数不允许调用虚函数，这种调用不会重定向到子类的实现
- 构造函数很难上报错误，如果需要有意义的初始化尽量使用Init()方法

## 禁用隐式类型转换
不要使用隐式类型转换，对于单参数构造函数和转换运算符加上explicit关键字

## 可拷贝和可移动
如果你的类需要就显示定义可拷贝(拷贝构造+拷贝赋值)和可移动(移动构造+移动赋值)函数，否则就把隐式拷贝和移动函数禁用
```
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
```
不要为基类提供赋值、拷贝构造、移动构造函数。如果基类需要可复制，提供一个public virtual Clone()函数和protected拷贝构造函数以供派生类实现。

## 结构体和类
仅当只有数据成员时使用struct，其他一概使用class。

结构体只提供构造、析构、Initialize()、Reset()、 Validate() 等用于设定数据成员的函数。

## 继承
如果使用继承的话定义为public继承

C++实践中继承应用于两种场合：
- 实现继承。继承基类的代码实现，减少编码工作量。不要过度使用实现继承，只有在“是一个”的情况下使用，否则组合更合适。
- 接口继承。仅继承基类的接口，自己实现代码。如果你的类有虚函数，则析构函数也应该声明为virtual。

对于重载的虚函数或虚析构函数也要声明virtual，并使用override或final标注。
好处：如果写错函数声明会编译报错，并且这些标记起到了文档的作用。
```
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
```

## 多重继承
用到多重继承的情况少之又少。只有以下情况允许使用多重继承：最多只有一个基类是非抽象基类，其他基类都是纯接口类。

## 接口
一般以Interface为后缀或以I为前缀，纯接口满足以下条件：
- 不定义任何构造函数，即使有也不带参数、并声明为protected
- 只有纯虚函数（virtual void fun() = 0;）和静态函数
- 必须定义虚析构函数（不能是纯虚函数）
- 没有非静态数据成员

## 运算符重载
除少数特定情况外，不要进行运算符重载，也不要创建用户定义字面量。常用的运算符重载有=、==、>、<、<<

## 声明顺序
类以public开始、protected在中间（少用）、private在最后。将类似的声明放在一起，建议以如下的顺序: 
- 类型 (typedef, using, enum, 嵌套结构和类)
- 常量
- 工厂函数
- 构造函数、赋值运算符
- 析构函数
- 其它函数（static以及非static成员函数和friend函数）
- 数据成员（static以及非static）
所有非static数据成员必须为private

# 4.函数

## 输入输出
- 一般以返回值为输出
- 所有输入参数排在所有输出参数之前，即使加入新参数也要按这个顺序
- 输入参数是值参或const引用(或特殊需要用const指针)，输出参数为指针【google硬性规定】。std::optional表示可选的按值输入
```
void Fun(const std::string &in, std::string *out);
```

## 编写简短函数
一般一个函数不要超过40行代码。如果函数太长就会很复杂，难以阅读和发现bug。

## 函数重载
若要使用函数重载，则应该让调用者一眼就明白对应哪个
```
// Good
class MyClass {
public:
    void Analyze(const string &text);
    void Analyze(const char *text, size_t textlen);
};
```
```
// Bad
class MyClass {
public:
    void Append(int a);
    void Append(char a);
};
// 改成AppendInt() AppendChar()更好
```

## 缺省参数
- 虚函数不允许使用缺省参数，因为难以保证所有重载声明都是同样的缺省
- 缺省参数与函数重载遵循同样的规则，一般建议使用函数重载。因为缺省参数会干扰函数指针, 导致函数签名与调用点的签名不一致。

## 函数返回类型后置语法
C++11新引入的
```
auto foo(int x) -> int;
```
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
流是C++中的标准I/O抽象，如iostream所示。主要用于调试日志记录和测试诊断

## 使用前置自增和自减
++iter效率更高，因为iter++多一次拷贝
	
## 尽可能多用const，让代码更安全
```
const int a = 10; // const标识常量，只读、不可修改
	
class A
{
public:
	int getA() const; // 类成员函数使用const标识不会改变成员变量的值
	void fun2(const std::string &str);
private:
	int m_a;
};
	
// 以下三种都表示对象a只读，只能调用const成员函数
const A a;
const A& a;
const A* pa; // 等于A const * pa;（可读性差）
	
// 下面是变态写法（不要用），表示指针不可修改
A* const pa;
```

## constexpr编译期求值
const还可以被volatile mutable改变，定义为constexpr则是真正意义上的常量
	
## 整型
使用C++11标识大小的整型，比如int16_t、int32_t、int64_t。不要使用传统的long类型，因为在不同操作系统可能是不同长度。
无符号整型很危险，尽量不要使用，可以用断言代替。
	
## 32位和64位的区别
- 指针大小、结构体对齐不一样
- 创建64位常量时使用LL或ULL作为后缀
```
int64_t value = 0x123456LL;
uint64_t value2 = 3ULL << 48;
```
	
## 谨慎使用宏
尽量以内联函数、枚举、常量代替
	
## 使用nullptr代替NULL
因为NULL在不同操作系统可能是不同定义，可能只是单纯的0
```
#ifndef NULL
    #ifdef __cplusplus
        #define NULL 0
    #else
        #define NULL ((void *)0)
    #endif
#endif
```

## 用sizeof(var)代替sizeof(type)
当变量类型改变时会自动更新

## 用auto代替长类型名
只在局部变量使用，保证很容易看出来是什么类型
```
std::map<int,int> kv;
auto it = kv.begin();
```

## 可以用列表初始化
```
class MyVector
{
public:
  MyVector(std::initializer_list<int> init_list)
  {
    for (int i : init_list)
      std::cout << i << ' ';
  }
};

MyVector vec = {1,2,3};
```

## 适当使用Lambda
- 小函数使用lambda，别超过5行
- 别用默认捕获[=]，所有捕获都要显示写出来
- 可以和std::function、std::bind搭配成通用回调
```
void testFunction(std::function<void(int)> f, int a)
{
    f(a);
}

testFunction([](int num) {std::cout << num; }, 10);
```

## 不要使用复杂的模板编程
尽量让代码读起来易懂

## Boost
只使用Boost中被认可的库

## Modern C++
以下头文件不要使用
```
<ratio>
<cfenv> <fenv.h> 
<filesystem> //没有足够的测试支持，并且存在固有的安全漏洞
```

## 用using声明别名
using比typedef更合适，因为它提供了与C++其他部分更一致的语法。注意不要在公共API中使用别名。
```
using TimeSeries = std::unordered_set<DataPoint, std::hash<DataPoint>, DataPointComparator>;
typedef int XL_RESULT; // 反着呢
```

## 用std::function声明回调函数
std::function表示C++函数对象，比C语言的回调函数更灵活，且语法方便
```
void testFunction(std::function<void(int)> f, int a)
{
    f(a);
}

typedef void(*cfun)(int); // 比较难写
void testCFun(cfun f, int a)
{
    f(a);
}
```

# 7.命名约定
最重要的一致性规则是命名管理. 命名的风格能让我们在不需要去查找类型声明的条件下快速地了解某个名字代表的含义: 类型, 变量, 函数, 常量, 宏, 等等, 甚至. 我们大脑中的模式匹配引擎非常依赖这些命名规则.

## 通用命名规则
自带描述性，别用他人看不懂的缩写
```
int price_count_reader;    // 无缩写
int num_errors;            // "num" 是一个常见的写法
int num_dns_connections;   // 人人都知道 "DNS" 是什么
```

## 文件命名
文件名全部小写，以_(更好)或-连接。头文件以.h结尾，源文件以.cc结尾，专门插入文本的文件以.inc结尾。
通常应尽量让文件名更加明确，http_server_logs.h 就比 logs.h 要好。
定义类时文件名一般成对出现, 如 foo_bar.h 和 foo_bar.cc, 对应于类 FooBar。
内联函数放在.h文件中。

## 类型命名
每个单词首字母大写，不包含下划线。如MyExcitingClass。
类, 结构体, 类型定义 (typedef), 枚举, 类型模板参数都是这样。
```
// 类和结构体
class UrlTable { ...
struct UrlTableProperties { ...

// 类型定义
typedef hash_map<UrlTableProperties *, string> PropertiesMap;

// using 别名
using PropertiesMap = hash_map<UrlTableProperties *, string>;

// 枚举
enum UrlTableErrors { ...
```

## 变量命名
变量(包括函数参数)和数据成员名一律小写，单词之间以_连接。
```
string table_name;  // 好 - 用下划线.
string tablename;   // 好 - 全小写.
string tableName;   // 差 - 混合大小写
```

类的成员变量以下划线结尾，结构体的不用
```
class TableInfo {
  ...
 private:
  string table_name_;  // 好 - 后加下划线.
  string tablename_;   // 好.
  static Pool<TableInfo>* pool_;  // 好.
};
struct UrlTableProperties {
  string name;
  int num_entries;
  static Pool<UrlTableProperties>* pool;
};
```

## 常量命名
声明为 constexpr 或 const 的变量, 或在程序运行期间其值始终保持不变的, 命名时以 “k” 开头, 大小写混合
```
const int kDaysInAWeek = 7;
```
所有具有静态存储类型的变量 (例如静态变量或全局变量) 都应当以此方式命名.

## 函数命名
常规函数使用大小写混合, 取值和设值函数则要求与变量名匹配: MyExcitingFunction(), MyExcitingMethod(), my_exciting_member_variable(), set_my_exciting_member_variable()。
对于首字母缩写的单词, 更倾向于将它们视作一个单词进行首字母大写 (例如, 写作 StartRpc() 而非 StartRPC())。
同样的命名规则同时适用于类作用域与命名空间作用域的常量, 因为它们是作为 API 的一部分暴露对外的, 因此应当让它们看起来像是一个函数。

## namespace命名
命名空间以小写字母命名。命名空间中的代码, 应当存放于和命名空间的名字匹配的文件夹或其子文件夹中。注意 不使用缩写作为名称 的规则同样适用于命名空间。要避免嵌套的命名空间与常见的顶级命名空间发生名称冲突

## 宏命名
尽量不用宏，如果一定要用，像这样命名: MY_MACRO_THAT_SCARES_SMALL_CHILDREN

## 枚举命名
单独的枚举值应该优先采用 常量 的命名方式，尽量少用 宏 方式的命名（可能导致枚举值和宏之间的命名冲突）
```
// better
enum UrlTableErrors {
    kOK = 0,
    kErrorOutOfMemory,
    kErrorMalformedInput,
};
enum AlternateUrlTableErrors {
    OK = 0,
    OUT_OF_MEMORY = 1,
    MALFORMED_INPUT = 2,
};
```

## 特例
如果你命名的实体与已有 C/C++ 实体相似, 可参考现有命名策略.
- bigopen(): 函数名, 参照 open() 的形式
- uint: typedef
- bigpos: struct 或 class, 参照 pos 的形式
- sparse_hash_map: STL 型实体; 参照 STL 命名约定
- LONGLONG_MAX: 常量, 如同 INT_MAX

# 8.注释
注释虽然写起来很痛苦, 但对保证代码可读性至关重要。注释固然很重要, 但最好的代码应当本身就是文档. 有意义的类型名和变量名, 要远胜过要用注释解释的含糊不清的名字。
你写的注释是给代码读者看的, 也就是下一个需要理解你的代码的人. 所以慷慨些吧, 下一个读者可能就是你!

## 注释风格
// 或 /* */ 都可以; 但 // 更 常用. 要在如何注释及注释风格上确保统一.

## 文件注释
在文件开头加入版权、作者、文件目的等
```
// Author: 
// Pursose: 
```

## 类注释
每个类的定义都要附带一份注释, 描述类的功能和用法, 除非它的功能相当明显.
```
// Iterates over the contents of a GargantuanTable.
// Example:
//    GargantuanTableIterator* iter = table->NewIterator();
//    for (iter->Seek("foo"); !iter->done(); iter->Next()) {
//      process(iter->key(), iter->value());
//    }
//    delete iter;
class GargantuanTableIterator {
  ...
};
```
如果类的声明和定义分开了(例如分别放在了 .h 和 .cc 文件中), 此时, 描述类用法的注释应当和接口定义放在一起, 描述类的操作和实现的注释应当和实现放在一起

## 函数注释
函数声明处的注释描述函数功能; 定义处的注释描述函数实现

基本上每个函数声明处前都应当加上注释, 描述函数的功能和用途. 只有在函数的功能简单而明显时才能省略这些注释(例如, 简单的取值和设值函数、构造析构函数).
函数声明处注释的内容:
- 函数的输入输出.
- 对类成员函数而言: 函数调用期间对象是否需要保持引用参数, 是否会释放这些参数.
- 函数是否分配了必须由调用者释放的空间.
- 参数是否可以为空指针.
- 是否存在函数使用上的性能隐患.
- 如果函数是可重入的, 其同步前提是什么?
```
// Returns an iterator for this table.  It is the client's
// responsibility to delete the iterator when it is done with it,
// and it must not use the iterator once the GargantuanTable object
// on which the iterator was created has been deleted.
//
// The iterator is initially positioned at the beginning of the table.
//
// This method is equivalent to:
//    Iterator* iter = table->NewIterator();
//    iter->Seek("");
//    return iter;
// If you are going to immediately seek to another place in the
// returned iterator, it will be faster to use NewIterator()
// and avoid the extra seek.
Iterator* GetIterator() const;
```

如果函数的实现过程中用到了很巧妙的方式, 那么在函数定义处应当加上解释性的注释

## 变量注释
类数据成员加上注释，全局变量必须加注释。特殊注意的地方也要注释

## 实现注释
对于代码中巧妙的, 晦涩的, 有趣的, 重要的地方加以注释

比较隐晦的地方要在行尾加入注释. 在行尾空两格进行注释. 比如:
```
// If we have enough memory, mmap the data portion too.
mmap_budget = max<int64>(0, mmap_budget - index_->length());
if (mmap_budget >= data_size_ && !MmapData(mmap_chunk_bytes, mlock))
  return;  // Error already logged.
```

如果某个函数有多个配置选项, 你可以考虑定义一个类或结构体以保存所有的选项

自文档化的代码根本就不需要注释
```
if (!IsAlreadyProcessed(element)) {
  Process(element);
}
```

## TODO注释
TODO 注释要使用全大写的字符串 TODO, 在随后的圆括号里写上你的名字, 邮件地址, bug ID, 或其它身份标识和与这一 TODO 相关的 issue. 
```
// TODO(kl@gmail.com): Use a "*" here for concatenation operator.
// TODO(Zeke) change this to use relations.
// TODO(bug 12345): remove the "Last visitors" feature
```
如果加 TODO 是为了在 “将来某一天做某事”, 可以附上一个非常明确的时间 “Fix by November 2005”), 或者一个明确的事项 (“Remove this code when all clients can handle XML responses.”).

## 弃用注释
通过弃用注释（DEPRECATED comments）以标记某接口点已弃用。在 DEPRECATED 一词后, 在括号中留下您的名字, 邮箱地址以及其他身份标识。仅仅标记接口为 DEPRECATED 并不会让大家不约而同地弃用, 您还得亲自主动修正调用点（callsites）, 或是找个帮手.

## 中文or英文？
英语不好就用中文注释吧，这样别人也更容易理解。

# 9.格式

## 行长度
每一行代码字符数最好不超过 80.

## 非ASCII字符只使用UTF-8
尽量不使用非 ASCII 字符, 使用时必须使用 UTF-8（或UTF-8 with BOM）编码格式。因为UTF-8更适合跨平台。
别用 C++11 的 char16_t 和 char32_t, 它们和 UTF-8 文本没有关系, wchar_t 同理, 除非你写的代码要调用 Windows API（广泛使用了 wchar_t）

## 缩进用2个空格
使用2个空格缩进. 不要在代码中使用制表符. 你应该设置编辑器将制表符转为空格

## 函数声明与定义
- 返回类型和函数名在同一行, 参数也尽量放在同一行, 如果放不下就对形参分行并对齐
- 左圆括号(总是和函数名在同一行，函数名和左圆括号间永远没有空格
- 圆括号()与参数间没有空格
- 右圆括号)和左大括号{间总是有一个空格
- 左大括号{总在最后一个参数同一行的末尾处, 不另起新行
- 右大括号}总是单独位于函数最后一行, 或者与左大括号同一行
- 缺省缩进为 2 个空格，换行后的参数保持 4 个空格的缩进
- 属性和展开为属性的宏, 写在函数声明或定义的最前面
```
ReturnType ClassName::FunctionName(Type par_name1, Type par_name2) {
  DoSomething();
  ...
}

ReturnType ClassName::ReallyLongFunctionName(Type par_name1, Type par_name2,
                                             Type par_name3) {
  DoSomething();
  ...
}

ReturnType LongClassName::ReallyReallyReallyLongFunctionName(
    Type par_name1,  // 4 space indent
    Type par_name2,
    Type par_name3) {
  DoSomething();  // 2 space indent
  ...
}

MUST_USE_RESULT bool IsOK();
```

## Lambda 表达式
Lambda 表达式对形参和函数体的格式化和其他函数一致

若用引用捕获, 在变量名和 & 之间不留空格.
```
int x = 0;
auto add_to_x = [&x](int n) { x += n; };
```

短 lambda 就写得和内联函数一样.
```
std::set<int> blacklist = {7, 8, 9};
std::vector<int> digits = {3, 9, 1, 8, 4, 7, 1};
digits.erase(std::remove_if(digits.begin(), digits.end(), [&blacklist](int i) {
               return blacklist.find(i) != blacklist.end();
             }),
             digits.end());
```

## 函数调用
要么一行写完函数调用, 要么在圆括号里对参数分行, 要么参数另起一行且缩进四格. 
```
bool retval = DoSomething(argument1, argument2, argument3);

bool retval = DoSomething(averyveryveryverylongargument1,
                          argument2, argument3);
			  
if (...) {
  ...
  ...
  if (...) {
    DoSomething(
        argument1, argument2,  // 4 空格缩进
        argument3, argument4);
  }
```

## 列表初始化格式
平时怎么格式化函数调用, 就怎么格式化 列表初始化
```
pair<int, int> p{foo, bar};

SomeOtherType{"Slightly shorter string",  
              some, other, values}};
	      
MyType m = {
    superlongvariablename1,
    superlongvariablename2,
    {short, interior, list},
    {interiorwrappinglist,
     interiorwrappinglist2}};
```

## 条件语句
倾向于不在圆括号内使用空格. 关键字 if 和 else 另起一行. 所有情况下 if 和左圆括号间都有个空格. 右圆括号和左大括号之间也有个空格
```
if (condition) {  // 圆括号里没有空格.
  ...  // 2 空格缩进.
} else if (...) {  // else 与 if 的右括号同一行.
  ...
} else {
  ...
}
```
如果能增强可读性, 简短的条件语句允许写在同一行. 只有当语句简单并且没有使用 else 子句时使用.
```
if (x == kFoo) return new Foo();
if (x == kBar) return new Bar();

if (condition)
  DoSomething();  // 2 空格缩进.
```
单行语句可以不用大括号。但如果语句中某个 if-else 分支使用了大括号的话, 其它分支也必须使用

## 循环和switch语句
switch 语句可以使用大括号分段, 以表明 cases 之间不是连在一起的。也可不用。
如果有不满足 case 条件的枚举值, switch 应该总是包含一个 default 匹配. 如果 default 应该永远执行不到, 简单的加条 assert:
```
switch (var) {
  case 0: {  // 2 空格缩进
    ...      // 4 空格缩进
    break;
  }
  case 1: {
    ...
    break;
  }
  default: {
    assert(false);
  }
}
```

在单语句循环里, 括号可用可不用：
```
for (int i = 0; i < kSomeNumber; ++i)
  printf("I love you\n");

for (int i = 0; i < kSomeNumber; ++i) {
  printf("I take it back\n");
}
```
空循环体应使用 {} 或 continue, 而不是一个简单的分号.
```
while (condition) {
  // 反复循环直到条件失效.
}
for (int i = 0; i < kSomeNumber; ++i) {}  // 可以 - 空循环体.
while (condition) continue;  // 可以 - continue 表明没有逻辑.
```

## 指针和引用
句点或箭头前后不要有空格. 指针/地址操作符之后不能有空格.
```
x = r.y;
x = r->y;
x = *p;
p = &x;
```

在声明指针变量或参数时, 星号与类型或变量名紧挨都可以。在多重声明中不能使用 & 或 *
```
// 好, 空格前置.
char *c;
const string &str;

// 好, 空格后置.
char* c;
const string& str;

int x, *y;  // 不允许 - 在多重声明中不能使用 & 或 *
char * c;  // 差 - * 两边都有空格
const string & str;  // 差 - & 两边都有空格.
```
在单个文件内要保持风格一致。如果是修改现有文件, 要遵照该文件的风格。

## 布尔表达式
如果一个布尔表达式超过 标准行宽, 断行方式要统一一下.
下例中, 逻辑与 (&&) 操作符总位于行尾:
```
if (this_one_thing > this_other_thing &&
    a_third_thing == a_fourth_thing &&
    yet_another && last_one) {
  ...
}
```

## 变量及数组初始化
用 =, () 和 {} 均可
```
int x = 3;
int x(3);
int x{3};
string name("Some Name");
string name = "Some Name";
string name{"Some Name"};
```
使用std::initializer_list 和 {} 时注意避免歧义

## 预处理指令
预处理指令不要缩进, 从行首开始.
```
  if (lopsided_score) {
#if DISASTER_PENDING      // 正确 - 从行首开始
    DropEverything();
#if NOTIFY               
    NotifyClient();
#endif
#endif
    BackToNormal();
  }
```

## 类格式
访问控制块的声明依次序是 public:, protected:, private:, 每个都缩进 1 个空格. 后两个要空一行
```
class MyClass : public OtherClass {
 public:      // 注意有一个空格的缩进
  MyClass();  // 标准的两空格缩进
  explicit MyClass(int var);
  ~MyClass() {}

  void SomeFunction();
  void SomeFunctionThatDoesNothing() {
  }

  void set_some_var(int var) { some_var_ = var; }
  int some_var() const { return some_var_; }

 private:
  bool SomeInternalFunction();

  int some_var_;
  int some_other_var_;
};
```

## 构造函数初始值列表
构造函数初始化列表放在同一行或按四格缩进并排多行.
```
// 如果所有变量能放在同一行:
MyClass::MyClass(int var) : some_var_(var) {
  DoSomething();
}

// 如果不能放在同一行,
// 必须置于冒号后, 并缩进 4 个空格
MyClass::MyClass(int var)
    : some_var_(var), some_other_var_(var + 1) {
  DoSomething();
}

// 如果初始化列表需要置于多行, 将每一个成员放在单独的一行
// 并逐行对齐
MyClass::MyClass(int var)
    : some_var_(var),             // 4 space indent
      some_other_var_(var + 1) {  // lined up
  DoSomething();
}
```

## namespace格式化
命名空间内容不缩进. 声明嵌套命名空间时, 每个命名空间都独立成行.
```
namespace {

void foo() {  // 正确. 命名空间内没有额外的缩进.
  ...
}

}  // namespace

namespace foo {
namespace bar {
```

## 水平留白
水平留白的使用根据在代码中的位置决定. 不要在行尾留没意义的空格（可以设置IDE自动删除）
```
void f(bool b) {  // 左大括号前总是有空格.
  ...
int i = 0;  // 分号前不加空格.
// 列表初始化中大括号内的空格是可选的.
// 如果加了空格, 那么两边都要加上.
int x[] = { 0 };
int x[] = {0};

// 继承与初始化列表中的冒号前后恒有空格.
class Foo : public Bar {
 public:
  // 对于单行函数的实现, 在大括号内加上空格
  Foo(int b) : Bar(), baz_(b) {}  // 大括号里面是空的话, 不加空格.
  void Reset() { baz_ = 0; }  // 用空格把大括号与实现分开.
  ...
```

操作符
```
// 赋值运算符前后总是有空格.
x = 0;

// 其它二元操作符也前后恒有空格, 不过对于表达式的子式可以不加空格.
// 圆括号内部没有紧邻空格.
v = w * x + y / z;
v = w*x + y/z;
v = w * (x + z);

// 在参数和一元操作符之间不加空格.
x = -5;
++x;
if (x && !y)
  ...
```

模板和转换
```
// 尖括号(< and >) 不与空格紧邻, < 前没有空格, > 和 ( 之间也没有.
vector<string> x;
y = static_cast<char*>(x);

// 在类型与指针操作符之间留空格也可以, 但要保持一致.
vector<char *> x;
```

## 垂直留白
垂直留白越少越好。基本原则是: 同一屏可以显示的代码越多, 越容易理解程序的控制流。
这不仅仅是规则而是原则问题了: 不在万不得已, 不要使用空行. 尤其是: 两个函数定义之间的空行不要超过 2 行, 函数体首尾不要留空行, 函数体中也不要随意添加空行.

# 10.规则特例
前面说明的编程习惯基本都是强制性的. 但所有优秀的规则都允许例外, 这里就是探讨这些特例.

## 现有不合规范的代码
对于现有不符合既定编程风格的代码可以网开一面.

当你修改使用其他风格的代码时, 为了与代码原有风格保持一致可以不使用本指南约定. 如果不放心, 可以与代码原作者或现在的负责人员商讨. 记住, 一致性 也包括原有的一致性.

## Windows 代码
Windows 程序员有自己的编程习惯, 主要源于 Windows 头文件和其它 Microsoft 代码. 我们希望任何人都可以顺利读懂你的代码, 所以针对所有平台的 C++ 编程只给出一个单独的指南.

如果你习惯使用 Windows 编码风格, 这儿有必要重申一下某些你可能会忘记的指南:
- 不要使用匈牙利命名法 (比如把整型变量命名成 iNum). 使用 Google 命名约定, 包括对源文件使用 .cc 扩展名.
- Windows 定义了很多原生类型的同义词, 如 DWORD, HANDLE 等等. 在调用 Windows API 时这是完全可以接受甚至鼓励的. 即使如此, 还是尽量使用原有的 C++ 类型, 例如使用 const TCHAR * 而不是 LPCTSTR.
- 使用 Microsoft Visual C++ 进行编译时, 将警告级别设置为 3 或更高, 并将所有警告(warnings)当作错误(errors)处理.
- 不要使用 #pragma once; 而应该使用 Google 的头文件保护规则. 头文件保护的路径应该相对于项目根目录 (如 #ifndef SRC_DIR_BAR_H_).
- 除非万不得已, 不要使用任何非标准的扩展, 如 #pragma 和 __declspec. 使用 __declspec(dllimport) 和 __declspec(dllexport) 是允许的, 但必须通过宏来使用, 比如 DLLIMPORT 和 DLLEXPORT, 这样其他人在分享使用这些代码时可以很容易地禁用这些扩展.

然而, 在 Windows 上仍然有一些我们偶尔需要违反的规则:
- 通常我们 禁止使用多重继承, 但在使用 COM 和 ATL/WTL 类时可以使用多重继承. 为了实现 COM 或 ATL/WTL 类/接口, 你可能不得不使用多重实现继承.
- 虽然代码中不应该使用异常, 但是在 ATL 和部分 STL（包括 Visual C++ 的 STL) 中异常被广泛使用. 使用 ATL 时, 应定义 _ATL_NO_EXCEPTIONS 以禁用异常. 你需要研究一下是否能够禁用 STL 的异常, 如果无法禁用, 可以启用编译器异常. (注意这只是为了编译 STL, 自己的代码里仍然不应当包含异常处理).
- 通常为了利用头文件预编译, 每个每个源文件的开头都会包含一个名为 StdAfx.h 或 precompile.h 的文件. 为了使代码方便与其他项目共享, 请避免显式包含此文件 (除了在 precompile.cc 中), 使用 /FI 编译器选项以自动包含该文件.
- 资源头文件通常命名为 resource.h 且只包含宏, 这一文件不需要遵守本风格指南.
