# S5 Full no-SM0 Owner / Seam Ledger

状态：2026-08-09。该台账记录 `BIOS[]` 的全静态 source composition 在移除 archived
SM0 adapter、恢复原始 `sim32.c` SAS owner 后所暴露的宿主闭包。它不是 runtime 准入，
也不授权为了让链接器安静而加入 local replacement、no-op 或私有 API 猜测。

## 固定输入与读法

诊断 target 为 `ntdos64-opennt-historical-transport-v2-owner-closure`：

- 默认关闭；不在 `ntdos64-run`、CTest 或任何 runtime 链接图中；
- 不使用 `/FORCE:UNRESOLVED`；
- `SIM32`、`_X86_` 下使用原始 `host/src/sim32.c`，而非 SM0；
- 最新日志为
  `artifacts/build/current/opennt-r2-bridge-ninja-i686/s5-v2-full-owner-closure-no-sm0-original-sim32-20260809.log`；
- 该日志共有 220 个未解析符号，均按 source reference 逐项计数。它表示完整 `BIOS[]`
  静态表带入的所有原始 source unit 的闭包宽度，不能被解释为 `50h/14h`、`50h/11h` 或
  NTIO prefix 的动态必需集合。

## 当前分组

| source family | 引用数 | 当前结论 | 后续动作 |
| --- | ---: | --- | --- |
| `dos/dem` | 53 | 一部分由 OpenNT host backend 提供；另一部分是 NT 内部 API 或 DOS/Win32 进程与文件语义。`demLoadDos` 的 `11h` 已有受控 trace 证据，但不证明其他 DEM 服务可用。 | 先恢复可由原始 host source owner 提供的最小动态调用链；私有 NT API 逐个登记为 seam，不替换 SVC。 |
| `base/video` | 45 | 视频 core 通过 `Video` 向量依赖完整绑定。跨版本证据 E-VEC-001 明确此 binding 未决。 | 冻结；不得令 `Video=C_Video`、假 handler 或 host shim 来降低 unresolved 数。 |
| `base/comms` | 21 | COM/LPT core 的 owner 可追至 `nt_com.c`、`nt_lpt.c`、`nt_ntfun.c`，属于原始 host backend。 | 只在真实串口/并口路径首次抵达时，以完整 owner group 恢复；不为 date/loader trace 提前加入。 |
| `base/disks` | 11 | floppy/fixed disk core 的 host owner 是 `nt_rflop.c` 与 `nt_fdisk.c`。其中 `HostFloppyReset`、`HostFdiskReset`、`pFDAccess` 可直接定位。 | 作为真正软盘/硬盘 trace 的 owner-first 候选；host A:/B: policy 需另行 caller-first 设计，禁止固定 inactive。 |
| `base/platform` | 14 | 含配置、ICA、ROM、PIT/idle、reset 等 PC/AT 与 host coordination 接缝。`host_runtime_inquire` 来自 `host/src/config.c`；ICA lock/unlock 来自 `nt_eoi.c`。 | 分别以动态 caller 抵达为准；不能把 config/eoi 拆成自写布尔量或锁替身。 |
| `host` | 33 | timer、fullscreen、mouse、reset、graph、session 等宽 source unit。很多下游仍依赖已失去的 private console/VDM API。 | 不因静态闭包而接入；先完成原始事件/console caller 的动态采证。 |
| overlay / CCPU / EMM | 43 | 包含已分类的 instrumentation、受限 reconstruction，及历史链本身的未恢复依赖。 | 保持既有治理分类；不得把 overlay 当作新的 host owner。 |

## 已定位的原始 owner

下列项目是“可以在固定 OpenNT 输入中找到定义”，而不是可以立即接入 runtime：

| unresolved family | 原始定义 | 边界判断 |
| --- | --- | --- |
| `host_runtime_inquire` / `host_runtime_set` | `base/mvdm/softpc.new/host/src/config.c` | 历史配置 owner；配置字典、资源与 profile 仍必须整体审计。 |
| `host_ica_lock` / `host_ica_unlock` | `base/mvdm/softpc.new/host/src/nt_eoi.c` | 历史 PIC/interrupt coordination owner；不可用自写 mutex 代替。 |
| `host_com_*` / `wow_com_*` | `base/mvdm/softpc.new/host/src/nt_com.c` | COM backend；只在 COM I/O trace 首次抵达时恢复。 |
| `host_lpt_*` / printer helpers | `base/mvdm/softpc.new/host/src/nt_lpt.c` | LPT backend；同样不能作为纯链接填充物。 |
| `HostFdiskReset` / `pFDAccess` | `base/mvdm/softpc.new/host/src/nt_fdisk.c` | 固定盘 backend 与 DOS floppy-access 计数共享状态。 |
| `HostFloppyReset` | `base/mvdm/softpc.new/host/src/nt_rflop.c` | 软盘 backend；应支持真正可访问的 host A:/B:、USB 或虚拟软驱，具体策略单独合同化。 |
| `KeyMsgToKeyCode` | `base/mvdm/softpc.new/host/src/nt_keycd.c` | 键盘消息转换 owner；不允许假 scan code。 |
| `host_nls_get_msg` | `base/mvdm/softpc.new/host/src/nt_nls.c` | NLS/error owner；接入 `nt_error.c` 会同时引入 session、timer、console 依赖，现阶段保持停留。 |

## 已确认的现代宿主 seam

这些名字在历史 caller 中存在，却不等价于普通 Win32 public API；必须先有到达它们的动态
trace，才能讨论窄 facade：

| import | 历史 caller | 当前处置 |
| --- | --- | --- |
| `GetConsoleInputWaitHandle`、`ReadConsoleInputExW`、`VDMConsoleOperation` | `host/src/nt_event.c`、`nt_det.c`、`nt_mouse.c` | private console seam；详见 `S5-EVENT-HOST-SOURCE-MATRIX.md`。 |
| `ShowStartGlass` | `host/src/nt_reset.c` | private user API；不可伪造成功。 |
| `ExitVDM` | `nt_reset.c`、`nt_event.c` | historical VDM lifecycle seam；`nt_term.c` 仅是目录参考，未列于 NT4 `host/src/sources`，不能据此建立替代 lifecycle。 |
| `NtVdmControl`、`NtQueryDirectoryFile`、部分 `Rtl*` | `dos/dem` | NT 内部 API seam；不得将其改写成自定义 DOS service。 |

## 执行顺序

1. 保持 `50h/14h` 与 `50h/11h` 作为有限 trace 证据，先恢复“可调用的 no-SM0 原始
   dispatcher closure”，而不是扩展 trace 的 service 覆盖。
2. 对 NTIO 的实际 prefix 执行做只读指令/状态记录，确定第一次抵达的 host owner。只有该
   owner 进入 owner-first recovery log。
3. 对每个首次抵达的原始 owner：单独建立 default-disabled object group，记录它新增的
   downstream symbols；不链接 archived SM0，不写 shim。
4. 当且仅当 downstream 已由原始 source 无法提供、且真实 trace 已到达时，定义一个固定
   输入/输出/失败语义的 `compat/host` seam；对 private console、VDM lifecycle、NT native
   API 分别处理。
5. 视频、事件循环、COM/LPT、FDC/HDC、NLS 不因 full-table 静态引用而提前实现。

这条顺序把“完整历史 host stack 的最终目标”和“当前 guest 启动的实际阻塞点”分开：前者
需要所有 owner closure，后者只允许由 NTIO 动态证据逐层打开。
