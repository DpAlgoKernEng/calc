# 企业级跨平台命令行计算器 - 项目架构设计

## 一、项目概述

### 1.1 项目目标
开发一个使用 CMake/C++17 构建的跨平台命令行计算器，支持三种计算模式，并具备良好的扩展性以便未来集成 Qt 图形界面。

### 1.2 技术栈
- **语言**: C++17
- **构建系统**: CMake 3.15+
- **目标平台**: Windows (MSVC/MinGW), macOS (Clang), Linux (GCC/Clang)
- **测试框架**: Google Test (gtest)
- **未来扩展**: Qt 6.x

### 1.3 三种计算模式

| 模式 | 功能描述 |
|------|----------|
| **标准模式** | 简单四则运算: +, -, *, /, 支持() |
| **科学计算模式** | 复杂嵌套表达式 + 数学函数 (sin, cos, tan, log, log10, exp, sqrt, pow, abs, floor, ceil, round) |
| **程序员模式** | 进制转换 (10/2/16/8进制) + 位运算 (^, &, \|, ~, <<, >>) |

## 二、系统架构设计

### 2.1 分层架构

```
┌─────────────────────────────────────────────────────────────┐
│                        Presentation Layer                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │   CLI UI     │  │  Qt GUI      │  │  Future UI   │       │
│  │  (当前实现)  │  │  (待实现)    │  │  (扩展)      │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────┴────────────────────────────────┐
│                        Application Layer                      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  Standard    │  │  Scientific  │  │  Programmer  │       │
│  │    Mode      │  │     Mode     │  │    Mode      │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│                    ┌──────────────┐                          │
│                    │ Mode Manager │                          │
│                    └──────────────┘                          │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────┴────────────────────────────────┐
│                          Core Layer                           │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │   Tokenizer  │  │    Parser    │  │   Evaluator  │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  AST Nodes   │  │  Operator    │  │   Function   │       │
│  │   (ExprTree) │  │  Definition  │  │   Registry   │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────┴────────────────────────────────┐
│                         Foundation Layer                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  Utilities   │  │  Math Lib    │  │   Platform   │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 目录结构

```
calc/
├── CMakeLists.txt                 # 主 CMake 配置
├── cmake/                        # CMake 模块目录
│   ├── Toolchain-*.cmake        # 跨平台工具链文件
│   ├── CompileOptions.cmake      # 编译选项配置
│   └── PackageConfig.cmake       # 包配置
│
├── include/calc/                 # 公共头文件
│   ├── calc.h                   # 主入口头文件
│   ├── core/
│   │   ├── token.h              # Token 定义
│   │   ├── tokenizer.h          # 词法分析器
│   │   ├── parser.h             # 语法分析器接口
│   │   ├── ast.h                # 抽象语法树节点
│   │   └── evaluator.h          # 求值器接口
│   ├── modes/
│   │   ├── mode.h               # 模式基类
│   │   ├── standard_mode.h      # 标准模式
│   │   ├── scientific_mode.h    # 科学计算模式
│   │   └── programmer_mode.h    # 程序员模式
│   ├── math/
│   │   ├── math_functions.h     # 数学函数库
│   │   └── converter.h          # 进制转换
│   └── utils/
│       ├── string_utils.h       # 字符串工具
│       └── error.h              # 错误处理
│
├── src/                         # 源代码
│   ├── main.cpp                 # 程序入口
│   ├── core/
│   │   ├── tokenizer.cpp
│   │   ├── parser/
│   │   │   ├── shunting_yard_parser.cpp   # 调度场算法
│   │   │   └── recursive_descent_parser.cpp
│   │   ├── ast/
│   │   │   ├── ast_node.cpp
│   │   │   ├── binary_op_node.cpp
│   │   │   ├── unary_op_node.cpp
│   │   │   ├── function_call_node.cpp
│   │   │   └── literal_node.cpp
│   │   └── evaluator/
│   │       ├── evaluator.cpp
│   │       └── evaluator_visitor.cpp
│   ├── modes/
│   │   ├── standard_mode.cpp
│   │   ├── scientific_mode.cpp
│   │   └── programmer_mode.cpp
│   ├── math/
│   │   ├── math_functions.cpp
│   │   └── converter.cpp
│   ├── ui/
│   │   └── cli/
│   │       ├── cli_app.cpp      # CLI 应用
│   │       ├── command_parser.cpp
│   │       └── output_formatter.cpp
│   └── utils/
│       ├── string_utils.cpp
│       └── error.cpp
│
├── tests/                       # 测试代码
│   ├── CMakeLists.txt
│   ├── unit/
│   │   ├── tokenizer_test.cpp
│   │   ├── parser_test.cpp
│   │   ├── evaluator_test.cpp
│   │   ├── math_functions_test.cpp
│   │   └── converter_test.cpp
│   ├── integration/
│   │   ├── standard_mode_test.cpp
│   │   ├── scientific_mode_test.cpp
│   │   └── programmer_mode_test.cpp
│   └── fixtures/
│       └── test_expressions.txt
│
├── docs/                        # 文档
│   ├── DESIGN.md                # 设计文档
│   ├── API.md                   # API 文档
│   └── USER_GUIDE.md            # 用户指南
│
├── scripts/                     # 构建和部署脚本
│   ├── build.sh
│   ├── build.bat
│   ├── test.sh
│   └── format.sh
│
└── examples/                    # 示例
    ├── standard_examples.txt
    ├── scientific_examples.txt
    └── programmer_examples.txt
```

## 三、核心类设计

### 3.1 Core Layer

#### Token (include/calc/core/token.h)
```cpp
enum class TokenType {
    NUMBER,
    OPERATOR,
    FUNCTION,
    LPAREN,
    RPAREN,
    COMMA,
    EOF
};

struct Token {
    TokenType type;
    std::string value;
    size_t position;
};
```

#### Tokenizer (include/calc/core/tokenizer.h)
```cpp
class Tokenizer {
public:
    explicit Tokenizer(const std::string& input);
    std::vector<Token> tokenize();
private:
    std::string input_;
    size_t pos_;
};
```

#### ASTNode (include/calc/core/ast.h)
```cpp
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::unique_ptr<ASTNode> clone() const = 0;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// 具体节点类型:
// - LiteralNode: 字面量节点
// - BinaryOpNode: 二元操作节点
// - UnaryOpNode: 一元操作节点
// - FunctionCallNode: 函数调用节点
```

#### Parser (include/calc/core/parser.h)
```cpp
class Parser {
public:
    virtual ~Parser() = default;
    virtual std::unique_ptr<ASTNode> parse(const std::vector<Token>& tokens) = 0;
};

class ShuntingYardParser : public Parser {
    // 实现调度场算法
};

class RecursiveDescentParser : public Parser {
    // 实现递归下降解析
};
```

#### Evaluator (include/calc/core/evaluator.h)
```cpp
class Evaluator {
public:
    virtual ~Evaluator() = default;
    virtual Result evaluate(const ASTNode* node) = 0;
};

struct Result {
    double value;
    bool success;
    std::string error_message;
};
```

### 3.2 Application Layer

#### Mode (include/calc/modes/mode.h)
```cpp
class Mode {
public:
    virtual ~Mode() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual Result evaluate(const std::string& expression) = 0;
};
```

#### StandardMode
- 支持操作符: +, -, *, /, ^(乘方)
- 支持括号
- 运算符优先级: ^(右结合) > *,/ > +,-

#### ScientificMode
- 继承 StandardMode
- 新增函数: sin, cos, tan, asin, acos, atan, sinh, cosh, tanh
- 新增函数: log, log10, exp, sqrt, cbrt
- 新增函数: abs, floor, ceil, round, trunc
- 常量: PI, E

#### ProgrammerMode
- 进制前缀: 0b(二进制), 0(八进制), 0x(十六进制)
- 位运算: &, |, ^, ~, <<, >>
- 进制转换功能

### 3.3 Foundation Layer

#### Converter (include/calc/math/converter.h)
```cpp
class Converter {
public:
    static std::string decimalToBinary(long long value);
    static std::string decimalToHex(long long value);
    static std::string decimalToOctal(long long value);
    static long long binaryToDecimal(const std::string& binary);
    static long long hexToDecimal(const std::string& hex);
};
```

#### Error Handling (include/calc/utils/error.h)
```cpp
enum class ErrorCode {
    INVALID_SYNTAX,
    UNEXPECTED_TOKEN,
    DIVISION_BY_ZERO,
    INVALID_FUNCTION,
    DOMAIN_ERROR,
    OVERFLOW,
    UNKNOWN_ERROR
};

class CalculatorException : public std::runtime_error {
public:
    CalculatorException(ErrorCode code, const std::string& message);
    ErrorCode getErrorCode() const;
private:
    ErrorCode code_;
};
```

## 四、CMake 构建系统设计

### 4.1 主 CMakeLists.txt 结构

```cmake
cmake_minimum_required(VERSION 3.15)
project(calc VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
include(cmake/CompileOptions.cmake)

# 查找依赖
find_package(GTest REQUIRED)

# 子目录
add_subdirectory(src)
add_subdirectory(tests)

# 安装配置
install(TARGETS calc_cli DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/calc)
```

### 4.2 跨平台支持

```cmake
# cmake/CompileOptions.cmake
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
```

```cmake
# cmake/Toolchain-mingw-w64-x86_64.cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
```

## 五、阶段实施计划

### 阶段一: 基础框架搭建 (Week 1-2)

**目标**: 建立项目骨架和构建系统

| 任务 | 描述 | 产出 |
|------|------|------|
| 1.1 | 创建目录结构 | 完整的项目目录 |
| 1.2 | 配置 CMake 构建 | 可编译的空项目 |
| 1.3 | 集成 Google Test | 测试框架就绪 |
| 1.4 | 实现基础错误处理 | error.h/cpp |
| 1.5 | 编写 CI/CD 配置 | GitHub Actions |

### 阶段二: 核心解析引擎 (Week 3-4)

**目标**: 实现 Tokenizer 和 Parser

| 任务 | 描述 | 产出 |
|------|------|------|
| 2.1 | 实现 Token 类和 Tokenizer | 可将字符串转为 Token 流 |
| 2.2 | 设计 AST 节点类层次 | 抽象语法树基类 |
| 2.3 | 实现调度场算法解析器 | 中缀转后缀表达式 |
| 2.4 | 实现 AST 构建逻辑 | 完整的表达式树 |
| 2.5 | 编写单元测试 | 90%+ 代码覆盖率 |

### 阶段三: 求值引擎 (Week 5)

**目标**: 实现表达式求值

| 任务 | 描述 | 产出 |
|------|------|------|
| 3.1 | 实现 Evaluator 接口 | 求值器基类 |
| 3.2 | 实现 Visitor 模式求值 | 遍历 AST 计算结果 |
| 3.3 | 实现运算符优先级 | 正确的运算顺序 |
| 3.4 | 错误处理集成 | 完整的错误报告 |
| 3.5 | 编写集成测试 | 端到端测试 |

### 阶段四: 标准模式实现 (Week 6)

**目标**: 完成基本四则运算

| 任务 | 描述 | 产出 |
|------|------|------|
| 4.1 | 实现 StandardMode 类 | 基础计算模式 |
| 4.2 | 支持括号分组 | 表达式分组 |
| 4.3 | 支持负数 | 一元负号 |
| 4.4 | 添加幂运算 ^ | 右结合运算符 |
| 4.5 | CLI 基本界面 | 命令行交互 |

### 阶段五: 科学计算模式 (Week 7-8)

**目标**: 实现数学函数支持

| 任务 | 描述 | 产出 |
|------|------|------|
| 5.1 | 实现 ScientificMode 类 | 科学计算模式 |
| 5.2 | 实现三角函数 | sin/cos/tan 及反函数 |
| 5.3 | 实现对数和指数函数 | log/exp/sqrt |
| 5.4 | 添加数学常量 | PI, E |
| 5.5 | 实现嵌套函数调用 | 多层函数嵌套 |

### 阶段六: 程序员模式 (Week 9)

**目标**: 实现进制转换和位运算

| 任务 | 描述 | 产出 |
|------|------|------|
| 6.1 | 实现 Converter 工具类 | 进制转换 |
| 6.2 | 支持 0b/0x/0 前缀 | 不同进制输入 |
| 6.3 | 实现位运算 | &, \|, ^, ~, <<, >> |
| 6.4 | 实现 ProgrammerMode | 程序员计算模式 |
| 6.5 | 添加位运算测试 | 测试覆盖 |

### 阶段七: CLI 界面完善 (Week 10)

**目标**: 完善命令行用户体验

| 任务 | 描述 | 产出 |
|------|------|------|
| 7.1 | 实现命令行参数解析 | --mode, --help, --version |
| 7.2 | 添加交互式 REPL 模式 | 持续输入计算 |
| 7.3 | 实现输出格式化 | 精度控制、进制显示 |
| 7.4 | 添加历史记录 | 计算历史回溯 |
| 7.5 | 颜色输出支持 | 高亮语法 |

### 阶段八: 测试与质量保证 (Week 11)

**目标**: 全面测试和代码质量

| 任务 | 描述 | 产出 |
|------|------|------|
| 8.1 | 单元测试完善 | 90%+ 覆盖率 |
| 8.2 | 集成测试 | 端到端场景测试 |
| 8.3 | 性能测试 | 表达式解析基准 |
| 8.4 | 代码格式化 | clang-format 配置 |
| 8.5 | 静态分析 | cppcheck/clang-tidy |

### 阶段九: 文档与部署 (Week 12)

**目标**: 完善文档和打包发布

| 任务 | 描述 | 产出 |
|------|------|------|
| 9.1 | API 文档生成 | Doxygen 配置 |
| 9.2 | 用户指南编写 | 使用说明 |
| 9.3 | 打包脚本 | 各平台安装包 |
| 9.4 | 发布准备 | 版本发布 |
| 9.5 | 架构文档更新 | 设计文档 |

### 阶段十: Qt 扩展准备 (Week 13-14)

**目标**: 为 Qt GUI 集成做架构准备

| 任务 | 描述 | 产出 |
|------|------|------|
| 10.1 | 分离 UI 层逻辑 | 核心引擎独立 |
| 10.2 | 设计 Qt 接口层 | 抽象 UI 接口 |
| 10.3 | 准备 Qt CMake 模块 | Qt 集成配置 |
| 10.4 | 设计信号/槽接口 | 模式事件系统 |
| 10.5 | 编写 UI 集成指南 | Qt 扩展文档 |

## 六、扩展性与未来规划

### 6.1 短期扩展 (3-6个月)
- 添加变量存储和引用
- 支持自定义函数定义
- 添加单位转换功能
- 支持复数运算

### 6.2 中期扩展 (6-12个月)
- Qt GUI 界面实现
- 插件系统架构
- 脚本语言支持 (Lua/Python)
- 表达式历史持久化

### 6.3 长期规划 (12+个月)
- 云端计算同步
- 机器学习辅助计算
- 图形化函数绘制
- 协作计算功能

## 七、质量标准

| 指标 | 目标 |
|------|------|
| 单元测试覆盖率 | > 90% |
| 编译警告 | 0 |
| 代码风格检查 | 通过 clang-format |
| 内存泄漏 | 0 (Valgrind 验证) |
| API 稳定性 | 语义化版本控制 |
| 跨平台兼容性 | Windows/macOS/Linux |
