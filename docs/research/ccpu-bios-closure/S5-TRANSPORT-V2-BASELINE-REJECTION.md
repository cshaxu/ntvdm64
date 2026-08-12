# S5 Transport V2 Baseline Rejection

状态：已审计；`historical_transport_v2` 仅保留为 default-disabled trace 对照，2026-08-09。

## 审计对象与可复现结果

对象为：

- `src/reconstruction/transport/historical_transport_v2.h`；
- `src/opennt/local/compat/host/historical_bios_bridge_v2.c`；
- `src/opennt/local/trace/fixtures/historical_transport_v2_helper.c`；
- 两个 root `historical_transport_v2_*` fixture。

2026-08-09 重新构建显示：root 的 record-layout 与 named-pipe caller 可编译；i386 helper 的
完整 source-closure 链接失败，`lld-link` 报告下列 duplicate owner：

```text
_host_error
_host_mouse_install1
_host_mouse_install2
_host_timer_event
_host_TimeStamp
```

根因是 helper 同时带入 archived
`local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c` 的 fail-closed 占位和原始
host owner。不得使用 `/FORCE`、符号重命名、删除原始 owner 或新增 no-op shim 使它链接。

## 为什么 v2 不是重建 monitor 的接口

即使上述链接冲突不存在，v2 也不满足 reconstructed-monitor 到 historical-host 的准入合同：

1. `initialize` 直接拥有 `sas_init`、`c_cpu_init`、`c_cpu_terminate` 和 `sas_term`，把 CCPU
   probe lifecycle 错当作 NT4 x86 `MONITOR` lifecycle；
2. 它只设置/读回 AX、CX、DX、CS、IP，然而 record 声称携带完整 general/segment/FLAGS state；
3. 它在历史调用前直接把 service byte 写到 CCPU SM0 RAM，并固定接受 `50h/14h`；这是 trace
   convenience，不是来自 reconstructed monitor 的 general selector handoff；
4. 它没有独立的 monitor-owned state backing，没有 response patch validation，也没有 host result
   到 monitor state 的 atomic commit；
5. 它调用前的 CCPU state 写入和 memory 写入无法在 later validation failure 时保持原样。

因此 v2 不得扩展到新的 selector/service，也不得作为 S4 continuation、BOP dispatcher、DEM
adapter 或 runtime bridge。它只保留为“旧 CCPU probe 曾可表达的 date trace wire format”对照。

## 后继 facade 的不可协商合同

任何后继记录或 facade 必须同时满足：

- 由 reconstructed-monitor session 拥有 state 与 shared aperture；不得调用 CCPU lifecycle、使用
  SM0 RAM 或依赖 `VdmTib`；
- request/response 使用 fixed-width values，无 pointer、handle、CRT ownership 或 DOS binary
  bytes；session token 与 monotonic sequence 必须匹配；
- 只接受已由 monitor 识别的 selector；facade 不读 service byte、不改 IP、不调用
  `DemDispatch`、不定义 DOS return；
- invocation 前验证 record、session、sequence、real-mode state 与 ordinary-RAM aperture；失败时
  不改变 monitor state 或 aperture；
- 历史调用结束后，先验证完整 response/state patch，再一次性提交到 monitor；不允许 mode、CRx、
  descriptor cache、A20、IRQ 或任意 FLAGS 写入；
- 首个可运行 selector 必须使用完整原始 `BIOS[]` owner closure。缺失的 generic `Video`/
  device/event owner 仍是独立硬门槛，不能以 reduced table、CCPU 短表或 fabricated handler 绕过。

这是一份设计拒绝记录，不授权新 helper 或 host shim。后续工作应先完成完整 `BIOS[]` 的 static
owner closure 采证和 generic Video 的独立准入判断，然后才可为 monitor facade 建立
compile-only 正反 fixture。
