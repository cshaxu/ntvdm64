# S5 Cross-Bitness Transport Cut

状态：设计准入，2026-08-09。此文规定 S5 的下一项可实施工作；它不宣称
`ntdos64-run` 已可执行 guest，也不授权将任何 fixture 接入默认 target。

## 结论

S4 monitor 与历史 i386 闭包之间不能只增加一个“调用 BIOS”的控制通道。
现有 S2 v1 fixture 和 S4 fixture 各自拥有 SAS RAM；对 `50h/11h` 而言，原始
`demLoadDos()` 会把 BYOB `NTDOS.SYS` 直接写到 `DI:0000`。若两端不共享同一
guest byte aperture，历史 DEM 虽可返回，software monitor 却看不到载入结果。

因此 S5 的最小 transport 必须同时有两个严格分离的面：

```text
x64 software monitor
  | fixed-width control records
  v
i386 historical helper
  | original BIOS[50h] -> MS_bop_0 -> DemDispatch -> DEM
  |
  +---- one shared ordinary-SAS byte aperture ----+
```

控制 record 绝不携带历史函数地址、host pointer、CRT object、异常或 DOS service
implementation；共享 aperture 只包含 guest-visible ordinary bytes，绝不包含
SoftPC metadata、`BIOS[]`、`Video` 或 host object。

## 已证实的来源事实

| 事实 | 原始 owner / 证据 | 对 transport 的约束 |
| --- | --- | --- |
| BOP selector `50h` 在原始 `MS_bop_0` 读取 CS:IP 所指的 service byte，分派后推进 IP 一个字节。 | `base/mvdm/softpc.new/host/src/nt_bop.c`；S5 return contract | monitor 消费 `C4 C4 selector`；helper 收到的 IP 必须仍指向 service byte，回包中的 IP 是原始 owner 更新后的值。 |
| service `11h` 是 `demLoadDos`。 | `base/mvdm/dos/dem/demdisp.c` | 不得在 monitor 或 transport 内重写该 service。 |
| `demLoadDos` 从 `DI:0000` 取得目标地址，并将 `ntdos.sys` 连续读入该地址。 | `base/mvdm/dos/dem/demmisc.c::demLoadDos` | 此首个 S5 service 至少需要 DI、CS:IP 以及共享 guest bytes。 |
| `sas_init` 通过 `host_sas_init(size)` 获取 `Start_of_M_area`；`sas_term` 仅在 `host_sas_term() != NULL` 时调用 `free(Start_of_M_area)`。 | `base/mvdm/softpc.new/base/ccpu386/ccpusas4.c` | shared-SAS provider 是历史 host allocation seam；它必须在 terminate 时返回 `NULL`，使原始 SAS 不会释放映射视图。 |
| 既有 S2 v1 只传 AX/CX/DX/CS/IP，且只准 `50h/14h` date trace。它还自行持有 SAS/CCPU 生命周期，并写入私有 SAS backing。 | `local/compat/host/historical_bios_bridge_v1.[ch]` | S2 v1 不能扩展成 S5 通用 transport；它与 shared aperture 不可组合。任何后续 record 必须先通过 `S5-SHARED-APERTURE-TRANSPORT-ADMISSION.md` 的单一 backing、历史 owner 与原子提交准入。 |

## V2 的窄范围

### 数据面：shared ordinary SAS

1. x64 parent 创建每 session 唯一命名的 Windows file mapping，当前 S5 cut
   固定为 2 MiB ordinary guest aperture；mapping 名称、pipe 名称与随机 session
   token 均不得出现在持久化 trace。
2. i386 helper 显式打开同一 mapping；由新的、独立登记的 historical-host
   `host_sas_init` provider 将其基址交给原始 `sas_init`。它仅接受首次的精确
   2 MiB 请求，任何二次、变长、越界或失败 mapping 都返回失败。
3. helper 的 `host_sas_term` 释放其 view/handle 后返回 `NULL`。这是
   `ccpusas4.c::sas_term` 所要求的所有权表达，不是泄漏规避技巧。
4. `memory_type`、ROM classification、SAS function-vector 和 CPU/DEM globals
   留在 helper 私有地址空间；v2 不把它们映射给 x64。
5. parent 只能在 helper 空闲、且没有未完成 control request 时读写 aperture。
   初版每次 request/reply 是同步的；没有并行 DMA、IRQ 或异步 host callback。

### 控制面：one request at a time

1. 使用 parent 启动的 i386 helper 与有方向的、消息边界明确的 Windows named
   pipe。该选择是现代 process seam，不是 OpenNT DOS/BIOS 语义。
2. v2 request/reply 均为 packed fixed-width C records，含 ABI version、结构大小、
   session token、sequence number、selector、service byte、完整 visible
   real-mode register snapshot 与显式 disposition。不得含 pointer、变长字段或
   guest-image bytes。
3. helper 只在自己的 i386 地址空间验证 record，并按原始 setters 装入 state，
   写 service byte 到 shared SAS，调用原始 `BIOS[selector]()`，然后通过原始
   getters 产生 reply。它不实现 selector switch、BOP decoder 或 DEM service。
4. v2 初始 allow-list 只有已经由 source 和 fixture 证明的 `50h/14h`。只有在
   共享 SAS alias fixture 通过、BYOB resource root 由原 `DemInit` 建立且
   `demLoadDos` 正向/负向测试齐备后，才能增加 `50h/11h`。
5. 若 record、sequence、session、mapping、selector、service 或 helper state
   无效，helper 返回 typed refusal；parent 停止 monitor 并产生 redacted stop。
   不得尝试另一 selector、合成 FLAGS、重放请求或继续 guest。

## 寄存器与原子性

v2 在 ABI 文档和 C header 同时固定全套可见 real-mode fields：AX/BX/CX/DX,
SI/DI/BP/SP, CS/DS/ES/SS, IP 和 FLAGS。它们是 transport snapshot，而非对
protected-mode state、descriptor cache、CRx、A20 或 interrupt state 的授权。

helper 的处理顺序必须为：完整预检 request -> 装入全部可见 state -> 调用一次
原始 BIOS entry -> 读取全部可见 state -> 构造并验证完整 reply -> 写回 reply。
只要 precheck 或历史调用前失败，shared aperture 与 monitor state 均不变；历史
调用开始后的 host/DOS 可见副作用按原始 owner 处理，不能由 transport 伪造回滚。

## 分阶段验收

1. **T-S5.1 control only**：x64 parent 启动 i386 helper；`50h/14h` 跨进程到达
   原 `BIOS[50h] -> MS_bop_0 -> DemDispatch`，并验证 request/reply version、
   bad token、bad sequence、unsupported selector 的 fail-closed 行为。
2. **T-S5.2 shared SAS**：在未执行 DEM load 前，parent 写入的指定 bytes 可由
   helper 的原 `c_GetLinAdd` 读到，helper 写入的指定 bytes 可由 parent 读到；
   mapping 外访问和重入请求均失败。此项不加载 DOS 文件。
3. **T-S5.3 original `50h/11h`**：在受限 BYOB directory 下，helper 先经原
   `DemInit`，再由 v2 request 到达原 `demLoadDos`。parent 只断言 shared aperture
   的 `0x8b00` 前缀与经验证 NTDOS input 相符，并记录原 IP 从 `0478h` 到
   `0479h` 的变化。
4. **T-S5.4 prefix execution**：将 T-S5.3 接到实际 S4 monitor 的 NTIO prefix。
   第一个未实现指令、memory mapping、I/O 或 event 产生 typed stop；这才是
   下一轮按 caller 增加能力的依据。

所有四项均必须是 `EXCLUDE_FROM_ALL`，不得让 `ntdos64-run`、默认 CTest 或任何
release target 依赖 helper。

## 明确不做的事

- 不把 CCPU/C-VIDC `Video` composition 当作 NTIO 的动态设备语义输入，也不以它
  解释任何 selector-50 service 的 caller contract；它保留为 R1 考古分支。
- 不引入 V86、WHPX、HAXM、Bochs、PCjs、NTVDMx64 或 `ntvdm64` runtime code。
- 不建立“任意 selector / 任意 BOP / 任意 guest-memory RPC”。
- 不新增 IVT/BDA、ROM、PIC/PIT、磁盘、键盘、显示或 DOS SVC 的合成实现。
- 不将 shared SAS 误称为完整 machine memory；其仅是当前 real-mode ordinary
  byte aperture，后续 mapping 语义必须由 T-S5.4 的真实 stop 触发。

## 下一项实现工作

先实现 T-S5.1 的独立 parent/helper control fixture 和 v2 record header；它不得
携带或使用 BYOB file path。T-S5.2 的 shared-SAS provider 只能在 T-S5.1 通过后
开始，因为它会首次引入一个可运行的 modern host seam。

**当前静态闭包限制（2026-08-09）：** 以上“不作为动态语义输入”的界线不等于
完整 `BIOS[]` helper 可以忽略 generic `Video`。原 `bios.c` 的完整 table 静态带入
video provider，C-VIDC `ev_glue.c` 对 81-slot generic `Video` 保持未解析引用；
`C_Video` 只解决 generated provider，不是该 storage/initializer 的替代。故在找回
`ccpu486.lib`/map/generator，或通过独立的 profile-aware reconstruction admission 前，
T-S5.1 的 clean-link helper 仍停在 source-closure 采证阶段。不得以 host shim、
`Video = C_Video`、short-table reuse、缩小 BIOS table 或 `/FORCE:UNRESOLVED` 跨越此限制。

## 已完成的 layout 证据

2026-08-09 已加入 `refs/archive/reconstruction/transport/historical_transport_v2.h` 与
默认关闭的 layout fixture。它在下列两个彼此独立的编译域中均构建、运行并返回
`0`：

- root MinGW x64：`ntdos64-historical-transport-v2-layout-test`；
- OpenNT island clang-cl i386：
  `ntdos64-opennt-historical-transport-v2-layout-fixture`。

两端均以编译期断言和运行期检查确认 state/request/response 分别为 44/68/72 byte。
这只证明 wire record 不受当前两种 target ABI 的布局差异影响；它不证明 named-pipe
control transport、shared SAS、历史 BIOS 调用或 NTIO 执行已经存在。

## T-S5.1 closure result

### 2026-08-09 composition boundary

已验证 default-disabled v2 helper 的 generated Ninja link line 确实包含首批原始
error/reset/timer/display-input owner object。故失败不是 CMake 传播问题：archived
trace adapter `ccpu_monitor_sm0_foundation.c` 导出了五个已由原始 `nt_error.c`、
`nt_mouse.c` 与 `nt_timer.c` 拥有的 symbol。

该 adapter 不得与这些原始 source owner 共存于任何 historical source-closure target。
`nt_term.c` 虽保留在目录中，但未列入原始 `host/src/sources` 的 `SOURCES=`，因此不再是
本研究 CMake 中的 historical owner target。不得以 local declaration 或 no-op replacement
把它重新接入。证据与退出规则见
`docs/research/ccpu-bios-closure/S5-ARCHIVED-ADAPTER-OWNER-COLLISION.md`。

v2 helper source 和 x64 pipe parent fixture 已作为 `unadmitted`、default-disabled
trace candidates 登记。2026-08-09 的首次 i386 helper link **未通过**。初次完整
register apply/return 尝试会暴露 `nt_event.c`、sound、video 和 system provider 的
历史 host closure；当前精确未解析项包括 `host_runtime_inquire`、`GetPerfCounter`、
`DisplayErrorTerm`、`VDMForWOW`、`ThreadInfo`、`TerminateVDM`、console input 和
mouse/display owners。

该结果不授权加入 `/FORCE:UNRESOLVED`、no-op callback 或任意 host shim。现有 S2 v1
date fixture 的成功仍只证明其较窄 register/accessor closure；它不能证明 v2 的完整
visible-state 传递已可运行。随后将 helper 缩为 date-only 的 AX/CX/DX/CS/IP 投影后，
链接失败集合没有减少：原始 `bios.c` 的完整 `BIOS[]` 表对所有 entry 的静态函数引用
已经足以拉入这些 owner。因此不能把任何 unresolved 归因于某一 register accessor，
也不能把缩窄 record 当成有效逃逸路径。下一步是完成静态 table、动态 selector 和 host
owner 的三栏 ledger，再按被真实路径证明的 owner 恢复 T-S5.1。

为定位该边界，T-S5.1 的 date-only helper 当前只将 AX/CX/DX、CS 与 IP 写入历史
state，并只从原 owner 读取这些字段；BX/SI/DI/BP/SP、DS/ES/SS/FLAGS 按 request
原样回显，绝不声称已被原 CPU owner 应用或读取。完整 visible-state apply/return
仍是 T-S5.1 closure 的待恢复子项，而非通过 record 字段存在即可假定。

补充：helper 与既有 S2 target 的 generated Ninja link line 使用同一批 direct
provider objects；旧 v1 EXE 未重链不能证明当前完整 closure。将 provider objects
改为 archive 虽可避免未被引用的 member，却不能绕过原 `bios.c` 的完整 `BIOS[]`
静态 table 对各 BIOS entry 的引用。因此 archive extraction 只能作为依赖采证手段，
不是逃避真实 BIOS/host closure 的实现方案。
