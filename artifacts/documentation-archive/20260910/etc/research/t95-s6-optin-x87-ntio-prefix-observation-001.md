# T95 S6 Opt-In x87 NTIO First-Prefix Observation 001

## Exact scope

This is the single admitted opt-in observation of the existing source-built
NTIO first-prefix package. The only executable difference is a root-local
`/DBX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=1` while rebuilding `fpu.cc`; it
enables a Bochs-local log before the pre-existing compatibility IRQ13 call.
The adapter, OpenNT, guest package, reset profile, ROMs, device declarations,
environment handoff and benchmark stop policy are unchanged.

## Build evidence

The r5 opt-in build is terminal compiler-failure evidence: its first compile
exposed a duplicated `BX_CPU_THIS_PTR RIP` prefix in default-disabled code.
The registered source correction changes only that operand to the existing
direct `RIP` macro. Fresh r6 then rebuilt and linked the companion. The child
wrapper recorded `0`; the foreground observer timed out after wrapper
completion, so its root-local wrapper/log/exit and recovered JSON record are
the terminal link evidence, with no second invocation.

| r6 artifact | SHA-256 |
| --- | --- |
| FPU source | `E46866152070F4FBCE4B5C39B1E3280D72C9FF5AB0D8A01EA7358AB600F79AB5` |
| FPU object | `43E5B11D9F41D29FB1F161E9FEC157F28160DC82F64D899783AB81D6AEEC2D53` |
| FPU archive | `03075014A57150E8BF71C0257774FDD0BC55D2C9FC2ABE69210A1550C52227E1` |
| companion executable | `74B186DC96518AFB01509B0C3A82EBF8ACD27DE5E8C238A51196608B2BB81073` |
| companion map | `E640E30549A15A0B862A119CE3F5C114C279287B7F730DB44DC67DC4478B1B12` |

## Runtime evidence

The evidence root is
`artifacts/analysis/t95-s6-optin-x87-ntio-prefix-001-20260811-001`.
It records the same payload identities as the prior first-prefix run:

| Input | SHA-256 | Role |
| --- | --- | --- |
| NTIO.SYS | `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937` | only placed source-built payload |
| NTDOS.SYS | `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84` | identity only; not placed |
| COMMAND.COM | `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43` | identity only; not placed |
| runtime profile | `48E18C4AEEABB41817E40D161AEC6766EC121A6128F44B331111789F6D1A4663` | PC-reset-floor-v1 unchanged |

The run completed in 3,103 ms without watchdog, logged `requested execution
plan applied`, and reached the same native benchmark stop (exit `1`) at tick
`10,000,004`. It emitted 26 structured x87 records, each immediately before
the existing `math_abort` line. The first is:

```text
cpu=0 cs=0000 rip=0000000000000590 cr0=60000010 cwd=0040
swd=82c1 twd=5555 tos=0 foo=0701 fcs=0000 fip=00000000000000fa
fds=0000 fdp=0000000000000d41
```

Later records change the last-x87 fields and status/tag values while retaining
the original compatibility branch (`CR0=60000010`, including `NE=0`). The run
again halts with `IF=0` and terminal `CS:IP=0000:0001`.

## Classification

Bochs `i387_t::reset` initializes `swd=0`, `foo=0`, `fip=0`, `fdp=0` and
`twd=0x5555`. The first observed snapshot instead already has `swd=0x82c1`,
`foo=0x0701`, nonzero `fip`/`fdp`, and the FPU status summary condition that
selects the original IRQ13 branch. Therefore the data **rejects reset FPU
non-initialization as the direct explanation** for the first diagnostic.

It does not identify the exact current instruction, prove an OpenNT service
is reached, identify IRQ13-vector ownership, or authorize an interrupt,
BIOS, VDM-state, CPU-profile or host-service repair. The state may have been
created by a reached guest path before the first captured compatibility check;
that is the next source-correlation question, not a permitted fix.
