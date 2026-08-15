# T248 调研报告：`ntvdm64` 的 VDM 分层与 core 边界

**状态：** 只读架构调研，供主线程决定后续任务。本文不构成源码变更、ABI 承诺、构建方案或第三方内容的分发方案。
**范围：** 当前 `ntvdm64` 的架构合同、已有 core 迁移任务，以及一个外部 DOS runtime 的公开历史源码所证明的运行形状。
**术语约束：** 本文将该第三方适配产品统一称为“**外部 VDM 外壳**”，将其 guest runtime 统一称为“**外部 DOS 实现**”。本文不将它们的专有名称、专有指令编码、服务号或数据布局写成 shared-core 合同。

## 1. 直接结论

`core` 需要继续扩展，但扩展方向是 **x86 通用机器正确性和无策略能力合同**，不是把 `vm` 的 PC/AT 控制器搬进来。

对以下设备的结论是明确的：

| 项目 | 是否要迁入 / 新增到 core | 结论 |
| --- | --- | --- |
| CPU real/protected/V86、异常、IRET、描述符、分页 | **要扩展** | 属于 CPU 本身的通用正确性，是复杂 DOS runtime 的硬前提。 |
| 受检查物理内存、ROM、entry plan、受限 execution transition、A20、PIC/PIT/IRQ | **要保留并完成** | 都是通用机器能力；不含任何 DOS ABI。 |
| host input、presentation、monotonic clock、可取消 wait | **T248 S1 现在定义合同，S2 据此实现** | 仅能是复制式、无 guest mutation 的 host capability。 |
| CMOS/RTC 芯片模型 | **不要迁入** | 外部 DOS 实现需要日期/时间结果，不等于 core 必须模拟该芯片和端口。 |
| FDC、ATA/HDC 控制器模型 | **不要迁入** | 外部 DOS 实现需要逻辑块 I/O，不等于必须运行这些寄存器级控制器。 |
| VM 的镜像路径、挂载/弹出、启动顺序、BIOS 内容、窗口/console | **不要迁入** | 都是 NXVM 产品策略。 |
| DOS drive/path/namespace、文件语义、进程语义 | **不要迁入** | 都是 runtime 与产品安全策略，绝不是 core/platform。 |

因此，今后的形状应是：

```text
core
  通用 x86 执行器、通用机器机制、狭义 host capability
    |
mantle（由 nxvdm 与外部 VDM 外壳这个已锁定的第二 consumer 共同决定）
  无 ABI 专名的 VDM runtime 装配层
    |                                      |
src/vdm -> nxvdm.exe                  外部 VDM 外壳 -> 外部 vdm.exe
  自有产品外壳                            外部产品外壳
    |                                      |
src/dos                                外部 DOS 实现
  自有 DOS runtime                        外部 ABI/runtime
```

`vdm` 与外部 VDM 外壳是**同级、对称的产品/适配层**。`dos` 与外部 DOS 实现是**同级、不可混用的 guest runtime**。差异只在 guest ABI 与实现来源；两者使用同一套由 mantle 组织的通用机器/宿主能力，而不能要求 core 理解它们的 ABI。

## 2. 事实、推论与未知项

### 2.1 已知事实

当前 `ntvdm64` 架构合同和已规划的 T243--T246 已建立如下基础：

* core machine 是确定性的 x86 机器库；它不拥有产品主循环、线程、UI、host 文件策略或退出策略。
* 物理 guest memory 访问必须经受检查的统一 RAM/provider 路由；不向调用者给出原始 guest pointer。
* undefined-instruction transition 是受限的执行器设施：固定注册窗口、冲突拒绝、由 core 消费 instruction bytes、有限 disposition、状态 patch 原子提交。handler 不得任意改 IP、ESP、段寄存器、控制寄存器、CPU mode 或 guest memory。
* ROM 映射和 entry plan 都具有冻结期、受检查范围和原子验证规则；entry plan 目前只描述 real-mode 状态。
* `vm` 仍拥有完整 PC/AT profile、BIOS 内容、设备选择、controller、介质和 UI。

固定的外部 VDM consumer 需求卡已经表明：复杂 DOS runtime 并不是普通 ROM boot guest，也不只是一个 DOS API 库。它依赖低内存/中断/存储/输入/显示/时间等机器可见结果，并经一种版本特定的 guest-to-host transition ABI 调用 host 服务。这个 ABI 会涉及文件、驱动器、时间、错误、调试、扩展内存、兼容子系统等服务族。

### 2.2 可作出的架构推论

从上述事实只能推出两件事：

1. core 必须提供可正确执行这些 guest 行为所需的**通用机器语义**。
2. 外部 VDM 外壳必须实现将其**特定 ABI**翻译为通用服务的适配器。

不能推出“现有 VM 的某个芯片控制器必须迁入 core”。外部 runtime 得到磁盘和时间服务，可以通过直接设备端口、固件服务、受控 transition 或其组合；服务结果的需求不能反推某个具体 controller 的归属。

### 2.3 仍未证实的事项

目标版本已经锁定。下列问题是当前外部 runtime 代码追踪应继续回答的对象；它们不妨碍本报告已经确定的 core/mantle/vdm/dos 边界。若源码无法唯一给出动态时序，再为那一项提出最小被动 trace，而不是把它笼统延后：

* 外部 DOS 实现的精确 entry 寄存器、低内存通信区和初始 IVT/BDA 内容；
* 每个 runtime transition 的参数、flags、内存范围、异步/重入规则；
* 启动和常规程序执行的最小设备集；
* 它是否会在目标路径上直接操作 RTC/FDC/ATA 端口；
* prepared-entry 是否可以替代其全部固件准备。

## 3. 三层责任模型

这是后续模块设计的硬边界；一个源文件可以服务于多个层，但它只能属于一个责任层。

### Layer 1：core layer

`src/core` 只解释通用机器和无策略 host 原语：CPU、RAM、A20、端口、IRQ、DMA、ROM、block registry、execution transition，以及复制式 input/presentation、monotonic clock 和 wait。它不解释 DOS、文件名、驱动器字母、DOS 内存块、runtime service 或产品 UI。

### Layer 2：composition layer

`src/mantle` 是 VDM composition；`src/vm/composition` 是 NXVM composition。两者都可以创建 core machine、绑定 provider、冻结 topology、应用 entry state 并驱动 session。区别是：

* mantle 只能使用 runtime-neutral binding，不能含外部 ABI 或自有 DOS ABI；
* VM composition 可以装配完整 PC/AT profile、firmware 与 controller，因为它为 NXVM 这一个产品服务。

### Layer 3：product layer

`src/vdm`、外部 VDM 外壳与 `src/vm` 的 product/platform/profile 部分都是产品层。它们拥有 CLI、console、授权、host path、进程转交、UI、退出策略和默认配置。`src/dos` 与外部 DOS 实现虽不是 UI 产品，但它们是 guest runtime 产品：各自解释 loader、DOS API、文件/进程/错误语义和自己的 ABI。

这一定义意味着：VM 同时有 Layer 2 的 composition 和 Layer 3 的产品代码；它没有成为 mantle 的依赖，也不会被 VDM 反向引用。

## 4. core/machine：确实要增加和扩展的内容

### 4.1 P0：CPU 可信语义

这是最重要、也最不能用外壳回调替代的缺口。core 应按独立任务完成：

* 286 protected mode：GDT/LDT/IDT、descriptor validation、异常、特权检查、protected transfer；
* 386 扩展：CR0/CR3、paging、CPL/IOPL、TSS I/O permission 和 V86 enter/exit；
* task switching、剩余指令、异常/中断与 IRET 的模式往返；
* 可重复的 instruction/fault/interrupt regression。

这里的关键规则是：**任何 CPU mode transition 都必须经 core CPU 的正式指令/异常/IRET 语义完成。** transition handler 绝不能成为第二套切换路径。

### 4.2 P0/P1：完成既有通用机器合同

下列能力应维持在 core/machine，并按需要扩展测试或精确语义：

* checked physical guest-memory read/write/query，以及普通 RAM/provider 的路由判定；
* immutable ROM byte image，且取指、读、写都走同一 routing；
* reset 后的 validated entry plan：只允许已验证 ordinary RAM 的 preload，先完整验证、再原子提交；
* 通用受限 execution transition：模式限制、1--15 byte pattern、物理取指匹配、注册期冻结、无 handler 保持默认 fault；
* A20 line 的机器可观察状态与地址折返语义；
* PIC/PIT/DMA、端口与 IRQ 的基础机制及可确定的时间推进。

这些能力共同支持“传统 firmware boot”与“direct prepared guest entry”两条路径，但不规定哪一个产品必须选择哪条。

### 4.3 P1：地址翻译观察，不是绕过 CPU 的内存捷径

当 protected/V86 语义已经可信后，core 可以增加只读、受检查的 address-translation observation：调用者显式指定 physical、real-mode segment:offset 或 protected linear 请求；core 返回结构化 success/fault 和受限结果，不返回 host pointer，不允许绕过 paging/segment 权限。

这与 T243 的 physical API 是不同概念。不要为了 loader/debug 方便而提前把“线性地址”伪装成“物理地址”。

### 4.4 core 需要完成的通用块 I/O；不需要芯片控制器

通用块 I/O **已经属于 core/machine，而且应当现在按正确边界完成**。当前 `src/core/machine/block_interface.h` 和 `block_provider.h` 已经是 product-neutral contract，VM session 已经将其 HDC 绑定到 core 的 block-provider slot。这已经证明 NXVM 是第一个 consumer；固定外部 VDM 外壳是第二个 consumer reference。这里不应再等“是否需要”的结论。

下一步应把当前单 slot、单一 CHS transfer 的最小形态升级为 core-owned block registry：

* 显式、无 DOS 含义的 `drive_id`；
* presence、read-only/removable、logical block size、capacity，以及可选 compatibility geometry；
* 基于 block index/LBA 的范围受检查读写；仅当某个 controller 需要时将 CHS 转换留在 controller；
* flush、media-changed、not-ready、read-only、I/O failure 等确定性中性结果；
* 绑定/解绑/冻结/重置/销毁规则；provider 只接收 opaque request，不接收 host path；
* 不让 block provider、平台回调或产品层直接取得 guest RAM pointer。

这项是 core/machine 任务，不是 mantle 任务：NXVM 已经需要并消费这一底层数据/命令边界。mantle 的职责只是在 session 中把产品授权的 block provider 绑定给 runtime；`vdm` 和外部 VDM 外壳各自决定 provider 来自哪里及可访问哪些 host 资源；`dos` 和外部 DOS 实现各自把它翻译为自己的 guest storage ABI。

### 4.5 不需要向 core 增加的硬件

对于本调研关心的硬件项，结论没有条件句：

```text
不要把 CMOS/RTC controller 加入 core。
不要把 FDC controller 加入 core。
不要把 ATA/HDC controller 加入 core。
不要把镜像文件 backend、host path 或 mount/eject 加入 core。
```

这不是对 block I/O 的否定：core 拥有的是 registry/request/provider contract；controller 仍留在 VM 或将来各自的产品适配器。CMOS/RTC 的日历结果不是 core 芯片模型的理由，FDC/ATA 的逻辑块需求也不是 core controller 的理由。

## 5. core/platform：T248 的准确工作范围

T248 S1 就是这份报告的执行性结论：它不再等待别人定义合同。T248 S2 应按本节的四个合同开始实现；固定的外部 VDM 外壳与 NXVDM 是设计上的两个 consumer，运行期验证可以用中性 test adapter 与 NXVM regression 完成。

### 5.1 允许进入 core/platform 的四类能力

| 合同 | core/platform 提供什么 | 明确不做什么 |
| --- | --- | --- |
| normalized input | 值语义的 `kind + sequence + monotonic timestamp + key/pointer payload`；producer 写队列、composition 在执行边界取走 | 不把事件写入 guest 键盘端口、IRQ 或内存；不拥有窗口策略 |
| presentation sink | `submit(frame/diagnostic)` 的复制式输出、返回背压/失败；明确 submitter 和 consumer 的寿命 | 不选择 renderer、字体、window、console 或 UI thread |
| monotonic clock | 单调递增 tick/纳秒读数，供 pacing/timeout/trace 排序 | 不提供 civil calendar，不设 guest 日期/时间，不写 RTC/CMOS |
| cancellable wait/wake | `wait(deadline, cancel)` 与 `wake()`；返回 timeout/woken/cancelled/error | 不拥有产品线程，不重入 machine，不改变 guest state |

四项共同规则：provider 的创建/销毁者、线程规则、buffer 有效期、调用状态、shutdown 语义、错误域和无 provider 的行为必须写入合同；platform 只产出/消费复制值，任何 guest mutation 都由 composition 在 core/machine 的正式接口上完成。T248 可据此直接实施，不再以“先有一个已完成的第二 binary”为前提。

### 5.2 明确不进入 core/platform 的东西

文件/目录、host path、drive visibility、DOS namespace、8.3、share/lock、mount/eject、wall-clock 到 guest calendar 的映射、process spawn、console、CLI、权限、外部组件发现与诊断，全部不进入 core/platform。

这些不是“能力名称不够抽象”的问题，而是本质上含有安全和产品政策。抽象名称不能把 policy 变成 core。

## 6. vm：迁出边界与保留边界

### 6.1 应保留在 vm/machine 与 vm/profile 的内容

* 默认 PC/AT topology；
* 合成 firmware image builder、其字节内容、BDA/IVT、firmware INT 服务和 POST；
* 从 reset vector 开始的传统启动选择；
* CMOS/RTC、FDC、HDC/FDD/HDD 的当前实现，及其端口、IRQ/DMA、geometry 和设备错误策略；
* 介质装配、启动顺序、默认 boot disk 和 boot failure 行为。

T245 的 ROM mapping 是 core；“生成和使用哪张 BIOS image”仍是 VM。两者绝不能混为一谈。

### 6.2 应保留在 vm/platform/product 的内容

* VM window/renderer/console；
* 主机事件如何映射成这台 PC 的键盘/鼠标设备动作；
* 镜像文件选取、路径授权、mount/eject 命令；
* VM CLI、配置、状态展示、用户交互和退出策略。

VM 可以消费 core/platform 的 input/sink/clock/wait provider，但不能把这些 provider 反向变成 core 的 UI 规定。

### 6.3 当前允许从 vm 迁出的范围

core 与 VM 的原则边界已经确定，mantle 也必须认可：core 拥有通用执行、通用 block registry 和无策略 host primitives；VM 保留 PC/AT controller、firmware 和产品政策；mantle 只在 core 之上绑定这些能力，绝不吸收 VM controller。当前可从 VM 迁出的项目是 T248 所定义、已审计为无 VM UI/boot/path policy 的 host provider 实现。当前没有确认的 controller 迁出项。

## 7. mantle：已确定边界的共享 VDM runtime 装配层

mantle 的**责任边界现在已经确定**：外部 VDM 外壳是外部 ABI consumer reference，NXVDM 是自有 ABI consumer。两者共享的不是 DOS ABI，而是 Layer 2 的 machine/session 装配。因此不需要再等“是否需要 mantle”的架构结论。

代码创建时机仍应服从任务排期：它应在 T248 合同可用、并且 `vdm` 开始装配第一条自有 runtime session 时以最小闭环建立；不应为了动态库包装而先创建空目录或空转发库。

### 7.1 mantle 的唯一职责

mantle 是 runtime-neutral 的 **VDM session envelope**，可以拥有：

* **machine composition：** 创建 core machine，注册已批准的通用 provider，冻结 topology，应用 entry plan；
* **session lifecycle：** `prepare -> ready -> running -> paused/stopping -> stopped/faulted` 的有限状态；只表达执行状态，不表达 CLI、窗口或退出码政策；
* **execution pump：** 驱动 core step/run、wait/wake、输入/输出转交、停止请求；
* **runtime binding：** 将 runtime 所声明的通用需求绑定到已授权的 clock、block/media、input、presentation、stream 等 capability；
* **transition gateway：** 代 runtime 注册 core 的受限 transition，并在 core 已规定的 state patch/atomicity 边界内转交；
* **neutral diagnostics：** 统一的结构化 fault/stop/event 记录，并保持脱敏和不含 guest content 的默认原则。

这表示 mantle 可以定义“runtime 需要一个块设备”这种无名称要求，但不解释该 runtime 从哪个 instruction、哪个 service number 或哪个 DOS 调用提出请求。

### 7.2 mantle 明确禁止拥有的内容

mantle 不得包含：DOS loader、程序段布局、DOS file/path/drive/namespace、任何专有 selector/service 表、特定启动文件、BYOB 校验、BDA/IVT bytes、BIOS 服务、host filesystem containment、window/console/CLI、host process policy、默认多 session 策略。

## 8. vdm：自有产品外壳

`src/vdm` 产出 `nxvdm.exe`。它是自有 VDM 产品，和外部 VDM 外壳的层级相同。它负责把“自有 DOS runtime”与“用户面对的宿主政策”装配起来。

### 8.1 vdm 的职责

* CLI、console、debug UX、显示选择、取消、日志展示、退出码和 session 数量政策；
* 用户授权的 host root、文件 containment、配置、默认拒绝策略；
* 输入/呈现/clock/block provider 的具体选择及其权限政策；
* 将 `dos` 提供的 runtime binding 接入 mantle；在 mantle 源码尚未落地的过渡期，保持同一合同并由 vdm composition 临时承载，不另造第二套接口；
* 若提供 host program delegation，则在此定义程序分类、参数引用、工作目录、环境、父 console、许可和结果映射。

### 8.2 vdm 的禁止项

`vdm` 不实现 VM BIOS 或默认 PC/AT controller；不把自有 DOS 的内部数据结构提升为 core；不实现外部 ABI；不让 `dos` 直接越过 vdm 的授权访问 host path。

## 9. dos：自有 DOS runtime

`src/dos` 是自有知识产权的 guest runtime，与外部 DOS 实现处于同一层，而不是 vdm 的 UI 子模块。

### 9.1 dos 的职责

* loader、程序段布局、环境、PSP、DTA、handle/FCB、进程创建与终止；
* 自有 DOS API、文件/目录/设备、错误/critical-error、内存与时间语义；
* 对 mantle runtime binding 的消费：将 runtime 的文件、块介质、输入、显示、clock 要求转换成 guest-visible DOS 行为；
* 可选的扩展内存/兼容模式，必须通过 core 的正式 CPU/A20/IRQ/memory 行为实现。

### 9.2 dos 的禁止项

`dos` 不拥有 window、console、CLI、host path 许可、主机进程策略或用户配置。它不依赖外部 DOS 实现的文件、私有协议或 ABI。

## 10. 外部 VDM 外壳与外部 DOS 实现

这两个部分应按与 `vdm + dos` 相同的垂直切分实现，而不是被当成一坨“兼容项目”。

### 10.1 外部 VDM 外壳：产品政策与 ABI 适配者

外部 VDM 外壳的责任是：

* 提供自己的 CLI、console、debug/trace UX、session/exit policy；
* 控制用户授权的 host 文件、目录、介质、输入、显示和 host program delegation；
* 发现并校验用户提供的 runtime 文件集合，保留版本/profile policy；
* 根据固定 profile 制备 entry 所需的 guest image/低内存状态；
* 解释外部 DOS 实现的专有 transition ABI、selector、附加服务字节、寄存器/栈/内存布局；
* 把其 ABI 请求映射为 mantle 提供的无名称 runtime binding；
* 定义未支持服务的 guest-visible 错误与实验性 trace 政策。

外部 VDM 外壳不能要求 core 或 mantle 出现其私有 opcode、selector、通信块、文件名、路径规则或调试协议。

### 10.2 外部 DOS 实现：guest runtime

外部 DOS 实现拥有它自身的 guest loader、内核、命令解释器、驱动、扩展内存/兼容组件和 guest API 语义。它不是 `ntvdm64` 的模块，也不被纳入默认构建、发布或测试依赖。

其与 mantle 的接缝只能由外部 VDM 外壳完成：外壳将 ABI 细节消化掉，再调用通用 runtime binding；mantle 和 core 永远不解析外部 DOS 实现的内部结构。

## 11. 时间、磁盘、键盘和显示：逐项落位

这是最容易混淆的部分，以下是最终责任表。

| 需求 | core | mantle | vdm | dos | 外部 VDM 外壳 | 外部 DOS 实现 | vm |
| --- | --- | --- | --- | --- | --- | --- | --- |
| x86 指令、IRQ、A20、内存 | 实现 | 使用 | 使用 | 使用 | 使用 | 使用 | 使用 |
| PC/AT RTC/FDC/ATA 芯片 | 不实现 | 不实现 | 不实现 | 不实现 | 不实现 | 不要求 | 实现/选择 |
| 单调时间原语 | 合同/provider | 绑定进 session | 选择 provider/policy | 消费 runtime 时间服务 | 选择 provider/policy | 消费其 ABI 时间服务 | 可消费 |
| guest calendar/日期时间语义 | 不实现 | 不解释 | 选择政策 | 自有实现 | 将外部 ABI 映射为 binding | 自身 ABI | BIOS/设备产品语义 |
| core block registry/provider | 实现并冻结合同 | 绑定 | 授权并提供 | 消费为 DOS storage | 授权并提供 | 消费其 ABI storage | 绑定 VM controller/provider |
| FDC/ATA 端口协议 | 不实现 | 不实现 | 不实现 | 不实现 | 仅在固定 runtime 代码追踪证明直接需要时由外壳承担 | 可能使用 | VM 产品实现 |
| 键盘/呈现原语 | 合同/provider | 绑定/转交 | UI 与权限政策 | 自有 guest 语义 | UI 与权限政策、ABI 适配 | 自身 ABI | VM 设备/UI 产品语义 |
| DOS path/drive/file 语义 | 不实现 | 不实现 | containment/policy | 自有实现 | containment/policy + ABI 适配 | 自身 ABI | VM 镜像策略 |

表中“外部 DOS 实现可能使用 FDC/ATA”并不等于 core 应模拟它们：若固定 runtime 代码追踪证实直接端口使用，首先由外部 VDM 外壳决定采用自己的设备适配、firmware 路径或逻辑服务实现。只有某个明确产品决定提供与 VM 相同的寄存器级模型时，才针对那个模型重新评估 core 提取；当前没有这样的需求。

## 12. 推荐任务顺序

1. **完成既有 core 基础任务。** 保持 T243--T246 的边界，不扩大 transition 或 entry plan 的权限。
2. **将本报告作为 T248 S1 的合同输入。** 按 input、presentation、monotonic clock、wait 四类补齐现有 provider 的 owner、线程、寿命和 VM policy 污染点，确认其实现落点。
3. **实施已确定的 T248 合同。** 以 copied input、sink、clock、wait 为上限；为每项写无 guest mutation、shutdown 和生命周期测试。
4. **优先推进 CPU 路线。** protected/V86、异常/IRET、描述符/分页比任何 controller 迁移更阻塞复杂 guest runtime。
5. **按已确定的 mantle 边界建立最小 session envelope。** 在 `vdm` 启动第一条自有 runtime session 时创建；外部 VDM 外壳的需求卡是第二 ABI consumer reference。不要为未来 DLL 创建空模块。
6. **完成 core block registry。** 将现有 slot 发展为带 drive identity、LBA request、media state 和冻结规则的通用合同；VM 立即迁移为其 consumer，外部 VDM 外壳按同一合同设计。
7. **不为 controller 新开迁移任务。** 外部 VDM 外壳由 core block registry + mantle binding 满足介质需求；CMOS/FDC/HDC 留在 VM，除非未来一个明确产品决定要直接提供相同的寄存器级硬件模型。
8. **外部兼容实验永远在仓库外进行。** 使用版本锁定 profile、脱敏 trace 和逐项 capability ledger；发现的专有 ABI 细节默认留在外部 VDM 外壳。

## 13. 验收规则

任何新 core 或 mantle 合同必须同时满足：

* 可以在不出现 DOS、外部 runtime、host path、UI、drive/namespace、专有文件或 selector 名称的情况下完整描述；
* 明确创建者、销毁者、线程、重入、冻结期、错误域、状态可写集合、原子性与默认行为；
* core/platform 对 guest 的 mutation 边只通过 core/machine 的正式路径发生；
* 有 focused boundary/rollback/error tests 与 NXVM 回归；
* 涉及 core contract 的能力有明确的 VM consumer 与固定外部 VDM consumer requirement；涉及将 VM controller 源码迁入 core 的，必须额外证明第二产品确实需要同一寄存器级模型；
* 默认构建、默认发布与常规 NXVM 运行不依赖任何外部 DOS 实现或用户提供文件。

## 14. 最终决策

* core **要扩展 CPU 正确性、A20、受检查 memory/translation、受限 transition 和狭义 host capability**。
* core **不要扩展 CMOS/RTC、FDC、HDC/FDD/HDD 或文件/镜像/驱动器政策**。
* vm **保留完整 PC/AT 机器和 BIOS 产品能力**。
* mantle **现在已确定为 Layer 2 的 runtime-neutral session/binding；在 `vdm` 第一条 session 开工时建立最小实现**。
* vdm 与外部 VDM 外壳 **同级且对称地承载产品政策与各自 ABI 适配**。
* dos 与外部 DOS 实现 **同级且各自承载 guest runtime 语义**。

这套切分让两个 VDM 都能共享更大的、但仍保持干净的 core；共享发生在 ABI 之下、产品政策之上。真正不可共享的内容被明确留在各自 runtime/外壳中，而不是伪装成通用接口。

## 15. 调研依据

### 当前工作树

* `docs/architecture/overview.md`
* `docs/architecture/module-layout.md`
* `docs/architecture/contracts.md`
* `docs/planning/status.md`
* `TODO.md` 中 T243--T248 与 CPU 路线任务
* `src/core/machine/{memory_interface.h,entry_plan_interface.h,undefined_instruction_transition_interface.h}`
* `src/core/platform/{host_surface_interface.h,display_frame.h,sleep.h}`
* `src/vm/machine`、`src/vm/platform`、`src/vm/composition/session`

### 固定外部 consumer 需求卡

* guest runtime 需要低内存/中断、存储、输入、显示、时间和 guest-to-host transition；
* 外部 ABI、启动文件集合、host 文件/路径策略和 guest 数据布局均由外部 VDM 外壳持有；
* 本报告只将这些需求归约为 core、mantle、产品外壳和 runtime 的责任，不在 shared contract 中保存其专有描述。
