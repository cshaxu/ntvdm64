# S5 No-SM0 Closure Result

状态：2026-08-09，default-disabled link-only diagnostic。研究对象仍是原始
`BIOS[50h] -> MS_bop_0 -> DemDispatch(14h) -> demQueryDate`；本记录不表示该链可运行。

## 设计

`ntdos64-opennt-historical-transport-v2-owner-closure` 复用 v2 固定 record helper 的 date-only
入口和完整历史 `BIOS[]` / DEM 静态输入，但不链接
`local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c`。

新的 `historical_bios_owner_closure_bridge.c` 没有分配器、BOP handler、DOS service 或 host
policy；它仅从原始 `ccpusas4.c` 所拥有的 `Start_of_M_area` 取得 SAS aperture。`sim32.c` 是
该历史 SAS 初始化/终止 route 的 source owner。

## 结果

生成的 Ninja link line 不包含 SM0 foundation。链接失败时：

- duplicate symbol：**0**；
- 不同 undefined symbols：**282**；
- 未使用 `/FORCE:UNRESOLVED`、alias、weak symbol 或 local success/no-op replacement。

原始日志：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-v2-owner-closure-no-sm0-20260809.log
```

这证明先前的 5 个 duplicate symbols 确实只由 SM0 占位与同名历史 owner 共存造成；同时也
暴露出 SM0 过去遮蔽的大量完整 host-stack 闭包。282 不是“要写 282 个 shim”的任务表。

## 首批分类

| 首批 undefined | 来源判断 | 当前分类 |
| --- | --- | --- |
| `host_runtime_inquire`, `config_*` | 原始 `config.c` owner | 原始 source closure，尚未因 date trace 动态到达。 |
| `host_ica_lock/unlock`, `host_EOI_hook`, `WOWIdle` | 原始 `nt_eoi.c` owner | EOI/interrupt lifecycle closure，不可作 no-op。 |
| `GetConsoleInputWaitHandle`, `ReadConsoleInputExW`, `VDMConsoleOperation` | NT4 private console declarations | 未采证的 private host seam；不得提前创建 shim。 |
| `ExitVDM` | 原始 `vdmapi.h` declaration，对应 historical server side | WOW/VDM lifecycle seam；不属于 date trace。 |
| `host_com_*`, `host_lpt_close_all` | 原始 COM/LPT owner groups | 完整设备 lifecycle closure。 |
| `CreateFileOem`, `Find*Oem`, `Delete*Oem` 等 | 原始 `oemuni` owner | 文件 namespace/redirector closure；不等同于当前受限 F1 facade。 |
| `EDL_fast_bop`, `LIM_*`, `c_VirtualiseInstruction` | CCPU/EMS/BOP contract | 仍是 source-owner 未定位或未采证的 stop conditions。 |

## 按引用 object group 聚合

下表按 linker 日志中的“引用此 undefined 的 object group”统计不同 symbol 数。它衡量的是完整
历史静态链接的宽度，不能推断当前 date trace 的动态执行宽度。

| object group | 不同 undefined symbols | 含义 |
| --- | ---: | --- |
| host display/input | 75 | 图形、鼠标、fullscreen 与显示 host owner 的完整闭包。 |
| DEM | 58 | `apfnSVC[]` 完整服务表与文件/进程/redirector 依赖。 |
| video provider | 45 | video globals、C-VID/显示更新的静态闭包。 |
| comms provider | 21 | COM/LPT host backend。 |
| event host | 19 | private console、键盘转换、VDD 与终止协作。 |
| disks provider | 11 | FDC/HDC host backend。 |
| reset shell | 8 | session、console、PIF 和 lifecycle。 |
| nt_bop | 8 | selector 50 之外的原始 BOP entry 依赖。 |
| timer | 7 | tick/heartbeat/RTC/interrupt lifecycle。 |
| 其余 group | 1-5 | BIOS/system/support/SAS/EMS 的边缘闭包。 |

因此最有价值的下一份采证不是接入一个大 host group，而是证明 date trace 的动态调用没有进入
这些 group；随后才针对 NTIO prefix 实际到达的第一个新 owner 扩展。

## 决策

1. 后续只对原始 date trace 实际抵达的 caller 恢复 owner closure。
2. 先审计 no-SM0 target 中 bridge 本身所需的最小 SAS/register 前置，不为完整 `BIOS[]` 的
   静态表输入增添 runtime 行为。
3. 只有到达 console、OEM filesystem、COM/LPT 或 `ExitVDM` 的真实 trace，才分别建立现代
   host capability 研究项。
