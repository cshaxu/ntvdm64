# M71 NT4 Boot Minimum Machine Matrix

Status: source-derived staged device classification, 2026-08-08.

## Scope and Rule

This is a source reachability matrix for the fixed NT4 startup code, not a
claim that every referenced historical device is required for the first bounded
`COMMAND.COM /C` profile. A device is promoted only after a trace proves that
the selected profile reaches it without an original fallback. This preserves
the rule against moving PC/AT product policy into core merely because it may
later be shared.

## Sources

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_msscs.c`
- `src/opennt/base/mvdm/softpc.new/base/bios/reset.c`
- `src/opennt/base/mvdm/softpc.new/base/keymouse/keybd_io.c`

## Classification

| Machine/service area | Startup evidence | First bounded profile disposition | Core boundary implication |
| --- | --- | --- | --- |
| Real-mode CPU, ordinary low RAM, IVT/BDA, ROM/reset mapping | host preloads NTIO then sets CS:IP; NTIO/NTDOS build data and vectors in low memory | required | existing prepared-entry, memory, ROM and CPU foundations; profile owns exact bytes/layout |
| A20 address semantics | `msproc.asm` retains A20-off execution state; `sysinit1.asm` attempts local A20 enable while selecting HMA | required semantic; exact HMA use is trace-gated | generic A20 belongs in core; HMA/loader policy remains profile-side |
| Memory-size service | `sysinit1.asm` issues BOP `12h` and `15h` instead of ordinary BIOS INT 12/15 calls | required profile response before full initialization | needs governed decode/profile adapter, not a core BOP name |
| NTIO/NTDOS service marker dispatch | startup uses service and BOP paths, including NTDOS load | required | configured decode observation is the remaining neutral core gap |
| Checked NTDOS image publication | NTIO asks DEM to load NTDOS at DI:0000 | required | paused ordinary-RAM transaction is the remaining neutral core gap |
| DOS memory allocator/data structures | `sysinit1.asm` builds arena, CDS/DPB/FCB and loads COMMAND | required guest behavior | profile/guest runtime; no new PC/AT device contract |
| XMS/HMA manager | `sysinit1.asm` calls `IsXMSLoaded`, requests XMS A20/HMA services, but is structured as `LoadDOSHiOrLo` selection | trace-gated fallback candidate; required only if selected profile cannot continue low | generic A20 is core; XMS service policy is wrapper/runtime-specific |
| Keyboard vectors and BIOS data | initialization reads keyboard flag `40:96`, sets INT 16-related vectors, and copies keyboard resident code | required for interactive command profile; trace-gated for noninteractive `/c` | KBC/input foundation can remain generic; keyboard policy/vector bytes are profile-side |
| Text/video INT 10 vectors | startup saves/restores INT 10 and NTIO owns video hooks | trace-gated for output-visible `/c`; required for interactive display | generic display device may remain core; BIOS/video vector policy is profile-side |
| PIC/PIT/interrupt delivery | historical reset initializes system interrupt/timer machinery; guest stack/interrupt setup is present | trace-gated until first real timer/keyboard/idle delivery | existing generic PIC/PIT foundation; no new NTDOS-specific contract |
| CMOS/RTC | historical full machine initializes them, but this static startup path does not prove first prefix requires their direct device semantics | deferred pending two consumers and trace | do not migrate merely for this profile |
| FDC/HDC controller mechanics | host loads NTIO/NTDOS from files; first prefix's file services are host-backed and source does not prove an INT 13 controller transaction before COMMAND | deferred pending actual trace/two consumers | do not migrate merely for this profile |
| EMS, mouse, serial, printer | startup contains optional resident driver/host integration paths | explicitly deferred | product/profile capabilities, not first core move |
| DPMI, WOW, redirector, VDD | source-visible optional product paths | explicitly deferred | outside first bounded profile |

## Result

The matrix validates the current boundary discipline:

1. no new device extraction from `vm` into core is justified solely by NT4
   boot-source references;
2. generic A20 and existing CPU/memory/interrupt/display foundations are the
   correct machine layer; and
3. first execution is blocked by the same two neutral backend facilities from
   M69, not by premature CMOS/FDC/HDC work.

The next admissible device decision is a redacted trace of one selected profile
after backend admission. It must record first actual touch of memory BOP,
A20/XMS, keyboard, display, timer, and block-device paths; a source mention
alone cannot promote a controller into core.
