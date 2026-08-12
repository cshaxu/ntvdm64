# SoftPC CCPU 缺损接口恢复记录

状态：`c_effective_addr` 已重建并验证；`c_sas_touch` 与
`c_VirtualiseInstruction` 保持 fail-closed trace guard，2026-08-08。

## 范围与证据

本记录只处理 `base/mvdm/softpc.new/base/ccpu386/sascdef.c` 导出的三个
源缺损符号。检索的 NT4/OpenNT、OpenNT-4.5、Win2000SRC 与 XP 源快照中均未找到
这三个符号的原始定义；NTVDMx64 的同名代码是诊断性 `int 3` stub，不能作为运行时
实现。Bochs、PCjs 等仅可帮助校验 x86 语义，不进入该实现或构建图。

| 符号 | 原始 ABI | 当前结论 | 运行时状态 |
| --- | --- | --- | --- |
| `c_effective_addr` | `(IU16 segment, IU32 offset) -> IU32` | 可由 CCPU 已有的 mode、selector-table 与 descriptor reader 精确定义 | 已重建 |
| `c_sas_touch` | `(IU32 address, IU32 length) -> IU8 *` | 仅见 SAS vector 槽位声明；没有已证实的 live caller 或原始实现 | trace guard |
| `c_VirtualiseInstruction` | `(IU32 eipInRom, IUH size, IU32 linearAddrOrPort, IU32 dataIn) -> IU32` | 仅见 SAS vector 槽位声明；现有 `c_IOVirtualised` 是不同的、已有实现的 V86/特权 I/O 辅助 | trace guard |

## 已重建：`c_effective_addr`

实现位于：
`src/opennt/overlay/base/mvdm/softpc.new/base/ccpu386/c_effective_addr.c`。

它严格限于原 CCPU vector 的地址计算职责：

1. 非保护模式或 V86 模式：返回 `segment << 4 + offset`；A20、物理寻址与 SAS
   路由仍由后续 CCPU/SAS owner 处理。
2. 保护模式：调用原 CCPU `selector_outside_table()`，随后调用原
   `read_descriptor_linear()`，返回 descriptor base 加 offset。
3. 不在 GDT/LDT 范围内的 selector 返回 `UINT32_MAX`。它不自行注入 `#GP`，因为
   该 vector 没有 fault 参数，且调用方保留原有验证/fault 职责。

实现被单独的 default-disabled object target
`ntdos64-opennt-ccpu-effective-addr-reconstruction` 选择。它没有 BOP、BIOS、DEM、
设备、host policy 或 DOS 语义。

已验证：

- `ntdos64-opennt-ccpu-effective-addr-fixture` 覆盖 real、V86、GDT、LDT 与无效
  selector，退出码为 0。
- `ntdos64-opennt-original-selector50-trace-fixture` 重新构建并退出码为 0，证明
  overlay object 未破坏现有的
  `CCPU -> BIOS[50h] -> MS_bop_0 -> DemDispatch` 受控历史链。
- `SOURCE-MANIFEST.json` 将该文件标记为
  `local-softpc-reconstruction-overlay` 并记录 SHA-256。

## 未重建的两个 SAS vector 槽位

`c_sas_touch` 与 `c_VirtualiseInstruction` 当前不能因“链接需要一个符号”而得到
猜测性的默认实现。它们的返回值均可能被调用方当作 guest-visible 指针或执行结果；
将其设为返回 RAM 指针、返回零，或模拟 ROM 指令都会在没有调用证据时改变历史行为。

当前 `local/shim/ccpu_unresolved_vector_guard.c` 只用于默认关闭的诊断目标：记录 ABI
参数与返回地址后终止。它不链接进正式 runtime，也不产生 BOP/SVC、设备或 DOS 返回值。

下一个准入条件不是“去掉 unresolved symbol”，而是以下之一：

1. 找到确实执行这两个 vector 的原始调用点，并能由相邻源代码确定成功、失败和
   ownership；或
2. 在真实 CCPU/SoftPC 初始化或 NTIO prefix trace 中由 guard 记录一次调用，随后围绕
   该具体地址、长度/指令和预期消费方构建最小夹具。

在此之前，SoftPC 继续沿不触达这两个槽位的历史链推进；guard 是正确的 stop，而不是
实现缺口被掩盖。
