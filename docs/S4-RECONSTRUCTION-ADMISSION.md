# S4 Reconstruction Admission

## 2026-08-09 NTDOS Post-Drive Increment

准入一个普通 real-mode CPU 语义：`TEST byte ptr [disp16], imm8` 的受检查 ordinary-RAM
读与 flags 更新。其唯一新增证据是固定、source-built `NTDOS.SYS` 的 `DosInit` 正常启动
分支：原始 `50h/0Fh` 返回后，`F6 06 80 14 01` 测试 `SCS_ISDEBUG`，再到达下一条原始
`50h/1Bh` BOP。该实现不读取 host state、不理解 DOS-data 名称、不实现任何 BIOS/BOP/DEM
service；未匹配的 `F6` form 继续 typed fail-closed。

root default-disabled target `ntdos64-reconstructed-monitor-s4-test` 已以代码段
`CS=1000h`、数据段 `DS=0` 的分离 aperture 执行固定字节，确认 `A2 0046` 的真实
guest data write 不会覆盖 instruction bytes，并在原始 `50h/1Bh` bridge refusal 停止。
这不表示 `0Fh` 或 `1Bh` 已由该 test 执行。详见
`docs/research/ccpu-bios-closure/S5-NTDOS-POSTDRIVE-MONITOR-SLICE.md`。

状态：实现准入，2026-08-09。

## 新代码分类

S4 的 software monitor 不是 OpenNT overlay：NT4 x86 原始产品的 execution path 是 V86，
而 x64 user-mode 需要独立重建 CPU mechanics。它也不是 archive 中早期的 BOP/DEM
prototype。为避免两种误归类，S4 采用受限类别：

```text
src/reconstruction/monitor/
```

它只允许实现 S3 已列出的 CPU/memory/execution mechanics。每个新增行为必须在
`S3-RECONSTRUCTED-MONITOR-SEMANTICS.md` 中有对应规则和 OpenNT source anchor。

## 允许范围

- per-session real-mode visible registers、ordinary RAM、checked fetch/read/write；
- bounded instruction budget、typed stop、physical fetch record；
- NOP、已证实 C4/D6 BOP decoding、`FE` invocation exit；
- 一个抽象、定宽 bridge transport interface；i386 fixture 可由 S2 record bridge 实现它。

## 禁止范围

- BIOS table、BOP/SVC dispatch、DEM/DOS result、IVT/BDA、firmware、PC/AT device；
- host filesystem、clock、console、media、input、network、process policy；
- V86、protected mode、CRx、descriptor cache、paging、IRQ/PIC/PIT；
- 从 Bochs、PCjs、NTVDMx64、ntvdm64 复制任何实现；
- 进入 `ntdos64-run`、默认 build 或默认 CTest 路径。

## 构建与验证

1. root MinGW target `ntdos64-reconstructed-monitor-s4-test` 只验证 portable monitor
   mechanics 的 NOP、unknown opcode、FE stop、bridge refusal 与 range failure。
2. OpenNT i386 target `ntdos64-opennt-reconstructed-monitor-s4-bridge-fixture` 将同一
   monitor source 与 S2 v1 historical bridge 组合，验证真实 `BIOS[50h]` date trace。
3. 两个 target 都必须 `EXCLUDE_FROM_ALL`；任何 root CLI linkage 或 OpenNT default
   runtime linkage 都是治理失败。

通过第 2 项仍不等于 x64 transport 已完成。它只证明 monitor contract 在同位宽验证
闭包中能调用原始历史 host chain；跨进程/跨位宽 transport 是后续 S4 子项。

## 实现与验证记录

S4 implementation 位于 `src/reconstruction/monitor/reconstructed_monitor_s4.[ch]`，其
source manifest 为 `src/reconstruction/SOURCE-MANIFEST.json`。root 的
`ntdos64-reconstructed-monitor-s4-test` 已使用 MinGW x64 构建并退出 `0`，覆盖 NOP
budget、D6 bridge refusal、完整 C4 `0xFE` exit、C4 `0x03FE` typed refusal 和 unknown
opcode。

OpenNT i386 island 的 `ntdos64-opennt-reconstructed-monitor-s4-bridge-fixture` 已构建为
`COFF-i386` 并退出 `0`。它将同一 monitor source 接到 S2 v1 record transport，运行
`C4 C4 50 14 D6 FE`，验证原始 date result、IP 从 0 经 historical service 至 4、再由
`D6 FE` 至 6 的 invocation exit。fixture adapter 只复制定宽 record，未实现 selector、
BOP、DEM 或 DOS handler。
