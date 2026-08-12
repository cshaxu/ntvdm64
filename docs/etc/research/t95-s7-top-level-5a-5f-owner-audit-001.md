# T95 S7 Top-Level 5A/5F Owner Audit 001

## Question

After the contained `50:42` service completed in the first real S7 trace,
what do the next reached top-level BOP identities `5F` and `5A` mean in their
OpenNT owners, and which (if any) can be resumed without importing SoftPC or
inventing DOS semantics in the adapter?

## Inputs and procedure

- Immutable source-built trace:
  `artifacts/analysis/t95-s7-runtime-trace-010-20260811-001`.
- BOP definitions: `src/opennt/base/mvdm/inc/bop.h:25-42`.
- Historical top-level dispatch: `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c:628-747`.
- Historical idle implementation: `src/opennt/base/mvdm/softpc.new/host/src/nt_unix.c:75-166`.
- Historical vector setup: `src/opennt/base/mvdm/softpc.new/base/keymouse/keybd_io.c:1911-2013`.
- The current adapter exception/result ABI and its real-mode exact-window
  services.  No guest-memory decode beyond the already captured instruction
  window was performed for this audit.

The trace was read in order after the already verified `50:42` bulk commit;
the source definitions and direct callees above were then compared, rather
than inferring semantics from the selector names.

## Observed trace facts

The r11 binary (`C926DBFDECE6066768BCF8353BB81D2F31D8AD841D823D1C1B9C18784FF22173`)
recorded the distinct `50:42` commit and accepted resume, then observed
`5F` with `AX=beef`, followed by `5A` with `AX=0000`.  Both were observed by
the generic catalog listener and passed through.  The subsequent segment-limit
and physical-memory failures are downstream evidence only: this record does
not assign either failure to either BOP.

## Owner classification

| Identity | Original owner and state effect | First-profile decision |
| --- | --- | --- |
| `5A` (`BOP_WAITIFIDLE`) | `MS_bop_A` reads AX. AX=0 calls `WaitIfIdle`; AX=1 calls `WakeUpNow`; other AX values only emit a non-product diagnostic. `WaitIfIdle` returns after `Sleep(0)` whenever the historical PIF/event/activity prerequisites are absent. Neither branch changes guest registers, flags, memory, interrupt state, or the BOP instruction stream. | Admit one exact, real-mode, three-byte `C4 C4 5A` adapter result: advance RIP by 3 with an empty CPU delta. It deliberately models the source's no-idle-event immediate-return branch; it creates no scheduler, event, VDD, PIF, or device feature. AX is preserved, including unknown values. |
| `5F` (`BOP_UNIMPINT`) | The reached NTVDM dispatch is `MS_bop_F`, not merely the scaffold's default `C4 C4 5F CF` interrupt stub. It calls `kb_setup_vectors`, which consumes an NTIO table, captures idle/keyboard/timer/INT 08/1C/13/15 state, and then restarts historical ICA interrupts before returning CF according to monitor mode. | Remain listener-only. A plain resume would silently omit table ownership and interrupt-enable timing. This needs a separate field-by-field mapping to existing Bochs CPU/PIC/PIT/keyboard/IVT owners and a source-proven composition design; it must not be copied into the adapter or treated as a DOS service. |

`5F` has no admitted Bochs change in this increment.  If later evidence proves a
Bochs-core alteration necessary, it must first be entered in
`adapter-external-intrusion-exceptions.md`; no such exception is created here.

## Bounded 5A contract

The service accepts only a valid real-mode #UD event whose first three captured
bytes are exactly `C4 C4 5A`, and only when the installed adapter runtime is
active.  It produces only `resume_rip = fault_rip + 3`; it reads no guest RAM,
writes no guest RAM, changes no GPR/flags/segments/control state, performs no
host sleep or wait, and handles no adjacent selector.  A malformed window,
wrong mode, overflow, or any other BOP remains listener-observable and
pass-through.

This is a contained replacement for the source's unavailable host-idle
plumbing, not a claim that NTVDM VDD idling is implemented.  The next trace may
establish only that execution passes this exact call; it does not authorize
`5F`, IRQ, PIC, keyboard, FPU, DOS search, host-drive, or CLI expansion.

## Verification plan

Add a focused adapter test for exact selector/mode/window matching and an
empty-delta resume.  Rebuild it in a fresh modern C11 root.  Only then create
one fresh MSVC runtime-trace root and one watchdog observation with the same
source-built inputs, preserving r11 and trace 010 unchanged.
