# CCPU40 与 V86 guest 契约一致性审计

## 候选包

本候选包在当前 M0 T420 完整收口后首先准入。它不是新的 host
实现、WOW16 修复或 guest 介质修改；它产出一份可关闭的、以原始
OpenNT V86 monitor 语义为基准的 CCPU40 host/guest 契约账本。其目的
是判定当前 Win32/x86 CCPU40 worker 是否向原始 DOS、DOSX/DPMI 与
WOW16 guest 提供了它们实际依赖的服务形状，并将 `WRITE.EXE` 无法
启动的已证实边界交给唯一的原始 owner 包恢复。

## 问题与判据

原始 guest 并不依赖“内核代码必须在场”，而依赖原 V86 monitor
呈现的可观察契约。CCPU40 取代了原 CPU30/内核 VDM monitor 的部分
执行角色，故必须逐项证明以下关系，而不能以 `COMMAND`、`MEM` 或
`EDIT` 的单项成功推断 WOW/DPMI 已等价：

```text
guest 发起点
  -> 原始 OpenNT V86/VDM owner
  -> 当前正式 x86 构建中的 CCPU40 / MVDM / adapter provider
  -> 返回、失败与清理语义
  -> 运行证据或明确的未达边界
```

审计范围为已选入产品的原始 DOS、`dpmi`/DOSX、`dpmi32`、`wow32` 与
其可达 CCPU40/SoftPC host 边界：BOP、特权指令/端口 I/O、异常与中断
反射、real/V86/protected-mode 转换、descriptor/table register、guest
memory/A20/EMS/XMS/DIB、异步事件以及 worker 退出清理。它不把 CPU30
重新引入产品，也不将 CSRSS、内核 VDM、Win32k 或 USER/GDI server
递归导入 standalone worker。

## 顺序与 S 切分

| S | 范围 | 退出条件 |
| --- | --- | --- |
| S1 | 冻结实际 x86 构建图与原始 V86 contract 面。枚举所有可达 BOP、模式转换、fault/interrupt、I/O、memory 与 lifecycle 边界，并为每项登记原始 guest caller、原始 owner、当前 provider、结构/寄存器 ABI 与 source/profile disposition。 | 账本双向闭合：每个 reached guest edge 有当前接收方；每个 reached CCPU40/adapter handler 有 guest 可达性或明确 profile 排除。 |
| S2 | 对 S1 的可疑或有限 binding 项逐项做原始源代码与当前路径对照；设计最小、默认关闭的边界 trace，验证 IP 推进、栈/寄存器、失败码、descriptor/table 与内存 lease 生命周期，不向指令执行循环增加常驻日志。 | 每项归为已等价、有限且有证据的适配、缺失/不等价，或不可达；没有“凭运行成功推断”的未分类项。 |
| S3 | 在不改变 guest 介质和功能语义的前提下，执行分层证据矩阵：`COMMAND`、`MEM`、`EDIT`、DOSX/DPMI 激活以及已选 `WRITE.EXE` 启动路径。记录实际命中边界、未命中能力与首个确定阻断点。 | 每个工作负载有可复现的命中集合；WRITE 失败若仍存在，归因到一个具体 owner/interface，而非笼统称为 CCPU 或 guest 问题。 |
| S4 | 复核账本与实现差异：找出由当前 CCPU40/adapter 自主实现、空桩、默认值或重复策略造成的疑点；为每一项指定唯一后续 owner、原始恢复梯级、最小修复边界与回归矩阵。 | 无无主缺口；可立即恢复的原始代码与必须保留的有限 adapter 明确分开；不在本审计包内实施跨包修复。 |

## 交付物与验收

- 一份索引化证据账本，含每条契约的输入、原始 caller/owner、当前
  provider、ABI、调用/返回与清理、静态/运行证据、置信度与后续 owner。
- 一份当前镜像 diff、adapter 与自主实现的分离清单；仅列出实际改变
  契约的项，格式差异不计为语义风险。
- 最小 trace 的开关、位置和撤除条件设计；若为验证不得不加入代码，
  它必须默认关闭、位于既有边界 owner，且在该 S 结束时移除或转为
  已登记的测试工具，不能进入镜像逻辑。
- 固定 x86 build、直接与交互式 `COMMAND`/`MEM`/`EDIT` 回归，以及
  DOSX/DPMI 和 `WRITE.EXE` 的真实运行观测。未能自动化的 Console
  观察明确标为限制，不能伪称通过。

本包关闭时只声称“契约已审计并有唯一修复归属”，不声称 `WRITE.EXE`
已运行成功。若账本证明当前失败直接由一个已选 package 的漏接边界
造成，依赖优先规则将该完整 owner 包提升到后续队首；不得在 trace
观察中临时重写 CCPU、DOSX 或 WOW 策略。

## 非目标与停止条件

不改变 guest 二进制、不引入 CPU30、kernel VDM、CSRSS/CSR transport 或
私人 GUI 替代层；不以 NTVDMx64 补丁批量替换源代码。发现需要新 guest
介质、内核专有状态、跨包行为修复、既有 `COMMAND`/`MEM`/`EDIT`
回归，或当前 T420 尚未收口时，停止并将结果记录为明确 handoff。
