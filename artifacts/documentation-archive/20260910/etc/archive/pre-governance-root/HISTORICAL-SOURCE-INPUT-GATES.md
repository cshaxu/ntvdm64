# Historical Source Input Gates

Status: source-governance record, 2026-08-06.

## Purpose

`src/opennt/` and `refs/ntvdmx64-derived/` contain the fixed historical source inputs
that have been adopted for this research. They replace neither the clean
non-invasive CLI nor the BYOB guest-file policy. They make the historical
input set reproducible without treating an old `toolchain-probe/` object as a
build input.

Each root preserves the upstream relative directory structure. Its
`SOURCE-MANIFEST.json` records the upstream repository, fixed revision,
per-file SHA-256 digest, research restriction, and whether a file is an exact
upstream copy, a local overlay, or a local compatibility input.

`toolchain-probe/` remains the record of one-off compiler, linker, and trace
diagnostics. It is not an input root for future CMake targets.

### NTIO Guest-Artifact Closure

On 2026-08-07 the complete unmodified NTIO source closure used by the
compatibility-build experiment was imported under its original paths below
`src/opennt/base/mvdm/`: `dos/v86/doskrnl/bios`, `dos/v86/inc`, and
`dos/v86/messages`. It contains 123 files, each recorded as an `upstream-copy`
in `SOURCE-MANIFEST.json`; the imported bytes match the fixed OpenNT revision.

The same rule now covers NTDOS: its complete 58-file
`dos/v86/doskrnl/dos` source directory was imported as exact upstream copies.
Together with the shared `inc` and `messages` trees, this is the managed
source closure for the current NTDOS compatibility-build experiment.

`COMMAND.COM` now has the same source treatment: its 51-file
`dos/v86/cmd/command` directory is managed as exact upstream input. Its
`COMMAND.CL*` and `.CTL` files are generated in the disposable stage by the
separately recorded BUILDMSG compatibility tool, so they are explicitly not
misclassified as imported OpenNT source. The upstream directory also contains
historical `COMMAND.COM` and intermediate `.exc`/`.map` build outputs; those
three binary artifacts are deliberately excluded from this source island.

The same exclusion applies to every historical directory, including checked-in
`.SYS`, `.BIN`, `.COM`, `.EXE`, `.OBJ`, `.LIB`, `.MAP`, and `.SYM` outputs.
Even where an upstream source snapshot includes them, they are build evidence
only and must remain outside `src/opennt`, its manifest, default builds, and
the research runner's BYOB profile.

`toolchain-probe/stage-opennt-ntio.ps1` and `stage-opennt-ntdos.ps1` now
accept only a manifested OpenNT root and validate every member of their source
closures before creating a disposable stage. Earlier JWasm compatibility rewrites
remain stage-local and are never copied back into this source island. The
stages themselves, their object files, link outputs, and generated guest images
remain research outputs rather than source inputs or release artifacts.

`stage-opennt-command.ps1` follows the same managed-root/hash rule before it
invokes BUILDMSG. Stage-local JWasm compatibility transforms are retired for
COMMAND and must not be used by the canonical Gate 0 path; see
`OPENNT-G0-TOOLCHAIN-REBASE.md`.

The `sdktools/masm` text-source closure is now also managed. Its 68 immediate
source/configuration files are exact `upstream-copy` entries under their
original paths, including the 31 C units named by `sdktools/masm/sources`.
`build-opennt-masm386-managed.ps1` verifies every imported file against the
root manifest before invoking the explicitly supplied x86 host compiler. Its
output is a disposable host tool only; it is not copied into `src/opennt` or
treated as a guest artifact.

`stage-opennt-himem.ps1` likewise verifies its eight text-source/build-descriptor
inputs before creating a disposable stage. It now also hash-verifies the full
shared `base/mvdm/inc` tree that it copies alongside the HIMEM source closure.
Historical `.map` and assembler `.err` outputs are excluded from the managed
closure.

## Compiler And CRT Islands

The root project is the modern island and requires MinGW-w64 GCC. It builds
the modern CLI, BYOB assembly, and any future modern-host shim. It cannot add
either historical tree as a CMake subdirectory.

Each historical root is a standalone CMake source project. `src/opennt/` is
configured in a 32-bit compiler environment with the historical compiler
profile that best preserves the source (currently `clang-cl`); `refs/ntvdmx64-derived/`
is separately configured only when its reference inputs are being studied.
This separation is deliberate: one CMake cache may select only one C compiler
and CRT, so a parent/child target layout cannot enforce the required boundary.

An eventual research executable may compose resulting artifacts only through
a narrow, versioned `extern "C"` adapter ABI. It uses fixed-width data,
explicit calling conventions, and caller-owned buffers. It may not transfer
C++ objects, exceptions, or CRT allocation ownership between islands. No such
adapter is linked today.

The currently verified configurations are intentionally separate:

```text
modern:   cmake -S . -B <modern-build> -G "MinGW Makefiles" \
            -DCMAKE_C_COMPILER=<mingw-w64-gcc>
historical OpenNT: run vcvarsall.bat x86, then
                   cmake -S src/opennt -B <opennt-build> -G "NMake Makefiles" \
                     -DCMAKE_C_COMPILER=<clang-cl> \
                     -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
                     -DCMAKE_C_FLAGS=/clang:--target=i686-pc-windows-msvc
```

The historical command may build only explicitly named object-gate targets.
It has no `ntdos64-run` target. Conversely, the modern root rejects either
historical-input option instead of silently compiling it with GCC.

## Source Boundaries

| Root | CMake target | Default state | Scope |
| --- | --- | --- | --- |
| `src/opennt/` | `ntdos64-opennt-ccpu-objects`, `ntdos64-opennt-cvidc-objects`, `ntdos64-opennt-host-stack-objects`, `ntdos64-opennt-bios-provider-objects`, `ntdos64-opennt-system-provider-objects`, `ntdos64-opennt-support-provider-objects`, `ntdos64-opennt-keymouse-provider-objects`, `ntdos64-opennt-disks-provider-objects`, `ntdos64-opennt-disk-host-backend-objects`, `ntdos64-opennt-oemuni-provider-objects`, `ntdos64-opennt-contained-media-fdisk-objects`, `ntdos64-opennt-contained-media-fdisk-test`, `ntdos64-opennt-ccpu-effective-addr-fixture`, aggregate `ntdos64-opennt-historical-objects` | Configured only with `cmake -S src/opennt`; every target is `EXCLUDE_FROM_ALL` | Fixed CCPU is isolated from the BIOS/BOP/DEM host stack and independently gated original provider layers. CVIDC is a separate original generated-C dependency gate; it is not linked into a runner. The effective-address fixture is a controlled monitor-ABI contract test, not a CCPU runtime. The aggregate currently contains CCPU plus the host stack only and performs no link. The contained-media target is a narrow original-boundary facade and test, not a historical-chain executable. |
| `refs/ntvdmx64-derived/` | `ntdos64-ntvdmx64-haxm-reference` | Configured only with `cmake -S refs/ntvdmx64-derived`; target is `EXCLUDE_FROM_ALL` | Fixed HAXM monitor reference inputs only. It is a visible object gate, not a compilable execution backend. |

The targets do not export or consume one another's include paths. In
particular, the NTVDMx64 target receives no OpenNT include root, and the
OpenNT target receives no NTVDMx64 include root. This prevents accidental
source-universe merging.

Separating CCPU from the host stack prevents an unresolved historical CCPU
generator/profile issue from hiding independently reproducible BIOS/BOP/DEM
closure evidence. It is not a runtime split: a future executable still must
explicitly link the complete historical chain after both object closures are
valid.

The OpenNT gate must be configured with `clang-cl` in an x86 toolchain
environment. Current MSVC rejects historical public-SDK macro constructs
before the selected historical source can be assessed. This is a compiler
compatibility requirement, not an authorization to alter the imported source.

### CCPU Compile Closure

The selected CCPU profile is the 130 `ccpu386/*.c` units named by the archived
`toolchain-probe/ccpu-backend-probe-v3/ccpu-sources.rsp` evidence. Every named
unit is now an exact fixed OpenNT copy under the same relative source path in
this root. The gate selects that closed set by excluding only `localfm.c` and
`vglob.c`, the two `ccpu386` C files absent from the archived response file.

CCPU predates C99. The default-disabled clang-cl gate therefore uses a GNU89
parser profile and downgrades only the two legacy declaration diagnostics that
the historical compiler accepted. This changes neither an imported source file
nor any guest/host behavior. It is solely enough to expose the next genuine
source or link dependency; unresolved historical host calls remain unresolved
until separately classified in the host-shim ledger.

The compatibility preamble intentionally does not include modern UCRT headers:
that would impose a modern declaration order on every CCPU unit and conflict
with original units that deliberately declare their host aliases locally.
`overlay/base/mvdm/softpc.new/base/ccpu386/c_xcptn.c` is the sole current CCPU
source overlay: its `host_getenv` declaration changes only from `char *` to
`const char *` input, matching the modern UCRT declaration. Its exception
control flow and all guest-visible behavior are otherwise unchanged.

`overlay/base/mvdm/softpc.new/base/ccpu386/ccpusas4.c` changes only the
`Length_of_M_area` definition from `PHY_ADDR` to the `IHPE` type already used
by the original `sas.h` declaration. Both are 32-bit in this mandatory x86
profile; the overlay resolves a modern compiler type-conflict diagnostic
without changing the represented memory limit. The gate also retains the
historical compiler's warning-level treatment of non-void return paths in
debug-only memory-read helpers.

`overlay/base/mvdm/softpc.new/base/inc/host.h` changes only the `memset4`
declaration to `unsigned int, unsigned int *, unsigned int`, exactly matching
the historical NT provider declaration. The overlay removes a legacy typedef
conflict without supplying or changing `memset4`.

The existing `overlay/base/mvdm/dos/dem/demfile.c` retains its historical VDD
handle algorithms but spells its four exported VDD handle definitions with
the contemporaneous `nt_vdd.h` ANSI prototypes. `VDDAssociateNtHandle` casts
its public `PVOID` to the original local `PDOSSFT` once. This resolves K&R
versus public-header conflicts without changing the VDD service contract.

The exact upstream `host/inc/mips/prod/pigreg_c.h` is retained under that same
relative source path because the archived CCPU source set includes it from
`sasCdef.c` and `c_getset.c`. It provides the historical register-layout
definition; it is not a selected MIPS runtime backend.

The source snapshot lacks the generated x86 SAS header required by
`sasCdef.c`. The owner authorized a narrowly constrained, manifest-recorded
overlay made by copying fixed OpenNT `base/cvidc/sas4gen.h` to the original x86
relative path and applying the fixed NTVdmx64 `sas4gen.patch`. This preserves
the CCPU backward-move entries while adding the two entries used by
`sasCdef.c` and the trailing instance-data entry. It is included only for the
default-off CCPU object target. A matching `zfrsrvd.c` overlay changes its
duplicate `Length_of_M_area` declaration from `PHY_ADDR` to the `IHPE` type
already declared by OpenNT `sas.h` and defined by the existing `ccpusas4.c`
overlay. Both are compile-compatibility declarations, not CCPU behavior.

The resulting CCPU object target completes under clang-cl/i686. This proves
neither link closure nor runtime validity; in particular, it does not prove
the trailing `IsPageInstanceData` behavior. `mips/prod` remains retained only
for the exact `PigReg_c.h` input.

The precise field-order evidence and recovery admission rule are recorded in
`docs/CCPU-SAS-GENERATED-ABI-AUDIT.md`.

## Toolchain And Future Adapter ABI

The modern research CLI, BYOB assembly, and modern-host shim remain MinGW-w64
GCC code. Historical OpenNT modules may instead use the x86 compiler profile
that best preserves their original C behavior (currently clang-cl); the two
worlds must not be forced into one compiler or CRT.

Any future executable historical chain therefore crosses through the narrow,
versioned `extern "C"` adapter ABI defined by the compiler-and-CRT-island
rule above. This is an adapter admission rule, not a currently linked runtime
boundary.

## Runtime Gate

Neither historical target is linked by `ntdos64-run.exe`, any default test, or
the source-only trace target. `ntdos64-run` remains the documented PE handoff
and explicit-BYOB-engine dispatcher.

The binding historical chain remains:

```text
CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
```

The current NTVDMx64 reference is explicitly HAXM-specific and therefore is
not a valid adapter for the CCPU-only runtime profile. Its inclusion here is
provenance preservation, not selection for execution. A future research
`ntvdm.exe` target may be created only after it explicitly names all of these
inputs in its own CMake target:

```text
historical CCPU + historical BIOS + original nt_bop + original DEM
  + one approved non-HAXM adapter + modern-host shim
```

That target must not inherit a source path implicitly, and it must not use an
NTVDMx64 HAXM monitor as the adapter. It also must satisfy the existing
host-shim ledger and runtime admission evidence before becoming executable.

## Host-Stack Object Closure

On 2026-08-06, the isolated x86 clang-cl target
`ntdos64-opennt-host-stack-objects` completed with 23 source objects:
historical `bios.c`, `sim32`, original `nt_bop`, all sixteen DEM units,
historical I/O dispatch, ICA, quick-event handling, and the register ABI
bridge. This is source/object closure evidence only. The target has no link
action, does not execute guest code, and does not establish a runnable
historical chain.

The independent CCPU object target now has object-level closure under the
owner-authorized SAS input recipe documented above. It remains before link and
runtime: this result does not invalidate any outstanding host/provider work or
authorize a synthetic SAS implementation.

The original BIOS provider layer also completes as a separate 13-object gate:
its build-id, CMOS-BIOS, control-BOP, EMS, equipment, illegal-BOP, memory,
reset, ROM BASIC, RTC, slave-BIOS, tape, and virtual-device units remain
historical providers rather than replacement handlers. The original system
provider layer completes as a separate 9-object gate for AT DMA, CMOS, ROM,
timer/strobe, instruction-detect, dummy/unexpected NT, and illegal-protected
mode support. These results are compile closure only. The system gate uses a
manifested declaration-only `TIME_FIELDS` compatibility preamble derived from
the fixed public OpenNT header, an empty missing-private-include compatibility
header, and a one-token `define` to `defined` overlay in `illegalp.c`; none of
those inputs implements clock, device, or guest behavior.

The complete original five-unit `keymouse` library (keyboard, keyboard I/O,
mouse, PPI, and mouse I/O) also completes as its own object gate with no local
source overlay. Its unresolved host mouse, display, and keyboard-thread calls
remain visible for later provider/shim classification; the gate neither links
them nor supplies replacement input behavior.

The remaining five members of the original `base/support` library now complete
as `ntdos64-opennt-support-provider-objects`: `terminat.c`, `time_day.c`,
`main.c`, `get_env.c`, and `xt.c`. The already-recorded historical `ios.c`
overlay remains in the host-stack gate, so it is deliberately not duplicated.
The proof is
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/support-provider-object-closure.log`.
This admits the original lifecycle, clock, environment, and XT-configuration
sources only as object inputs. It does not select historical `host_main` as the
research executable entry, link a CCPU session, or add a substitute startup
policy.

The original ten-unit `disks` controller gate completes independently. Its
two historical host backends remain a separate, intentionally unclosed
diagnostic gate: OpenNT's `host/src/sources` places them in a 47-unit library
and requires private VDD, MVDM, Windows, and NT include roots missing from the
fixed snapshot. Its target-local `ntdddisk.h` only redirects the historical
disk declarations to fixed `winioctl.h` after the source's own `nt.h`; it does
not implement or admit direct host-drive access.

The complete original `base/comms` library is imported from the fixed upstream
revision under its original path as a separate default-disabled object gate.
It contains `com.c`, `printer.c`, `printer_.c`, `prt_scrn.c`, and
`rs232_io.c`, preserving the historical owners of the BIOS serial and printer
entry points. The gate selects `PRINTER` but explicitly removes `GISP_CPU`:
the latter only causes `com.c` to include the upstream hardware-assisted CPU
header, whose lone `h_exception_t` declaration is incomplete in this source
tree. That hardware-assisted path is not part of the non-V86 CCPU research
profile, and no typedef or handler substitute is added. The gate is not a
serial/printer host backend and never enters a default build or executable.

On 2026-08-06, `ntdos64-opennt-comms-provider-objects` completed all five
original C units. The case-sensitive provider audit is retained as
`historical-with-comms-provider-casesafe-*`: 221 objects, 3,398 definitions,
1,589 undefined references, and 398 external undefined names. It resolves
exactly `_com_init`, `_com_post`, `_rs232_io`, `_printer_init`,
`_printer_io`, and `_printer_post`. The newly visible dependencies are only
the original `host_com_*`, `host_lpt_*`, and WOW port callbacks; no DOS SVC,
BOP dispatcher, or replacement serial/printer handler was added.

A same-revision inventory of OpenNT's historical `host/src/sources` confirms
that all 45 listed C members already exist at their original paths below
`src/opennt/base/mvdm/softpc.new/host/src` and are individually manifested.
The remaining work is therefore not to acquire missing host source. It is to
select a direct-CCPU, non-V86 provider closure from those inputs and classify
its unavailable dependencies. In particular, `nt_sec.c` (`NtVdmControl`
initialization), `config.c` (PIF/console product setup), and full-screen/V86
modules remain historical reference inputs, not automatic candidates for the
research runner.

The separately default-disabled `ntdos64-opennt-contained-media-fdisk-objects`
target contains the local SH-12 facade at the original `nt_fdisk_*` boundary.
Its companion `ntdos64-opennt-contained-media-fdisk-test` is the only target
that executes it. On 2026-08-06 that test passed against a temporary 4 KiB
regular file, including exact 512-byte read/write, range rejection, and
pre-open rejection of a `\\.\\PhysicalDrive0` request. Neither target links
DEM or CCPU, and neither can become a normal runtime path without the separate
historical-chain link and admission evidence required above.

The original two-unit `oemuni` library now has its own default-disabled
`ntdos64-opennt-oemuni-provider-objects` object gate. On 2026-08-06 it stops
at `file.c` before object creation: the historical source requires private
`TEB`/`PEB` fields, `UNICODE_STRING`/`OEM_STRING`, and RTL conversion/heap
interfaces absent from a modern user-mode ABI. This is a source-proven reason
to retain the original files as reference inputs while implementing SH-11 only
at their published `*Oem` call boundary; it is not permission to alter DEM or
recreate private NT internals. The failure is reproducible by targeting the
gate with `toolchain-probe/build-opennt-support-provider.cmd`.

The separate `ntdos64-opennt-contained-oemuni-objects` and
`ntdos64-opennt-contained-oemuni-test` targets are default-disabled. Their
smoke test passes only a wrapper-configured temporary root and proves
contained file operations, session environment/reset, and lexical/reparse
rejection. It does not link DEM or CCPU. Its case-sensitive symbol evidence
is retained as `historical-with-contained-media-oemuni-casesafe-*`.

The additionally default-disabled
`ntdos64-opennt-contained-oemuni-demdir-fixture` compiles the original
`base/mvdm/dos/dem/demdir.c` beside the same facade. Its isolated test harness
supplies only passive register reads, guest-memory lookup, carry observation,
and `demClientError` recording; it is neither a CCPU adapter nor a BOP/SVC
dispatcher. On 2026-08-06 the fixture passed create-directory,
set-current-directory, and delete-directory paths beneath a configured `C:`
root, and verified that `C:\..\escape` takes DEM's existing error path. It
does not establish file DTA/FCB/SFT behavior, link CCPU, or admit an engine.

The archived `artifacts/build/legacy/referenced/build-source-gate-default`
graph was also configured and built
with both historical options unset on 2026-08-06. Its four tests passed:
`whpx-real-mode-hlt`, `whpx-real-mode-io`, `whpx-real-mode-int`, and
`opennt-reloc`. No historical object target appeared in that default build.

That historical cache was later found to name a missing
`nmake` program and therefore is retained only as historical evidence. A fresh
`artifacts/build/current/default-mingw-verify` directory was configured on
2026-08-07 with
MinGW-w64 GCC 16.1.0, `NTDOS64_ENABLE_TRACE_INSTRUMENTATION=OFF`,
`NTDOS64_ENABLE_OPENNT_HISTORICAL_OBJECTS=OFF`, and
`NTDOS64_ENABLE_NTVDMX64_REFERENCE_INPUTS=OFF`. Its full build succeeded and
the same four tests passed. The target list contained only the modern CLI,
WHPX probes, and relocation/strip tools; it contained no historical-island
object or facade target.

After the compiler/CRT-island separation was made explicit on 2026-08-06,
the root project was configured with MinGW-w64 GCC 16.1.0. Its full default
build completed and the same four tests passed. Independently, `src/opennt`
was configured with clang-cl 22.1.8 in an x86 MSVC environment and its
`ntdos64-opennt-host-stack-objects` target completed with 23 source objects. The
standalone OpenNT build directory contains no modern runner target or linked
historical executable. The CCPU gate has since completed independently with
the authorized SAS overlay, but still has no link action and remains outside
the modern runner graph.

## Research And Distribution Limits

The manifests retain the source-origin and license/research notices. No
historical source target is a release dependency, and no resulting historical
binary, Microsoft guest component, or BYOB file may enter a release package.
The upstream checkouts remain read-only evidence and must stay unmodified.
