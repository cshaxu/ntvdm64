# M0 T243 S1 — BOP-5F Source, ABI and Lifecycle Audit

## Scope and conclusion

The reached `C4 C4 5F` is the NTIO `spckbd.asm` handoff, not a generic
unimplemented interrupt.  The exact selected historical route is:

```text
spckbd.asm (CS:SI kio_table, DS resident segment, DI/CX iret table,
             AX=BEEF)
  -> nt_bop.c:MS_bop_F
  -> keybd_io.c:kb_setup_vectors
  -> historical ICA release + MONITOR CF=1
  -> spckbd.asm:JC isk_int9
```

The full `keybd_io.c` translation unit is not directly composable in the
current first profile.  Its ordinary body uses historical CCPU/SAS direct
pointers, `ResumeTimerThread`, keyboard ownership, `host_simulate`, video,
printer and ICA product-shell facilities.  Composing it would enable a broad
device shell before an owner-specific lifecycle exists.  No Bochs intrusion
is proposed.

The first usable source-first rung is therefore a narrow source-shaped
composition seam: retain the source's `kio_table` layout and the exact
`useHostInt10` state publication used by `kb_setup_vectors`, with the
already-declared CLI `STREAM_IO=2` profile.  The current checked-RAM ABI is
the smallest replacement for the historical `sas_store_no_check`.  The
adapter owns recognition and this OpenNT/NTIO-specific table interpretation;
the mantle remains an opaque synchronous RAM/lifecycle mechanism and learns
neither selector nor table format.

## Original ABI

`spckbd.asm:2059-2110` prepares the table and issues the BOP:

| Input | Original meaning | First-profile disposition |
| --- | --- | --- |
| `CS:SI` | 33-word `kio_table`, including final `BEEFh` guard | Read only through checked RAM; table is never retained. |
| `DS` | resident keyboard-code/data segment | Used only with the table's word 17 offset to address `use_host_int10`. |
| `DI:CX` | monitor iret-BOP table and entry spacing | Explicitly unavailable: no historical monitor iret hook is composed. |
| `AX=BEEFh` | table ABI/version guard | Required exactly; any other value declines. |
| `CF` after return | MONITOR `MS_bop_F` completion; guest `JC isk_int9` | Set only after all selected checks and the one state publication commit. |

The source table indices consumed by `kb_setup_vectors` divide as follows:

| Words | Historical owner/effect | Current disposition |
| --- | --- | --- |
| `0..16` | key maps, dummy/INT05/INT15/RTC/keyboard/INT10 callback addresses and idle state | Not decoded. They require the later keyboard/timer/INT15/video lifecycle package. |
| `17` | `useHostInt10` byte offset; `sas_store_no_check(KbdSeg:offset, getNtScreenState())` | Required now. Write the declared `STREAM_IO=2` byte to checked ordinary guest RAM. |
| `18..31` | baby video table, mode change, printer, wait, FDC, VGA/INT15 tables, INT08/INT13 and stream buffers | Not decoded. Each retains its native guest owner or a future source-proven device package. |
| `32` | `BEEFh` assertion guard | Required now; reject mismatch before any guest write. |

## Lifecycle mapping

| Original step | Current owner/disposition |
| --- | --- |
| `kb_setup_vectors` table state | Source-shaped adapter seam for the one `useHostInt10` byte only; no CCPU pointer, callback or device state is copied. |
| `host_ica_lock` / `DelayIrqLine=0` / `ica_restart_interrupts` | Not called. The active minimal machine initializes its native 8259 during `bx_ntvdm_minimal_machine_c::initialize`; no adapter may inject or restart IRQs. The S2 seam requires only an active checked-RAM stage, which proves the machine lifecycle is live. |
| `ResumeTimerThread` | Explicitly unavailable in the headless profile; no timer thread is manufactured. |
| `setCF(1)` | Adapter typed result, after successful one-byte commit; it is the original MONITOR guest-visible branch outcome. |
| physical keyboard, IRQ1, PPI/8042, host input | Deferred to a later evidence-admitted PC device package. S2 does not claim interactive keyboard support. |

## S2 admission

S2 is limited to a source-shaped, headless `STREAM_IO=2` BOP-5F handoff:

1. recognize only real-mode `C4 C4 5F`, `AX=BEEF`;
2. checked-read exactly the table guard and word 17 from `CS:SI`, with
   overflow/aperture rejection;
3. checked-write exactly one byte, `2`, to `DS:word17`;
4. resume at `RIP+3` with CF set, no other CPU or device state mutation;
5. decline malformed version, mode, address, read or write cases without a
   partial write.

This is neither a replacement keyboard device nor a generic BIOS handler.
It carries the first profile across the original `JC isk_int9` branch while
leaving later physical input, timer, video and monitor hooks in their named
owner packages.

## Verification plan

A focused fixture must prove the positive table/byte/CF/RIP transaction and
each negative case above.  A formal Ninja link must include the new v2 route.
Only after the package regressions pass may one bounded source-built guest run
observe the post-`5F` control path; that run cannot enlarge S2.
