# M0 T396 S3 Win16 bootstrap / WOW32 loader frontier

Date: 2026-09-06  
State: evidence; S3 remains active

## Scope

This observation admits no guest-media, BOP, or WOW32-source change.  It
tests the corrected app-owned declaration choice: a source-classified DOS
target receives the existing selected pure-DOS PIF, while a source-classified
NE target retains the original empty `PifFile` carrier.

The parent is the S3 relinked x86 `ntvdm32.exe`:

| Asset | SHA-256 |
| --- | --- |
| `build/M0-T396/S3/ntvdm32.exe` | `8469B58ADE1CE96FA502A66A9CD2E72FBA6849713EEB215E584D031D1C362AF5` |
| S2 complete provider `wow32.dll` | `4333E8D705B017AF784C3855B1DBDA87E17C6EB2EBFB11D82BD8D8B5CA58F9A6` |
| selected staged `WOWEXEC.EXE` | `38EF366EB55AECC63183F32493B837CB4BDF059D969DD27185FCE6266CDC074D` |

The immutable input was staged at short root `O:\ntvdm64\s`; its manifest
hash is `9e1653b31de6f3f890e8bf28b387443ebedcc73e6369d55d829004ea4d5e266e`.
That path is a test deployment only, not a source/build dependency.

## DOS contrast

The Console-owning observer ran `MEM.EXE` from that package and returned
`0x00000000`.  The existing copied BaseVDM boundary reported
`pif-bytes=44` on both the available and consumed DOS record.  Thus the
fix did not withdraw the selected pure-DOS configuration from a DOS target.

## Win16 result

The same container ran the selected declared target
`system32\WOWEXEC.EXE`.

* Its initial copied BaseVDM record reported `state=0104` and `pif-bytes=0`.
  The PIF distinction is therefore a real record-boundary result, not an
  app-side intention.
* The unchanged original DEM observer opened `REDIR.EXE`, `DOSX.EXE`, and
  `C:\WINDOWS\SYSTEM.INI`.  This is the original default AUTOEXEC/DOSX
  bootstrap that the former pure-DOS PIF had suppressed.
* The original trace reached `50:36` and DPMI `53h` calls, including the
  existing `MVDM-DPMI-XMEM request=00001000 status=00000000 stage=4`.
* It did **not** reach `51h` / `MS_bop_1`.  The process ended
  `0xC0000409`; the live-module sampler captured 15 modules and
  `loaded-wow32=no` before exit.

The result is a precise earlier frontier: default Win16/DOSX bootstrap and
the already-closed DPMI32 provider are now reached, but the first original
WOW32 loader BOP has not yet been reached.  The staged DLL's presence is not
treated as a loader success, and `W32Init` / `W32Dispatch` are not claimed.

## Follow-on CPU40 return frontier

The first S3 image establishes the loader frontier above.  A separately
instrumented, behavior-neutral x86 rebuild (`SHA-256`
`E7F5687BAD30345AE3783D86C798556F2622D4F532D08AC65840850AE6F8752C`) was
then staged in a fresh short-root test package (`O:\\ntvdm64\\y`), with the
same immutable `WOWEXEC.EXE` and provider DLL.  It made no guest-media,
loader, callback, or BOP-result change.  Its default-off report records:

* the automatic DOSX lifecycle completes `53:11` in real mode; and
* the selected Win16 lifecycle subsequently enters `53:01`, completes the
  protected-mode `53:11`, and returns to original DOSX code at `00C7:0486`.
  That code executes `POP ES; CLC; RET`; the ordinary CPU40 near return then
  transfers to `00C7:0113`.

Only after that confirmed return does the trace observe control at the BIOS
`F000:E05E` `INT 19h` location and the process ends `0xC0000409`.  This is
not evidence that the pending hardware interrupt causes the terminal: it
only eliminates `53:11` itself as the first failed return and narrows the
next audit to the original protected DOSX execution/fault path between
`00C7:0113` and the BIOS transfer.  `51h`, `WOW32.DLL`, `W32Init`, and
`W32Dispatch` remain unreached/unclaimed.

A repeat using the same short-root form recorded `00C7:0113`'s first decoded
original opcode as `1E` (`PUSH DS`) and remained alive until the bounded
30-second observer timeout.  It did not load `WOW32.DLL`.  Consequently the
earlier `0xC0000409` is a reproducible observation but not yet a stable sole
terminal; the current frontier is the original DOSX basic block beginning at
`00C7:0113` and its subsequent wait/fault routing.  Neither outcome changes
the conclusion that `51h` is unreached.

## Current descriptor frontier

A subsequent behavior-neutral diagnostic relink traced the complete first
basic block rather than only its first opcode.  The protected `53:11` return
executes `POP ES; CLC; RET` and reaches the original DOSX sequence
`00C7:0113`: `PUSH DS; PUSH AX; MOV AX,0040; MOV DS,AX`.

The CCPU receives the exact `DS=0040` request three times.  It does not return
from `load_data_seg`, so there is no successful loaded-DS record and no read
of the following original `DS:0314` gate.  This is an earlier, concrete CPU40
descriptor frontier: the generic CCPU protected segment loader validates
selector `0040h` as an ordinary descriptor but the required descriptor is not
currently usable.  (The nearby DPMI386 special case labelled “Selector 40” is
decimal 40 / `0028h`, so it is explicitly not evidence for a `0040h` mapping.)
The next implementation audit is therefore the CPU40 selector-`0040h`
descriptor carrier, not WOW32's BOP 51 loader.

That carrier has now been restored from the original non-WOW DOSX definition:
`SEL_BIOSDATA` is a writable ring-3, 16-bit data cache at base `0x400` with
limit `0xffff`.  The repeated `MOV DS,0040h` now returns successfully and the
following original `DS:0314` word is `0600h`; its `TEST ...,0003h` is false,
so DOSX follows the normal fall-through path.  This proves the fixed GDT
entry is no longer the live frontier.  After the IRQ0 path returns, the
verified original far jump is `00C7:013A -> 00CF:0081`; it is not a BIOS
`INT 19h` transfer.  BOP `51h` remains unreached and the `00CF:0081` path is
the next original execution frontier.

## BOP FD real-mode continuation repair

The next trace reached original WOW DOSX `00CF:118C`, whose bytes are
`C4 C4 FD` (`BOP_SWITCHTOREALMODE`).  Before the repair, the generated
CPU40 BIOS table selected `illegal_bop` for `FDh`: execution merely continued
at `00CF:118F`, then the source `RET` consumed poisoned `AAAAh` stack data and
raised the recorded `RETN` limit fault.  This was a build-topology omission,
not a WOW32 loader failure.

The x86 BIOS object now selects its original monitor-gated `FDh` table entry,
and the existing CPU40 DPMI mode-switch owner restores the source's five-word
`DS,SP,SS,IP,CS` continuation frame.  CCPU must capture the frame first, clear
PE, then load those selectors, because its setters install descriptor caches
immediately whereas the original kernel-VDM provider wrote a passive context.

The fresh staged observer record `wowexec-fd-cache.txt.bop-return.txt` shows
that `AAAAh` is gone and `FDh` returns into the original real-mode DOSX carrier
(`D200:...`) instead of executing zeros at protected `00CF:118F`.  DOSX then
performs the expected `53:02` protected-interrupt registrations.  The
diagnostics-free repeat remains alive through the observer's 30-second window,
but still has `loaded-wow32=no` and no `51h` dispatch.  Thus this closes the
poisoned-return frontier only; it does not claim WOW32 loading or S3 closure.

The final relinked parent image is
`D934F828C5E902CB215834E7A17207698D58098E698537CE5663E747D534F524`.
`wowexec-fd-final.txt` repeats the diagnostics-free console-owning run from
fresh short root `O:\ntvdm64\=`: it reaches the bounded timeout with 30
modules sampled and `loaded-wow32=no`.  That is a clean liveness result after
the repaired `FDh` path, not a loader-success assertion.

The same hash-verified package was then observed for 120 seconds with every
default-off product diagnostic suppressed.  It again timed out with 30
modules, `loaded-wow32=no`, and no `51h` result.  The process accumulated less
than one CPU-second during that window, so the remaining state is not merely
the slow 256-entry `53:02` registration loop.  The observer's longer bound is
an observer-only option; its ordinary ten-second default and its child/guest
contract are unchanged.  This makes the current stop a quiescent post-DOSX
bootstrap frontier rather than a timed-out claim that WOW32 loaded.

## Reproduction records

Generated records (not project documentation authority) remain under
`build/M0-T396/S3/`:

* `dos-mem-observation.txt` and `.base-vdm.txt`;
* `wowexec-observation-4.txt`, `.base-vdm.txt`, `.dem-open.txt`,
  `.bop-return.txt`, and `.json`.
* `wowexec-return-near.txt` and `.bop-return.txt` for the follow-on
  CPU40-return observation.
* `wowexec-opcode-0113-short.txt` and `.bop-return.txt` for the repeated
  short-root fixed-opcode observation.
* `wowexec-fd-final-120s.txt` for the longer diagnostics-free quiescence
  observation.

The observer's live module sampling is host-only Toolhelp enumeration.  It
does not attach a debugger, alter a thread, inject a DLL, or write guest
memory.

## Post-FDh wait attribution

The fresh formal x86 relink with the repaired FDh continuation and an opt-in,
bounded CPU40 `HLT` observation was deployed to the same short-root package
and observed for ten seconds with all other product diagnostics disabled.  It
again timed out with `loaded-wow32=no`; its main stopped instruction resolves
to original `Sim32pGetVDMPointer`, and the HLT report remained empty.  This
rules out the specific claim that the post-DOSX quiescence is CPU40's HLT
loop.  It does not yet identify the caller's requested linear mapping or
establish a WOW32 loader result; the next frontier is the original
`Sim32pGetVDMPointer` caller/mapping path.

## Sim32 caller/mapping attribution

The stopped-in-`Sim32pGetVDMPointer` attribution was tested rather than used as
a causal conclusion.  The formal x86 product adds a default-off, bounded
scalar observation at the original converter's existing post-conversion point.
App captures and removes its report-path variable before original environment
construction, so the diagnostic does not alter the DOS guest environment.

The hash-verified short-root observation of `system32\WOWEXEC.EXE` again
remained live through the ten-second bound (`result=timeout`), loaded 29
modules, and did not load `WOW32.DLL`.  It emitted exactly 21 converter rows,
all with `pm=0`, before the bounded observation stopped receiving calls.  Map
resolution identifies the repeated caller RVA `00075122` as original
`VdmFreeVirtualMemory+0x112`; the remaining observed startup callers include
original DEM configuration/file paths (`demChMod`, `demIoctlDiskGeneric`) and
COMMAND `ExpandConfigFiles`.  No row records a protected-mode WOW/DOSX pointer
conversion or a BOP `51h` transition.

Therefore the prior sampled Sim32 location is not evidence of an active
Sim32 mapping loop, nor is it a justified repair target.  The live frontier is
after these finite startup conversions, at the original host event/wait or
guest scheduling boundary which should eventually return control to the
Win16/WOW bootstrap.  The next audit must attribute that wait's owning
original caller and its wake condition; it must not synthesize BOP `51h` or
alter `WOW32` loading.

## Win16 record-owner correction

### Question and inputs

The preceding observations correctly showed an empty PIF for the selected NE
input, but did not prove that the standalone container had selected the
original *WOW* request path rather than merely running a DOS record with a
Win16-looking command line. The audited inputs are original
`softpc.new/host/src/nt_reset.c`, original `dos/command/cmdmisc.c`, original
BaseSrv `srvvdm.c`, and the current app-to-copied-BaseVDM boundary.

### Source result and correction

Original `host_applInit` sets `VDMForWOW` only when its host argument parser
receives `-w`; original `cmdGetNextCmd` selects `GetWowKernelCmdLine` only
when that state is true. Original BaseSrv likewise keeps DOS and WOW command
records disjoint. The standalone boundary had source-classified an NE image
but still published `BASE_VDM_COMMAND_DOS`, and only appended `-f` to SoftPC.

The narrow correction is therefore limited to that boundary: a classified
Win16 image receives the existing `-w` launch selector, is published as a
WOW record, and does not arm the DOS one-record exhaustion terminal. The
copied broker now preserves the existing DOS/WOW discriminant instead of
rejecting the latter. DOS and unknown/no-command requests retain their
existing pure-DOS profile behavior. No guest byte, BOP result, WOW32 body,
or loader behavior was added.

### Verification and current result

The focused x86 `base_vdm_local_fixture.exe` passes after exercising the
Win16 disposition: it requires `-f -w`, `BASE_VDM_COMMAND_WOW`, zero PIF
bytes, and no DOS exhaustion terminal. The broker unit test also passes the
disjoint WOW publish/peek/consume case. Formal x86 relink produced
`original-softpc-process.exe` SHA-256
`085CB6578F69AF4156D19ED2147C9313A2794030D1F40318584675E5E91F989D`.

The hash-verified console-owning run of `system32\WOWEXEC.EXE` from the same
fixed short-root package still ended `0xC0000409`, sampled 15 modules, and
reported `loaded-wow32=no`; the existing scalar Sim32 observation made 64
real-mode rows before termination. This is evidence that the corrected owner
selection has not by itself crossed the known original DOSX frontier. It is
not evidence against the correction, and it does not claim BOP `51h`,
`W32Init`, or `W32Dispatch`.

### Follow-up

The next bounded audit should attribute the original DOSX execution/fault
path after the corrected WOW request has entered it, with a source-owned
observation at the first defensible boundary. It must not replace the WOW
queue, wake condition, or loader with app-side behavior.
