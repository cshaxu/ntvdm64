# S5 demGetDPBList CCPU SAS Closure

状态：2026-08-09。该记录只证明一个 default-disabled 的原始 DEM guest-write
路径；它不把 CCPU/SAS profile 宣称为 NT4 x86 V86 monitor 的复原，也不构成
DOS runtime、磁盘或介质实现。

## 已执行的历史链

固定 NT4 `NTDOS.SYS` 的 `msinit.asm::DosInit` 在 `charinit` 后先调用
`C4 C4 50 0F`，再以 DOS data segment 中的 `ES:BP` 调用：

```text
BIOS[50h] -> MS_bop_0 -> DemDispatch(0Fh) -> demGetDrives
BIOS[50h] -> MS_bop_0 -> DemDispatch(46h) -> demGetDPBList
```

本次 fixture 保持同一个 `DemInit` session，未写入 `PhysicalDriveTypes`，也未
自行构造任何 `DPB`。`0Fh` 的原始 owner 先发现并填充 26-slot cache；随后
`46h` 的原始 `demgset.c::demGetDPBList` 根据其中的 removable/fixed 条目，直接
在原始 `GetVDMAddr(ES,BP)` 结果写 `DPB::DriveNum`、`Unit` 和 `Next`，然后更新
BP。

## 受限 CCPU/SAS 采证 profile

`src/opennt/overlay/base/mvdm/inc/softpc.h` 只在
`NTDOS64_CCPU_SAS_GETVDMADDR` profile 下将 `GetVDMAddr(seg, off)` 转交给原始
`ccpusas4.c::c_GetLinAdd`。`dem-ccpu-sas-objects` 是该 profile 的唯一使用者；
它已作为 `overlay/reconstruction` 登记，且只进入 default-disabled trace。

这使原始 `demGetDPBList` 写入当前 CCPU/SAS aperture。它不是对 V86 raw
guest-linear address 如何映射的推论；不得由此新增 low-address alias、通用
host-pointer 转换器，或扩大此 overlay 的适用范围。

## 验证证据

构建目标：

```text
ntdos64-opennt-original-demgetdpblist-trace-fixture
```

执行结果：

```text
fixture-exit=0
demgetdpblist fixture: ip=0479 count=7 bp=00f7 size=33
```

fixture 逐项断言：

1. 两次服务均通过完整原始 `BIOS[50h]`、`MS_bop_0` 与 `DemDispatch`；
2. 每个被原始 cache 选中的 DPB 的 `DriveNum` 和 `Unit` 与 cache drive 一致；
3. 非末项 `Next` 指向下一 DPB 的原始 segmented 表示，末项为 `0xffffffff`；
4. 原始 owner，而非 fixture，写入全部 7 个 DPB 并把 `BP` 前移到 `00f7`；
5. vectored exception observer 只记录失败现场，不能修补结果或影响分派。

来源输入为 `src/opennt/base/mvdm/dos/dem/demgset.c::demGetDPBList`；fixture 为
`src/opennt/local/trace/fixtures/original_demgetdpblist_trace_fixture.c`，其
SHA-256 为 `E75AEDD4B8FB808834A69ECDE883AB91E905A23DC710F72BE0E79D3A6D2AE814`。

## 边界结论

这条路径消费的是已有 DEM drive cache 和一块可写 guest-memory destination；
它不读取磁盘扇区、不枚举 host A:/B:、不创建 FDC/HDC，也不定义 DOS namespace。
所以它是 software-monitor guest-memory translation/mapping contract 的真实消费者，
而不是任何 PC/AT controller 或 host capability 的提前准入。

下一阶段应继续以连续的原始 `DosInit` 调用序列审计真实 owner、输入和写入，优先
选择不要求新增设备策略的调用；每个新 seam 均须对应明确的历史 host dependency。
