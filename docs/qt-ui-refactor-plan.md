# Qt 计算器 UI 重构实施方案

> 将 Qt 计算器 UI 界面重构为玻璃拟态（Glassmorphism）设计风格，参考 `examples/js_example` 的 React/TypeScript 实现

---

## 一、任务概述

### 目标
将现有的扁平风格 Qt 计算器 UI 重构为现代化玻璃拟态设计，匹配 React 示例的视觉风格和交互体验。

### 当前状态
- Qt 6 Widgets，扁平设计风格
- 蓝色主色调 (#4a90e2)
- 分割器布局（左侧键盘 + 右侧历史面板）
- 单选按钮模式选择器

### 目标设计
- 玻璃拟态（Glassmorphism）效果
- 渐变背景：蓝-紫-粉渐变
- 胶囊形状模式选择器
- 悬浮历史按钮 + 徽章
- 全屏历史模态框
- 居中卡片布局
- 悬停缩放动画

---

## 二、设计参考分析（基于 examples/js_example）

### 2.1 整体布局结构

```
┌─────────────────────────────────────────────┐
│  模式标签 + 历史按钮                   │
│  ┌────────────────────┐                  │
│  │ 🧮 标准  🧪 科学 💻 程序 │  │
│  └────────────────────┘                  │
├─────────────────────────────────────────────┤
│  表达式预览 (小字体)                   │
│  主结果显示 (大字体，右对齐)             │
├─────────────────────────────────────────────┤
│                                         │
│           计算器按钮网格                   │
│                                         │
│                                         │
└─────────────────────────────────────────────┘
└─────────────────────────────────────────────┘
```

### 2.2 样式主题定义

| 元素 | 浅色模式 | 深色模式 | 用途 |
|------|-----------|----------|------|
| 背景渐变 | `#E0E7FF → #F3E8FF → #FFE4E6` | 应用主背景 |
| 玻璃卡片 | `rgba(255,255,255,0.8)` + `border: 1px solid rgba(255,255,255,0.5)` + `border-radius: 24px` | 主容器 |
| 等号按钮 | `#2563EB → #4F46E5` | 主要操作 |
| 运算符按钮 | `#F97316 → #EC4899` | +, -, ×, ÷ |
| 数字按钮 | `#FFFFFF` + `border: 1px solid rgba(0,0,0,0.08)` | 0-9 |
| 灰色功能按钮 | `#E5E7EB → #D1D5DB` | %, CE, C, DEL |
| 科学函数按钮 | `#F3E8FF → #E0E7FF` | sin, cos, tan 等 |
| 程序员按钮 | `#CCFBF1 → #A5F3FC` | OR, XOR, AND 等 |
| 表达式预览 | `#6B7280` (text-sm) | 14px, 等宽字体 |
| 主显示 | `#111827 → #374151` | 48px, 黑体 |
| 胶囊标签 | `#F3F4F4` + 圆角 9999px | 模式选择 |
| 悬浮历史按钮 | 透明背景 + 悬停缩放 | 48x48 圆形 |

### 2.3 按钮网格布局

| 模式 | 列数 | 行数 | 按钮总数 | 按钮高度 |
|------|------|------|----------|---------|
| Standard | 4 | 6 | 24 | h-64 |
| Scientific | 7 | 5 | 35 | h-56 |
| Programmer | 6 | 6 | 36 | h-56 |

### 2.4 动画效果

- **悬停缩放**: 1.05x (数字按钮、运算符)
- **悬停放大**: 1.10x (历史记录项)
- **淡入淡出**: 200ms 模态框切换
- **阴影过渡**: hover 时阴影增强

---

## 三、技术实现方案

### 3.1 技术栈

- **Qt 6 Widgets** - 保持现有的 Qt Widgets 框架
- **QSS (Qt Style Sheets)** - 用于实现所有视觉效果
- **QPropertyAnimation** - 用于缩放和淡入淡出动画
- **QGraphicsOpacityEffect** - 用于透明度控制

### 3.2 架构决策

1. **保持现有架构**: 不改变核心架构（Controller、信号/槽机制）
2. **渐进式更新**: 先完成 QSS 样式，再验证效果
3. **组件复用**: 新组件独立实现，不影响现有逻辑

### 3.3 实现难点与解决方案

| 难点 | 解决方案 |
|--------|----------|
| 玻璃拟态效果 | 使用 `QGraphicsOpacityEffect` + 半透明背景，QSS 控制样式 |
| 悬停缩放动画 | `QPropertyAnimation` 操作 `geometry` 属性，实现 1.05x 缩放 |
| 历史模态框 | 全屏覆盖层 + `QPropertyAnimation` 控制透明度淡入淡出 |
| 圆角按钮 | QSS `border-radius: 9999px` 实现完美圆角 |

---

## 四、分步实施计划

### 第 1 步：准备阶段

#### 1.1 创建新组件头文件
- [ ] `src/ui/qt/widgets/history_button.h`
- [ ] `src/ui/qt/widgets/history_modal.h`

#### 1.2 创建新组件源文件
- [ ] `src/ui/qt/widgets/history_button.cpp`
- [ ] `src/ui/qt/widgets/history_modal.cpp`

#### 1.3 更新 CMakeLists.txt
- [ ] 添加新源文件到 `WIDGET_SOURCES`
- [ ] 添加新头文件到 `WIDGET_HEADERS`

---

### 第 2 步：样式表更新

#### 2.1 重写 default.qss
**主要变更**:
- 移除所有旧的内联样式定义
- 添加主窗口渐变背景
- 添加玻璃卡片样式
- 定义按钮样式类型（calcButton, operator, functionGray, functionPurple, functionTeal, baseMode）
- 定义模态框样式（historyOverlay, historyHeader, historyItem）
- 定义标签样式（expressionLabel, resultLabel, modeTab）

#### 2.2 样式文件结构

```
/* ============================================================================
   主窗口 - 渐变背景
   ============================================================================ */
QMainWindow {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                               stop:0 #E0E7FF,
                               stop:0.5 #F3E8FF,
                               stop:1 #FFE4E6);
}

/* ============================================================================
   玻璃卡片容器
   ============================================================================ */
QWidget#glassCard {
    background-color: rgba(255, 255, 255, 0.8);
    border: 1px solid rgba(255, 255, 255, 0.5);
    border-radius: 24px;
}

QWidget#headerBar {
    background-color: rgba(255, 255, 255, 0.5);
    border-bottom: 1px solid rgba(227, 232, 238, 0.5);
    border-top-left-radius: 24px;
    border-top-right-radius: 24px;
}

/* ============================================================================
   胶囊形状模式标签
   ============================================================================ */
QPushButton[buttonType="modeTab"] {
    background-color: rgba(243, 244, 246, 0.8);
    color: #374151;
    border: none;
    border-radius: 9999px;
    padding: 10px 20px;
    font-size: 14px;
    font-weight: 500;
    font-family: -apple-system, BlinkMacSystemFont, "SF Pro Text", sans-serif;
}

QPushButton[buttonType="modeTab"]:hover {
    background-color: rgba(229, 231, 235, 0.9);
}

QPushButton[buttonType="modeTab"][active="true"] {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:0,
                              stop:0 #2563EB,
                              stop:1 #4F46E5);
    color: white;
}

/* ============================================================================
   悬浮历史按钮
   ============================================================================ */
QPushButton#historyButton {
    background: transparent;
    border: none;
    padding: 12px;
    border-radius: 9999px;
    min-width: 48px;
    min-height: 48px;
}

QPushButton#historyButton:hover {
    background-color: rgba(243, 244, 246, 0.8);
}

QLabel#historyBadge {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:0,
                              stop:0 #2563EB,
                              stop:1 #4F46E5);
    color: white;
    border-radius: 9999px;
    min-width: 20px;
    min-height: 20px;
    padding: 2px 6px;
    font-size: 10px;
    font-weight: bold;
}

/* ============================================================================
   显示区域
   ============================================================================ */
QWidget#displayArea {
    background: qlineargradient(x1:0 y1:0 x2:0 y2:1,
                               stop:0 #F9FAFB,
                               stop:1 #F3F4F6);
}

QLabel#expressionLabel {
    color: #6B7280;
    font-size: 14px;
    font-family: "SF Mono", "Monaco", "Consolas", monospace;
    background: transparent;
    border: none;
}

QLabel#resultLabel {
    color: #1F2937;
    font-size: 48px;
    font-weight: 700;
    background: transparent;
    border: none;
}

/* ============================================================================
   计算器按钮 - 通用
   ============================================================================ */
QPushButton[buttonType="calcButton"] {
    background-color: #FFFFFF;
    border: 1px solid rgba(0, 0, 0, 0.08);
    border-radius: 16px;
    color: #1F2937;
    font-size: 18px;
    font-weight: 600;
    min-height: 64px;
}

QPushButton[buttonType="calcButton"]:hover {
    background-color: #F9FAFB;
    border-color: rgba(0, 0, 0, 0.2);
}

/* ============================================================================
   等号按钮 - 蓝色渐变
   ============================================================================ */
QPushButton#equalButton {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #2563EB,
                              stop:1 #4F46E5);
    color: white;
    border: none;
    border-radius: 16px;
    font-size: 18px;
    font-weight: 600;
    box-shadow: 0 4px 8px rgba(37, 99, 235, 0.3);
}

QPushButton#equalButton:hover {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #1D4ED8,
                              stop:1 #4338CA);
    box-shadow: 0 6px 12px rgba(37, 99, 235, 0.4);
}

/* ============================================================================
   运算符按钮 - 橙粉渐变
   ============================================================================ */
QPushButton[buttonType="operator"] {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #F97316,
                              stop:1 #EC4899);
    color: white;
    border: none;
    border-radius: 16px;
    font-size: 18px;
    font-weight: 600;
    box-shadow: 0 4px 8px rgba(249, 115, 22, 0.3);
}

QPushButton[buttonType="operator"]:hover {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #EA580C,
                              stop:1 #DB2777);
    box-shadow: 0 4px 8px rgba(234, 88, 12, 0.3);
}

/* ============================================================================
   功能按钮 - 灰色渐变 (标准/科学)
   ============================================================================ */
QPushButton[buttonType="functionGray"] {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #E5E7EB,
                              stop:1 #D1D5DB);
    color: #374151;
    border: none;
    border-radius: 16px;
    font-size: 16px;
}

QPushButton[buttonType="functionGray"]:hover {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #D1D5DB,
                              stop:1 #9CA3AF);
}

/* ============================================================================
   科学函数按钮 - 紫色渐变
   ============================================================================ */
QPushButton[buttonType="functionPurple"] {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #F3E8FF,
                              stop:1 #E0E7FF);
    color: #4C1D95;
    border: none;
    border-radius: 16px;
    font-size: 14px;
}

QPushButton[buttonType="functionPurple"]:hover {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #E9D5FF,
                              stop:1 #C7D2FE);
}

/* ============================================================================
   程序员按钮 - 青色渐变
   ============================================================================ */
QPushButton[buttonType="functionTeal"] {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #CCFBF1,
                              stop:1 #A5F3FC);
    color: #0F766E;
    border: none;
    border-radius: 16px;
    font-size: 14px;
}

QPushButton[buttonType="functionTeal"]:hover {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #99F6E4,
                              stop:1 #A5F3FC);
}

/* ============================================================================
   进制选择按钮
   ============================================================================ */
QPushButton[buttonType="baseMode"] {
    background-color: rgba(229, 231, 235, 0.8);
    border: none;
    border-radius: 16px;
    font-weight: bold;
    color: #374151;
    font-size: 14px;
}

QPushButton[buttonType="baseMode"][active="true"] {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:0,
                              stop:0 #2563EB,
                              stop:1 #4F46E5);
    color: white;
}

QPushButton[buttonType="baseMode"]:hover {
    background-color: rgba(209, 213, 219, 0.9);
}

QPushButton[buttonType="baseMode"]:disabled {
    opacity: 0.5;
    background-color: #E5E7EB;
}

/* ============================================================================
   历史模态框
   ============================================================================ */
QWidget#historyOverlay {
    background-color: rgba(255, 255, 255, 0.95);
    border-radius: 24px;
}

QWidget#historyHeader {
    background-color: rgba(255, 255, 255, 0.5);
    border-bottom: 1px solid rgba(227, 232, 238, 0.5);
}

QLabel#historyTitle {
    color: #1F2937;
    font-size: 24px;
    font-weight: 700;
    background: transparent;
}

QPushButton[buttonType="historyItem"] {
    background-color: white;
    border: 1px solid rgba(229, 231, 235, 0.5);
    border-radius: 16px;
    padding: 16px 20px;
    text-align: left;
    min-height: 80px;
}

QPushButton[buttonType="historyItem"]:hover {
    background: qlineargradient(x1:0 y1:0 x2:1 y2:1,
                              stop:0 #EFF6FF,
                              stop:1 #E0E7FF);
}

QLabel#historyEmptyState {
    color: #9CA3AF;
    font-size: 14px;
}

/* ============================================================================
   传统组件样式保留
   ============================================================================ */
/* 保持菜单栏、状态栏、对话框的基本样式，适配新主题 */
QMenuBar {
    background-color: rgba(255, 255, 255, 0.8);
    border-bottom: 1px solid rgba(227, 232, 238, 0.5);
}

QStatusBar {
    background-color: rgba(255, 255, 255, 0.8);
    border-top: 1px solid rgba(227, 232, 238, 0.5);
}

/* 隐藏 Splitter (不再使用) */
QSplitter {
    display: none;
}
```

---

### 第 3 步：新组件实现

#### 3.1 HistoryButton 组件
**功能**：悬浮历史按钮 + 徽章计数
**实现要点**：
- QWidget 容器 + QPushButton 按钮
- 绝对定位的 QLabel 徽章
- 悬停时背景色变化
- setCount() 方法更新徽章

**文件**：
- `src/ui/qt/widgets/history_button.h`
- `src/ui/qt/widgets/history_button.cpp`

#### 3.2 HistoryModal 组件
**功能**：全屏历史记录模态框
**实现要点**：
- 覆盖层设计 (半透明背景 + 圆角)
- 淡入淡出动画
- 点击外部区域关闭
- ESC 键关闭
- 空状态显示

**文件**：
- `src/ui/qt/widgets/history_modal.h`
- `src/ui/qt/widgets/history_modal.cpp`

---

### 第 4 步：ModeSelector 改造

#### 4.1 移除 QRadioButton，改用 QPushButton
**变更**：
- 布局从横向单选按钮改为胶囊形状标签
- 添加图标：标准(🧮)、科学(🧪)、程序员(💻)
- 蓝色渐变激活状态

**文件**：
- `include/calc/ui/qt/widgets/mode_selector.h`
- `src/ui/qt/widgets/mode_selector.cpp`

---

### 第 5 步：MainWindow 布局重构

#### 5.1 移除 Splitter 分割器布局
**变更**：
- 从水平分割器改为垂直居中布局
- 添加玻璃卡片容器
- 添加头部栏（模式标签 + 历史按钮）

#### 5.2 新增组件集成
- HistoryButton 添加到头部栏右侧
- HistoryModal 作为子窗口创建

**文件**：
- `include/calc/ui/qt/widgets/main_window.h`
- `src/ui/qt/widgets/main_window.cpp`

**新布局结构**：
```
MainWindow (QMainWindow)
└── QWidget#centralWidget
    └── QVBoxLayout
        ├── stretch (上)
        ├── QWidget#glassCard (固定最大宽度 600px)
        │   ├── QWidget#headerBar
        │   │   ├── ModeSelector (左侧)
        │   │   └── HistoryButton (右侧)
        │   ├── QWidget#displayArea
        │   │   ├── QLabel#expressionLabel (顶部，小字体)
        │   │   └── QLabel#resultLabel (底部，大字体)
        └── QWidget#buttonContainer
            └── QStackedWidget
                ├── KeypadWidget (标准模式)
                ├── FunctionPanel (科学模式)
                └── ConverterPanel (程序员模式)
    └── HistoryModal (隐藏的模态框)
        └── stretch (下)
```

---

### 第 6 步：DisplayWidget 更新

#### 6.1 调整显示顺序
**变更**：
- 表达式预览移到顶部 (小字体 14px)
- 主结果显示在底部 (大字体 48px)
- 移除 QLineEdit 改用 QLabel (只读)
- 隐藏状态标签（保持干净玻璃拟态）

**文件**：
- `include/calc/ui/qt/widgets/display_widget.h`
- `src/ui/qt/widgets/display_widget.cpp`

---

### 第 7 步：KeypadWidget 动画效果

#### 7.1 添加 eventFilter
**实现要点**：
- 拦截 QEnter/QLeave 事件
- 使用 QPropertyAnimation 实现 1.05x 缩放
- 200ms 持续时间，OutCubic 缓动

**文件**：
- `include/calc/ui/qt/widgets/keypad_widget.h`
- `src/ui/qt/widgets/keypad_widget.cpp`

---

### 第 8 步：FunctionPanel 样式更新

#### 8.1 更新按钮类型属性
**变更**：
- 设置 `buttonType="functionPurple"` 用于科学函数按钮
- 调整按钮大小为 50x60
- 更新字体大小为 14px

**文件**：
- `src/ui/qt/widgets/function_panel.cpp`

---

### 第 9 步：ConverterPanel 样式更新

#### 9.1 移除内联样式
**变更**：
- 移除 setStyleSheet() 调用
- 依赖 QSS 样式
- 保持现有输入验证功能

**文件**：
- `src/ui/qt/widgets/converter_panel.cpp`

---

## 五、文件变更清单

### 5.1 新建文件（4个）

| 文件路径 | 用途 |
|---------|------|
| `include/calc/ui/qt/widgets/history_button.h` | 悬浮历史按钮头文件 |
| `src/ui/qt/widgets/history_button.cpp` | 悬浮历史按钮实现 |
| `include/calc/ui/qt/widgets/history_modal.h` | 历史模态框头文件 |
| `src/ui/qt/widgets/history_modal.cpp` | 历史模态框实现 |

### 5.2 修改文件（9个）

| 文件路径 | 主要变更 |
|---------|---------|
| `src/ui/qt/resources/default.qss` | 完全重写样式表，添加玻璃拟态效果和渐变色 |
| `include/calc/ui/qt/widgets/mode_selector.h` | 改用 QPushButton，胶囊标签 |
| `src/ui/qt/widgets/mode_selector.cpp | 实现胶囊标签 + 图标 |
| `include/calc/ui/qt/widgets/main_window.h` | 添加玻璃卡片、头部栏、历史模态框成员 |
| `src/ui/qt/widgets/main_window.cpp | 重写布局，集成新组件 |
| `include/calc/ui/qt/widgets/display_widget.h` | 移除 QLineEdit，改用 QLabel，调整显示顺序 |
| `src/ui/qt/widgets/display_widget.cpp | 实现新显示逻辑 |
| `include/calc/ui/qt/widgets/keypad_widget.h` | 添加 eventFilter 声明 |
| `src/ui/qt/widgets/keypad_widget.cpp` | 实现悬停缩放动画 |
| `src/ui/qt/widgets/function_panel.cpp` | 更新按钮样式属性 |
| `src/ui/qt/CMakeLists.txt` | 添加新源文件和头文件 |

---

## 六、验证方法

### 6.1 编译验证
```bash
# 清理构建目录
rm -rf build

# 配置并编译
cmake -B build -S .
cmake --build build --target calc_qt_gui --config Debug

# 运行可执行文件 (macOS)
./build/Debug/calc_qt_gui.app/Contents/MacOS/calc_qt_gui
```

### 6.2 功能验证清单

| 功能 | 验证点 |
|------|--------|
| [ ] 主窗口显示 | 居中卡片，渐变背景 |
| [ ] 模式切换 | 胶囊标签，三模式切换正常 |
| [ ] 基础计算 | 标准模式计算功能正常 |
| [ ] 科学函数 | 三角函数计算功能正常 |
| [ ] 程序员模式 | 进制转换功能正常 |
| [ ] 显示效果 | 表达式预览 + 主结果显示 |
| [ ] 历史记录 | 悬浮按钮显示，模态框正常 |
| [ ] 悬停动画 | 按钮 1.05 倍缩放 |
| [ ] 按钮样式 | 等号蓝色渐变，运算符橙粉渐变 |

### 6.3 边界情况检查

| 平台 | 预期行为 |
|------|---------|
| macOS | 玻璃拟态通过半透明背景实现 |
| Windows | 可能需要启用 DWM 模糊效果（非必需） |
| Linux | 依赖窗口管理器 |

---

## 七、风险与注意事项

### 7.1 技术风险

| 风险 | 说明 | 缓解措施 |
|------|------|---------|
| QPropertyAnimation 性能 | 矩时间动画可能影响低端设备性能 | 添加开关控制（高配设备）|
| 复杂 QSS 样式 | 分层管理，避免内联样式污染 |
| 模态框内存 | 多层叠加可能导致内存泄漏 | 确保 deleteLater 正确调用 |

### 7.2 兼容性考虑

| 特性 | 说明 |
|------|---------|
| 主题切换 | 当前仅实现浅色主题，深色模式可后续扩展 |
| 字体适配 | 使用系统字体栈，确保多平台显示一致 |
| 国际化 | 保持现有中文界面，图标使用 Emoji |

### 7.3 测试建议

1. 在不同屏幕尺寸下测试布局
2. 验证所有按钮的悬停和点击效果
3. 测试历史模态框的打开和关闭
4. 边界情况下测试计算器功能

---

## 八、预计工作量

| 阶段 | 预计工时 |
|------|----------|
| QSS 样式重写 | 2-3 小时 |
| 新组件开发 | 4-6 小时 |
| 主窗口重构 | 2-3 小时 |
| 组件集成与调试 | 2-3 小时 |
| 测试与验证 | 1-2 小时 |
| **总计** | **约 12-18 小时** |

---

## 九、后续扩展方向

完成玻璃拟态重构后，可以考虑以下扩展：

1. **深色模式** - 添加暗色主题支持
2. **主题自定义** - 允许用户选择配色方案
3. **动画配置** - 可配置动画时长和缓动函数
4. **窗口特效** - 添加窗口淡入/淡出动画
5. **手势支持** - 添加滑动删除历史记录

---

**文档版本**: v1.0
**创建日期**: 2025-01-20
**参考来源**: `examples/js_example`
