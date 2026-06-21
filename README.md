# Facial Recognition Login System

> 基于 Qt 6 + C++17 的人脸识别与密码双模式登录系统，零外部依赖，纯 Qt 原生实现。

[![Qt](https://img.shields.io/badge/Qt-6.5.3-green)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C++-17-blue)](https://en.cppreference.com/w/cpp/17)
[![MSVC](https://img.shields.io/badge/MSVC-2019%2F2022-purple)](https://visualstudio.microsoft.com/)
[![Platform](https://img.shields.io/badge/platform-Windows%2010%2F11-lightgrey)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/license-MIT-yellow)](LICENSE)

---

## 功能特性

| 功能 | 说明 |
|------|------|
| 🔐 **密码登录** | 用户名 + 密码验证，SHA-256 哈希存储 |
| 👤 **人脸识别登录** | 摄像头实时检测 + LBP 特征匹配，非接触式登录 |
| 📝 **账号注册** | 在线采集人脸 + 设置密码，一次性完成注册 |
| 🎨 **暗色主题** | Fusion 风格 + 自定义暗色调色板 |
| 💾 **本地存储** | SQLite 数据库，数据完全本地化 |
| 📷 **实时预览** | 摄像头画面实时显示，人脸检测框可视反馈 |

---

## 界面预览

```
┌──────────────────────────────────────┐
│        Welcome to FaceLogin          │
├──────────────────────────────────────┤
│ [Password Login]  [Face Recognition] │
├──────────────────────────────────────┤
│   Tab 1 - 密码登录:                   │
│   Username: [____________]           │
│   Password: [____________]           │
│   [Login]                            │
├──────────────────────────────────────┤
│   Tab 2 - 人脸登录:                   │
│   ┌──────────────────────────┐       │
│   │    Camera Feed (实时)     │       │
│   │    ┌──────────┐          │       │
│   │    │ 人脸检测框│          │       │
│   │    └──────────┘          │       │
│   └──────────────────────────┘       │
│   [Start Camera] [Recognize Face]    │
├──────────────────────────────────────┤
│   [Register New Account]             │
└──────────────────────────────────────┘
```

---

## 技术架构

```
                        ┌─────────────┐
                        │   main.cpp  │  入口 + 暗色主题
                        └──────┬──────┘
                               │
                        ┌──────▼──────┐
                        │AppController│  登录→主窗口→注销 调度
                        └──────┬──────┘
                               │
              ┌────────────────┼────────────────┐
              │                │                │
     ┌────────▼──────┐ ┌──────▼──────┐ ┌───────▼───────┐
     │  LoginDialog  │ │RegisterDlg  │ │  MainWindow   │
     │  双模式登录    │ │  人脸注册    │ │   主面板       │
     └──────┬────────┘ └──────┬──────┘ └───────────────┘
            │                 │
     ┌──────┴────────┬────────┴──────┐
     │               │               │
┌────▼─────┐  ┌──────▼──────┐  ┌─────▼──────────┐
│FaceDetect│  │FaceRecognize│  │DatabaseManager │
│ 人脸检测  │  │  人脸识别    │  │  SQLite数据库   │
└──────────┘  └─────────────┘  └────────────────┘
```

### 人脸检测算法

采用 **YCbCr 肤色分割 + 形态学处理**，零外部依赖：

1. RGB → YCbCr 色彩空间转换
2. 肤色阈值过滤（Cb: 77~127, Cr: 133~173）
3. 腐蚀(3×3) → 膨胀(5×5) → 腐蚀(3×3) 去噪平滑
4. 最大连通区域包围盒提取
5. 宽高比验证 + 边界扩展

### 人脸识别算法

采用 **LBP (Local Binary Patterns) 直方图** + **卡方距离**：

1. 人脸 ROI 缩放到 100×120 统一尺寸
2. 逐像素计算 8-邻域 LBP 编码（0~255）
3. 生成 256-bin 归一化直方图作为特征向量
4. 卡方距离比对：`χ² = ½Σ(h₁[i]-h₂[i])²/[h₁[i]+h₂[i]]`
5. 阈值 0.35，低于阈值则匹配成功

---

## 项目结构

```
Facial recognition login system/
├── FaceRecognitionLogin.pro    # qmake 工程文件
├── CMakeLists.txt              # CMake 工程文件（备选）
├── .gitignore                  # Git 忽略规则
├── README.md                   # 本文件
│
├── main.cpp                    # 程序入口
├── appcontroller.h/.cpp        # 应用生命周期控制器
│
├── logindialog.h/.cpp          # 登录对话框（密码+人脸双标签页）
├── registerdialog.h/.cpp       # 注册对话框（人脸采集+账号创建）
├── mainwindow.h/.cpp           # 登录成功后的主面板
│
├── facedetector.h/.cpp         # 人脸检测模块（肤色分割）
├── facerecognizer.h/.cpp       # 人脸识别模块（LBP直方图）
├── databasemanager.h/.cpp      # SQLite 数据库管理模块
│
└── _build/                     # 构建输出目录
    └── release/
        └── FaceRecognitionLogin.exe
```

---

## 环境要求

| 组件 | 版本 |
|------|------|
| **Qt** | 6.5.3 (MSVC 2019 64-bit) |
| **编译器** | MSVC 2019/2022 |
| **CMake** | 3.16+（可选） |
| **操作系统** | Windows 10/11 |

---

## 构建方式

### 方式一：Qt Creator（推荐）

1. 打开 Qt Creator
2. `File → Open File or Project...` → 选择 `FaceRecognitionLogin.pro`
3. 选择 **Desktop Qt 6.5.3 MSVC2019 64bit** 套件
4. 点击 `Ctrl+B` 构建，`Ctrl+R` 运行

### 方式二：Visual Studio

```powershell
# 1. 打开 VS 开发者命令行
# 2. 运行：
qmake FaceRecognitionLogin.pro -spec win32-msvc
nmake

# 或使用 jom（并行构建更快）：
jom
```

### 方式三：命令行（CMake + Ninja）

```powershell
cmake -G "Ninja" -DCMAKE_PREFIX_PATH=D:\Qt\6.5.3\msvc2019_64 -B _build .
cmake --build _build --config Release
```

---

## 使用说明

### 1. 注册账号

1. 点击 **"Register New Account"**
2. 填写用户名（≥3字符）和密码（≥6字符）
3. 摄像头自动打开，对准人脸
4. 检测到人脸后点击 **"Capture Face"**
5. 点击 **"Complete Registration"** 完成

### 2. 密码登录

切换到 **Password Login** 标签，输入用户名密码，点击 Login。

### 3. 人脸登录

1. 切换到 **Face Recognition** 标签
2. 点击 **"Start Camera"** 开启摄像头
3. 正对摄像头，等待检测框出现
4. 点击 **"Recognize Face"** 自动匹配登录

### 4. 注销

登录成功后点击 **Logout** 返回登录界面。

---

## 数据库

采用 SQLite 本地存储，文件位于程序同目录 `users.db`：

```sql
CREATE TABLE users (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    username      TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,           -- SHA-256 哈希
    face_data     BLOB                    -- LBP 特征序列化
);
```

- 密码经过 SHA-256 哈希后存储，不保存明文
- 人脸特征序列化为二进制 BLOB 存储

---

## 依赖

| 依赖 | Qt 模块 |
|------|---------|
| 窗口界面 | `QtWidgets` |
| 摄像头采集 | `QtMultimedia` / `QtMultimediaWidgets` |
| 数据库 | `QtSql` (SQLite) |
| 基础库 | `QtCore` / `QtGui` / `QtNetwork` |

> 无需额外安装 OpenCV、dlib 等第三方库，人脸检测和识别均基于纯 C++/Qt 实现。

---

## License

MIT © danshoudasiwusong