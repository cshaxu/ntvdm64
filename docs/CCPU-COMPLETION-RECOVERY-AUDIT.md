# CCPU Completion Recovery Audit

Status: source-recovery evidence, 2026-08-07. This record covers the
research-only, default-disabled OpenNT x86 compiler island. It does not
authorize a runner, guest execution, a BIOS/BOP/DEM replacement, a V86 route,
HAXM, or any default build dependency.

## Question

Can the locally available historical inputs recover the CCPU pieces that are
absent from the fixed OpenNT source snapshot, before an independently designed
software monitor or CCPU replacement is considered?

The three direct CCPU link names remain:

```text
c_sas_touch
c_VirtualiseInstruction
c_effective_addr
```

## Search Result

The symbols were searched as source definitions in the admitted OpenNT input,
the local NT4.5 and XP MVDM trees, the local NTVDMx64 input including MinNT
patches, and indexed public source results. No original definition was found.
The local historical OpenNT object database was already searched separately
for omitted generated inputs; it contains no recoverable `host/genPg` source
or matching generator.

NTVDMx64 adds `c_sas_touch` and `c_VirtualiseInstruction` in
`ntvdmpatch/patches/common/fmstubs.c`, and its old-source and MinNT makefile
patches add that file to the NTVDM build. Those are not recovery candidates:
each implementation intentionally executes `int 3`, then returns `NULL` or
zero. They are diagnostic assertions that the code path is expected not to
run, not a historical implementation and not a usable SAS or instruction
contract. NTVDMx64 supplies no `c_effective_addr` definition.

NTVDMx64 additionally has an experimental `cvidc-new` project. Its own
documentation says it is a handcrafted rewrite of generated CVIDC code,
because the original EDL input is unavailable and the original generated code
has known defects. It is therefore an independent implementation, not an
OpenNT input, and is excluded from this recovery path.

### Caller-Reachability Recheck (2026-08-09)

The fixed NT4 tree, local NT4.5/NT5 source trees, and the admitted CCPU source
group were rechecked for direct callers of `c_sas_touch` and
`c_VirtualiseInstruction`. Both names occur only as the two corresponding
`SasVector` slots: generated `sas4gen.h` declarations/macros and
`ccpu386/sascdef.c::cSasPtrs` initializer entries. No CCPU instruction source,
BIOS entry, BOP dispatcher, DEM source, or NTDOS source caller was found.

This establishes a narrower status than “missing CCPU implementation”:

- the two slots are source-unrecovered ABI surface, but not a demonstrated
  dynamic execution dependency of the current NT4 software-monitor path;
- their existing fail-closed guards must remain trace containment, not grow
  into speculative SAS mapping or instruction-virtualization semantics; and
- implementation is not admitted merely to close an archive or executable
  link. A future restoration needs a real original caller plus an explicit
  input/output and failure contract.

`c_effective_addr` remains different: it has real CCPU callers and retains its
separately governed, fixture-only recovery path.

## Recovered Original Dependency: CVIDC

The historical CCPU build selection links both `ccpu386.lib` and `cvidc.lib`.
The original OpenNT `base/mvdm/softpc.new/base/cvidc` tree was previously not
in the fixed `src/opennt` input. On 2026-08-07, all 145 files were copied from
the fixed OpenNT revision under the same relative directory, labeled as
`upstream-copy` in `SOURCE-MANIFEST.json`, and hash-compared against the
read-only local source tree:

```text
source files:      145
destination files: 145
SHA-256 mismatch:    0
```

`ntdos64-opennt-cvidc-objects` is a separate `EXCLUDE_FROM_ALL` gate. It uses
exactly the 39 C units named by the original `cvidc/sources` file; `ev_stubs`,
`sascdef`, and `vglob` are deliberately excluded because that historical file
does not select them. The object gate succeeds under the existing x86
clang-cl profile.

COFF-symbol inspection finds that original CVIDC defines none of
`c_sas_touch`, `c_VirtualiseInstruction`, or `c_effective_addr`.

## Link Diagnostic

The separate initialization-only target
`ntdos64-opennt-ccpu-cvidc-register-bridge-fixture` links:

```text
original CCPU archive + original CVIDC archive + original platform providers
```

It invokes only the existing `c_cpu_init`/register bridge fixture. It never
calls `c_cpu_simulate`, dispatches a BOP, enters `BIOS[]`, calls DEM, or makes
a DOS startup decision.

The target includes two local fail-closed guards solely to separate the
source-unrecovered SAS-vector slots from unrelated host-link closure. They
execute `__debugbreak` if called and are not a runtime completion. Its
deliberate link failure therefore leaves the remaining seven unresolved names:

```text
sprintf
read_pointers
LIM_b_write
LIM_w_write
config_inquire
host_read_resource
c_effective_addr
```

Thus CVIDC is a restored historical dependency with successful source closure.
The two guarded slots are isolated evidence, not restored behavior; the
remaining host closure still blocks execution.

### Revalidation (2026-08-06)

The same default-disabled target was rebuilt from the current fixed input with:

```text
toolchain-probe\build-opennt-support-provider.cmd
  ntdos64-opennt-ccpu-cvidc-register-bridge-fixture
```

It compiled the original CCPU FPU archive, original CVIDC archive, and the
selected platform-provider archive, then failed at the intentionally
unclosed executable link. The unresolved imports were exactly:

```text
sprintf
read_pointers
LIM_b_write
LIM_w_write
config_inquire
host_read_resource
c_effective_addr
```

This is a link diagnostic, not a failed guest run: no guest media, BIOS BOP,
DEM service, DOS return, or device result was synthesized. Its classification
is important for the next work boundary:

- `sprintf` is a historical x86 CRT/toolchain binding, not a DOS service;
- `read_pointers` belongs to the original video provider and must retain its
  video/host-display owner;
- `LIM_b_write` and `LIM_w_write` are the EMS discontinuity documented in
  `OPENNT-EMS-PROFILE-GAP.md`, not ordinary RAM writes;
- `config_inquire` and `host_read_resource` are original configuration/ROM
  resource seams; and
- the locally tested `c_effective_addr` algorithm still needs an admitted
  historical monitor selector/descriptor context before it can serve ROM
  discovery.

Accordingly, this revalidation confirms that CCPU is not blocked solely by
the two opaque vector slots. Those slots are still semantically unrecovered,
but the nearer closure work is historical video, EMS, ROM-resource,
configuration, CRT, and monitor-context composition. None of those may be
filled by a self-written BOP, DEM, IVT/BDA, or PC/AT success substitute.

`config_inquire` and `host_read_resource` are not missing source interfaces.
Their original NT4 owners are respectively `host/src/config.c` and
`host/src/nt_rez.c`, both already covered by the fixed-source manifest. A new
default-disabled `ntdos64-opennt-config-resource-objects` compile gate keeps
those original units together solely to expose their old profile, file-search,
console, disk, and resource dependencies. It is deliberately not linked into
the CCPU diagnostic: a successful object compile does not make its historical
configuration defaults or resource lookup policy a modern runtime contract.

The first compile attempt identified one further fixed-source input required by
the original `config.c`: its private MVDM declaration header `vdmapi.h`.
`public/internal/base/inc/vdmapi.h` was imported under its original relative
path from the local fixed OpenNT source; it is byte-identical to the local
OpenNT 4.5 copy (SHA-256 `B942656BA8761DDB9325A7EEA7997394562D2E57797C923FF5688D7BE86426DE`)
and differs from the later XP copy. The manifest records it as an upstream
copy. This adds declarations such as `VDMINFO` and `GetNextVDMCommand`; it
does not implement the obsolete service or allow an invented replacement.

The next original declaration dependency is
`public/internal/windows/inc/conapi.h`, byte-identical between the same local
OpenNT and OpenNT 4.5 inputs (SHA-256
`1A78135501F91AC2A004CB68C5831C626E907481AFC62970EB97747D16485DE2`) and
again divergent in XP. `config.c` uses its console display-mode and key-shortcut
surface alongside ordinary console title calls. The header is therefore a
manifested compile input only. Any eventual implementation belongs to a
versioned modern-host console capability shim and must preserve the original
caller boundary; it must not become CCPU, BIOS, or DOS policy.

The incomplete fixed NT4 staging lacks `pmvdm.h`. A prior conclusion that the
include was stale was incorrect and has been withdrawn: the locally available
OpenNT 4.5 source contains `windows/inc/pmvdm.h` (SHA-256
`D9BC8A4E975F9D2BC1AEF89D8E8854F2DC8AD8FAC53B49D6F4998BCCAFD4AFC8`). It
defines both the Program Manager `Reserved` grammar
`dde.%d,hotkey.%d,ntvdm.%d,` and the property bits
`PROPERTY_HAS_CURDIR`, `PROPERTY_HAS_HOTKEY`, and `PROPERTY_HAS_TITLE`.

The fixed OpenNT and OpenNT 4.5 `config.c` files are byte-identical (both
SHA-256 `2A87D13D633E62B7BDA2D982127FEEA19B20B5A779FF1E384966C2EB3CE6DDFA`).
The header's only symbols consumed by that file are those three constants; the
same values persist in the locally available XP header. This permits an
explicit, narrowly scoped compatibility-header admission under
`public/internal/windows/inc/pmvdm.h`. The manifest labels its foreign source
path and evidence. It is not claimed to be a recovered fixed-NT4 file, is not
a general source-profile merge, and carries no host behavior.

The one-line local `config.c` overlay remains removed; the gate selects the
byte-preserved original source. The admission establishes the constants and
their syntax only. It is not evidence that the PIF property protocol can be
invented or omitted.

Before the admission, the isolated i686 clang-cl gate reached original
`config.c:53` and failed solely with `pmvdm.h file not found` (apart from
historical SDK warnings). After the admission it reached `config.c:344`, where
the generic staging `local/include/nt.h` caused an undeclared `NtCurrentPeb()`
to be implicitly typed as `int`. The staged original NT umbrella headers do
contain the historical PEB definition, but cannot coexist with the forced
modern `windows.h` staging preamble: their basic NT types are redefined.

The active default-disabled gate therefore uses a one-site compiler overlay of
`config.c`. It preserves the original `config()` control flow and temporary
title format, replacing only the obsolete
`NtCurrentPeb()->ProcessParameters->ConsoleHandle` query with unresolved
`ntdos64_legacy_console_identity()`. That symbol is a named modern host-context
seam, not a PEB structure, a fabricated process block, or an implementation.
Any later shim must obtain a process-local console identity through documented
host facilities and must state how the result is used. No compiler overlay or
4.5 include directory participates beyond this explicitly manifested input.

With that admission and one-site host-context overlay, the isolated i686
clang-cl `ntdos64-opennt-config-resource-objects` gate now reaches successful
object closure for the original `config.c` and `nt_rez.c`. This is only a
compile result. `ntdos64-opennt-config-resource-link-fixture` is separately
default-disabled and contains a no-op `main`; it includes both objects directly
solely to enumerate their unresolved external host dependencies. It never
calls configuration or resource code and is not a runnable VDM path.

The remaining research boundary is therefore more precise: `config.c` expects
the historical PEB process-parameter layout and a CSR/BaseSrv-backed
`GetNextVDMCommand` transport. The 4.5 header establishes the known PIF
property-string syntax, but not a modern transport, scheduler, console ABI, or
runtime admission. Those remain a versioned modern-host process/console-shim
boundary and must not become CCPU, BIOS, DEM, or DOS implementation.

### Original XMS provider object closure

The fixed-source `base/mvdm/xms.486` provider is now staged as one
default-disabled object gate: `xms.c`, `xmsa20.c`, `xmsblock.c`, the
historically selected x86 `i386/xmsmem86.c`, `xmsdisp.c`, `xmsmisc.c`, and
`xmsumb.c`. The i686 clang-cl gate initially reached object closure for the six
top-level provider units; the x86 memory provider is staged separately because
the original `i386/sources` selects it outside that top-level list. This
establishes only that their source and
declarations can be analysed under the selected historical compiler island; it
neither initializes XMS nor enters any guest path.

Two narrowly recorded compiler-compatibility adjustments were required. The
staging `local/include/nt.h` now supplies only the `NTSTATUS` and `PNTSTATUS`
base typedefs from the historical `ntdef.h` and the base `STATUS_SUCCESS`
constant from `ntstatus.h`, because importing the complete
legacy type header conflicts with the already forced modern `windows.h`
preamble. The active `xmsblock.c` overlay rewrites only old cast-lvalue pointer
assignments into equivalent typed assignments accepted by clang-cl. It preserves
the original allocation, move, and access algorithm and supplies no SAS, A20,
UMB, guest-memory, or host behavior. Both changes and hashes are recorded in
`src/opennt/SOURCE-MANIFEST.json`.

`ntdos64-opennt-xms-link-fixture` is a separate default-disabled no-op-main
target that includes the XMS objects directly and never calls them. Its only
purpose is to enumerate unresolved historical provider dependencies at link
time; it must not become an XMS substitute, a bootstrap aid, or a runnable
VDM path.

The unresolved `SA*` allocation family has a distinct historical owner:
`base/mvdm/suballoc/suballoc.c`, selected with `DEV_DBG` by its original
`sources` file and accompanied by its private `suballcp.h`. It is staged as
the separate default-disabled `ntdos64-opennt-suballoc-objects` gate. This is
source restoration, not a custom allocator and not yet a link admission to
the XMS fixture. The separate suballocator object gate now reaches successful
i686 clang-cl object closure; only its deliberate link-only aggregation with
XMS remains to be measured.

With the original suballocator included directly, the XMS link-only fixture no
longer imports any `SA*` symbol. Its remaining imports divide into historical
machine/monitor calls (register access, A20 state, and `UpdateKbdInt15`), the
historical `ReserveUMB` owner, `DbgPrint`, and the legacy native-memory calls
`NtAllocateVirtualMemory`, `NtFreeVirtualMemory`, and `NtCurrentProcess` used
only by the selected x86 memory provider. The original `nt_umb.c` source is
already staged and now has its own default-disabled object gate; it is not yet
linked into the fixture. That gate has since reached i686 clang-cl object
closure; its only observed diagnostic is the original source's non-void return
warning at `nt_umb.c:844`, not a staging or host-seam failure. The next
link-only measurement adds it to the XMS fixture without calling any provider.

With `nt_umb.c` included directly, `ReserveUMB` also closes naturally. The
remaining 20 link imports are limited to CCPU register/SAS/A20 surface
(`get*`, `set*`, `c_sas_connect_memory`, and A20 wrappers), the historical
keyboard `UpdateKbdInt15` callback, `DbgPrint`, and the x86 provider's legacy
native-memory calls. This is an ownership observation only: no symbol has been
stubbed or implemented to make the fixture link.

Adding the whole historical keyboard/mouse/PPI provider removes the isolated
`UpdateKbdInt15` import but expands the fixture to 127 unresolved imports. The
increase is expected evidence of the original topology, not a regression: the
provider now reaches the CCPU register and SAS calls, BIOS table, PIC/PIT/I/O
and timer surface, video mode/display surface, and host keyboard/mouse/idle
lifecycle. Source ownership confirms these families: CCPU/CVIDC own the
`c_*` register/SAS calls; `bios.c` owns `BIOS`; the base video/system/provider
libraries own display, I/O, ICA, and timer calls; and the NT host files own
idle, timer, mouse, and reset-time function-table state. The fixture is thus
frozen as a bounded closure probe. It must not be treated as a checklist for
adding isolated callbacks or as a runnable device stack.

The existing `ntdos64-opennt-historical-provider-archive` is the proper
non-executing aggregation boundary for these original machine providers. Its
selection now includes the independently gated XMS, suballocator, and UMB
objects in addition to the earlier BIOS/system/support/keymouse/disk/comms/
video groups. This archive remains default-disabled, has no executable entry
point, and contains no modern-host facade, CCPU archive, guest media, or
runtime admission. The standalone i686 clang-cl gate rebuilt this expanded
archive successfully on 2026-08-07 as
`ntdos64-opennt-historical-provider-archive.lib` (2,337,008 bytes).

### CCPU/CVIDC Current Link Baselines

The current initialization-only CCPU/CVIDC fixture still has exactly seven
unresolved imports: `sprintf`, `read_pointers`, `LIM_b_write`, `LIM_w_write`,
`config_inquire`, `host_read_resource`, and `c_effective_addr`. It does not
execute CCPU. The separate original-video closure fixture resolves
`read_pointers` from its historical owner, then exposes the expected video,
clock, input, and stream-policy boundary. Its 20 remaining imports do not
include `read_pointers`; they include only original provider or host-policy
families plus the separately constrained `c_effective_addr` and EMS writes.

The next measurement links the already built, default-disabled complete
historical-provider archive into that same no-op video closure fixture. This
is a linker-only classification step: it must neither call `c_cpu_simulate`
nor turn any resulting unresolved name into an ad-hoc service, device, or DOS
startup substitute.

The 20-symbol full-provider closure has a coherent original host display/input
owner group: `copy_fnc.c`, `nt_graph.c`, `nt_fulsc.c`, `nt_sound.c`, and
`nt_mouse.c`. They are therefore admitted first as the one default-disabled
`ntdos64-opennt-host-display-input-objects` object gate, rather than as
individual callback replacements. `nt_timer.c` remains independently gated:
its known historical console/thread declaration dependency must be classified
separately before any clock behavior is admitted.

The first display/input compile pass stopped only because the forced modern
`winsock.h` preamble defines the legacy compatibility macro `h_addr`, which
collides with a local `copy_fnc.c` identifier. The group gate undefines that
single SDK macro at compile scope. No original source or behavior is changed.
Because `winsock.h` is itself included by the forced preamble, the compatibility
header is forced immediately after that preamble; a command-line undef alone
would be redefined too early.

The subsequent `conapi.h` blocker is satisfied only as a fixed OpenNT
declaration input (`SHA-256 1A78135501F91AC2A004CB68C5831C626E907481AFC62970EB97747D16485DE2`),
already manifested under `public/internal/windows/inc`. It supplies no console
implementation or runtime admission.

The display/input group also exposes a fixed NT4 header-order discrepancy:
`nt_graph.h` declares `host_stream_io_enabled` as Insignia `boolean` (`IBOOL`,
or signed `int`), while a later `winsigni.h` include changes otherwise-unbound
`BOOL` into `UINT` before `nt_graph.c` defines the storage. NT4.5 retains the
same source spelling, so importing that source would not repair the profile.
The default-disabled gate applies a compiler-only header immediately after the
staging preamble that fixes `BOOL` to signed `int` before any historical header
is read. It changes neither an original source file nor any layout or runtime
behavior; it merely preserves the type relationship already expressed by
`boolean = IBOOL` and the original Win32 `BOOL` typedef.

The next compile failure was likewise a declaration boundary, not missing
sound behavior. `host/src/nt_sound.c` includes the historical Native NT
umbrella and uses `OBJECT_ATTRIBUTES`, `UNICODE_STRING`, `IO_STATUS_BLOCK`,
`NtCreateFile`, `NtDeviceIoControlFile`, and `FILE_OPEN_IF`; the existing
global staging `local/include/nt.h` deliberately exposes only the smaller
suballocator subset. A separate display/input-only declaration header now
reproduces just those layouts, values, and prototypes from the retained
OpenNT `public/sdk/inc/ntdef.h` and `ntioapi.h`. It also undefines the modern
multimedia `PlaySound` macro so the historical local helper keeps its original
name. It supplies no sound, file, device, or host-policy implementation.

With those two compile-only inputs, the complete original
`ntdos64-opennt-host-display-input-objects` x86 object gate builds successfully
under clang-cl: `copy_fnc.c`, `nt_graph.c`, `nt_fulsc.c`, `nt_sound.c`, and
`nt_mouse.c`. The gate is still `EXCLUDE_FROM_ALL`, is not in the historical
provider archive or any executable, and has never been run. This is evidence
that the original host display/input source boundary can be reconstructed as
objects; it is not evidence that the legacy console, graphics, mouse, beep,
or Native device paths work on a modern host.

The video closure probe then directly includes those five original objects for
measurement only; it does not change the aggregate historical-provider archive.
This first exposed two obsolete local fixture fallbacks,
`host_mouse_install1` and `host_mouse_install2`, whose original owner is
`nt_mouse.c`. The closure target alone now excludes those two fallback
definitions, while the smaller register fixtures retain their fail-closed
baseline. Thus the original mouse owner, rather than a local substitute, is
the only definition on the measured historical-owner path.

With `/errorlimit:0` on that never-run link-only probe, the full closure reports
162 unresolved symbols. This is a dependency inventory, not a runtime failure:
the probe has no guest entry and does not call `c_cpu_simulate`. Compared with
its prior 20-symbol closure, the original display/input sources resolve
`fwd_word_fill`, `memfill`, `memset4`, `host_disable_stream_io`,
`host_enable_stream_io`, `host_os_mouse_pointer`, `host_ring_bell`,
`host_stream_io_update`, `paint_screen`, `ResumeMouseInterrupts`,
`SuspendMouseInterrupts`, `VirtualX`, and `VirtualY`. The remaining members
of that earlier set are exactly the expected unrecovered boundaries:
`c_effective_addr`, `Cpu_define_outb`, `GetPerfCounter`, `host_memset`,
`HostIdleNoActivity`, `LIM_b_write`, and `LIM_w_write`.

The larger list now classifies the next work without inventing a DOS runtime:
native-import linkage (`NtCreateFile`, `NtDeviceIoControlFile`, RTL routines),
the original console/graphics palette and paint-table family, configuration and
PIF state, timer/idle and keyboard event machinery, serial/parallel/floppy
host backends, SAS A20 operations, and the still-separated EMS and CCPU port
paths. No local implementation was added for any of these names. In
particular, the Native-I/O compatibility header gives `nt_sound.c` declarations
only; the unresolved `Nt*` and `Rtl*` imports demonstrate that no modern host
implementation has yet been admitted or exercised.

The paint-table and palette family has since been recovered as a separate
original owner group: `host/src/nt_hosts.c`, `nt_cga.c`, `nt_ega.c`, and
`nt_vga.c`. These fixed, manifested inputs compile together in the
default-disabled `ntdos64-opennt-host-graphics-render-objects` gate. The sole
source-level accommodation is a target-local wrapper around the original
`debug.h`: under the already-empty `PROD` trace configuration it accepts one
fixed-NT4 `nt_cga.c` `sub_note_trace4` invocation with a trailing comma. The
original `.c` file, its paint logic, and all non-trace behavior remain
unchanged.

Directly adding that group to the same never-run closure probe reduces the
complete unresolved set from 162 to 108 symbols. It resolves the original
`Red`, `Green`, `Blue`, `CreateDisplayPalette`, `nt_text`, and the full
CGA/EGA/VGA paint-table families. `SetConsolePalette` remains unresolved
because it belongs above the renderer in the historical console/mode-transition
surface; it must not be recreated inside `nt_graph.c` or the render gate. The
remaining 108 names preserve distinct next owners: console transition and
event/timer state, configuration/PIF state, host serial/parallel/floppy
backends, Native import linkage, SAS A20 operations, EMS, and the constrained
CCPU port/effective-address paths. None is linked into the runner or executed
by this probe.

The historical `config.c` plus `nt_rez.c` configuration/resource owner group
also builds as its own default-disabled object gate. Direct inclusion in the
same closure resolves `config`, `config_activate`, `config_get_active`,
`config_inquire`, `number_of_floppy`, and `host_read_resource` from their
original sources. The complete unresolved set consequently grows from 108 to
118 rather than shrinking: the newly admitted original configuration path
truthfully exposes its further host dependencies, including drive selection,
PIF/profile state, resource retrieval, and Native memory imports. This is the
correct closure result; no configuration result is replaced by a local default
or made part of the runner.

The remaining console calls (`SetConsolePalette`, `ConsoleMenuControl`,
`ShowConsoleCursor`, and `VDMConsoleOperation`) have a different status. The
fixed OpenNT `conapi.h` declares them, while the implementation belongs to the
historical Windows console client/server rather than to SoftPC or DEM. They
are therefore explicit future modern-host compatibility-shim dependencies,
not source-recovery candidates and not functions to recreate inside a VDM or
graphics provider. The local cross-snapshot owner search records this split
before any host behavior is designed.

The next complete original storage-owner group, `host/src/nt_fdisk.c` plus
`nt_rflop.c`, compiles as the default-disabled
`ntdos64-opennt-disk-host-backend-objects` gate under its historical x86 Native
header profile. The profile requires `_X86_` so retained `nt.h` selects
`ntx86.h`, and the target-local `ntdddisk.h` declaration redirect establishes
the source's adjacent Win32 scalar declarations before retained `winioctl.h`.
Neither adjustment changes disk policy or supplies an I/O implementation.

Direct closure inclusion resolves every measured original fixed/floppy owner:
`host_fdisk_valid`, `host_fdisk_change`, `host_fdisk_active`,
`host_fdisk_term`, `host_fdisk_get_params`, `host_fdisk_seek0`,
`host_fdisk_rd`, `host_fdisk_wt`, `host_gfi_rdiskette_valid`,
`host_gfi_rdiskette_change`, and `host_gfi_rdiskette_active`. The closure
falls from 118 to 115 unresolved symbols. Its retained warning that the floppy
thread is cast from cdecl to the Win32 thread calling convention, together with
the remaining Native/direct-drive imports, is deliberate evidence that this
is not executable modern-host behavior. The group remains outside the
historical provider archive, all normal products, and every execution path.

The EMS `LIM_b_write` and `LIM_w_write` imports have an identified historical
implementation in the separately staged NT4.5 `base/dos/emm_mngr.c` input. Its
write routines maintain LIM page mapping and notify SAS after the physical
write. The fixed NT4 source snapshot omits that middle provider. The separate
NTVDMx64 HAXM SAS source only calls these functions; it does not define their
semantics. Consequently neither input is admitted to the fixed-NT4 CCPU link
profile: this is cross-version recovery evidence, not a license to replace EMS
with normal RAM writes or to blend the profiles.

The complete original keyboard/mouse/PPI provider object gate also reaches
successful i686 clang-cl closure without a local compiler overlay. Its later
inclusion in the XMS link-only fixture is intentionally whole-provider, not a
one-symbol substitute for `UpdateKbdInt15`.

The guards' relocation scope was subsequently checked across all 129 objects
in the selected CCPU archive. Each opaque symbol is imported only by
`ccpu386/sascdef.c.obj`, where it occupies a tail callback slot in the
generated `cSasPtrs` initializer; no selected CCPU instruction object imports
either name directly. This permits a later fail-closed *reachability*
experiment, but does not provide their behavior or make either guard a runtime
implementation. The full contract and admission rule are in
`CCPU-MISSING-INTERFACE-CONTRACT.md`.

`host_simulate` is no longer part of that unresolved set only for the legacy,
default-disabled diagnostic fixtures that still link
`local/archived/legacy-adapters/ccpu_host_simulate_bridge.c`. This
source-derived CCPU-only bridge calls `c_cpu_simulate()` and was based on the
small wrapper portion of OpenNT `host/src/nt_cpu.c`, deliberately excluding its
NT-host `IcaLock` assertions. Per M92 it is archived: it is not an admitted
runtime implementation and cannot advance a DOS/BIOS/BOP path. The retained
fixture result proves only the recursive interpreter-call boundary; a future
runtime path must recover and verify the original lifecycle/context contract
separately.

## `c_effective_addr` Contract Fixture

`overlay/base/mvdm/softpc.new/base/ccpu386/c_effective_addr.c` is a narrowly scoped local
completion, not an imported historical source restoration. It follows the
available monitor implementation's mode split and selector-table behavior:
real/V86 mode computes `segment << 4 + offset`; protected mode resolves a
descriptor base; an invalid selector returns `0xffffffff`.

The default-disabled `ntdos64-opennt-ccpu-effective-addr-fixture` builds and
runs under the x86 clang-cl island. It supplies a controlled monitor ABI for
real mode, V86, GDT, LDT, and invalid-selector cases. The test intentionally
does not link the CCPU archive: doing so would require independently admitting
unrelated SAS, LIM, ROM, and host-simulation dependencies merely to construct
synthetic descriptors. The fixture therefore validates only this interface's
proven contract and cannot be cited as CCPU execution or DOS-startup evidence.

This removes `c_effective_addr` from the missing-*semantics* set, but not from
the existing full historical-link diagnostic until the monitor-side selector
and descriptor providers are restored as separately evidenced dependencies.

## `read_pointers` Ownership Result

`read_pointers` has an admitted original provider: under the active CCPU
profile, `base/mvdm/softpc.new/base/video/ega_read.c` defines the global and
initializes it through `ega_read_init()` / `ega_read_routines_update()`. CCPU
physical byte and word reads pass its callbacks to the SAS/video routing path.
The complete default-disabled `ntdos64-opennt-video-provider-objects` gate now
compiles successfully with that same profile.

It is therefore a provider-closure task, not a missing-interface task. Do not
place a zero callback table or ordinary-RAM alias in the CCPU diagnostic. The
next evidence target must link the original video object set in a *separate*
closure probe, record its additional display/port/initialization imports, and
keep that larger surface out of the current CCPU/CVIDC baseline.

`ntdos64-opennt-ccpu-cvidc-video-closure-fixture` is that separate,
default-disabled link-only probe. It adds the complete original video object
set to the unchanged CCPU/CVIDC baseline and must never be run. Its acceptance
criterion is not a successful executable: `read_pointers` must resolve from
the original provider, while every newly observed unresolved symbol is recorded
as a video-provider boundary before any host compatibility code is considered.

The first probe has established the next boundary. `read_pointers` is provided
by `ega_read.c`; the first unresolved video group is instead the VIDC global
accessor/state layer: `setVideo*` names are defined by original
`ccpu386/vglob.c`, and `Video` storage is defined by original
`ccpu386/localfm.c`. Both units are intentionally outside the archived CCPU
FPU link profile used by the baseline. They must therefore be evaluated as a
separate, explicitly selected historical CCPU/VIDC profile addition, not added
silently to the baseline or recreated as a modern display shim.

The dedicated default-disabled
`ntdos64-opennt-ccpu-vidc-global-objects` gate records this exact profile
increment. `localfm.c` remains staged as the original provenance input; its
abbreviated historical include context leaves the SAS/VIDC types incomplete
under clang-cl, so the local `ccpu_vidc_global_storage.c` overlay reproduces
only the storage absent from the selected archive. The archive already owns
`Sas`, so the overlay deliberately supplies only `Gdp`, `Cpu`, and
`Video`; this preserves the historical one-definition boundary. The overlay
uses the generated-header order evidenced by `sascdef.c` and adds no CPU,
video, port, or host behavior. Only the video-closure probe consumes it; the
CCPU archive and its smaller baseline fixtures remain unchanged.

**Governance supersession, 2026-08-09:** the preceding paragraph is retained
as the historical description of an archived comparison probe, not as an
admission. `localfm.c` is absent from the fixed historical `sources` list and
`ccpu_vidc_global_storage.c` is classified as `archived/legacy-adapters` in
`docs/LOCAL-CODE-GOVERNANCE-LEDGER.md`; neither may be linked into a current
closure, fixture, runner, or runtime. The current R1 work keeps the CCPU
five-slot `Video` and C-VIDC 81-slot `Video` ABI domains separate. See
`docs/R1-CCPU-CVIDC-ABI-MATRIX.md` and
`docs/R1-GENERIC-VIDEO-RECONSTRUCTION-ADMISSION.md`.

With that profile in place, the link probe confirms that `read_pointers` and
the VIDC storage/accessor layer resolve. The remaining observed closure is
substantially larger and has four distinct historical owners:

- host display/input policy: `working_video_funcs`, `VirtualX`,
  `VirtualY`, `paint_screen`, stream-I/O hooks, bell, idle, and performance
  counter symbols are owned across `host/src/nt_reset.c`,
  `nt_mouse.c`, `nt_graph.c`, `nt_fulsc.c`, `nt_sound.c`,
  `nt_unix.c`, and `nt_timer.c`;
- PC reset and mouse notification: `soft_reset` is owned by
  `base/bios/reset.c`, while `mouse_video_mode_changed` is owned by
  `base/keymouse/mouse_io.c`;
- CPU and memory-controller mechanisms: `Cpu_define_outb` is declared by
  `ccpu386/ntstubs.c`, but that CCPU-profile implementation explicitly
  discards both arguments and is therefore a no-op compatibility stub, not an
  admitted port-dispatch provider. `LIM_b_write` and `LIM_w_write` belong
  to the EMS provider;
- portable host copy helpers: `memfill`, `fwd_word_fill`, and `memset4`
  are implemented by `host/src/copy_fnc.c`.

These are not candidates for an ad hoc display shim. They establish the next
historical profile boundary: host display integration, BIOS/reset, input, EMS,
and CPU port dispatch must each be evaluated from their own original sources,
with an explicit consumer and no guest execution until their complete chain is
verified.

The evidence command was:

```text
toolchain-probe\build-opennt-support-provider.cmd
  ntdos64-opennt-ccpu-cvidc-video-closure-fixture
```

It successfully compiled the complete original video object set and the
explicit VIDC storage increment. Its expected link-only failure then reported
the owners above; it did not report `read_pointers`, `Video`, or any
`setVideo*` accessor as unresolved. The fixture was not run.

## Communications-Host Closure Increment

The original communications-host owners are now compiled together in the
separate, default-disabled `ntdos64-opennt-comms-host-backend-objects` gate:
`host/src/nt_com.c`, `nt_lpt.c`, `nt_ntfun.c`, and `nt_wcom.c`. This is a
historical host group, not a serial, printer, or DOS replacement. It is not in
the provider archive, the research runner, or any default target.

The gate requires only source-provenanced declarations: the original private
`public/internal/base/inc/ntddvdm.h` input, the already-staged original
private console declarations, the historical `try/except/finally` spelling
map, and declaration-only Native I/O types retained from OpenNT's public
headers. None provides device I/O, console handling, exception handling, or
DOS behavior.

Adding those object files directly to the existing link-only closure fixture
changed the unresolved-symbol count from 115 to 101. It resolves the complete
previously observed serial/parallel owner set:

- `host_com_lock`, `host_com_char_read`, `host_com_check_adapter`,
  `host_com_da_int_change`, `host_com_disable_open`, `host_com_ioctl`,
  `host_com_open`, `host_com_read`, `host_com_reset`, `host_com_unlock`, and
  `host_com_write`;
- `host_lpt_status`, `host_print_auto_feed`, `host_print_byte`,
  `host_read_printer_status_port`, `host_reset_print`, and
  `host_set_lpt_direct_access`;
- `wow_com_inb` and `wow_com_outb`.

The retained source exposes exactly five newly observable historical host
dependencies: `NtWaitForSingleObject`, `NtWaitForMultipleObjects`,
`RtlNtStatusToDosError`, `VdmUnhandledExceptionFilter`, and
`RcErrorBoxPrintf`. The first three are Native NT synchronization/status
surface; the latter two are the historical VDM process/console error path.
They are evidence for narrow, named modern-host shim obligations after the
original chain is selected. They are not authorization to add substitute
implementations or to claim serial/printer runtime support. The fixture's
link failure remains expected and it was not run.

## CCPU EOI and Timer Callback Gates

The original `host/src/nt_eoi.c` now compiles as its own default-disabled
object gate under the selected non-`MONITOR` profile. Its direct `ntexapi.h`
include is guarded only for that target because the source's only executive
call, `NtVdmControl`, is inside the excluded `MONITOR` branch. This avoids
reconstructing unrelated executive declarations while preserving the original
EOI owner and callback implementation unchanged. The object is not linked into
a fixture or runtime.

The neighboring original `host/src/nt_timer.c` remains a recorded compile
boundary. Its private console declaration route is now present; the remaining
errors are historical NT runtime declarations and semantics:
`RtlExtendedLargeIntegerDivide`, `RtlExtendedIntegerMultiply`, the `try`
spelling, and `STATUS_ALERTED`. The large-integer helper results feed
heartbeat, elapsed-time, and wait logic, so replacing them with unverified
modern CRT arithmetic would change timing behavior. Recover their precise x86
ABI and status/exception semantics from matching historical headers or build
inputs before admitting a declaration-only compatibility surface. No timer
behavior has been implemented or redirected.

The original `host/src/nt_cpu.c` is a distinct, harder boundary. Its object
gate stops on `GDP_PTR` and the complete `ConstraintR*` register-constraint
set. Those names encode CCPU-generated data layout and instruction-constraint
ABI, not an operating-system callback surface. It must remain outside modern
host-shim work until the matching generated CCPU inputs or an independently
validated reconstruction are available.

When `nt_eoi.c` and `nt_timer.c` are selected as original owners in the
link-only closure fixture, their trace-only fallbacks are excluded only for
that fixture. The measurement resolves eight previously observed symbols:
`GetPerfCounter`, `host_DelayHwInterrupt`, `host_GetSysTime`,
`host_timer_init`, `RegisterEOIHook`, `ResumeTimerThread`, `ThreadInfo`, and
`VdmUnhandledExceptionFilter`. The unresolved closure grows from 101 to 192
symbols because the original heartbeat/startup path makes its real downstream
dependencies reachable. Those dependencies divide into native NT
synchronization, timing, process, memory and object calls; historical VDM
control/lifecycle and PIF policy; OEM file/path/environment wrappers; DOS,
DPMI, command and debug dispatch; and CCPU register/SAS access. This is
evidence that timer/EOI recovery must stay part of the complete historical
host composition. It is not a basis for a standalone timer shim or for
admitting any of those paths to the default runner.

## CCPU Scope Result

This closure result is direct evidence that CCPU is a software CPU interpreter
and part of the SAS/VIDC dispatch environment, not a complete PC/AT machine or
a self-contained DOS host:

- the video device behavior lives in the separate original `base/video`
  object family plus host display/input modules; it is not a video ROM
  embedded in CCPU;
- EMS is outside CCPU: its physical-write path imports the LIM provider;
- BIOS reset policy, keyboard/mouse behavior, host timer policy, console
  painting, and stream I/O each have separate original owners;
- the CCPU-specific `Cpu_define_outb` stub confirms that a normal generic
  I/O registration path cannot be assumed to restore VGA semantics for this
  interpreter profile.

Accordingly, the research runner must recover the historical profile
composition and its narrow modern-host shims, rather than treat CCPU as a
drop-in replacement for a virtual machine's CPU, firmware, video, or EMS
subsystems. The same observation supports the future neutral boundary:
machine/video/EMS capability contracts can be inferred from these consumers,
but historical source-specific structures and stubs must not be made part of
that contract.

The cross-version EMS recovery result is documented separately in
`OPENNT-EMS-PROFILE-GAP.md`: the available NT4.5 middle layer compiles, but
its `NTVDM` branch does not provide the two CCPU LIM-write imports. It is
therefore evidence of a historical profile discontinuity, not an admitted
provider.

## Disposition

The remaining A-path work is source recovery and semantic classification, not
writing a generic stub:

1. Search for a source-provenanced CCPU generator/build artifact outside the
   currently available snapshots, with exact profile and ABI evidence.
2. Classify the direct callers of `c_sas_touch` and `c_VirtualiseInstruction`,
   including fault, address, and reentry behavior.
3. Restore the monitor-side selector/descriptor providers before the
   `c_effective_addr` completion is used by a full historical-link diagnostic.

If no original provider or generator can be obtained, the result must be
recorded as a historical-input limit. It is then evidence for a separate
software-monitor/core execution design, not permission to place CCPU-private
APIs into the shared core.
