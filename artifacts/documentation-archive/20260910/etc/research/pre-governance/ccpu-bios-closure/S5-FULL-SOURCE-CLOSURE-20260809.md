# S5 Full Source Closure Result (2026-08-09)

状态：default-disabled diagnostic 的当前重跑结果。该记录只归类 link blockers；不为它们新增
shim、stub、`/FORCE`、source replacement 或 runtime admission。

## 运行

```text
cmake --build artifacts/build/current/opennt-r2-bridge-ninja-i686 \
  --target ntdos64-opennt-direct-ccpu-full-source-closure-fixture --parallel 1
```

结果：预期失败。完整日志保存在
`artifacts/logs/r1-full-source-closure-20260809.log`，仅是本地研究产物，不属于 source 或 release input。

## 当前未解析符号

| 符号组 | 直接引用者 | 分类 | 可否现在补写 |
| --- | --- | --- | --- |
| `getEAX`、`setECX`、`setEDX`、`setEDI`、`setESI`、`setEBX`、`setEAX`、`setEBP` | `dpmi32/register.c` | 历史 DPMI/V86 register-context ABI | 否。CCPU accessors 不是 NT4 x86 monitor owner，不能混接。 |
| `VdmDebugLevel`、`VdmFeatureBits`、`GetFastBopEntryAddress`、`ThreadSetDebugContext` | `dpmi32/i386/dpmi386.c` | 私有 VDM/DPMI/debug context | 否。必须先确定 historical V86 owner 或从 first-command scope 排除 DPMI。 |
| `getIntelRegistersPointer` | `dbg/dbg.c` | 历史 debug register view | 否。不得构造伪 CPU register pointer。 |
| `nt_keybd_funcs` | `host/src/nt_reset.c:179` | V86 keyboard-monitor integration | 否。不是 keyboard device 的普通 host provider。 |
| `ShowStartGlass` | `host/src/nt_reset.c:243` | 历史私有 reset UI dependency | 否。不能以 no-op 或普通 message box 宣称 reset 已恢复。 |
| `EDL_fast_bop` | `ccpu386/c_main.c:3344` | CCPU generated/extended-BOP entry | 否。正常 `C4 C4 50` 不授权猜测 `...FE` generated ABI。 |
| `host_get_jump_restart`、`host_get_q_calib_val` | `system/qevnt.c:125,542` | host event/timing calibration | 否。不能用固定数或 sleep 替代历史 scheduler contract。 |
| `__sys_errlist` | historical CRT user | 老 CRT compatibility | 暂不单独处理；它只能在 owner closure 允许后作为 compiler/CRT compatibility 项处理。 |
| `c_sas_touch`、`c_VirtualiseInstruction` | `ccpu-archive(sascdef.c.obj):cSasPtrs` | 缺失/不明 SAS generated vector completion | 否。没有 NT4 原始定义或可采纳的 caller ABI；继续 fail-closed。 |

## 这次结果证明什么

1. BIOS/system provider object 能被独立编译，但“可编译”不等于可以与完整 SoftPC runtime 链接。
2. reset/BDA/CMOS 不是唯一障碍：即使它们被纳入 full source graph，仍立即进入 V86/DPMI、debug、event
   和 CCPU generation 的历史 owner boundary。
3. 当前阻塞不支持向 core 提出“再加几个设备就能跑 NTDOS”的结论；缺的是 historical composition 与
   monitor-side ownership 的闭合，而不是某一个通用 byte I/O API。
4. 该失败没有改变已通过的 bounded fixtures 或 BYOB validator；它们不能被用作 full runtime 证明。

## 后续动作的优先级

1. 保留 `c_sas_touch`、`c_VirtualiseInstruction` 的 fail-closed 状态，继续仅寻找原始 generation input 或
   version-matched archive/map evidence。
2. 将 DPMI/debug/fast-BOP 作为**可从首个 real-mode command path 延后**的 subsystem，而不是被 linker
   拉入时就用本地定义填平。
3. 对 reset 仅保留 static/dynamic owner audit；直到完整 BIOS table 与 historical monitor composition 有
   可审计闭包，禁止新的 reset-state runtime fixture。
4. 继续推动 software monitor 本身的真实 real-mode execution 证据；它是令 original owners 可被实际
   caller 驱动的必要前置，而非让 full-source diagnostic 通过的替代办法。

关联：`S5-RESET-BDA-CMOS-OWNER-CLOSURE.md`、`CCPU-MISSING-INTERFACE-CONTRACT.md`、
`CCPU-SEMANTIC-RECONSTRUCTION-RECOVERY-MATRIX.md`、`S5-FULL-NOSM0-OWNER-SEAM-LEDGER.md`。
