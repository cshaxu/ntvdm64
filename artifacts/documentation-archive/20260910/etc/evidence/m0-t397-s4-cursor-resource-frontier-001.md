# M0 T397 S4 — original cursor-resource frontier

## Question

After the ordinary `WRITE.EXE` route reaches the first real Win16
application initialization, does the remaining private USER cursor output
slot receive a public-compatible cursor payload?

## Method

All observations used the Console-owning `O:\t396\ntvdm32.exe` stage and
the existing `console-startup-observer` with `system32\WRITE.EXE`, a
30-second timeout and no inherited diagnostic selector. Fixed, compile-time
provider traces were used only to identify the original call sequence; each
diagnostic object was subsequently rebuilt without its marker before the
ordinary provider DLL was restored.

The bounded adapter recovery uses public `LoadCursor`/`LoadIcon` for the
null-module integer stock-resource case. It never treats the packed
`HMODINST32` value as a native module handle.

## Observation

The stock calls now return source-visible aliases:

```text
IDC_WAIT  -> 000003EE
IDC_IBEAM -> 00000386
IDC_ARROW -> 0000019E
```

The same run enters and returns original `WU32LoadAccelerators`, then
registers all four first-instance WRITE classes:

```text
MSWRITE_MENU
MSWRITE_DOC
MSWRITE_RULER
MSWRITE_PAGEINFO
```

The next original `WU32LoadCursor` calls are WRITE module resources.  The
original NTUSER owner is `ntuser/client/clres.c`:
`WowServerLoadCreateCursorIcon → ConvertDIBIcon`.  Its actual carriers were
an `RT_ICON` split DIB (752 bytes, beginning `28 00 00 00`) and an
`RT_CURSOR` payload (320 bytes, beginning `1F 00 00 00`): the latter prefix
is its original hotspot `(31,0)`.  The bounded public-GDI adapter now creates
the colour icon plus AND mask through `CreateIconIndirect`, and consumes the
cursor hotspot before creating its monochrome cursor.  Both cross the original
alias boundary (icon aliases `180E` and `13CE`; cursor alias `13B6`) without
treating packed `HMODINST32` as a native module or retaining a USER cache.
WRITE then creates `MSWRITE_MENU`.

The subsequent ordinary, no-diagnostic 30-second observation with the
restored late-loader parent and rebuilt ordinary provider supersedes the
earlier diagnostic GPF as the current frontier.  It reports `loaded-wow32=yes`
and sees a visible `#32770` top-level window titled `Write`, as well as the
non-visible `MSWRITE_MENU` window.  The process remains alive at the observer
timeout.  Thus the earlier `00C7:1828F` diagnostic stop is retained only as
historical instrumentation evidence, not as the ordinary-runtime terminal.

The title and class alone are not sufficient to identify that window as the
WRITE application surface.  The observer's passive `--observe-write-capture`
snapshot shows its rendered content is the original WRITE error dialog:
`Not enough memory for Write to complete this operation. Quit one or more
applications to increase available memory, and then try again.`  Its sole
button is `OK`.  The earlier close/menu probes therefore affected this error
dialog, not a WRITE document window, and are rejected as application UI or
lifecycle evidence.  The capture observer now takes no input action.

Treating the split DIB as a public `.cur`/`.ico` file, or fabricating a
default hotspot, remains rejected.  Fixed-path diagnostic providers were
rebuilt out before restoring the ordinary staged DLL.

### Follow-up: low-memory source boundary

The first `MmwCreate` failure edge is no longer speculative. In the original
path the custom class's `vpszMenu` is a Win16 resource name, but its
`HMODINST32()` identity is packed and cannot be a public USER32 `HINSTANCE`.
The projection consequently used to create `MSWRITE_MENU` without a native
menu, and the guest's immediate `GetMenu(hWnd) == NULL` went to `Error`.
`ADAPTER-WOW-020` preserves the original `WU32LoadMenu` resource sequence
(`FindResource16`, `SizeofResource16`, `LoadResource16`, `LockResource16`,
the server construction callback, then `UnlockResource16`) immediately before
the existing public projected-window creation. Its public-compatible callback
consumes only the already converted Unicode menu template with
`LoadMenuIndirectW`; it never treats the packed module as native or takes
resource ownership.

The ordinary, non-trace provider independently confirms the result: the
passive `t397_s4_write_class_menu_normal` observation reports
`top-level-window-04-class=MSWRITE_MENU` and `menu-present=yes`, and restores
the staged DLL to SHA-256
`4F264F3658F06A8750C86F8581E45082BECEBC15ACC86B96CF34379BA2CB2980`
afterwards. The same run still finds only the three controls of the source
low-memory panel. Thus the `GetMenu` edge is closed in the product image. In
the original `MmwCreate` order `FInitMemory()` is the only remaining failure
edge before the first horizontal-scroll-bar creation; no child control was
ever observed. The ordinary product failure is therefore `FInitMemory()`, not
the later child-control or page-info/DC setup.

The actual guest source keeps the error path narrow. In
`mvdm-guest/wow16/write/initwin.c`, `FInitWinInfo` creates the parent window,
then rejects a set `fMessageInzFailed`, `InitCommDlg`, `ObjInit`, `FInitArgs`,
or a null `ValidateMemoryDC` before calling `Error(IDPMTCantRunM)`. Within the
parent `MmwCreate` callback, the named failure edges are `FInitMemory`, the
three child/control `CreateWindow` calls, and the page-info DC/brush setup.
Thus “not enough memory” is a historical umbrella message, not evidence that
DPMI allocation alone failed.

Two adjacent hypotheses are now excluded without changing the product path.
Kernel31's WOW implementation of `GetTempFileName` is not a missing WOW32
thunk: it chooses `TEMP` itself and creates the scratch file through DOS
`INT 21h/5B00`.  A bounded ordinary run with both inherited `TEMP` and `TMP`
set to the existing `O:\\t396` stage reached the identical source-owned
three-control panel.  The scratch-file root therefore does not explain this
failure.  Separately, the retained CPU40 trace reaches the original
`DpmiFreeSpace` `DPMICALL 0500h` route as `53:0B` with clear carry, and its
`53:07` shared-XMS allocation succeeds.  The original `config()` WOW branch
sets `xmsMemorySize` to 8192 KiB, independently of the one-megabyte DOS-only
`MEM.EXE` profile.  The next investigation must consequently identify the
specific `FInitMemory` local/global-heap or initialization subcall; increasing
the configured memory or adding a `GetTempFileName` thunk would be unsupported
workarounds.

A fixed-destination diagnostic provider (without an inherited selector) did
reach provider init, both WOW shell windows, the four WRITE class
registrations, and `MSWRITE_MENU` creation. Its altered DLL then exposed a
guest `00C7:1829` GPF and the original `Application Error` panel before the
ordinary low-memory panel. Since the unmodified provider instead reaches the
ordinary low-memory panel, this GPF is not accepted as product behavior or as
the identity of a source edge: it establishes only a lower bound after menu
window creation. The diagnostic DLL was restored out of the stage immediately.

One narrower follow-up diagnostic was also rejected.  It compiled a separate
provider that emitted only fixed success/failure markers around the existing
`WG32CreateBitmap`, `WU32GetDC`, and `WK32GetProfileString` calls—the only
host-facing failure candidates in `FSetScreenConstants`.  In the isolated
stage that DLL terminated before the observer could create its report (exit
code 67) and produced no marker file.  The original normal DLL was restored
from its exact pre-run backup, again yielding SHA-256
`4F264F3658F06A8750C86F8581E45082BECEBC15ACC86B96CF34379BA2CB2980`.
Therefore file-writing from this provider, even at those three narrow calls,
changes the early WOW execution enough to be inadmissible as runtime evidence.
No result from that diagnostic is used to classify `FSetScreenConstants`.

### Follow-up: `FSetScreenConstants` exclusion

The observer container itself was re-run successfully with the normal staged
DLL before a second, more restrictive diagnostic was admitted. That
diagnostic compiled a fixed destination but selected **only** three
`write-init-*` records; unlike the earlier broad provider trace it did not
write at provider initialization, window creation, menu creation, or error
paths. It recorded:

```text
MVDM-WOW32-PROVIDER write-init-get-dc-success
MVDM-WOW32-PROVIDER write-init-profile-string-nonempty
MVDM-WOW32-PROVIDER write-init-profile-string-nonempty
```

The diagnostic still reaches the same source-owned low-memory dialog. In the
original `FSetScreenConstants` order, `CreateBitmap(1,1,1,1,NULL)` precedes
`GetDC(hParentWw)`, and both profile calls occur within its
`FGetPrinterFromProfile` leg. Therefore the preceding `FCreateRgbp`, the
reserved local block, `hpfnfcb`, and the entire false-return surface of
`FSetScreenConstants` have been crossed. The missing `WG32CreateBitmap`
record does not weaken that conclusion: the source cannot reach `GetDC`
without a non-null bitmap, and that particular GDI invocation does not use
the selected public thunk body.

The test DLL was restored in the same command's `finally` clause; the stage
again hashes to
`4F264F3658F06A8750C86F8581E45082BECEBC15ACC86B96CF34379BA2CB2980`, and the
temporary source markers were rebuilt out of the provider. The remaining
finite failure set is now `FInitDocs`, `FInitProps`, `FInitFiles`, or the
final emergency save-heap `HAllocate`; it is no longer a generic local heap,
GDI/DC, profile, temp-root, or DPMI/XMS premise.

## Disposition

This is progress beyond the prior shell-only state: original OLECLI setup,
stock cursors, accelerator setup, four WRITE class registrations and
menu-window creation all run.  The first visible surface is a source-owned
WRITE low-memory error dialog, not a document window.

The cursor-resource frontier is closed. The next bounded frontier is the
source-first cause inside `FInitMemory()`—its Win16 local/global allocations
and the finite `FSetScreenConstants`/documents/properties/files subcalls—not
the already-unreached child/control or memory-DC conditions. No WRITE
keyboard/menu/document/lifecycle behavior is yet accepted.

### Reproducibility hold: stage product identity changed

The original 16-bit toolchain was subsequently exercised in a build-only
diagnostic tree.  It compiled the mirrored `initmmw.c`, linked the resulting
single replacement object with the mirrored original WRITE objects, and
applied the original `WRITE.RES`.  This proves the source/tool lineage is
usable for a future controlled subcall experiment, but does **not** yet
classify any `FInitMemory` cohort: the later controlled run was invalidated
before guest execution.

Specifically, the stage product had changed since the preceding accepted
normal observation.  The accepted run used the then-current image at base
`0x001c0000` and loaded `WOW32.DLL`; the later `O:\\t396\\ntvdm32.exe`
was timestamped `2026-09-08 21:49:37` and hashes to
`4e961f777704476ce8f19f6bfde72167127f42e1452c83d8dfd026cc491650d1`.
Under the exact same console-owning observer invocation it stopped with only
15 modules, `loaded-wow32=no`, and no top-level or child window—even when the
unmodified, byte-identical original `WRITE.EXE` was staged.  The later run
therefore cannot distinguish a rebuilt WRITE binary from the changed product
container and is rejected as S4 behavior evidence.  Each temporary
`WRITE.EXE` substitution restored the stage file to its recorded original
SHA-256 `08ee1659788880fb593815ff9609bfe76743c1182d05e6702764bf3dff68b30b`.

That prerequisite is now satisfied by the preserved-parent isolation control
below.  Any subsequent one-object guest diagnostic must be launched through
the same diagnostic-free ordinary container, then may partition `FInitDocs`,
`FInitProps`, `FInitFiles`, and the final save-heap allocation.

### x86-only reconstruction control (2026-09-09)

The active product scope is x86 `ntvdm32.exe` only.  A fresh full x86 parent
link and the complete 77-body x86 WOW32 provider link both succeeded.  The
provider build also exposed and corrected a build-tool selection defect: an
omitted `-OnlySourceLeaf` was treated as a one-element null list, so the
fallback skipped every source body and attempted a nonsensical partial link.
The corrected fallback compiled all source bodies and linked `wow32.dll`
(510,976 bytes) against that parent import library.

Those first output files were not a replacement acceptance baseline: their
generator used Node 24.  Three short-root (`O:\\n397*`) passive/diagnostic
controls using the overwritten S3 parent all reproduced `loaded-wow32=no`; the
S3 artifact itself now has the same changed SHA-256 as
`O:\\t396\\ntvdm32.exe`.

The documented local Node 22.22.1 runtime was then found at
`O:\\.nvm\\versions\\node\\v22.22.1\\bin\\node.exe`.  A new formal parent
and same-generation complete provider were built with it, with hashes
`d001c29b34f076947edbdecca112a370cdbd8c6a15ae51969854747497518c56` and
`90b0b43e8d9969c2ede0a0dde7600f4a6e4985d6bcf5ba1a5a317ca062c7ca10`.
Its passive short-root **diagnostic-container** `WRITE.EXE` run still did not
load WOW32.  The
app-captured COMMAND and post-copy BOP observers establish the useful next
frontier: the launch record is consumed, original DOSX/DPMI services
`53:01`, `53:04`, `53:07`, and `53:14` execute, but no `51h` dispatch occurs
before CCPU waits in `c_cpu_take_event`.  This is a pre-`MS_bop_1`
bootstrap/interrupt-event frontier, not a WOW32 loader or x64 problem.  No
x64 compile, link, or runtime issue was investigated or changed.

### Preserved-parent isolation control (2026-09-09)

The later no-WOW result is not explained solely by the fresh-parent generator
or by the `4e961f...` S3 parent replacing the parent used by the earlier
ordinary WRITE captures.  A new short-root stage, `O:\\n397e`, was copied from
the accepted media without its diagnostic log directory.  It received the
preserved pre-replacement x86 parent
`a2000cbb16c8de2743aff57288e15834266a115df567f43887516b0ecff3ce20` and the
unchanged ordinary complete provider
`4f264f3658f06a8750c86f8581e45082becebc15acc86b96cf34379ba2cb2980`.

The first Console-owning observer invocation retained its **default
diagnostic environment variables** (although no explicit trace file was
requested):

```text
ntvdm32.exe -f -o --command system32\WRITE.EXE
```

ran for 30,250 ms.  It reported 16 modules, `loaded-wow32=no`, no top-level
window and a CCPU thread stopped in the parent image.  It did not modify the
existing `O:\\t396` stage.  This does not test the ordinary product path:
the default observer selectors are part of its altered diagnostic container.

A second run constrained the inherited `PATH` to only the isolated root, its
`system32` media directory and `C:\\Windows\\System32`.  It again timed out
after 30,250 ms without WOW32 or a window (15 modules rather than 16).  The
search-path control therefore does not restore the altered diagnostic
container.  It is not a product PATH requirement and does not authorize a
PATH-dependent fallback.

The same parent, provider, media and command were then run with the
observer's `--observe-without-diagnostics` switch.  This ordinary control
timed out only because the live original WRITE process remained open; it
loaded `WOW32.DLL` (33 modules), created the visible `WOWExec` and `Write`
windows, and exposed the source-owned three-control message:

```text
Not enough memory for Write to complete this operation.
```

Thus the preserved `A2000...` x86 parent plus `4F264...` provider is again a
valid ordinary S4 baseline.  The prior pre-`MS_bop_1` reports are evidence
that the observer's default diagnostic environment changes this bootstrap;
they are not evidence of a product regression, a missing DPMI/WOW32 provider,
or an x64 source issue.  Ordinary acceptance must continue to use the
diagnostic-free container.

### Reproducible matching-pair and guest-tool stress control (2026-09-09)

The externally preserved stage was subsequently removed, so it cannot remain
an acceptance dependency.  A fresh manifest-controlled short-root stage,
`O:\\n397g`, was instead generated from the workspace with the matching
Node 22.22.1 pair: parent
`d001c29b34f076947edbdecca112a370cdbd8c6a15ae51969854747497518c56` and
complete provider
`90b0b43e8d9969c2ede0a0dde7600f4a6e4985d6bcf5ba1a5a317ca062c7ca10`.
In the same diagnostic-free Console owner, `system32\\WRITE.EXE` loaded that
staged `WOW32.DLL` (34 modules), created the visible `WOWExec` shell and the
visible `Write` low-memory dialog.  This is a current, reproducible ordinary
baseline rather than an external-directory dependency.

The same stage also separates tool availability from compiler execution.
Launching `w\\write\\cl16.exe` with no arguments through `ntvdm32.exe`
exits normally.  Its first direct original-style `initmmw.c` compile
invocation, with `/AM /G2sw /Os /Zpe /PLM` and explicit guest include paths,
terminated the product with `0xc0000005` after roughly 2.3--3.0 seconds.
That result is now classified as **missing historical compiler pass layout**,
not a CPU40/DPMI fault: `cl16` starts `c13216`, `c23216`, `c33216` and its
message files as peer programs, while the selected external program starts
with the package root as its current directory.  Copying those unchanged
historical peers into that isolated root removes the access violation and
returns the compiler's own code `2` instead.

The code-2 remainder persists with explicit Write/include directories, with
a root-named source, and with the complete governed `cl16` sibling set
colocated at the root.  A separate original 357-byte `aaa.c` control under
that same complete set instead returns to `0xc0000005`.  Therefore sibling
layout explains the first AV but does not establish a stable compiler
execution path; the remaining failure is a general historical-toolchain
environment/argument frontier, not a WRITE-source diagnostic.  The captured
console contains only the compiler banner, so it is not yet possible to
classify code 2 as a source error or a remaining environment dependency.  The
package's `COMMAND.COM /c` external-`.BAT` route also returns normally without
executing the batch body, so inherited historical compiler logs are not fresh
evidence.

This is a bounded 16-bit compiler toolchain-layout frontier, not a WRITE
`FInitMemory` cause and not authority to change CPU/BOP semantics inside this
S4 dialog audit.  It proves the tool executable is found and that the
matching WOW pair is sound; it does not claim that the guest compiler or full
guest build chain is accepted.

The retained historical one-object diagnostic `WRITE.EXE`
(`127a3bc6ec4626f11130a4243f2177c40a469a66c10c3879dbf372ecf8e541d7`,
distinct from the selected original) was
also run once in the same `O:\\n397g` diagnostic-free container.  It loaded
WOW32 and displayed the identical three-control low-memory panel.  It
therefore provides no cohort partition on the restored baseline and is not
used as `FInitDocs`, `FInitProps`, `FInitFiles`, or save-heap evidence.  The
stage's original `WRITE.EXE` was restored immediately and verified back at
`08ee1659788880fb593815ff9609bfe76743c1182d05e6702764bf3dff68b30b`.

### Guest local-heap boundary exclusion (2026-09-09)

The source route from WRITE's `HAllocate()` is now classified precisely.
`heapmain.c` calls the guest-visible Kernel31 `LocalAlloc(LHND, cb)` and
converts its zero return to WRITE's `hOverflow` sentinel.  Kernel31's
original `linterf.asm:ILocalAlloc` allocates from the current 16-bit local
heap through `lenter`, `halloc`/`lalloc` and `lleave`; it returns zero on that
heap's allocation failure.  This is not the WOW32 `RET_LOCALALLOC` callback
path.  That return entry exists only for a 32-bit USER-side caller crossing
back into a 16-bit instance-local heap (`wcall16.c:LocalAlloc16` →
`wow16cal.asm:WOW16LocalAlloc` → Kernel31 `LOCALALLOC`).

Consequently, the low-memory dialog does not justify a repair to
`wcall16.c`, `wcall32.c`, a native-pointer conversion, or any native-x64
compatibility branch.  The unresolved `FInitMemory` frontier remains inside
the selected original Win16/KERNEL local-heap and later WRITE initialization
cohorts, to be isolated by a source-shaped x86 observation only.

### Exact remaining false-return surface (2026-09-09)

The remaining four cohorts are not equivalent.  Source review gives the
following finite return surface after the already-observed initial heap and
screen-constant work:

| Cohort | Can return `FALSE` to `FInitMemory` | First non-local boundary |
| --- | --- | --- |
| `FInitDocs` | `hpdocdod` allocation, either initial `DocCreate`, or `hszFlatSearch` allocation | none before those results; all are WRITE/Kernel31 local-heap work |
| `FInitProps` | only `vhgchpFormat = HAllocate(...)` | none |
| `FInitFiles` | only `FnCreateSz(... dtyNetwork)` or either final `cwBTE` `HAllocate` | `FnCreateSz → FCreateFile → GetTempFileName`, then `FAccessFn` |
| final save reserve | `vhrgbSave = HAllocate(cwSaveAlloc)` | none |

`PnAlloc` and `PchGetPn` are not false-return edges: they immediately index
the page-buffer path and would fault or take their own file/error behavior,
not return the `FInitMemory` umbrella failure.  Conversely, `FCreateFile`
explicitly turns a failed `GetTempFileName` **or** a failed follow-on
`FAccessFn` into `FnCreateSz == fnNil`; the former TEMP-root control therefore
does not by itself exclude the latter access/open contract.  The next dynamic
observation must distinguish that single file-service result from the three
remaining guest-local allocations.  It must not change guest semantics or
add a speculative memory-size workaround.

### Rejected environment-record probe (2026-09-09)

Kernel31's original `IGetTempFileName` proves the candidate's exact DOS path:
it obtains the name from `TEMP` (or the Windows directory), executes
`INT 21h/5B00` to create it, then `INT 21h/3E` to close it.  The selected
NTDOS dispatch maps function `5Bh` through `$CreateNewFile` to
`SVC_DEMCREATENEW` and original `demCreateNew`/`demCreateCommon`.

An existing default-off `MVDM_DEM_CREATE_REPORT_PATH` observer appeared to be
the narrowest possible dynamic witness.  It is **rejected**.  In the same
`O:\\n397g` x86 stage and Console-owning no-diagnostics container, exporting
that variable changed the normal source-owned low-memory panel into an
earlier visible `Application Error` panel and emitted no create report.  The
variable is inherited by the guest bootstrap, so this result cannot be read
as an absent create call or a create failure.  It instead proves that the
environment-driven DEM reporter is itself an altered guest container for this
frontier.  The ordinary product run without that variable remains the only
runtime baseline.  A future partition must use a non-environment guest-safe
observation mechanism or an isolated source-shaped diagnostic that does not
cross the guest inherited environment.

### Save-reserve phase witness (2026-09-09)

An isolated, compile-time x86 witness wrote only fixed phase labels at the
four source-map locations in WRITE's `FInitMemory`; it neither reads guest
memory nor exports an environment selector.  The ordinary Console-owning
`O:\\n397g` run retained the normal WOW32 load and source-owned low-memory
dialog.  Its sole label was:

```text
MVDM-T397-WRITE-INIT phase=save-buffer cs=00C7 ip=0118
```

The selected WRITE map resolves that site to
`vhrgbSave = HAllocate(cwSaveAlloc)` with `cwSaveAlloc = 128` words (256
bytes).  The absence of earlier labels proves that `FInitDocs`, `FInitProps`,
and `FInitFiles` were crossed before the umbrella failure.  It does not by
itself prove whether the final `HAllocate` returns zero, but the source has no
remaining `FInitMemory` false edge after that call site.  The dominant next
candidate is therefore the final Kernel31 local-heap allocation/growth path,
not scratch-file creation, WOW32 local-allocation callbacks, or any x64
condition.

### Rejected broad CCPU witness (2026-09-09)

A separate broad instruction recorder for selector `00C7` used the generic
machine observation path, which translates guest addresses while CCPU is
executing.  That altered image raised a general-protection fault in
`KRNL386.EXE` at `0001:7A7F`, unlike the ordinary low-memory-dialog run.
Its large instruction log is retained only to make the rejected experiment
auditable.  It is not product evidence and cannot select a repair.  Subsequent
dynamic work must remain scalar and non-reentrant at the observed host/guest
boundary.

### DPMI shared-XMS reallocation exclusion (2026-09-09)

The next candidate was tested rather than inferred.  A temporary x86-only
scalar witness at CPU40 `DpmiReallocateVirtualMemory` recorded its
`SAReallocate` scalar inputs/results directly to the isolated stage; it made
no guest-memory translation, read, retention, or write.  The matching
`O:\\n397g` ordinary `WRITE.EXE` observation still loaded `WOW32.DLL` and
remained at the same source-owned low-memory dialog, but produced no witness
record at all.  Thus the reached final Kernel31 local-heap allocation does
not call the DPMI linear-memory reallocation route.  `MVDM-HOST-DIV-224` and
the CPU40 shared-XMS `SAReallocate` contract are excluded from this WRITE
failure; no source repair is selected.

The temporary body and its isolated build helper were removed, the ordinary
x86 image was rebuilt, and its map no longer contains the witness symbol.
The external stage was restored from that ordinary image.  The next source
owner is Kernel31's local-heap/segment-growth behavior before any DPMI
reallocation boundary; this remains unrelated to x64.

### Final-save local-heap callback partition (2026-09-09)

A second temporary x86-only scalar observer armed at the already-established
WRITE `FInitMemory` final-save instruction (`00C7:0118`) and then recorded
only first hits of three selected original Kernel31 entry points.  It neither
translated guest memory nor used an environment-controlled diagnostic.  The
no-diagnostic ordinary `WRITE.EXE` stage retained the same low-memory panel
and recorded `write-final-save` followed by `IGlobalReAlloc`
(`021F:0EF8`); it recorded neither `LocalNotifyDefault` (`021F:8FF2`) nor
`ragrow` (`021F:556F`).  The raw record is retained at
`artifacts/research/m0-t397-s4-write-final-save-phase.txt`.

This is deliberately a partition, not an attribution: `IGlobalReAlloc` may
be a later unrelated Kernel consumer, so the record does not prove that the
final reserve invokes it.  It does prove that the reached route does not use
Kernel31's default no-free-local-block callback/growth path during the bounded
observation.  The exact original WRITE source also corrects the next cohort:
`HAllocate` first calls `LocalAlloc`, then on zero calls
`FTryGrow → GivePages → LocalReAlloc(rgbp)` before its retry.  The remaining
audit must resolve that WRITE-owned reclaim/retry return path, not repair
CPU40 shared XMS, x64, or the excluded default `LocalNotifyDefault` path.
The temporary CCPU body and helper were removed, the ordinary x86 image was
rebuilt, and the isolated stage was restored from it.

### Final-save instruction decoding correction (2026-09-09)

The prior scalar marker at `00C7:0118` is a **pre-call** marker, not a return
marker. The selected `WRITE16.EXE` NE segment-5 bytes decode as `push 128` at
`0118`, followed by the relocated far `HAllocate` call at `011B`; the return
address is `0120`. `0128` is the failed-allocation branch, while `012B` is the
success tail. Thus the earlier `IGlobalReAlloc` record remains only a later
bounded-run observation; it cannot characterize the `HAllocate` result.

A one-call `CALLF` observer was then tested at the decoded source call using
only the already-decoded far operand. It preserved the ordinary no-diagnostic
WOW32/WRITE low-memory-dialog result but made no record: CCPU's internal
instruction position at that invocation is not the expected `00C7:0120`.
It does not select a repair or negate the source route. The observer and build
helper were removed immediately, then the ordinary x86 image was rebuilt and
restored to `O:\\n397g` with SHA-256
`803A6F12034BFD9CA36D1FE0AE1372C07C07BEDEFD56B70DF7D3BC247DBEE98D`.
The remaining frontier is still the original Kernel31/WRITE local-heap
allocation result, not a CPU40 shared-XMS or x64 issue.

### Selector-to-WRITE attribution withdrawn (2026-09-09)

The preceding local-heap discussion must not be used to select a repair. A
fresh bounded scalar x86 observation recorded the *actual decoded sequence*
for the prior `00C7:0118` marker: `0110:E8`, `0113:1E`, `0114:50`,
`0115:B8`, `0118:8E`, `011A:9F`, `011B:F7`, `0121:75`, `0123:F7`, and
`0129:75`. That disagrees with the immutable, source-identical `WRITE.EXE`
segment-5 bytes (`0118:68`, followed by far call `011B:9A`). The guest media
itself is verified identical across the project, `O:\\n397g`, and the
OpenNT `nt\\private\\mvdm\\wow16\\write\\WRITE.EXE` source (SHA-256
`08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B`).

Thus a raw selector/IP pair is not a stable module identity in this run and
the earlier claimed crossings of WRITE `FInitDocs`, `FInitProps`,
`FInitFiles`, final `HAllocate`, the derived `SAReallocate` exclusion, and
the `LocalNotifyDefault`/`ragrow` partition are all **withdrawn as WRITE
evidence**. They remain rejected diagnostic experiments only; none authorizes
a CPU, DPMI, WOW32, local-heap, or x64 repair. The temporary scalar observer
and helper were removed, and the ordinary x86 stage was rebuilt/restored with
SHA-256 `F2C2C9E42ED0BDB8F2E5C0B3A39E1F1EE9A5A122B985CA84FA4FCE9F12753087`.

The valid S4 fact remains narrower: ordinary x86 `WRITE.EXE` loads
`WOW32.DLL` and reaches its own visible low-memory dialog. A future causal
audit must first establish a module-qualified mapping (rather than infer one
from a selector) before it can trace the dialog's initiating condition.

The first module-qualified attempt is also negative, but safely informative:
a bounded CCPU-code-page signature search for the source-identifying sequence
`68 80 00 9A` (WRITE's final-save `push 128; call far`) produces no record in
the ordinary no-diagnostic run. It does not prove which earlier WRITE branch
fails, but it proves the final reserve is not reached in that run. The
signature observer was removed immediately; the restored ordinary x86 stage
has SHA-256 `CD0B25CBAC3D26BD71BB5EFE1172B1C895C7DD4F5EF0F6CC28401BCA1D2DC8E8`.

### Rejected `FInitMemory` signature trace and restored ordinary x86 image (2026-09-09)

A later temporary CCPU observer correctly matched the immutable WRITE
`FInitMemory` prologue, but its expanded per-instruction file-write trace did
**not** preserve the ordinary route: the no-diagnostic Console-owner run ended
at an `Application Error` dialog (`<unknown>00C7:1829`) instead of WRITE's
low-memory dialog. Its detailed instruction sequence is therefore rejected as
causal evidence, including any apparent control-flow or call result.

The temporary body and its disposable build command were removed. A fresh
Win32/x86 CCPU object/archive/product relink was staged at `O:\n397g` as
`ntvdm32.exe` (SHA-256
`462458D05874B6FD77EE863D604490B2BC66F44810204AC8270B14228DDFBEF4`). The
same no-diagnostic, Console-owning 20-second observation again loads
`WOW32.DLL`, shows the visible `WOWExec` shell and the source-owned `Write`
`#32770` low-memory dialog with its `OK` button, then remains live until the
observer timeout. This is the restored ordinary x86 baseline. No native-x64
compile, link, stage, runtime, or acceptance work occurred or is required.

### CCPU host-I/O observer boundary (2026-09-09)

A final, smaller source-qualified experiment confirms the causal reason that
the preceding CCPU traces are unusable. It matched only the immutable
`FInitMemory` prologue and attempted one host `fopen`/`fprintf` record before
observing any cohort return. That single record was enough to change the
otherwise no-diagnostic run into the same `Application Error`
`<unknown>00C7:1829` panel. The one-line `bound cs=14F7` output establishes
only that the prologue matcher fired; it establishes **no** `FInitDocs`,
`FInitProps`, or `FInitFiles` result and is rejected as causal evidence.

Therefore host file I/O from the CCPU decode loop is not a valid S4 observer
transport, even at one call. The temporary source and helper were removed,
then the ordinary x86 product was rebuilt/staged (SHA-256
`925BBAF31410E9F381EE2B1CD437F56A2E1082B352DF93E5CD8096FF7B150F72`). A
fresh Console-owner run returns to the normal `WOW32.DLL` plus `Write`
low-memory dialog. The next observation must use a non-CCPU host-I/O path or
a guest-safe external mechanism; it must not turn this rejected transport into
a CPU, DPMI, WOW32, local-heap, or x64 repair.

### Rejected static-marker transport (2026-09-09)

An alternative temporary x86 observer wrote only a process-local static marker
from the CCPU decoder; the existing Console observer then read the launched
process image through `ReadProcessMemory`. Unlike host file I/O, this retained
the ordinary WOW32/WRITE low-memory dialog. However, the externally recovered
marker remained its initialized image copy and did not provide a verifiable
live decode-state record. It therefore proves no `FInitMemory` cohort result.

The marker source, the observer extension, and their build helper were all
removed rather than retaining an ambiguous diagnostic transport. The ordinary
x86 product and ordinary observer were rebuilt; the staged image is
`C4BFB5B7096DD5BF4B43D2AC2C19AE1E9792FCFC2B61E80981577FFB2CB05798` and the
subsequent no-diagnostic WRITE run again loads WOW32 and reaches the original
low-memory dialog. The S4 causal frontier is unchanged: obtain an observer
whose live state is independently verifiable before attributing a repair.

### Rejected XMS-capacity control and x86 restore (2026-09-09)

A reversible x86-only control changed the original WOW configuration's
`xmsMemorySize` from `8192` KiB to `32768` KiB, rebuilt only the x86 parent,
and ran the ordinary Console-owner observer without diagnostic environment
variables. The 20-second run still loaded `WOW32.DLL`, showed the same visible
`WOWExec` shell and `Write` `#32770` panel, and timed out with the unchanged
text `Not enough memory for Write to complete this operation.` Thus the panel
is not explained by the simple total-XMS-capacity threshold tested here.

The control was removed immediately. The original `8192` KiB source value is
again in place, the x86 parent was rebuilt and relinked, and the restored
`O:\\n397g\\ntvdm32.exe` has SHA-256
`B9C5D59817D19530D3759693FD6ED7804FCE0D0C8CD905F1213314F36CE8D6C1`.
A fresh 10-second no-diagnostic run again loads `WOW32.DLL`, displays the
same `Write` low-memory panel and remains live to timeout. This control
selects no source repair. It reinforces the current boundary: x86
`ntvdm32.exe` is the only product/acceptance target; native-x64 work remains
retired except for a demonstrated architecture-neutral mapping-manager defect.

### Rejected CCPU external-event observer (2026-09-09)

The attempted replacement for file-I/O and static-marker transports was also
rejected. A temporary x86-only decoder check matched the immutable first
thirteen bytes of WRITE's `FInitMemory` code page and, only on that match,
created and signaled a one-shot named Win32 event. An external observer waited
for that event without supplying any diagnostic environment variable or guest
input. The event did not signal, but more importantly the same run changed the
ordinary WRITE result into the known `WRITE caused a General Protection Fault
in module <unknown>00C7:1829` panel. It therefore cannot support either a
negative reachability result or a causal claim.

The event code and helper were removed. A newly rebuilt/staged ordinary x86
image (SHA-256 `E821A59A60442B69575CD1A4EFEF5A3C94C6A07235FF98FF394B3E95313662BB`)
again loads `WOW32.DLL` and displays the original Write low-memory dialog in a
no-diagnostic Console-owner run. The evidence now rules out another whole
class of observer transport: any host call from the CCPU decode path,
including a single event operation, is invasive for this route. The next
observer must reside outside that decode path; no CCPU, local-heap, DPMI,
WOW32, or x64 repair is selected.

### Read-only NTVDMx64 DPMI/WOW comparison (2026-09-09)

The external `ntvdmx64` tree was inspected as comparison material only; no
source, binary, guest media, build input, or runtime dependency was imported.
Its documented `dpmimem.patch` raises the *DOS* DPMI default from 8 MiB to 16
MiB and accepts PIF/environment overrides.  That is not a candidate for this
ordinary WOW run: the current original configuration already selects the
source-defined WOW DPMI value of 16 MiB, and the earlier reversible increase
of its distinct WOW XMS allocation from 8 MiB to 32 MiB left the same WRITE
panel unchanged.

Its `common/dpmi.patch` confirms that CPU40 CCPU-only DPMI needs explicit
protected/real-mode transition state rather than the kernel VDM monitor
assumptions.  The current x86 product already has the corresponding selected
CPU40 transition cohort (`DpmiSwitchToRealMode`,
`DpmiSwitchToProtectedMode`, and the CPU40 IDT carrier); therefore the patch
is not evidence of a missing local-heap allocation repair.

The external `minnt/wow32.patch` is also not an import candidate.  Its core
changes are W10-specific private-USER compatibility: releasing/reacquiring a
thunk lock around public calls, an ad-hoc `MsgWaitForMultipleObjects` pump,
and private-window/dialog fallbacks.  Those mechanisms would reintroduce the
private scheduler and presentation ownership this product deliberately does
not have.  It nevertheless provides a useful source comparison: the original
task order remains material.  The current original `WK32WOWInitTask` creates
the task suspended; `W32Thread` completes `InitTask`, signals the existing
`ghevWaitCreatorThread`, and the creator crosses `WK32Yield` before the new
task's `host_simulate` entry.  The next causal audit is therefore that
source-defined shared-CCPU task hand-off, using a transport outside CCPU
instruction decoding.  No x64, DPMI-size, or external private-USER repair is
selected.

### Shared-CCPU task-handoff source audit (2026-09-09)

The current source already establishes the newly created WOW task's **TLS
unwind** precondition.  `WK32WOWInitTask` creates `W32Thread` through
`host_CreateThread`; that wrapper calls `ccpu386newthread()` in the child
before the WOW task body runs, and `ntthread.c` allocates a per-thread
`ThreadSimBuf` for `ccpu386SimulatePtr`/`ccpu386Unsimulate`.  Thus a missing
child TLS jump frame is not the explanation to repair.

That same audit shows why this does not prove safe shared execution:
CCPU's
`simulate_level`, `CCPU_SR`, instruction pointer, GDTR/IDTR, CPL and decoder
operand state remain process-global in `c_main.c`; `host_simulate()` forwards
directly to the selected CCPU simulation vector and has no CPU-owner lock.
The original model relies on the WOW non-preemptive hand-off (new task
`InitTask` → `ghevWaitCreatorThread` → creator `WK32Yield`) to ensure that
only the scheduled task enters guest execution.

The existing `MVDM-HOST-DIV-255` first-turn release preserves only the first
event hand-off.  It is not evidence that every later `host_simulate` entry is
single-owner, and this static fact alone does not attribute WRITE's low-memory
panel to a race.  The next admission-quality observation must establish,
without entering the CCPU decode loop, whether the ordinary WRITE run has one
or concurrent CCPU entrants across this existing task hand-off.  No locking
change, scheduler substitute, memory-size alteration, or x64 work is selected
until then.

### Shared-CCPU scheduler and passive thread snapshot correction (2026-09-09)

The subsequent source audit refines the preceding boundary.  The current
`ADAPTER-WOW-017` is not a mere `SwitchToThread` substitute: once `InitTask`
has registered both original WOW task threads, `wow_private_user_yield_to`
marks the yielding task waiting and uses `SignalObjectAndWait` to transfer the
existing per-task event token to an already waiting peer.  If that peer has
not yet waited, the current task waits without leaving a runnable token.  This
retains the source-owned first-task creator event (`ghevWaitCreatorThread`) and
implements the later cooperative hand-off without treating the opaque USER
thread ID as a Win32 handle.  Thus the source has a bounded no-two-runnable
task mechanism after task registration; it is still not a proof about every
possible callback re-entry.

An ordinary, no-diagnostic, 10-second Console-owner run of the restored x86
stage (`E821A59A60442B69575CD1A4EFEF5A3C94C6A07235FF98FF394B3E95313662BB`)
again loaded `WOW32.DLL`, created visible `WOWExec` and Write `#32770`
windows, and displayed the same source-owned low-memory text.  Its external
thread snapshot resolved one final product return chain through original
`nt_bop` and CCPU `c_main` code, but that thread's instruction pointer was in
the public USER/kernel wait at capture time; another product thread resolved
to original SoftPC sound/timer code.  This is a passive end-state observation,
not evidence that CCPU entries were concurrent or serial throughout startup.
It selects no scheduler, CCPU, allocation, DPMI, or x64 repair.

### Reversible `MmwCreate` outer-guard control (2026-09-09)

The source-owned text in the observed `Write` dialog is `IDPMTCantRunM`, the
terminal `FInitWinInfo` failure text.  It is not itself a report that
`FInitMemory` returned zero.  `MmwCreate` executes earlier in that initializer
and has its own result edges for the menu, `FInitMemory`, three child-control
creates, `GetDC`, brush creation, and object selection; later
`FInitWinInfo` calls can also select the same terminal text.

To test only the previously inferred outer `FInitMemory` edge, a copy of the
selected x86 stage guest `WRITE.EXE` was backed up outside the product tree.
The NE code byte at file offset `0x2386`, the `JNZ` in the `MmwCreate`
`FInitMemory()` result check (`75 03 E9`), was changed to `EB 03 E9` for one
no-diagnostic, Console-owning 10-second observation.  The run retained the
same `WOW32.DLL` load, `MSWRITE_MENU`, visible `WOWExec` shell and visible
`Write` low-memory dialog with identical text.  The stage guest was restored
immediately; its SHA-256 is again
`08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B`.

This control does **not** prove that `FInitMemory` normally succeeds: forcing
execution after a real failure may permit a later malformed-state failure.
It does prove that the panel cannot uniquely identify that outer guard.  The
previous Status inference which called `FInitMemory` precise solely because a
post-return control was absent is therefore withdrawn.  The finite next
source boundary is the remaining `MmwCreate` result edges and the subsequent
`FInitWinInfo` owners, using only reversible external x86-stage controls or
non-invasive observation.  It selects no product source repair and no x64
work.

### `MmwCreate` child-control guard controls (2026-09-09)

Three further isolated external x86-stage controls each replaced one verified
`MmwCreate` child-control `JNZ` failure guard with the corresponding
unconditional short jump, restoring the stage guest from the same backup
before the next run.  The controls were: horizontal-scroll creation at
`0x23b7`, vertical-scroll creation at `0x23ed`, and page-info creation at
`0x2423`.  In all three no-diagnostic seven-second Console-owner observations,
WOW32 loaded, `MSWRITE_MENU` existed, `WOWExec` and the `Write` `#32770`
dialog were visible, and the dialog had the identical `Not enough memory...`
text and three child controls.

Each result has the same deliberate limitation as the outer-guard control:
it does not prove the bypassed create normally succeeds, because another
later failure can retain the terminal text.  It does establish that none of
these three individual result guards uniquely selects that text.  The staged
guest was finally restored and rehashed as
`08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B`.
Remaining finite `MmwCreate` candidates are its DC/brush/selection edges;
no source repair is selected.

### DC/GDI guard controls and original conversion audit (2026-09-09)

The remaining individual controls completed the `MmwCreate` surface.  Forcing
the `GetDC` (`0x2433`) or `CreateSolidBrush` (`0x2448`) zero-result branch to
fall through retains the ordinary low-memory dialog.  Forcing the
`SelectObject` success edge (`0x2455`) instead produces an `Application Error`
GPF, while WOW32 still loads.  That GPF is not a repair result; it shows that
the original failure guard protects a real downstream state requirement.
The stage guest was restored after every run and finally rehashed as
`08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B`.

Source inspection gives this candidate a finite, non-private cause.
`WG32CreateSolidBrush` returns its native public GDI brush through the original
`GETHBRUSH16`; `WG32SelectObject` converts the guest HDC and brush through
the original `HDC32`/`HOBJ32` forms before calling public `SelectObject`.
Those forms reach `walias.h:GDI32`, hence `hConvert16to32`.  The current
`MVDM-HOST-DIV-252` body deliberately returns `NULL` when `GdiQueryTable()`
does not expose NT4's shared handle table.  Thus any GDI handle first returned
by `GDI16` can subsequently become a null native handle at `SelectObject`.

The admitted recovery is deliberately smaller than a GDI provider: a
session-scoped adapter maps native public GDI identities to the original
aligned 16-bit carrier (`identifier << 2`) and resolves that carrier on the
reverse `GDI32` crossing, using the existing host-resource mapping manager.
It rejects zero, malformed carriers, missing session binding and identifiers
beyond the original 14-bit GDI carrier range.  It exposes no GDI table,
persists no native handle in guest state, and creates no GDI/USER object
manager.  Focused round-trip/tombstone tests and a rebuilt ordinary x86 Write
run are required before it can be considered recovered.

### x86 GDI projection build and ordinary Write control (2026-09-09)

The focused x86 fixture proves the admitted carrier contract without a
native-GDI dependency: two distinct nonzero native values publish as distinct
aligned 16-bit carriers, repeat publication is stable, both reverse lookups
recover the original values, and zero plus a malformed low-bit carrier are
rejected. It compiles `mapping_manager.c` and
`wow_gdi_handle_projection.c` with the x86 MSVC toolchain and exits `0`.

The x86 parent was relinked from the formal graph's existing object and
library set with its generated `ntvdm-wow32.def` exporting only the existing
session host-resource accessor and the three mapping-manager operations used
by the late-loaded provider. The complete WOW32 object set was then relinked
against that x86 import library. This is an x86 link proof, not a clean
full-graph rebuild: Ninja's network-worktree dependency scan was observed to
stall before it launched a compiler child, so no clean-build claim is made.
`dumpbin` verifies that the parent exports
`session_host_resource_mappings`, `mapping_manager_publish`,
`mapping_manager_lookup_value`, and `mapping_manager_lookup_identifier`; the
rebuilt WOW32 imports the same four symbols from `ntvdm32.exe`.

For an ordinary product control, those two newly linked x86 files temporarily
replaced only `ntvdm32.exe` and `WOW32.DLL` in the isolated `O:\n397g` stage.
The Console-owning observer ran `ntvdm32.exe system32\WRITE.EXE` with
`--observe-without-diagnostics` for 15 seconds. It loaded the replacement
`WOW32.DLL`, retained `WOWExec`, `MSWRITE_MENU`, and the visible Write
`#32770` low-memory dialog with its normal text. Both staged files were
restored in `finally`, and their pre-run SHA-256 values matched afterward.
The result proves the projection does not regress this ordinary x86 frontier;
it does not claim that the low-memory dialog is fixed or that WRITE lifecycle
has completed.

### Post-`MmwCreate` original failure ledger (2026-09-09)

The unchanged ordinary dialog means the remaining owner cannot be inferred
from `MmwCreate` alone. Source audit of original `FInitWinInfo` establishes
the finite successors which can reach its shared `InzFailed` label after a
successful parent-window return: accelerator loading, first-instance cursor
loading, `InitCommDlg`, `ObjInit`, command-line/document initialization
(`FInitArgs` or its `fMessageInzFailed` signal), and `ValidateMemoryDC`.
The earlier cursor/message-resource groups and the parent `CreateWindow`
already precede that return; `LoadCursor(IDC_WAIT)` and `FRegisterWnd` use
direct `FALSE` returns rather than the observed shared error path.

This is a source classification, not attribution. The next bounded evidence
must identify which of those post-return calls reaches `InzFailed` without
changing guest semantics; it must not convert the common low-memory text into
a claim about one named allocation, GDI, DPMI, or x64 path.

Original `ValidateMemoryDC` is additionally constrained by the shipped
`write.map`: it is `0027:0eca` and either preserves a nonzero `vhMDC` or calls
public `CreateCompatibleDC` with the current window DC; it can leave `vhMDC`
zero if the Write `vfOutOfMemory` flag is already set. `vfOutOfMemory` is data
at `0054:0232`. These are static module-relative identities only. A prior
attempt to treat a fixed runtime selector as that module was disproven, so
they are not safe runtime watch addresses until a module-qualified NE segment
mapping is established.

### Reversible `ValidateMemoryDC` result control (2026-09-09)

The mapped `FInitWinInfo` segment gives an exact external control without
requiring a runtime selector assumption. The post-`ValidateMemoryDC` check is
the audited `75 03` (`JNZ` to successful initialization) at the caller site
`FInitWinInfo` `0002:0434`, file offset `0x2174`. In an isolated stage, that byte was changed
to `EB` while the x86 parent/provider pair from the admitted GDI projection
was staged. The guarded bytes were verified before modification.

The 15-second no-diagnostic Console-owner run again loaded WOW32 and retained
WOWExec, `MSWRITE_MENU`, and the same visible Write low-memory `#32770`
dialog. Thus this branch does not uniquely select the observed panel. It does
not establish whether `vhMDC` was normally null: continuing after a real
failure can retain a later `InzFailed` cause. `ntvdm32.exe`, `WOW32.DLL`, and
the immutable staged `WRITE.EXE` were restored in `finally`; each restored
SHA-256 matched its pre-run value. The remaining finite source paths are the
other post-parent `FInitWinInfo` result edges.

### Further `FInitWinInfo` result controls (2026-09-09)

Three additional one-byte controls partitioned three independently audited
post-parent result edges in the same isolated x86 stage.  The verified
branches were `ObjInit` at `FInitWinInfo 0002:0414` (file offset `0x2154`,
`74 20`), `FInitArgs` at `0002:0421` (`0x2161`, `74 13`), and the
`fMessageInzFailed` signal at `0002:0428` (`0x2168`, `75 0c`).  Each test
changed only its conditional branch opcode to the corresponding unconditional
short jump, used the already-audited x86 parent/WOW32 GDI pair, and ran the
same 15-second no-diagnostic Console-owning `WRITE.EXE` observation.

All three observations loaded WOW32 and retained WOWExec, `MSWRITE_MENU`,
and the same visible Write low-memory `#32770` dialog.  They therefore exclude
only a *unique* attribution to the tested branch.  They do not establish that
any bypassed operation normally succeeds: a later route to the common
`InzFailed` label may still select the same panel.  After each run, the
temporary parent, provider and immutable guest were restored; the final
SHA-256 comparisons against all pre-run values passed.  The remaining bounded
source candidates include the preceding accelerator/cursor and `InitCommDlg`
edges, and any common-label predecessor not yet independently controlled.  No
product source change, x64 build, or x64 repair follows from these controls.

### `InitCommDlg(0)` contract and guarded-continuation control (2026-09-09)

The original Write map resolves `InitCommDlg` to `0051:0000` in
`COMMDLG_TEXT`; `FInitWinInfo` is segment `0002`.  Its source is unusually
strict: in the `case 0` bootstrap path it initializes the `OPENFILENAME`
carrier, calls `LoadString` for the static strings without testing their
return values, and returns `FALSE`.  The exact caller sequence in segment 2
is `call far; or ax,ax; jz success` at `0002:031c..0325`; its verified guest
bytes place the `JZ` at file offset `0x2063` (`74 03`).  Thus a nonzero result
at that branch would contradict the ordinary `case 0` source contract, but
static mapping alone cannot prove that the ordinary runtime reaches this
call or what its live `AX` contains.

One isolated x86 control changed only that `JZ` opcode to `EB`, forcing the
post-`InitCommDlg` success continuation.  The 15-second no-diagnostic
Console-owning run still loaded the staged WOW32 provider and the WOWExec
shell, but changed the terminal from the low-memory panel to the original
`Application Error` dialog: `WRITE caused a General Protection Fault in
module <unknown>00C7:1828F`.  The fixed runtime selector is intentionally not
attributed to the static Write `0051` segment.  This is not a product repair
or proof that the ordinary call returns nonzero; forced continuation can make
uninitialized required state fail later.  It does establish that the guarded
continuation protects real downstream state and makes `InitCommDlg(0)`'s
return contract a finite, source-backed next observation target.

The temporary x86 parent, WOW32 provider, and guest control byte were restored
in `finally`; all three restored SHA-256 values matched their pre-run values.
No product source, x64 build, or x64 repair was introduced.

### Module-qualified selector-observation admission (2026-09-09)

The retained original `src/mvdm-host/vdmdbg` source documents and implements
the missing mapping operation.  `VDMProcessException` consumes the VDM debug
event initialization needed to capture the active Kernel selector and internal
kernel offsets; only then can `VDMGetModuleSelector(process, thread, 2,
"WRITE", &selector)` resolve the live selector for the static Write segment
2.  The inverse `VDMGetSelectorModule` can reject a false mapping before a
selector/IP is used as a Write code address.

This mechanism requires a debugger event container and therefore changes the
observation environment.  It is admitted only as a separate, x86, test-only,
read-only module-selector probe.  It may establish whether the product emits
enough original VDMDBG event state to map WRITE segment 2, but it may neither
change guest/product inputs nor replace the ordinary no-diagnostic Console
run as product evidence.  If its required initialization is not emitted, that
is an unavailable diagnostic mechanism—not evidence of a WOW32, CPU, BOP, or
x64 defect.

The x86 probe compiled with `/W4 /WX /MT` and ran against the isolated stage.
Over its 15-second bounded debug-child lifetime it received 27 debug events
but **zero** `STATUS_VDM_EVENT` records.  Consequently the system VDMDBG DLL
never received the required initialization event and returned no selector for
`WRITE`, segment 2 (`selector=0000`; probe exit `4`).  The probe itself was
corrected to initialize all report fields before this final run.  This makes
the VDMDBG route unavailable for the current custom CCPU container; it does
not identify a product defect, authorize a repair, or weaken the ordinary
no-diagnostic x86 baseline.  The probe remains test-only and its debug child
is terminated by the probe at cleanup.

### Early `HszCreateIdpmt` allocation controls (2026-09-09)

The earliest source-level shared-error cohort in `FInitWinInfo` consists of
the seven bootstrap message allocations for `hszCantRunM`, `hszCantRunF`,
`hszWinFailure`, `hszNoMemory`, `hszDirtyDoc`, `hszCantPrint`, and `hszPRFAIL`.
The map puts their segment-2 zero-result branches at offsets `0070`, `007d`,
`008a`, `0097`, `00a4`, `00b1`, and `00be` respectively.  In the immutable
guest these are `JZ` branches to the shared `InzFailed` cleanup; the matching
file offsets are `0x1db0`, `0x1dbd`, `0x1dca`, `0x1dd7`, `0x1de4`, `0x1df1`,
and `0x1dfe`.

Each branch was independently controlled in an isolated x86 stage by
replacing only its two-byte conditional jump with `NOP; NOP`, thereby forcing
the normal fall-through without redirecting to a different existing target.
Every eight-second no-diagnostic Console-owner observation loaded WOW32,
created WOWExec and `MSWRITE_MENU`, and displayed the same Write `#32770`
`Not enough memory for Write to complete this operation` dialog.  The two
runs whose first summary did not retain the multiline text were repeated
solely to capture their full window identity; both were the same Write dialog.

Each individual result excludes only that allocation's failure guard as the
unique selector of the common panel.  It does not prove that a bypassed
allocation normally succeeds, because an arbitrary later common-label path
may remain.  Every run restored the temporary parent, provider and guest in
`finally`; all SHA-256 comparisons against the pre-run values passed.  No
product source, x64 build, or x64 repair is selected.  The finite remaining
pre-parent candidates are the cursor/accelerator and class-registration
paths whose source contracts have not yet received equivalent controls.

### Stock-cursor and accelerator controls (2026-09-09)

The next three direct `FInitWinInfo` error edges precede class registration and
parent-window creation: the `IDC_IBEAM` and `IDC_ARROW` `LoadCursor` results,
and `LoadAccelerators`.  Their segment-2 guards were independently verified
at file offsets `0x1ea9` (`74 2d`), `0x1ebc` (`74 1a`), and `0x1ed6`
(`75 03`).  The two cursor failure jumps were temporarily replaced by two
NOPs to force fall-through; the accelerator success jump was changed only
from `JNZ` to `JMP`.

All three x86 no-diagnostic controls retained WOW32, WOWExec,
`MSWRITE_MENU`, and the same original Write low-memory dialog.  Parent,
provider and guest hashes were restored and verified after every isolated
run.  These are again only non-unique-attribution results: none establishes
the normal API result, but each removes that one guarded edge as a sole
explanation of the common panel.  The next source-exact edge is Write's
post-registration, module-specific memo cursor load; no product source or
x64 action is selected.

### Memo-cursor control and `FInitWinInfo` audit boundary (2026-09-09)

After `FRegisterWnd` succeeds, source line 298 loads Write's module-specific
memo cursor and branches directly to `InzFailed` on a null result. The
source/map/disassembly correspondence places its `JNZ` success edge at
segment `0002:027e`, guest file offset `0x1fbe` (`75 17`). Replacing only
that opcode with `EB` in one isolated x86 run retains the same WOW32,
WOWExec, `MSWRITE_MENU`, and Write low-memory dialog; stage restoration and
all three SHA-256 checks pass.

Together with the visible four Write class registrations (source evidence
that the direct `FRegisterWnd` false-return path was not taken), the separate
parent/MmwCreate controls, and the later `fMessageInzFailed`, `InitCommDlg`,
`ObjInit`, `FInitArgs`, and `ValidateMemoryDC` controls, this completes the
finite source-level `FInitWinInfo` failure-edge audit. No individual forced
continuation except `InitCommDlg` changes the ordinary terminal, and that one
changes it only to a later GPF. It would be unsound to turn this into a
product repair: no control measured the ordinary live return value, and the
available VDMDBG module selector mechanism is unavailable in this CCPU
container. The audited result is an explicit observation boundary, not a
claim that the remaining error is fixed, guest behavior is correct, or a
CPU/BOP/x64 repair is warranted.

### WRITE media identity correction (2026-09-09)

The apparent segment-51 disassembly/map tension does **not** establish a
guest-media mismatch.  The active `WRITE.EXE` and retained `write16.exe` are
byte-identical (both SHA-256
`08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B`), and
`write.map` and `write16.map` are also byte-identical (both SHA-256
`0B646E63902989639F61345DB535ECFE341A27025A7860CEF471472C79BDCB0E`).  The
project's T274 WOW16 output manifest registers the same executable hash as
the canonical OpenNT-4.5 `write16.exe` input.  No guest file is replaced,
rebuilt, or repaired.  The unresolved relationship between the static map's
segment label and the raw segment-51 decode is therefore a diagnostic mapping
limitation, not evidence permitting a media change.

### External CPU40/SAS/LDT resolution of the forced-control GPF (2026-09-09)

The prior `00C7:1828` error-panel text was not a module identity. A new
test-only x86 extension to `console-startup-observer` uses the exact matching
parent link map to read the launched process only: `CCPU_SR`, `CCPU_IP`,
`CCPU_SAR`, `CCPU_CR`, and `Start_of_M_area`. It never adds a product marker,
changes an environment variable, calls the child, or writes guest memory. The
observer resolves an explicit LDT selector by the original CPU40 two-level
page rule when paging is enabled, then reads code from dynamic SAS backing.

The 15-second no-diagnostic `WRITE.EXE` control took 138 samples, with 126
stable double reads. `Start_of_M_area` was live `MEM_PRIVATE`, not the image,
and the active CCPU code bytes separately matched retained retail `KRNL386`.
For the panel selector it recorded `00C7:1828`, LDT base `00126800`,
descriptor base `00110800`, and bytes
`1f 07 55 8b ec 50 1e b8 40 00 8e d8 8b 46 06 a9`. CR0 was `00000011` and
CR3 zero, so paging was off and the direct SAS physical read is the original
CPU40 access rule for this run.

Those bytes uniquely occur at `0x4628` in staged `system32/DOSX.EXE`. The
staged file and retained `src/mvdm-host/dpmi/486/dosx.exe` have identical
SHA-256 `C5AF29A29ABF167B243DAABF877459E8278B8C9A339BF8E1E2576EAD5F6CEEFF`.
The retained `dosx.map` puts `PMIntrReflector` at `0A8D:1783`; `1828` is
inside that original protected-mode interrupt reflector, not WRITE's
`FInitMemory`, local heap, or source segment.

This observation follows only the isolated control that forced the
`InitCommDlg(0)` failure guard to continue. It proves that forced path can
reach a later DOSX reflector GPF; it does **not** show an ordinary DPMI32
provider, CPU, DOSX, WOW32, or allocator defect and selects no repair. The
temporary map-matched parent substitution was restored in `finally`; the
external stage returned to SHA-256
`E821A59A60442B69575CD1A4EFEF5A3C94C6A07235FF98FF394B3E95313662BB`. The
ordinary no-diagnostic WRITE low-memory path remains separate.

### Default-off CCPU fixed-instruction observer correction (2026-09-09)

The existing bounded `mvdm_softpc_record_cpu_fixed_instruction()` helper is
called by CCPU for a finite list of prior diagnostic selector/IP ranges. Its
own body claimed to be default-off, but did not first test
`MVDM_BOP_RETURN_REPORT_PATH`: in an ordinary run without the variable it
still incremented `reports`, resolved a guest effective address, acquired a
six-byte guest-memory lease, and released it. That makes an ordinary run
observationally non-neutral and invalidates the label “default-off.”

The x86-only correction adds the missing early environment-variable guard
before the counter and guest-memory operations. It changes no decoder state,
register, BOP route, guest byte, public ABI, or mapping-manager contract; when
the selected report is enabled its existing formatted-report behavior is
unchanged. The generated formal x86 graph compiles the amended
`obj/adapter-softpc/mvdm_softpc_machine_observation.obj` successfully. A
linked ordinary-product comparison remains required before attributing any
WRITE behavior to this isolation correction.

The existing complete x86 parent root was then updated only by the recorded
formal compile command, `softpc-bindings.lib` archive command, and parent link
command; all three returned zero. Its `original-softpc-process.exe` was
temporarily substituted for `O:\n397g\ntvdm32.exe`, then run through the
existing Console-owning observer with `--observe-without-diagnostics` and
`system32\WRITE.EXE`. The observer returned zero and the original staged
binary SHA-256 was restored exactly
(`E821A59A60442B69575CD1A4EFEF5A3C94C6A07235FF98FF394B3E95313662BB`).
Because diagnostics were intentionally disabled, the observer emits no report
file; this is a non-regression/container result only, not evidence that WRITE
created a document window, completed its initialization, or exited normally.
