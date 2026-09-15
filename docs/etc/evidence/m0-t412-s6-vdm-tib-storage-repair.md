# T412 S6 VDM TIB storage repair

## Cause and historical baseline

Owner requested historical comparison before repair, then approved repair,
build, deployment to `O:\winnt`, testing and commit/push. This is a bounded
S6 regression repair, not T412 closure or full WOW16 acceptance.

S4 closure `c889dff28` retains actual MEM output and COMMAND interaction.
S5 closure `b90e82c60` also records successful launcher returns, but some
observations updated only the broker beside an older worker. Those records
do not establish a reproducible same-commit full-product baseline.

The source-shaped printer carrier uses `VDM_TIB.PrinterInfo`; non-MONITOR
`softpc.new/host/src/stubs.c` still defined `int VdmTib`. The linker accepted
this four-byte COMMON symbol without selecting the real storage object from
`monitor-bindings.lib`. The declarations agree on the full type in the real
storage and printer objects; the defect was provider selection, not differing
structure layouts. The printer carrier was retained in `1daff0ace` research.

An external x86 hardware write breakpoint, attached after ordinary debugger
initialization, caught `MonitorInitializePrinterInfo` writing `0x22FD` over
the event-thread handle (previously `0x194`). In the failing image:
`VdmTib + 0xFB8 == ThreadInfo.EventMgr.Handle`. The stopped instruction maps
to the original `prt_State = State` assignment. The retained S4/S5 maps
instead place that out-of-bounds field inside `nt_cpu_info`, 372 bytes beyond
the event handle; old successful execution did not prove valid storage.
Raw evidence: `O:\winnt\logs\m0-t412-s6-handle-early.log`.

## Minimal repair and rejected alternatives

- Remove only the scalar placeholder, with mirror divergence registration.
- Make existing complete `VDM_TIB` storage an explicit initialized definition.
- Call the existing monitor binding at worker startup, initializing original
  printer modes and the worker-local TEB view before original execution.
- Gate the formal worker link on its actual map and storage COFF object:
  real definition, selected storage owner, full printer capacity, binding
  membership, and no overlapping symbols. The same gate rejects the old S5 map.

Original printer algorithms and CCPU remain unchanged. No link-order workaround,
new VDM implementation, worker reaper or CPU change is introduced. The abandoned
uncommitted SetDirectories RPC extension and added per-stage/thread diagnostics
were removed; their snapshot remains under the ignored S6 handle-watch build
directory. Earlier committed diagnostics are not represented as new fixes.

## Build and observation

Fresh MSVC x86 CCPU40 `/MT` build:
`build/M0-T412/S6/tib-storage-repair`, 478 Ninja steps. After integrating the
mandatory link gate, the final relink passes with 4,208 bytes of storage owned
by `mvdm_vdm_tib.obj`, at preferred address `0x006DEF18`, without overlap.
The ordinary sandbox Ninja invocation stalled before compilation; the authorized
build outside that restriction completed. Defender settings were not changed.

Final package at `O:\winnt`:

| EXE | SHA-256 |
| --- | --- |
| run16.exe | F17F9DF87597F6594CBCF12BB952291248A6FC8745A88DE54CBC4702D115A9BF |
| basesrv.exe | 7A9321BEB6BC262D7A5B55699A1CC6B3DE10610D9A05B5D1F46DCE523126EEAC |
| ntvdm.exe | 6647F7E80CCF25AD2D3A03D0C8C75A003BB8FA358DD975DBFBCC93D392222E78 |

Real non-debug Console observations use positional product arguments:

- Final `run16 MEM.EXE`: report displayed, launcher exit 0 within 20 seconds;
  `O:\winnt\logs\m0-t412-s6-tib-final-mem.txt` and companion Console snapshot.
- `run16 COMMAND.COM /c ver`: version output and exit 0 in the repair build;
  final-relink repetition is recorded as `m0-t412-s6-tib-final-command.txt`.
- Final `run16 EDIT.COM`: editor/modal text and Console mouse mode `0x1B0`
  observed; the interactive program remains live at the ten-second observation
  limit. This is startup coverage, not full mouse/editor acceptance.
  Record: `m0-t412-s6-tib-final-edit.txt`. An earlier run against a retained
  service returned 31 without guest-open evidence; the clean-service run
  supersedes it for startup evidence, not as proof that service reuse is fixed.

MEM's pre-existing implausible largest-program-size number is not accepted as
correct. Full WOW16, full interactive EDIT, and the remaining S2--S5 matrix
are not closed by this repair. Test-owned resident workers are explicitly
cleaned up; production worker lifetime remains source-owned. Owner testing
uses `run16.exe <binary>` with no added diagnostic product arguments.
