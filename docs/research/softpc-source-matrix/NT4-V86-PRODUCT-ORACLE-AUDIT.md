# NT4 V86 Product Oracle Audit

状态：2026-08-09，只读 product-image audit。该 i386 V86 product 仅作为 NT4 historical host/BIOS/DEM
边界的旁证，绝不是 x64 execution backend、CCPU composition 输入或 runtime dependency。

## 固定样本

| 项 | 值 |
| --- | --- |
| 文件 | `D:\home\repos.hobby\opennt-4.5\nt\public\sdk\lib\i386\ntvdm.exe` |
| SHA-256 | `06618D9788036AC59EF6C37FF4000DF9EF39C58521C595DB875A7E2ABD84C231` |
| 格式 | `COFF-i386` / `IMAGE_FILE_MACHINE_I386` / 32-bit |
| PE subsystem | Windows CUI |
| OS version metadata | 4.0 |
| entry point RVA | `0x2791A` |

采证命令为本机 `C:\Program Files\LLVM\bin\llvm-readobj.exe --file-headers` 与
`llvm-nm.exe`；不会执行该二进制，也不会把它复制进本项目。

## 符号级证据

符号表包含以下 historical host/BIOS/DEM owner：

```text
_MS_bop_0@0       0f012bc0
_DemDispatch@4    0f012710
_memory_size@0    0f01d8a0
_cassette_io@0    0f02e140
_sas_memory_size@0 0f027f90
```

这与 NT4 source 的 `BIOS[50h] -> MS_bop_0 -> DemDispatch`、`BIOS[12h] -> memory_size`、
`BIOS[15h] -> cassette_io` owner mapping 一致。它是“这些符号确实共同进入 V86 产品”的 product-level
旁证。

同一次筛选没有给出 `_c_cpu_*`、`_c_effective_addr` 或 `_C_Video` 定义。这与已记录的事实一致：此样本是
NT4 x86 `MONITOR` product，而非可替代 CCPU/C-VIDC missing composition 的 library。符号缺失也不能反推
CCPU 从未存在，只能说明它不在该固定 V86 image 的可见定义中。

## 准确用途与禁止用途

可用：

- 对照 source-level BOP/BIOS/DEM owner 是否属于历史 V86 product；
- 审计某个 symbol 是否应由 V86 product 而非 CCPU composite 提供；
- 作为 NT4 与后续版本 image-layout/exports 的只读比较基线。

不可用：

- 从此 image 抽取、链接、反汇编复制或再发布实现；
- 把 V86 monitor 当成 x64 可运行路线，或用它填 CCPU/C-VIDC 缺失符号；
- 因为它含有 `memory_size`/`cassette_io`，就跳过 reset、BDA、CMOS、完整 BIOS static closure；
- 把 image symbol evidence 当成 BYOB files、COMMAND runtime 或 native process handoff 的成功证据。

## 后续跨版本采证

若取得 NT4、NT4.5、NT5.0、NT5.1 的合法对应 product image，可只比较：COFF machine、PE metadata、
固定 owner symbol 是否存在、import/export 集差异。每一份样本都须独立记录路径、hash、来源限制与采证工具。
不得因符号相似而混用其源码、objects 或运行时行为。
