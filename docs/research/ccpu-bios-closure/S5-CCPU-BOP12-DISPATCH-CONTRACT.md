# S5 CCPU BOP12 Dispatch Contract

状态：2026-08-09。本文闭合固定 NTIO `sysinit` 首个 `BOP 12h` 的 CCPU
解码与原始 BIOS owner。结论是：BOP 解码语义仍在 OpenNT 的
`ccpu386/c_main.c` 中；当前阻断不是“缺一条 BOP 指令实现”，而是原始 CCPU
与完整 `BIOS[]`/SAS/host 生命周期的可组合构建。

## 原始字节到 CCPU 分派

固定 `NTIO.SYS` 在 `sysinit` 的可执行目标以：

```text
C4 C4 12    BOP 12h
```

开头。`src/opennt/base/mvdm/softpc.new/base/ccpu386/c_main.c:3293-3355`
已经包含历史解码路径：

1. 对 opcode `0xC4`，它先读取 ModRM；
2. `((modRM & 0xFC) == 0xC4)` 将 `C4 C4` 识别为 BOP 标记，而非普通
   `LES`；
3. `D_Ib(0)` 读取第三个字节 `0x12`；
4. `UPDATE_INTEL_IP(p)` 在回调前将 guest IP 前移到 BOP 后，即固定 image
   中的 `0x0EE9`；
5. 低字节不是 `0xFE` 时，原样执行 `bop(ops[0].sng)`。

`bios.h:179-183` 定义 `bop(n)` 为 `(*BIOS[n])()`。而 `bios.c` 的原始完整
表把 index `0x12` 绑定到 `memory_size`；`mem_size.c` 读取 SAS 的
`MEMORY_VAR` word，并只以 `setAX(memory_size)` 返回。故首段的精确关系是：

```text
guest C4 C4 12
  -> original ccpu386 C4C? decoder
  -> IP consumed to next byte
  -> bop(0x12)
  -> original BIOS[0x12]
  -> original memory_size()
  -> SAS[MEMORY_VAR] -> AX
  -> guest resumes at 0x0EE9
```

## 与 DOS BOP 的严格区分

`nt_bop.c:146-159` 的 `MS_bop_0` 属于 `BIOS[0x00]` 的原始 DOS-emulation
owner。它在 CCPU 已完成 BOP 指令消费后，从 **新的** `CS:IP` 读取一个 service
byte，调用 `DemDispatch(DemCmd)`，然后把 IP 再推进一个字节。

因此：

- `C4 C4 12` 不会、也不得调用 `MS_bop_0`；
- `C4 C4 00 <service>` 才走 `BIOS[0] -> MS_bop_0 -> DemDispatch`；
- `MS_bop_0` 的额外一个 IP 消费只属于 selector `0` 的 service byte，不能
  泛化给 selector `0x12`；
- 任何自写 “统一 BOP handler” 若模糊这两种 IP 规则，都会改变历史语义。

## 当前可执行结论与阻断

已确定的工作：

1. **不新增** BOP decoder、`BOP 12h` handler 或伪造内存大小的 shim；
2. 将 R1.1 的目标精确为：用原始 CCPU 的既有 decoder 调到原始完整
   `BIOS[]`，并以原始 SAS 支持 `memory_size()`；
3. 通过一次 default-disabled、只记录的 trace 断言 BOP 前后的 `IP` 和 `AX`。

尚未确定/不可绕过：

1. 完整 CCPU 静态 composition（包含历史生成输入和 session owner）尚未形成
   可运行组合；
2. `BIOS[]` 不能为此缩成单项表，`memory_size()` 也不能被独立 wrapper 伪装成
   历史 runtime；
3. `MEMORY_VAR` 的初始化必须来自原始 SAS/reset 路径；测试直接写入该地址只能
   是 fixture，不是启动成功证据。

所以 R1.1 仍是最高 ROI 的实施工作，但它的修复单位是**原始 CCPU/BIOS/SAS
组合与生命周期**，不是 CPU 指令或 DOS service 的新实现。

## 证据来源

- `src/opennt/base/mvdm/softpc.new/base/ccpu386/c_main.c:3293-3355`
- `src/opennt/base/mvdm/softpc.new/base/inc/bios.h:179-183`
- `src/opennt/base/mvdm/softpc.new/base/bios/bios.c` (`BIOS[0x12]`)
- `src/opennt/base/mvdm/softpc.new/base/bios/mem_size.c`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c:146-159`
- `docs/research/ccpu-bios-closure/S5-SYSINIT-ENTRY-STATIC-TRACE.md`
