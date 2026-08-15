# 外部 VDM Core/Composition 边界复核报告 02

状态：仅研究报告。本文件不修改相关实现项目的源代码、架构权威文档、构建图、任务状态或发布策略。

合规范围：本文只讨论中性的虚拟机架构结论。任何外部 VDM、外部 DOS runtime、外部系统文件和历史实现细节均为受限研究输入；它们不是 `core`、`mantle` 或 NXVM 的依赖、ABI、默认后端、构建输入或发布内容。本文避免记录受限实现的专名、服务号、文件名、指令编码及私有接口标识。

## TL;DR

既有的 core 迁移方向正确。能够支持外部 DOS runtime 的环境需要一台可配置的 x86 虚拟机，而不只是 DOS API。checked guest-memory、A20、不可变 ROM mapping、real-mode entry plan 和受限的 instruction transition 都应当位于 composition 之下。

边界还应继续向外推进，但必须有选择性。不能把全部 `src/vm/machine` 搬进 core；应当迁入或新增中性的设备机制和 host provider contract，而 PC/AT topology、firmware 内容、host path、boot/media policy 和产品 UX 仍留在 composition。

```
core
  可复用的机器机制、设备模型、host capability contract

mantle
  中性的整机装配、provider 绑定、入口与让出控制

vm
  NXVM PC/AT profile、firmware、boot policy、媒体路径、UI 与 CLI

外部 VDM adapter
  外部 DOS bootstrap、外部 runtime 适配、DOS namespace、host hand-off
```

路线不是“只做 contract”，而是一条连续的迁移链。第一步建立两组明确分层的 contract：`core/machine` 的 multi-device media/device contract，以及 `core/platform` 的 file/directory/stream/host-clock/host-input capability contract。第二步以这些 contract 为边界，拆分并迁入 MC146818、FDC、ATA 的 profile-neutral controller implementation，同时让 NXVM profile 继续提供 wiring 与 backend。第三步让 mantle 以同一套 core capability 装配非 NXVM machine。第四步才由 external VDM adapter 选择并使用 mantle configuration。media 的 host backing 由 platform capability 或 composition provider 实现，但 controller 对 media 的 guest-device 访问属于 machine。

CMOS/RTC 已具备 controller/profile 拆分的条件。MC146818-compatible 状态机、端口协议、日历事件与 IRQ 产生应成为 core 机制；PC/AT 默认值、NMI routing、BIOS 时间服务与 wall-clock policy 仍留在 composition。FDC 和 ATA/HDC 的 controller mechanism 均已确定应迁入 core；各自的 media contract adapter 是迁移实现的技术前置，用于切断对 VM backend 的直接依赖，而不是等待消费者或重新决定归属。trace 的作用不是决定 ATA controller 是否属于 core，而是界定第一阶段要支持的 ATA command、PIO/DMA mode、addressing mode 和 error timing 范围。

## 1. 证据与边界规则

现有代码表明，`core` 已拥有 CPU/RAM/ports、A20、PIC/PIT/DMA、KBC/AUX、VADP、checked memory routing、ROM mapping、entry plan 和受限的 undefined-instruction transition。VM 当前拥有 CMOS、FDC、HDC、FDD/HDD、firmware 构造、默认连线和产品交互。

外部 VDM 研究表明，某类历史外部 DOS runtime 需要的是一台已准备好的机器，加上 monitor、firmware 和 host service；它并不只是调用普通 DOS API。因此，外部 VDM 是判断哪些机制应当可复用的真实消费者。但这不意味着其 runtime dispatch、文件布局、DOS 数据结构或私有 host call 适合作为共享 API。

一个能力只应在同时满足以下条件时进入 core：

1. 它是 guest machine mechanism 或 host capability contract，而不是 DOS service 或产品策略。
2. 不选择 PC/AT 默认值、ROM 字节、host path、drive letter、窗口模式或某个外部 runtime 版本也能配置它。
3. 不使用外部 VDM/DOS 专名、服务标识或 private host API 就能描述其 API。
4. NXVM 能通过 profile adapter 使用它，mantle 也能用另一种 composition 使用它。

以下问题应留在 core 之上：port/IRQ/DMA 默认值、firmware 字节、IVT/BDA 内容、boot order、镜像文件、UI、DOS namespace、外部 runtime service 语义，以及对现代 host 的 process hand-off。

这里的门槛不是“必须再等一个消费者”。外部 VDM/DOS 是额外的真实消费者候选。真正的门槛是：是否已将产品策略剥离、是否已形成中性 contract、以及能否被 NXVM 与 mantle fixture 或 external runtime trace 验证。

## 2. 目标分层

```
外部 DOS files 与 external runtime
  -> external VDM adapter：bootstrap、runtime dispatch、DOS policy
  -> mantle：中性机器装配与 lifecycle
  -> core：机器机制与 host contract

NXVM
  -> vm：PC/AT profile、firmware、UX
  -> core：同一组可复用 mechanism/contract
```

mantle 不是第二个 VM，也不是外部 VDM compatibility layer。它的有限职责是：配置 machine、绑定中性 provider、冻结 topology、在 reset 后应用经过验证的 entry plan、运行 bounded slice、投递 event、返回 typed outcome，并管理中性诊断和资源生命周期。

mantle 不得理解外部 DOS file layout、runtime dispatch token、DOS structure、DOS path 或 NXVM firmware policy。

external VDM adapter 负责外部 DOS discovery、bootstrap、historical runtime state、DOS namespace、redirector policy、DOS error 与 host process transfer。它只对接 mantle，绝不依赖 VM。由此 core/mantle 仍可自由发布，也不会包含受限 runtime ABI。

## 3. 已在 Core 中且应保持的能力

`core/machine` 不只是 contract 集合。它包含 profile-neutral 的实际 guest-device implementation，以及这些实现之间的 machine-internal contract。`core/platform` 则定义 host-facing capability contract。composition 选择实例、注入端口/IRQ/DMA/clock/media wiring，并把 host backing 包装成 machine provider。

| 范畴 | 结论 | 边界理由 |
| --- | --- | --- |
| CPU、RAM、physical map、ports | 留在 core | 所有 composition 共用的 guest 执行与路由基础。 |
| A20/HMA | 留在 core | 机器地址语义，不是 DOS policy。 |
| PIC/PIT/DMA/KBC/AUX | 留在 core | profile 负责接线，但设备机制可复用。 |
| VADP/video base | 留在 core | guest adapter 与 renderer/window 产品策略是两件事。 |
| ROM mapping | 留在 core | 验证与路由通用；内容由 core 上层提供。 |
| Real-mode entry plan | 留在 core | 任何 loader 都需要原子、受检查的 state application。 |
| Instruction transition | 留在 core | 通用受限 trap，不能成为外部 runtime ABI。 |
| Input/presentation/wait primitives | 留在 core/platform | primitive 通用，policy 留在上层。 |

两项现有限制是正确的，必须保持：

* transition handler 只能返回受验证的结果，例如 unhandled、handled-and-resume、stop 或 fault。它不能直接切换 real/protected/V86 mode；mode transition 必须继续由正式 CPU 语义完成。
* entry plan 仅在 stopped reset boundary 应用已经检查过的 state。它不让 core 生成 BIOS、IVT/BDA 内容、POST code 或 PC/AT policy。

## 4. 设备与芯片边界复核

### 4.1 CMOS/RTC：迁入控制器机制，保留 Profile

建议：将 `src/vm/machine/cmos.*` 中 MC146818-compatible controller 的部分拆出并迁入 `core/machine`。

core 设备应拥有：index/data register protocol、register mask、register-C acknowledgement、calendar/update behavior、已有实现中的 BCD/binary 与 12/24-hour behavior、periodic/update/alarm event、可配置 IRQ assertion，以及 reset/advance-time behavior。它的配置输入是 port pair、IRQ sink、clock source 和 initial NVRAM image。

composition/profile 应保留：PC/AT 选择 `70h/71h`、PIC routing、NMI policy；描述 NXVM 的 default CMOS bytes；deterministic 或 wall-clock policy；持久化/UI；BIOS setup、时间服务、BDA timekeeping 与 POST。

这并不是要求每台机器都有 RTC，而是让 RTC 成为可选且可配置的 machine device。它是当前最强的迁移候选，因为它已相当参数化，也不拥有 image path 或 boot policy。

#### 多 Profile 的明确边界

将 MC146818-compatible model 放入 `core/machine`，不表示 core 默认拥有一块 PC/AT CMOS，也不表示每个 profile 必须安装它。它应是可选 device：某个 profile 可以实例化它并给出自身的接线与初始状态；另一个 profile 可以使用不同 RTC，或完全不安装 RTC。

`core/machine` 的 MC146818 device 只负责芯片语义：寄存器、日历/定时状态、periodic/update/alarm 条件，以及单一可配置 IRQ output。它不定义其 IRQ 接到哪一个 PIC input，也不定义 BIOS 如何读取时间。

以下均是 composition 的 profile glue，不能随 controller 迁入 core：

* 是否使用 `70h/71h`，或其他 port decode；
* RTC output 如何连到 PIC，包括 PC/AT 中常见的 IRQ8 选择；
* 与 address port 高位相关的 NMI mask 行为。该行为属于 PC/AT system glue，不属于 RTC 芯片自身；
* CMOS initial image、设备配置默认值、持久化和 wall-clock/deterministic policy；
* BIOS setup、BDA timekeeping、boot/POST 以及 firmware 时间服务。

因此，多 profile 的装配形态应是：

```
PC/AT profile
  -> instantiate core MC146818 device
  -> install PC/AT port and NMI glue
  -> route RTC IRQ to the selected PIC input
  -> provide PC/AT CMOS defaults and firmware policy

other compatible profile
  -> instantiate the same device with its own ports, IRQ route and initial image

non-compatible profile
  -> instantiate another RTC device, or no RTC device
```

### 4.2 FDC：迁入 Controller；Contract 是实现前置

结论：`src/vm/machine/fdc.*` 中的 8272A-compatible controller mechanism 应迁入 `core/machine`。`neutral floppy-media contract` 不是“等到需要时再决定”的条件，而是迁移实现的第一步：controller 必须先有不依赖 VM `t_fdd` 的 device-level endpoint，才能在不携带 image/path/mount policy 的情况下进入 core。

core FDC 应拥有 command/parameter/result 与 status state machine、DRQ/DMA sequencing、可配置 IRQ 与其自身 register port registration、sector request formation、reset 以及确定性的 controller error。它接受 neutral provider，不得引用 VM `t_fdd`。

它不得拥有镜像分配、path、mount/eject command、default drive count、topology、PC/AT supplementary port glue 或 firmware interrupt service。

VM 继续拥有标准 FDC port-address choice、IRQ6、DMA2、PC/AT supplementary port glue、drive topology、image policy 与 firmware disk service。mantle 可以配置另一种 topology，也可以没有 FDC。外部 DOS 是具体 trace 消费者，不代表每个 mantle machine 都需要 floppy controller。

### 4.3 ATA/HDC：迁入 Controller，保留 Profile 与 Backend

ATA/HDC 应进入 `core/machine`，但当前 `src/vm/machine/hdc.*` 不能原样移动。它现在把四个不同层次的职责耦合在一起：guest-visible ATA controller、PC/AT IDE topology、VM HDD image object 和 firmware disk policy。迁移的工作是拆开它们，而不是等待决定其归属。

core 的 optional ATA controller 应拥有：task-file register/status/state-machine mechanics、command execution ordering、PIO data transfer、配置化的 port/IRQ/master-slave wiring、对 machine media provider 的 sector/byte range request，以及 reset/ready/absent/write-protect/I-O failure mapping。它不得包含默认 `1F0h` port、默认 IRQ、默认 primary/secondary channel、镜像 path 或任何 firmware service。

VM/profile 继续拥有 IDE topology、默认 port/IRQ/DMA 接线、drive slot 选择、CHS/LBA presentation policy、image path、mount/eject/persistence、boot policy 与 firmware disk service。mantle 可以选择其他 wiring、synthetic provider 或完全不安装 ATA。external VDM adapter 也可以选择 redirected 或 synthetic block storage；core 不替它选择 backing。

外部 DOS runtime 的常规路径可能主要经由 firmware 或 host service 访问磁盘，而不直接对 ATA port 编程；这不改变 ATA controller 的 core 归属。一台可配置的完整 guest machine 必须允许 DOS workload、boot path 或 firmware profile 使用该 controller。trace 只应决定首期 controller scope，例如先支持哪些 ATA command、CHS/LBA 形式、PIO 是否足够、DMA 是否延后，以及错误/ready timing 要达到何种精度。

### 4.4 FDD/HDD Backend：保留在 Composition

FDD/HDD 的镜像分配、file loading/creation、insertion/removal、persistence 与 user interaction 都是产品和媒体策略。VM 可以成为 core media-provider contract 的实现者，但这些选择仍应由 VM 负责。

### 4.5 Video、Keyboard 与 Interrupt Devices

VADP、KBC/AUX、PIC、PIT、DMA 无需立刻再迁移。现有的 mechanism/profile 划分合理：core emulates adapter、queue、timing 与 interrupt transfer；VM 选择 renderer、console/window、firmware 与 input policy；mantle 暴露中性 frame/input endpoint；external adapter 处理 DOS/firmware convention。

### 4.6 Firmware、IVT、BDA 与 Reset 内容

core 仅保留 ROM routing 与 checked entry application。firmware image construction、ROM byte、IVT/BDA content、reset-vector/POST、interrupt service 都必须保持为 composition content。

NXVM 从其 PC/AT profile 提供这些内容；external adapter 提供 external DOS bootstrap 或 prepared state；mantle 只验证并应用，不负责创造它们。

## 5. 必须新增或扩展的 Core Subsystem 与 Contract

这里必须区分两条不同的边界：

* `core/machine` 是 guest machine 本身。它同时包含 profile-neutral 的 device implementation，并定义这些 implementation 如何访问一块可插拔的逻辑媒体；它不知道媒体来自文件、内存、网络还是其他 host resource。这是机器内部的设备边界，不是另一层面向 guest 的 API。
* `core/platform` 是 host-facing surface。它定义 composition 如何安全、可取消地获得 host file、directory、stream、clock 和 input resource；它不知道 FDC、ATA、drive letter 或 DOS 文件系统语义。

composition 是两者之间的装配者：它可以用 platform 的 file capability 建造一个 media provider，再把该 provider 绑定到 machine 的 media/device contract。

### 5.1 `core/machine`：Profile-Neutral Media Subsystem

该 subsystem 由两类东西组成，缺一不可：

* 实际 device implementation：MC146818、FDC、ATA 等可选 controller 的寄存器、状态机、IRQ/DRQ、reset 与错误语义；它们接收 profile 注入的端口、IRQ、DMA、clock 和 device wiring。
* machine-internal media contract：controller 用它访问 logical media，而不直接拥有 VM image object 或 host resource。

因此，contract 不是 controller 的替代品；它是让 controller implementation 能够留在 `core/machine` 的解耦点。

现有 block provider 过窄：它本质上是用于 firmware geometry lookup 的单一 fixed-CHS slot，无法让 FDC/HDC 与 VM media 解耦。应扩展或 version 该 contract，至少包括：

* opaque device identity 与多个独立 present device；
* kind/capability：removable、present、read-only、flushable、geometry-known、change-detectable；
* 带明确 sector size/count 的 logical-sector read/write；
* optional geometry metadata，但不能作为唯一 addressing method；
* 用于 mount/eject detection 的 monotonic media generation；
* typed failure：absent、changed、write-protected、invalid-range、transient、permanent；
* 初期使用 synchronous completion；以后预留显式 queued-completion extension point，而不是 re-entrant host callback。

这是一份 `core/machine` contract：controller implementation 通过它访问 guest-visible media，core 拥有 contract 与 controller/provider glue。VM、mantle、external adapter 实现 provider。drive letter、host path、directory enumeration 和 file handle 明确禁止进入 contract。

host file 或其他 backing resource 不会直接穿透到 controller。composition 负责把 `core/platform` 的能力包装成一个带有媒体 identity、容量、扇区读写、media generation 和 typed failure 的 machine media provider。这样 FDC/HDC 只看到设备级结果，不会因 host I/O 细节污染 guest device model。

### 5.2 `core/platform`：Host Capability Contract

当前 core/platform 已有 input、presentation 与 wait primitive，但还需要新的 neutral contract；这不是 VM 代码迁移。

| Contract | core/platform 可以定义 | 必须留在 core 上层 |
| --- | --- | --- |
| File | opaque handle；open/read/write/seek/stat/close；typed error | DOS path、8.3、sandbox root、share/lock policy |
| Directory | opaque enumeration 与 entry metadata | wildcard、current directory、DOS translation |
| Stream | byte read/write/flush/close | console UX、shell redirection grammar |
| Media backing | 建立/关闭 backing resource 所需的 opaque host handle、capability 和 typed error | guest sector protocol、controller state、mount UI、local path、persistence/default |
| Clock | sampled host clock 与 wake/wait source | machine virtual time、device tick、timezone/locale、DOS clock policy |
| Input | copied host key/pointer event source | KBC/AUX guest queue、focus、console flush、keymap policy |

这些 contract 必须采用 fixed-width type、opaque ownership、cancellation 和 typed result。不得暴露 C++ object、CRT allocation ownership、native handle/private API 或外部 runtime callback。特别地，machine virtual time 及 PIT/RTC 的推进属于 `core/machine`；`core/platform` 只提供可选的 host time sample 或等待原语。类似地，KBC/AUX 的 guest input queue 属于 `core/machine`；`core/platform` 只提供 host input event source。

## 6. Execution 与 Transition 边界

现有 execution provider 提供 reset、refresh、time advance 等生命周期工作；它不是可替换的 CPU execution backend。owned core CPU 仍是正常 executor。任何 historical CPU experiment 必须隔离，只有在发现精确的缺失能力、并具备与 owned CPU 的 parity test 后，才考虑为 core 引入 neutral alternate execution-engine contract。

同样，transition facility 是 generic instrumentation/trap mechanism，绝不是某个外部 runtime dispatch interface。没有匹配 pattern 时，应保持正常 CPU undefined-instruction behavior。

## 7. 迁移顺序与 ROI

本节是连续路线图，不是互相替代的候选清单。media/platform contract 是 controller migration 的实现地基；完成它不会结束设备工作，反而使 RTC、FDC、ATA 的实际迁入能够不携带 VM backend 和 PC/AT policy。每一步都应保留 NXVM 的 profile adapter 和回归验证，随后才扩展 mantle 的装配面。

| 优先级 | 工作 | ROI | 原因 |
| --- | --- | --- | --- |
| P0 | version media subsystem contract 与 host capability contract | 很高 | 让 core device implementation 不知道 path/DOS semantics，同时解锁 storage/host integration。 |
| P0 | 统一 architecture doc 与实际 ownership | 高 | 当前文档把 CMOS 同时描述成 core-side 与 VM-private。 |
| P1 | 拆分并迁入 MC146818 controller mechanism | 高 | 最小的真实 controller move；接线可配置。 |
| P1 | 定义 mantle configure/freeze/entry/run-slice/outcome envelope | 高 | 给 external VDM adapter 稳定 target，不依赖 VM。 |
| P1 | 令 NXVM FDD/HDD 成为 media-provider implementation | 高 | 用现有产品证明 contract。 |
| P2 | 建立 floppy-media adapter 后迁 FDC controller | 中高 | 归属已确定；adapter 仅切断 VM backend 依赖，保留 topology/image 在 core 上层。 |
| P2 | 在 media contract 后拆分并迁 ATA controller | 中高 | controller 已属于 core；当前工作重点是解除 VM backend/profile 耦合。 |
| P2 | 用 trace 界定 ATA 首期 command/mode/timing scope | 中 | 决定 PIO、DMA、CHS/LBA 和错误时序的实施范围，而非 controller 归属。 |
| P3 | 考虑 alternate execution engine | 仅调研 | 需要比 bootstrap 更多的证据。 |

## 8. 后续迁移的 Admission Test

在把 VM component 迁入 core 前，必须验证：

1. Public API 中没有 hard-coded PC/AT port/IRQ/BIOS byte、image path、drive letter、DOS structure 或 external runtime token。
2. Port、IRQ、DMA、clock、provider wiring 都是显式的，且在 runtime 前验证。
3. Topology 在正确 lifecycle window 冻结。
4. Provider failure 是 typed 的，不能导致 partial reset、entry 或 preload。
5. NXVM regression 通过，且没有把行为隐藏在 core default 中。
6. 一个 mantle fixture 能在不引用 VM header、firmware 或 UI code 的情况下配置它。
7. Public header/symbol 中没有 external VDM/external DOS/private host vocabulary。

## 结论

第一阶段迁移停在了正确的平台。下一阶段应是：建立中性的 media subsystem contract 与 host capability contract；完成 CMOS controller 与 profile 的拆分并迁入 controller；再在该 contract 之上迁 FDC 和 ATA controller。trace 用于确定 ATA 的首期 command/mode/timing scope，而非重新决定 controller 归属。

firmware、IVT/BDA content、path、media policy、PC/AT default、UX 以及全部外部 DOS behavior 都应留在 composition 或 external VDM adapter。这会使 core 足以同时服务 NXVM 和面向外部 VDM 的 mantle，同时不会让它变成隐藏的 PC/AT product 或任何受限 runtime compatibility layer。
