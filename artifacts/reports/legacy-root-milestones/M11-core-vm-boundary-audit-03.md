# M11 Core/VM 边界现状审计报告 03

状态：只读架构审计，2026-08-07。本报告不修改 `ntvdm64` 的源代码、
构建图、任务状态或发布策略；文件存放于独立研究目录，且不包含受保护
guest 字节或私有 runtime ABI 细节。

## 审计问题与结论

问题：经过前两份 M11 边界报告，以及其后的 M5 实施后，当前
`ntvdm64` 的 `core`/`vm` 划分，是否已经为保留的整机产品与未来外部
DOS 研究适配器提供了正确基础？

**结论：对机器机制而言基本是；对中性装配层和原始历史 dispatcher
桥接而言还不是。** 当前最重要的缺口不是继续迁移一般 PC/AT 设备。
M5 已将有证据支撑的控制器机制迁入 `core`。下一条架构边界是规划中的
`mantle` session envelope；只有当历史 runtime 能被链接并证明需要时，
才应再引入狭窄的事务桥接。

```text
当前、已验证
  core：CPU/RAM/bus/A20 + 可选控制器机制 + 有界执行
  vm：PC/AT 装配、firmware 字节、媒体镜像策略、原生 UX

尚未实现、且不得即兴补齐
  mantle：中性配置 / 生命周期 / 执行 / 诊断 envelope
  historical adapter：版本专属 bootstrap、runtime dispatch、DOS 策略
```

本审计确认第二份报告的 `core -> mantle -> vm | adapter` 模型，并取代
第一份报告中“未必需要 mantle 层”的暂定结论。仓库的
`docs/planning/roadmap.md` 也已将 mantle 置为 M6，在 core/VM 收敛之后。

## 方法与范围限制

本报告对当前 M5/T283 基线进行了静态、只读审计：

* `docs/planning/status.md` 将 T243--T246、T270--T283 记录为已完成，
  并记录 artifact `vm-0-5-0283` 的 110/110 CTest 结果。
* 审阅了 `src/core` 与 `src/vm` 下的源代码所有权和调用路径。
* 以前两份 M11 报告为提案基线。
* 仅以 NT4 历史 runtime 研究检验某能力是否确实中性；本报告不会使
  任何历史二进制、服务 ABI 或受限源码成为项目依赖。

静态证据能够证明所有权和 contract 形状；它**不能**证明外部 runtime
已经到达命令 shell、所有 CPU mode path 都兼容，或历史 host dispatcher
已经可以在当前 core 上运行。

## 审计快照

| 范畴 | 当前所有者 | 审计判断 |
| --- | --- | --- |
| CPU/FPU、RAM、A20、physical mapping、ports、PIC/PIT/DMA/KBC/VADP | `core/machine` | 正确。这些是所有消费者共用的 guest-machine mechanics。 |
| checked physical-memory route | `core/machine` | 正确，可由未来中性 loader 使用；能区分 ordinary RAM/provider，并拒绝零长度访问。 |
| immutable ROM byte image | `core/machine` | 机制归属正确；ROM 内容与 firmware 构造仍须在 core 上层。 |
| reset-bound real-mode entry plan | `core/machine` | 归属正确；它在提交前验证所有 preload 均为 ordinary RAM。 |
| pattern-bound `#UD` transition | `core/machine` | 正确且有意受限；适合 trace/stop，但尚非历史 dispatcher bridge。 |
| media identity/provider/result contract | `core/machine` | 正确的 shared device-facing boundary；已将 controller 与 VM image object 解耦。 |
| FDC 与 ATA PIO controller mechanism | `core/machine` | 迁移已完成；`core_machine` 拥有状态及 refresh。 |
| MC146818 register/calendar/IRQ mechanism | `core/machine` | 机制归属正确；VM 仍拥有 PC/AT port adapter 与 defaults。 |
| PC/AT port/IRQ/DMA topology、CMOS defaults、firmware、IVT/BDA、boot policy | `vm` | 正确地留在 product/profile composition；不得迁移。 |
| FDD/HDD image loading、path、persistence、mount/eject command | `vm` | 正确地在 core 之上；这是 host/media policy，不是 controller mechanism。 |
| native window、console handle、rendering UI、command grammar | `vm/platform`、`vm/product` | 正确地在 core 之上；T282 已将 native-handle ownership 移出 core。 |
| opaque synchronous backing handle | `core/platform` | 良好的第一项中性 host capability；不包含 pathname 或 DOS vocabulary。 |
| directory、stream、host-clock sample、child-process hand-off | 尚无 shared contract | 是真实的未来需求，但不是自动的 `vm -> core` 迁移；只有出现 owned/mantle consumer 与 policy-free contract 后才可引入。 |
| 中性的非-NXVM session assembly | 尚无；M6 已规划 | 当前最主要的架构缺口。 |

## 实现已经证实的事项

### 1. 第二份报告建议的设备迁移已基本完成

`core_machine` 已在 `src/core/machine/machine.h` 中拥有 FDC 与 HDC
存储（字段 `fdc`、`hdc`），并在
`src/core/machine/machine.c` 的 `core_machine_advance_scheduler` 中推进
它们。VM 已不再实现这些 controller state machine；它的 composition
只在 configuration 阶段借用 core device，再提供所选 ports、IRQ/DMA route
与 frozen media registry。

这正是目标划分：

```text
core：controller register、command state、DRQ/IRQ、DMA-facing behavior、
      reset/refresh、typed media result handling
vm：  “该 profile 在此处接入该 controller”、选定媒体 identity、
      backing image、用户 mount/eject/persist policy
```

T276 与 T278 分别记录了迁移；T283 的 core-only fixture 覆盖 FDC media I/O
和 ATA PIO media I/O。它比第一份报告的预测更强：这不再只是合理的未来
迁移，而是已验证的实现。

### 2. RTC 的拆分正确，且 execution-provider hook 是有意保留的组合点

中性的 MC146818 mechanism 位于 `src/core/machine/rtc.*`。VM 的
`machine_devices.c` 保留 PC/AT 专属的 `70h/71h` port adaptation、bit-7 NMI
语义和 NVRAM defaults，并通过既有的 core execution-provider time hook
推进 RTC。

不要把它改为 core 中强制存在的 PC/AT RTC instance：可选 device mechanism
可复用，但所选 port pair、IRQ、calendar seed、NMI policy 和 CMOS 内容皆为
profile 事实。因此下一项需求是 mantle 的可选 time-driven device
lifecycle binding，而不是再做一次 RTC 迁移。

### 3. Firmware 内容留在 core 上层，划分正确

`core_machine_register_immutable_rom_mapping` 复制并以与 memory 相同的
device-provider route 暴露只读 physical byte image。这解决了可复用部分：
range validation、overlap rejection、immutable read 与跨 reset 的 mapping。
它并不声称 core 能构建 BIOS 或理解 IVT/BDA 内容。

PC/AT firmware assembler、POST flow、interrupt-service body、reset-vector
选择、boot-sector policy 和 ROM byte 仍属于 VM profile 内容。这消除了
第一份报告中的一个歧义：generic ROM mapping 属于 core；“BIOS image
builder”不能仅因其生成 ROM 就成为 generic core service。未来 adapter
可经 mantle 提供自己的 prepared image/entry state，而无需继承 NXVM
firmware policy。

### 4. Entry plan 是真实的 core 边界，而不是 VM bootstrap code

`core_machine_apply_entry_plan` 仅接受 stopped 且刚 reset 的 machine，
验证 real-mode CS:IP 对应关系，逐项经过 checked memory route 验证 preload，
要求其为 ordinary writable RAM，并在变更 CPU 或 memory 前拒绝 overlap。
这就是未来 BYOB loader 所需的正确 primitive：loader 在 core 之上验证与
选择字节；core 负责将它们以原子的 machine-side 方式应用。

它有意不 preload ROM/device mapping、不选择 boot disk、也不构造 BDA/IVT。
这些都是 composition/bootstrap 的选择。

### 5. `#UD` transition 正确，但远窄于 adapter ABI

当前 registry 允许有界 arbitrary pattern，运行前 freeze，拒绝 identical 或
prefix-overlapping registration，并只提供 copied CS/EIP、general register
和 EFLAGS。response 只能为 unhandled、handled-resume、stop、fault 之一；
消费字节数固定为 registered pattern length。它不能直接切 CPU mode、
改 segment state、任意改 IP、改 mapping 或安装 host callback。

这种限制是优点。NXVM 当前不需要 handler，而未来 adapter 可用它进行
bounded probe，且 core 不会因此获得 runtime-specific identity。但历史链
要求完整的 mutable CPU/segment/stack context 与精确的多字节 IP-consumption
phase；当前 callback **不得**被扩展成承担这项职责。

## 边界发现

### 发现 A：没有尚待立即执行的 `vm/machine -> core` 迁移

最初的高置信候选（RTC mechanism、FDC mechanism、ATA PIO mechanism、
multi-media provider contract）均已跨过边界。余下的
`vm/machine/fdd.c` 与 `hdd.c` 是 in-memory image provider，带 direct file
open/persist helper；它们是 product-owned media implementation，并非 guest
controller。将它们迁入会把 pathname、mount、persistence 与 release policy
塞进 core，重建边界迁移原本要避免的“隐形 PC/AT product”。

### 发现 B：未完成的层是 mantle，不是 core firmware 或 DOS

今天 alternate consumer 可以直接使用 `core_machine`，但它不得不自行复刻
configuration ordering、provider ownership、reset、entry-plan application、
bounded run loop、stop decoding、diagnostic 与 teardown。VM session 已拥有
这些装配知识，但也携带 PC/AT profile 和 product choice；直接复用将违反边界。

M6 应创建 roadmap 已定义的窄、policy-free envelope：

```text
mantle session
  从 neutral descriptor 创建 core machine
  bind 已经选择好的 provider 与 optional device adapter
  freeze topology
  reset -> apply validated entry plan -> bounded run/pause/stop
  返回 typed observation / fault / stop record
  负责 ordered provider teardown
```

它不得接受 DOS path、drive letter、由 NXVM 选择的 firmware byte、runtime
service token 或 window/console handle。若 M5/M7 期间 NXVM 直接使用 core
较简单，可以保持；未来 adapter 必须使用 mantle 而不是 VM composition。

### 发现 C：历史 dispatcher bridge 需要一项未来 core extension

NT4 历史 NTDOS 证据将缺失 transaction 说明得很具体：原始 host-side
dispatch path 期待 live complete CPU state、segment/stack register、checked
guest memory 与明确的 instruction-byte ownership phase。当前 core 只暴露
有意部分化的 `#UD` snapshot，因而不足以调用 original dispatcher。

仅当可独立链接的 historical consumer 证明确有需要时，才应新增一项由
mantle 包装的**中性 core transaction**：

1. 在定义的 instruction boundary 停止；
2. 捕获完整、带 mode tag 的 CPU snapshot；
3. 在 transaction lifetime 内提供 checked guest-memory operation，绝不
   借出 raw RAM/SAS pointer；
4. 接受已验证的 complete state delta 与 staged memory effect；
5. 验证后原子提交全部 effect，包括文档化的 IP consumption；
6. real/protected/V86 transition 始终只由 core CPU executor 完成。

adapter 在 mantle 上解释其私有 request。core 不得获得 runtime service
number、historical name、BOP semantics 或“arbitrary switch mode”结果。这是
**未实现且必须以证据触发的 extension**，绝不可表述为当前 #UD hook 已经
支持 NTDOS。

### 发现 D：`core/platform` 已有正确的第一 primitive，但尚非完整 external-host surface

`core_platform_backing_resource` 是恰当的 opaque、synchronous contract：
size/read/write/flush/close 与 typed result，不包含 path。它可以为 media
provider 打基础，而无需让 controller 理解 host file。

当前 VM FDD/HDD implementation 仍直接使用 `STD_FOPEN`；这对 NXVM 当前
image policy 可以接受，但不是可复用的 external DOS namespace。未来
adapter 还会需要 rooted directory enumeration、file/stream operation、clock
sampling，及可能受控的 host-process hand-off。这些 contract 应严格拆分：

* `core/platform` 可定义 opaque handle、byte-buffer operation、cancellation
  与 typed result；
* mantle 可将已授权的 capability 绑定到一个 session；
* adapter/product 负责 root selection、pathname translation、DOS naming、
  sharing/locking、redirection grammar、allow-list 与 process policy。

没有任何证据支持将 VM 的 file command path 或 console UI 迁入 core。在
新增宽泛的 `file/directory/stream/clock` interface 前，应要求 concrete
owned 或 mantle-facing consumer 及 contract review；当前 roadmap 的延后决定
是正确的。

### 发现 E：VM profile 边界还有两处健康的粗粒度接缝

`vm_session_machine_devices_initialize_hdc` 在连接 generic core HDC 前，
显式验证 retained default PC/AT port/IRQ/DMA topology；RTC adapter 同样先从
profile 找 port，再应用 profile CMOS byte。这些 check 是 VM-specific，而
不是 core leakage。它们应通过规划中的 M7 profile work 变为 declarative，
而不是被移进 core。

唯一的粗糙处在于：非 NXVM consumer 当前还没有 neutral descriptor 来表达
等价 topology。mantle/M7 应提供该 descriptor；core 应继续接收已解析的
port/route，而不是持有 PC/AT default model。

## 与前两份报告的对照

| 早期结论 | 第三次审计处置 |
| --- | --- |
| Core 应从 primitives 扩展为 shared guest compatibility foundation。 | 已证实。M5 已将它落实，同时未迁移 product policy。 |
| RTC、FDC/HDC、media boundary 是强迁移候选。 | 已证实并由 T270--T278 实施。 |
| Generic ROM mapping 与 reset-bound prepared entry 属于 core。 | 已证实并由 T243--T246 实施。 |
| Firmware builder/IVT/BDA 应成为 generic core functionality。 | 已修正：只有 generic ROM routing 和 checked entry 属于 core；firmware construction/content 留在 composition。 |
| 需要 generic transition hook。 | 对 bounded neutral tracing 成立，但不足以承接 original historical dispatch；需要独立 future transaction。 |
| 不需要 mantle layer。 | 已过时。为防止 alternate consumer 导入 VM composition，最小 mantle 已成为必要层。 |
| file/directory/stream/clock/process contract 应立即进入 `core/platform`。 | 已延后并细化：opaque backing resource 已准入；较宽 contract 必须等待真实 neutral consumer 且严格排除 policy。 |

## 建议的执行顺序

1. **仅以自身证据收束 M5。** 不要因为外部 runtime 未来可能使用，重新打开
   已完成的 controller migration。
2. **将 M6 mantle 作为下一项边界工作。** 首个验收应为 VM-vocabulary-free
   owned fixture：create、bind、freeze、reset、apply entry plan、bounded run
   和 deterministic teardown。
3. **让 M7 profile declarative。** 把 VM 中保留的 PC/AT assertion 迁入
   profile descriptor/validator，而非 core default。
4. **保持 historical adapter research 隔离。** 用其确定 transaction 与
   host capability 的精确需求，然后分别提出经过审查的 core/mantle extension。
5. **最后才准入新的 platform contract。** 从最小、已经证明的 opaque
   capability 开始；DOS path、DOS error 或 host process policy 永不得进入 core。

## 未来边界修改的验收条件

任何未来准入都应证明以下全部事项：

* NXVM regression 仍经 VM composition 使用该 migrated mechanism；
* VM-free mantle fixture 能在不含 VM header 或 product vocabulary 的条件下
  bind 同一 mechanism；
* frozen topology 禁止晚期 device/provider/transition replacement；
* reset 与 entry application 在 validation failure 时不存在局部可见状态；
* 若准入 transition bridge，必须证明 full-state 与 memory effect 在验证后
  原子提交，并且不能直接改变 CPU mode；
* external runtime identifier、binary、service table、path 或 UI policy 均不
  进入 `core` 或 `mantle`。

## 最终决定

core 边界已经按当前证据允许的程度向外推进。`core` 现已是一套实用、可复用
的 machine foundation；`vm` 也大致只保留了它应保留的内容：一个具体、可
启动的 PC product。

不要再把 VM firmware、image-file 或 UI code 迁入 core。下一步应构建
mantle；完整 historical-dispatch transaction 和额外 host capability 必须
作为精确、单独取证后的未来 extension。这条路线既可支持 external research
adapter，也不会把 canonical NXVM core 变成伪装的 external DOS runtime。

## 证据引用

* `D:\home\repos.hobby\ntvdm64\docs\planning\status.md`
* `D:\home\repos.hobby\ntvdm64\docs\planning\roadmap.md`
* `D:\home\repos.hobby\ntvdm64\src\core\machine\machine.c`
* `D:\home\repos.hobby\ntvdm64\src\core\machine\entry_plan_interface.c`
* `D:\home\repos.hobby\ntvdm64\src\core\machine\rom_mapping_interface.c`
* `D:\home\repos.hobby\ntvdm64\src\core\machine\undefined_instruction_transition_interface.h`
* `D:\home\repos.hobby\ntvdm64\src\core\platform\backing_resource_interface.h`
* `D:\home\repos.hobby\ntvdm64\src\vm\composition\session\machine_devices.c`
* `D:\home\repos\ntdos64\artifacts\reports\legacy-root-milestones\M11-VM-to-core-NTDOS-compatibility-foundation.md`
* `D:\home\repos\ntdos64\artifacts\reports\legacy-root-milestones\M11-external-vdm-core-composition-boundary-reassessment-02.md`
* `D:\home\repos\ntdos64\docs\CORE-HISTORICAL-TRANSITION-BRIDGE-GAP.md`
