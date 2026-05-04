# Box

一个基于 Unreal Engine 5.5 的推箱子解谜游戏。玩家需要推动箱子构造平台来通过关卡。

---

## 如何开始

### 环境要求

- **Unreal Engine 5.5**
- **Visual Studio 2022**（编译 C++ 代码）
- **Windows 10/11**

### 打开项目

1. 确保已安装 **Epic Games Launcher** 和 **Unreal Engine 5.5**。
2. 在项目根目录找到 `Box.uproject`，右键选择 **Generate Visual Studio project files** 生成 `.sln` 文件。
3. 双击 `Box.uproject` 即可用 Unreal Editor 打开项目。

### 从 Level1 开始游玩

1. 在 Unreal Editor 中，点击顶部菜单 **文件 → 打开关卡**，或直接按 `Ctrl+O`。
2. 导航到 `Content/Level/Level1.umap` 并打开。
3. 点击工具栏上的 **运行**（▶ 按钮）即可开始游戏，或按 `Alt+P` 直接在编辑器中运行。

💡 **建议佩戴耳机游玩！** 游戏中的音效是重要的反馈信息，耳机能提供更好的沉浸式体验。

---

## 项目结构

```
Box/
├── Source/Box/
│   ├── BoxCharacter.cpp/h    # 玩家角色（移动、推箱子状态机）
│   ├── MyBox.cpp/h           # 可推动的箱子 Actor
│   ├── KillZone.cpp/h        # 即死区域
│   ├── BoxHUD.cpp/h          # HUD 管理
│   ├── BoxHUDWidget.cpp/h    # HUD UI 组件
│   └── BoxGameMode.cpp/h     # 游戏模式
├── Content/
│   ├── Level/                # 关卡（Level1.umap ~ Level3.umap）
│   ├── Prop/                 # 蓝图 Actor（箱子、检查点等）
│   └── Sound/                # 音效资源
└── Box.uproject
```

## 推箱子（Push Box）实现说明

推箱子是游戏的核心玩法，由三个类协作完成：

| 组件 | 文件 | 职责 |
|------|------|------|
| `ABoxCharacter` | [BoxCharacter.h](Source/Box/BoxCharacter.h) / [BoxCharacter.cpp](Source/Box/BoxCharacter.cpp) | 玩家角色，管理输入、碰撞检测和推进状态机 |
| `AMyBox` | [MyBox.h](Source/Box/MyBox.h) / [MyBox.cpp](Source/Box/MyBox.cpp) | 可推动的箱子，管理移动和玩家附着 |
| `EPushState` 枚举 | `BoxCharacter.h` | 三段式状态：`None` → `ReadyToPush` → `Pushing` |

### 整体流程

```
玩家靠近箱子 → NotifyHit() 触发
    ↓
蛇形吸附 (SnapToPushPosition) → 玩家旋转面向箱子
    ↓
PushState = ReadyToPush → 等待玩家方向输入
    ↓
HandlePushInput() → 解析输入方向 → GetPushDirection() 校验
    ↓
Box.CanMoveTo() → Sweep 检测目标位置是否可移动
    ↓
Box.BeginMove() → 玩家 Attach 到箱子 → 箱子 Lerp 移动
    ↓
PushState = Pushing → 移动中屏蔽玩家输入
    ↓
Box.Tick() → Lerp 到达目标 → DetachPlayer() → OnBoxMoveFinished 广播
    ↓
PushState = ReadyToPush → 可继续推
```

### 碰撞检测 [NotifyHit](Source/Box/BoxCharacter.cpp#L40-L76)

当玩家角色与另一个 Actor 发生碰撞时触发：

1. **过滤箱子**：通过 `Cast<AMyBox>(Other)` 判断碰撞对象是否为可推动的箱子。
2. **过滤空中碰撞**：玩家在下落中不能推箱子。
3. **过滤顶面碰撞**：当 `HitNormal.Z > 0.5f`（即玩家落在箱子顶部）时，不触发推箱子。
4. **计算吸附方向**：基于玩家与箱子的相对位置，选择 X 轴或 Y 轴中偏移更大的方向作为推的方向，将玩家吸附到箱子的对应侧面。

### 蛇形吸附 [SnapToPushPosition](Source/Box/BoxCharacter.cpp#L247-L267)

- 在非推状态下，计算箱子侧面到玩家的精确位置。
- 将玩家沿着推的方向推到离箱子 `SnapDistance`（默认 25 单位）处。
- 同时将玩家沿垂直方向夹紧，使角色对齐到箱子表面的中心区域。
- 设置玩家朝向为面对箱子。

### 方向解析 [GetPushDirection](Source/Box/BoxCharacter.cpp#L269-L301)

- 将输入的二维向量投影到世界坐标系（考虑相机朝向）。
- 只在四个主方向（±X, ±Y）中选择，优先选择绝对值更大的分量。
- 确保输入方向与玩家-箱子方向点积为正（即玩家在推箱子而非拉箱子）。
- 防止侧面偏移输入被误判（侧向偏移大于正向偏移时返回零向量）。

### 箱子移动 [AMyBox::BeginMove](Source/Box/MyBox.cpp#L73-L87)

- 将玩家 `AttachToActor` 到箱子上，禁用角色移动组件（设为 `MOVE_None`）。
- 播放推箱子动画（`PushMontage`）和音效（`PushSound`）。
- 使用 `FMath::Lerp` 在 `MoveDuration`（默认 0.5 秒）内从起始位置移动到目标位置。
- 移动采用 **线性插值**，加速和减速均由 Alpha 值 0→1 线性控制。

### 可移动性检测 [AMyBox::CanMoveTo](Source/Box/MyBox.cpp#L59-L71)

使用 `SweepSingleByChannel` 进行扫描检测，确保目标位置没有障碍物。碰撞形状为箱子范围 × 0.9 以防止与邻居箱子的边缘碰撞。

---
