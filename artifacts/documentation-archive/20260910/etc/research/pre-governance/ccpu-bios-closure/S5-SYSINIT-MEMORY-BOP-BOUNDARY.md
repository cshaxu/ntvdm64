# S5 Sysinit Memory BOP Boundary

状态：2026-08-09，source-level owner audit。目标为固定 NT4 EN-US；本文不新增 shim、BIOS handler、
machine implementation 或 startup success 条件。

## 已证实的调用链

NT4 `base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm` 在 CONFIG projection 前执行：

```text
sysinit1.asm:825    BOP 12h
  -> BIOS[12h] = memory_size                 (base/bios/bios.c:159)
  -> memory_size()                            (base/bios/mem_size.c:44-54)
  -> sas_loadw(MEMORY_VAR)
  -> AX = MEMORY_VAR

sysinit1.asm:1012-1017  AH = 88h; BOP 15h
  -> BIOS[15h] = cassette_io                  (base/bios/bios.c:162)
  -> cassette_io(), case INT15_EMS_DETERMINE  (base/bios/tape_io.c:69,117-134)
  -> CMOS ports / CMOS_U_M_S_LO, CMOS_U_M_S_HI
  -> AX = extended-memory KB
```

`host_get_memory_size()` 仅在 reset 时通过
`base/bios/reset.c:855` 把 base-memory value 写入 `MEMORY_VAR`；当前 NT host header 将它固定为
`640`（`host/inc/host_gen.h:85-89`）。`memory_size()` 自身不访问宿主，不重新计算容量，也不向
host capability 询问内存。

同样，`BOP 15h` 的 selector 名称来自早期 cassette/INT 15 入口，但 `AH=88h` 走的是扩展内存大小
分支，而不是软驱、磁带或文件系统。该分支在 `PM` profile 下从原始 CMOS I/O 读取上下字节；非 `PM`
分支返回 AX=0。它没有调用 CONFIG、DEM、DOS file service 或 Windows directory API。

## 边界分类

| 项 | 原始 owner | 固定状态输入 | 不属于 |
| --- | --- | --- | --- |
| base memory KB (`12h`) | BIOS `memory_size()` | BDA `MEMORY_VAR`，由 reset/profile 初始化 | command host、DOS namespace、文件 capability、wrapper SVC |
| extended memory KB (`15h/AH=88h`) | BIOS `cassette_io()` 的 INT 15 case | CMOS extended-memory bytes，受 SAS memory size 与 CMOS initialization 影响 | CONFIG projection、FDC/HDC image policy、host filesystem |
| 初始化的容量 policy | reset/machine profile | 实际 guest RAM layout、BDA/CMOS initialization、A20/HMA 相关后续行为 | 在 `sysinit1.asm` caller 中预写 AX 的 trace hack |

所以这一组不是“为了 NTDOS 另加一个微软接口”。它证明 future machine layer 至少要能以一致的 guest
memory profile 初始化 BDA `MEMORY_VAR` 与 CMOS capacity fields；BOP handler 继续是历史 BIOS owner。
这也不意味着应立即把完整 CMOS、reset 或 `BIOS[]` 搬入 runtime。当前完整表仍受 C-VIDC/static closure
阻塞，且本研究禁止用单项替代 handler 绕开该门槛。

## 对未来实现的约束

1. 不得把 `BOP 12h`/`15h` 实现为 wrapper 的 selector switch，或由 trace fixture 在每次调用前写 AX。
2. 不能把 `host_get_memory_size() == 640` 误读为对所有 profile 的永恒值。它是这个 NT4 historical
   host header 的 reset-time policy；future profile 要显式提供容量并让 BDA/CMOS 共同派生。
3. `15h/AH=88h` 未来需在真实的 machine reset/BDA/CMOS owner closure 上验证；没有完整 BIOS static
   closure 前，只能记录预期输入和到达顺序，不可宣称 guest init 可用。
4. A20、HMA、`INT 15h` block move / virtual-mode 等同在 `cassette_io()` 的其他 case 中，不能因
   `AH=88h` 的窄审计而被一并宣称支持。

## 对 core / mantle 的信息量

这份证据支持的是中性、可配置的 machine profile 能力：可验证 RAM 容量、BDA/CMOS 初始字节，以及
它们与可访问 guest memory 的一致性。它**不**支持把 `BOP 12h`、`BOP 15h` 或 NT4 的 BIOS table
作为 core ABI；这些仍是 mantle/historical composition 的 consumer 语义。待完整 historical owner
可组合后，再以真实 BOP execution fixture 验证该 profile，而不是用本审计充当执行证据。
