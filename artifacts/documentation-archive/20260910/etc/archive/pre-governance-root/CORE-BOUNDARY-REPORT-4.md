# Core 边界研报 4：以 NTDOS64 研究需求复核 ntvdm64 Core/VM 边界

**状态：** 只读研报，2026-08-08。
**范围：** 本文直接阅读 `D:\home\repos.hobby\ntvdm64` 当前 `src/core`、`src/vm`、`src/vdm`、架构合同与 M5 队列；并以本仓库已经整理的 NT4/OpenNT 历史链研究为需求侧证据。本文不修改 `ntvdm64`，也不把任何历史微软接口或二进制视为其产品依赖。

## 结论

**Core/VM 的方向性边界已经清楚，且机械设备层的迁移基本到位；但它还不是一个可被外部 NTDOS wrapper 直接、稳定地使用的 guest-backend 边界。**

更准确地说，当前 `core` 已经可供一个受同一源码版本约束的第二个消费者组装一台中性 x86 机器；`tests/machine/core_mantle_shape_smoke.c` 已经证明了最小的 core-only 组合形状。它尚不具备以下含义：

- 不是已发布的 `core.dll` 或版本化 SDK；
- 不是已实现的 `mantle` runtime；
- 不是已验证可以驱动 NT4 NTDOS 历史 BOP/DEM 链的执行后端；
- 不是让外部项目直接采用 `vm_session`、默认 BIOS 或 VM 私有存储代码的许可。

因此，若 NTDOS64 将来选择 `ntvdm64/core` 作为 guest 后端，正确定位是：**先作为研究性、源级锁定的候选后端；待补齐少数中性执行边界并由 first-party mantle 证明后，才讨论独立库/ABI。** 不能在当前状态把它称作可用的外部后端。

## 本次复核的事实基线

### 已经在 Core 的能力

以下能力已经在 `src/core`，不应为了 NTDOS 再从 VM 复制一份：

| 领域 | 现状 | 关键接口/实现 |
| --- | --- | --- |
| 机器生命周期与有限量执行 | 已具备。调用者控制每个同步 quantum，core 不创建宿主线程或无限循环。 | `core_machine_create/reset/run/request_stop`，`src/core/machine/machine_interface.h` |
| CPU、RAM、端口、PIC、PIT、DMA、KBC、显示 | 已属于 core machine，且 CPU/PIC 生命周期已从 VM 收回。 | `src/core/machine/*`；M5 T295/T296 |
| 物理内存、A20、只读 ROM | 已具备 checked physical read/write/query、A20 及冻结的 immutable ROM/provider 路由。 | `memory_interface.h`、`entry_plan_interface.h`、`firmware_interface.h` |
| 初始状态 | 已具备受限的 real-mode entry plan 与原子 preloads。 | `core_machine_apply_entry_plan` |
| RTC/CMOS、FDC/HDC、媒体注册表 | 控制器机制、拓扑提交和多媒体 provider registry 已在 core；媒体内容和策略不在 core。 | `machine_interface.h`、`media_interface.h`；M5 T296 |
| 固件回调 | 已具备 configure/reset/after-run 的受限 provider，以及配置期 ROM 注册和 checked memory/port 操作。 | `firmware_interface.h`；M5 T297 |
| 输入、呈现、基础 host 资源 | core platform 有 copied input source、display mailbox、可取消等待和同步 backing-resource 抽象。 | `src/core/platform/*_interface.h` |
| 调试与产品会话基础 | 已有 opaque product session registry、命令和 copied debug observation。 | `src/core/product/session/*`；M5 T298 |

这说明此前“CMOS、FDC/HDC、ROM、media 必须再从 VM 移入 core 才能谈 NTDOS”的判断已过时。M5 T296/T297/T300 已将可共享的**设备机制**及其配置冻结、失败传播和权限边界下沉到 core。

### 仍然正确留在 VM 的能力

当前 `src/vm/composition/session/session.c` 仍负责将默认 PC/AT profile 翻译为显示端口、DMA、CMOS 默认值、FDC/HDC 路由，并绑定默认 BIOS、镜像路径、键鼠映射、窗口/Console transport 与执行循环。这是正确的产品组合，不是 core 泄漏：

- 默认 PC/AT topology、CMOS 字节值、boot preference；
- 合成 BIOS 内容、BDA/IVT/POST 和传统 reset-vector 启动策略；
- FDD/HDD 文件路径、创建/插入/弹出、镜像格式和宿主文件策略；
- NXVM 的窗口、Console、debug UX、host key mapping、pacing 与进程退出策略；
- `vm_session` 本身及其 `vm_platform_*` transport。

这些内容不应迁入 core。否则 core 会重新变成一台隐含的默认 PC/AT，反而使 NTDOS adapter 被迫继承 NXVM 的 BIOS、启动和宿主策略。

## 为什么当前边界尚未“外部可用”

### 1. 缺少 pre-decode guest transition gateway

NT4 NTDOS 的历史 host 链需要在 guest 执行流的特定标记/服务边界把控制权交给 host 侧 dispatcher。它需要的是**通用、配置期注册且冻结的预解码 transition**，而不是把某个未定义指令或某个特定字节序列硬编码成 Microsoft 专用接口。

当前 core 没有公开的 transition registry。M5 T293 反而已移除旧的 post-`#UD` transition；当前故障路径只会记录 copied fault diagnostic。M5 队列将此列为条件任务 T301，尚未开始。队列表里现有的 `switch` 一词在实施前必须收紧：它至多能表示由 core 验证并执行的既有 CPU 语义结果，绝不能成为 handler 自行切换 real/protected/V86 mode 的权限。

建议的中性合同应具有这些硬限制：

- 注册的是有限长度、精确定义的已取指 physical bytes/解码形态，注册只允许在 `INITIALIZED`，freeze 后不可变；
- handler 只接收 copied CPU execution point、受检查的 guest-memory 读接口和显式 capability context；
- 结果只能是 `unhandled`、`handled-resume`、`stop`、`fault` 等有限 disposition；**不得允许 handler 任意改写 CPU mode、CRx、descriptor cache 或直接 switch real/protected/V86**；
- IP 消费、FLAGS 可写掩码、寄存器 patch 白名单和 RAM patch 都先验证、后一次性提交；失败保持原 CPU/内存状态；
- 未注册模式必须保留 core 原本的正常 decode/fault 语义。

这不是“只为 BOP”增加接口。它是受约束的 guest-to-host transition 机制，未来 mantle 可用于中性 runtime service、调试 stop point 或其它被证实的 consumer；但在真正的 first-party consumer 证明前，不应实现成任意指令 override。

### 2. 缺少 paused/STOPPED ordinary-RAM 多段原子事务

`core_machine_memory_read/write/query` 目前一次处理一个连续 physical range。entry-plan 的 preload 虽然在初始启动时可验证和提交，但它不能替代已经 reset/paused 的 machine 上由 adapter 提交一组 guest image、参数块或服务数据的能力。

NTDOS 研究侧所需的最小中性能力是：调用方提交 copied spans，core 先检查**全部**范围均为 ordinary RAM、无溢出、无重叠且不命中 ROM/device provider，再一次性写入。预检失败时不得留下部分写入；它不宣称 cycle atomicity，也不得建立第二条 memory route。

这是 M5 T302 的明确待办，依赖 T301 的处理结果。当前不存在 `memory_transaction` 或 `multi_span` 公共接口。

### 3. 当前 entry plan 只解决 real-mode 初始装配

`core_machine_entry_plan` 是很好的初始启动合同：它限定 real-mode register state、ordinary-RAM preload、entry physical/route，并避免产品直接操纵 CPU 私有结构。但它刻意不是运行时 CPU setter，也不支持保护模式初态、CRx、descriptor cache 或任意 translate。

这项限制应保留。对 NTDOS 而言，正确路径不是索要“全 CPU state 可写”，而是：

- 首次启动使用 entry plan；
- 运行期只通过 T301 的受限 transition disposition/patch 或 core 正式指令语义改变状态；
- 调试/诊断若需要地址翻译，新增只读、显式语义的 checked translation query，而不是暴露 CPU/RAM pointers。

NTDOS/DOSX/DPMI 后续是否触发更广的 286/386、分页、TSS、异常和 IRET 语义，仍必须由实际 trace/corpus 决定。当前 core 的 286/386 支持是“已证实的有界子集”，不能宣传为通用 386 或完整 V86 后端。

### 4. 外部 ABI 和 mantle 均未实现

架构文档明确把 `core.dll`、`mantle.dll` 设为中期目标；当前构建仍是单体 `nxvm.exe`，也没有 ABI version、capability negotiation 或发布 SDK。`src/mantle/` 尚不存在；`src/vdm/` 只是 minimal scaffold，并不消费 core。

所以 NTDOS64 目前不能安全地把 `vm_session` 当成 adapter API，也不能从 core 私有头文件、test fixture 或 raw borrow 拼出一个外部 runtime。即使将来采用 DLL，边界也应是窄的、`extern "C"`、固定宽度类型、显式 calling convention 的 ABI；不得跨边界传 C++ 对象、异常或 CRT 分配所有权。

### 5. Host 集成不是 core 的缺口，而是 mantle/wrapper 的职责

NTDOS wrapper 将需要 DOS namespace、路径/目录枚举、文件/lock/critical-error 策略、console stream、时钟、输入、进程交接和取消策略。它们不能为了“外部 NTDOS”直接进入 `core/platform`：core 的职责是中性的 event、presentation、等待和 backing-resource 原语。

未来 mantle 应只负责把 core 的同步 execution quantum、input/presentation 和 transition gateway 组织成一个无 DOS 策略的 session。实际 NT4 文件发现、DOS 驱动器映射、`COMMAND.COM /C`、Win32/Win64 宿主进程转交、BYOB manifest 与容器限制，属于外部 wrapper 的 host/product policy，不能进入 core 或 mantle。

## 采用 Core 作为候选 guest 后端时的条件清单

### 必须先补齐（阻止真实 NTDOS adapter）

1. **T301：受限 pre-decode transition registry。** 按上述合同实现，并由一个 first-party、非 Microsoft 专用 consumer 证明；不得复活 post-`#UD` 恢复路径。
2. **T302：停止/暂停态的 multi-span ordinary-RAM transaction。** 复用现有 checked mapping route；全量 preflight 后才提交。
3. **T304：第二消费者 closure。** 不只是 `core-mantle-shape-smoke`，还要证明 T301/T302（如获准实现）与 NXVM 回归同时成立，并清除所有 VM/private borrow 依赖。
4. **M6：真实 mantle session。** 它应拥有 lifecycle、有限 quantum pump、runtime binding、transition dispatch 和 factual diagnostics；不拥有 DOS SVC、路径策略、UI 或外部 ABI。

### 必须以 NT4 trace/corpus 验证，不能预先扩张

1. NTIO 到 NTDOS/COMMAND 的最初执行段实际要求的 CPU profile、A20、PIC/PIT/IRQ、键盘、显示、RTC、块设备最小集合；
2. DOSX/DPMI 是否需要 core 当前尚未证实的 386 control transfer、paging、TSS、特权/IDT 或 FPU 语义；
3. transition handler 所需的最小 copied CPU fields、memory patch 和 host-service scheduling；
4. FDC/HDC/CMOS 是否真的进入该启动路径。它们已可供 composition 配置，但不是“因为存在就必须启用”；
5. 时间、异步 IRQ、host I/O 短读/失败与 DOS critical error 的可观察顺序。

### 不应为了此路线迁入 Core

- 默认 BIOS、BOP table、DEM dispatcher、NTDOS 文件布局或任意 Microsoft selector；
- DOS drive letters、8.3/设备名、host path policy、redirector、process handoff；
- FDD/HDD 镜像文件实现和用户媒体选择；
- NXVM 的 UI、Console、debug command、session 管理或 host input mapping；
- 任意“外部 wrapper 专用”环境变量、注册表、私有 Windows API 或兼容 hack。

## 推荐实施顺序

1. 在 NTDOS64 保持现有历史 SoftPC/OpenNT 修复研究独立，继续用它产生启动 trace、依赖表和 CPU/设备最小集；不要以当前 core 替代历史链的未恢复部分。
2. 将本文的两项 core 缺口作为对 ntvdm64 的中性需求输入：先为 T301 写 S1 合同和 first-party consumer 证据，再决定是否实施；T302 随后进行。
3. T304 关闭后，在 ntvdm64 内实现 M6 mantle 的最小 session，并仅以自主 runtime/fixture 证明它；外部 NTDOS 不进入该仓库的 runtime graph。
4. 只有 mantle 的合同稳定后，NTDOS64 才制作一个版本锁定、独立构建的 adapter，使用 BYOB NT4 EN-US 文件做 bounded trace。此时 adapter 是验证 consumer，不是 core 的定义者。
5. 最后才评估 `core.dll`。DLL 化是 ABI 固化与发布问题，不是当前消除 core/vm 边界混乱的前置条件。

## 对“边界是否清晰可用”的最终判定

| 问题 | 判定 |
| --- | --- |
| 哪些通用设备机制应留在 core？ | 已相当清晰，且 RTC/CMOS、FDC/HDC、media、ROM、DMA 等主要迁移已完成。 |
| 哪些产品策略应留在 VM？ | 清晰：PC/AT profile、BIOS、镜像路径、boot/UI/host policy 都应留在 VM。 |
| 是否可开始设计 mantle？ | 可以做合同和只读研究；按 ntvdm64 自身路线，M6 实现仍应等待 M5 T304。 |
| 是否可直接将当前 core 作为 NTDOS64 的 guest backend？ | 不可作为稳定/外部 backend；可作为受版本锁定的研究候选，但真实 adapter 会立刻被 T301/T302 和 CPU corpus 问题阻断。 |
| 还需要把 VM 大规模迁回 core 吗？ | 不需要。下一步是补两个中性执行合同并完成 consumer closure，而不是继续搬 BIOS、文件或产品代码。 |

## 复核入口

- `D:\home\repos.hobby\ntvdm64\docs\architecture\contracts.md`
- `D:\home\repos.hobby\ntvdm64\docs\planning\status.md`
- `D:\home\repos.hobby\ntvdm64\docs\planning\m5-pcat-hardware-convergence.md`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\machine_interface.h`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\memory_interface.h`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\entry_plan_interface.h`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\firmware_interface.h`
- `D:\home\repos.hobby\ntvdm64\src\core\machine\media_interface.h`
- `D:\home\repos.hobby\ntvdm64\src\vm\composition\session\session.c`
- `D:\home\repos.hobby\ntvdm64\tests\machine\core_mantle_shape_smoke.c`
