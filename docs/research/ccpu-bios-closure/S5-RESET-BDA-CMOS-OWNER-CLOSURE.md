# S5 Reset / BDA / CMOS Owner Closure

状态：2026-08-09，NT4 static owner audit。目的不是从 `reset.c` 提炼一个本地 BIOS scaffold，而是确定
NT4 `sysinit` 所见 BDA/CMOS memory values 的唯一历史初始化链及其当前可组合性。

## 固定版本对照

| 源文件 | NT4 SHA-256 | NT4.5 | XP | 结论 |
| --- | --- | --- | --- | --- |
| `base/bios/reset.c` | `46E2507AD47182125ED59206780763C5DADF6EA752A43D4D946451DE1024D90B` | 同 hash | `58E194446CCD2BD9BD1B2380DD366DF5469ABF7AB066F6A72B9498D7C6731506` | NT4.5 是同源旁证；XP 已演化。 |
| `base/system/cmosnt.c` | `00156A734520B5D59442BF9D6127F4E722A7A697F9BE33F34FEECF4AEF06D850` | 同 hash | `439400837D05104D419D1CBAB840C6C2CEC40038FE5EDB01298EB4D3D2A00F24` | 同上。 |
| `base/bios/mem_size.c` | `2E8937E8FC10FBC69425692FB8B127B20E786FAE37FD453049B62DD0472C9E8F` | 同 hash | 同 hash | `BOP 12h` owner 跨样本稳定。 |
| `base/bios/tape_io.c` | `3AA86F1FAF07F68937629F1D6E410EB41F498E17DC6D747647C7D6E5697C80AA` | 同 hash | `78D1B702A486D67679FDFB5EDB6612993416E8AD9E4C9F3DB425E678ED278A10` | NT4 的 `15h/AH=88h` 语义不能由 XP 回推。 |

## NT4 原始 ownership

```text
reset()
  -> [NTVDM/X86GFX condition] clear low 640 KiB, setup_ivt()
  -> ica0/ica1 init + post
  -> cmos_init(); cmos_post()
  -> ppi_init(); ppi_get_switches(); BDA EQUIP_FLAG
  -> BDA MEMORY_VAR = host_get_memory_size()
  -> timer / keyboard / video initialization

cmos_post()
  -> gfi_drive_type + config_inquire device policy
  -> sas_memory_size() -> CMOS extended-memory bytes
  -> host_GetSysTime / ICA coordination on RTC paths
```

`setup_ivt()` 本身写入的不只是 `INT 12h` 和 `INT 15h`。原始 `reset.c:164-272` 写入 timer、keyboard、
diskette、video、serial、printer、RTC、redirector 等多个 IVT vector，且选择会受 graphics configuration
影响。`reset.c:783-884` 又把 SAS backing、PIC、CMOS/PPI、timer、keyboard、video 纳入同一初始化序列。

因此对于 `sysinit1` 的早期 `BOP 12h`/`15h`，正确的历史前置条件是一个由 reset 完整建立的 machine
profile，而不是：

- 在 guest prefix 或 fixture 中预写 AX；
- 单独复制 `MEMORY_VAR` 或 CMOS byte；
- 单独调用 `memory_size()` / `cassette_io()`；
- 手写 IVT/BDA 以绕过 `reset()`；
- 用 XP 的较晚实现替换 NT4 reset/CMSOS policy。

## 当前可组合性

项目已保留默认关闭的原始 object 分组：

- `ntdos64-opennt-bios-provider-objects` 含 `mem_size.c`、`reset.c`、`tape_io.c`；
- `ntdos64-opennt-system-provider-objects` 含 `cmosnt.c`、timer、DMA 等；
- reset trace overlay 只用于观测，不能当 runtime reset implementation。

但 `SM2-UNRESOLVED-SYMBOL-LEDGER.tsv` 已证明这个完整 owner 集仍进入未闭合的 host/runtime/video/device
依赖，例如 `reset.c` 的 `_host_reset`，以及 `cmosnt.c` 的 `_host_runtime_inquire`。这不是一个允许加
no-op shim 的漏洞：这些 symbols 对应原始 session lifecycle、硬件配置、介质和 RTC/ICA coordination。
完整 `BIOS[]` static closure 也仍受 C-VIDC/Video composition 问题约束。

结论：此时**没有**证据允许把 reset/BDA/CMOS 的任意子集进入正常 runtime；也没有一个安全的“小型
reset fixture”可以声称恢复 NT4 startup。

## 允许的下一步：最小 closure 验收，而不是替代实现

只有在以下每项都可用原始 owner 满足时，才可以建立一个 default-disabled 原始 reset closure fixture：

1. 一份固定、同版本的 original object/link composition 或等价可审计 source closure，包含完整 `BIOS[]`
   static owner，不使用 `Video=C_Video`、缩短 vector、`/FORCE` 或 local handler；
2. 原始 `reset()` 与其所需 host lifecycle owner 均真实链接，且 host seam 有逐项的 contained policy 或
   fail-closed stop；
3. fixture 在 reset 返回后只读取、记录并比对 BDA `MEMORY_VAR`、CMOS capacity fields 与预先声明的
   ordinary SAS capacity，不执行 NTIO/DOS/BOP success path；
4. 不同 RAM/profile 的正反例能证明三者一致，失败不得留下部分 guest mutation；
5. 所有 trace 仅包含容量 bucket、地址类别和结果，不记录 BYOB bytes、host paths 或 guest memory dump。

在这些 gate 满足前，正确行动仍是 source/static audit 与既有 bounded traces，而不是向 `src/` 新增
machine/BIOS 替代代码。

## 关联记录

- `S5-SYSINIT-MEMORY-BOP-BOUNDARY.md`
- `S5-BIOS-STATIC-DYNAMIC-OWNER-LEDGER.md`
- `S5-FULL-NOSM0-OWNER-SEAM-LEDGER.md`
- `SM2-UNRESOLVED-SYMBOL-LEDGER.tsv`
- `NT4-NT5-SOFTPC-EVIDENCE-LEDGER.md` 中的 `E-INIT-001`
