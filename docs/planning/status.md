# Pre-Governance Historical Status Ledger

> **Superseded as current authority on 2026-08-09.** Current work, technical
> baseline, and active packet are maintained only in [../STATUS.md](../STATUS.md).
> This expanded ledger is retained as evidence during the migration documented
> in [../etc/operations/governance-migration.md](../etc/operations/governance-migration.md).

## Current Task

**Native-image dispatch supplement (2026-08-09):** the modern CLI now records
the complete separation between NT4 COMMAND's historical `/z` shell-out
protocol and its own direct PE32/PE32+ child-process dispatch. PE targets stay
outside the guest; NE/WOW targets are explicitly rejected rather than being
misrouted to a DOS engine. This is a runner policy correction, not a
`SVC_CMDEXEC` implementation or a SoftPC runtime claim. Authority:
`docs/research/ccpu-bios-closure/S6-NATIVE-IMAGE-HANDOFF-CONTRACT.md`.

**Video/SAS recovery-limit supplement (2026-08-09):** a second original
generic-Video consumer, the `gfx_updt.c` marking-mode switch, confirms that
the missing generic vector has reset and runtime lifecycle significance but
still supplies no historical initializer. The two `c_sas_touch` and
`c_VirtualiseInstruction` names are later patched SAS slots with no original
dynamic callers, not missing OpenNT semantics to implement. Authority:
`docs/research/ccpu-bios-closure/S5-VIDEO-AND-PATCHED-SAS-RECOVERY-LIMIT.md`.

**Historical selector-50 regression (2026-08-09):** the default-disabled
`ntdos64-opennt-original-bios50-date-fixture` was rebuilt in the i386
historical build tree and exited `0`. This preserves the bounded original
`BIOS[50h] -> MS_bop_0 -> DemDispatch` date-service evidence after the current
source/governance audit; it is not an NTIO, reset, or DOS-runtime result.

**Next Gate:** S5 software-monitor-to-historical-host transport. S1--S4 are
completed bounded evidence gates. S5 has completed the original NTIO
`50h/11h` BYOB `NTDOS.SYS` load and its immediate original idle notification;
the source-level return-to-monitor contract is now recorded. The fixed-record
wire layout has passed independently in the x64 and i386 build domains, but
the first helper link is blocked by the original complete `BIOS[]` static
closure. Active work is the static-selector/dynamic-reachability/host-owner
ledger, not a new transport or device shim. The current ordered route is
`docs/SOFTPC-RECONSTRUCTION-ROADMAP.md`; the implementation cut and closure
evidence are `docs/S5-CROSS-BITNESS-TRANSPORT-CUT.md` and
`docs/research/ccpu-bios-closure/S5-BIOS-STATIC-DYNAMIC-OWNER-LEDGER.md`;
the selector-50 and DEM service-table boundary is recorded in
`docs/research/ccpu-bios-closure/S5-SELECTOR50-OWNER-CLOSURE.md`; the first
event-source versus private-console-seam classification is
`docs/research/ccpu-bios-closure/S5-EVENT-HOST-SOURCE-MATRIX.md`; original
C-VID archive closure is recorded in `docs/S5-VIDEO-INPUT-RECONCILIATION.md`.
The full no-SM0 static owner/seam separation is recorded in
`docs/research/ccpu-bios-closure/S5-FULL-NOSM0-OWNER-SEAM-LEDGER.md`; it
prevents complete-table unresolved imports from being mistaken for the next
dynamic NTIO dependency.

**Static-closure correction (2026-08-09):** generic C-VIDC `Video`
composition is not a proven NTIO dynamic dependency and cannot justify a
video/device shim. It is nevertheless a hard static closure input for any
clean-link helper retaining original complete `BIOS[]`: `C_Video` resolves
the generated provider but not the 81-slot generic `Video` storage/initializer
required by `ev_glue.c`. Until a matching historical composition artifact is
recovered or a separately admitted profile-aware reconstruction exists, v2
transport remains an evidence candidate and must not advance to a runnable
helper through aliases, short-table reuse, a reduced BIOS table, or
`/FORCE:UNRESOLVED`. Authority:
`docs/research/softpc-source-matrix/NT4-CCPU-CVIDC-PRODUCT-REACHABILITY.md` and
`docs/S5-VIDEO-INPUT-RECONCILIATION.md`.

**C-VID provider closure supplement (2026-08-09):** the original 19-unit
video provider inventory now compiles in the fixed C-VID profile, and adding
the original `ccpu386/vglob.c` resolves its named `getVideo*`/`setVideo*`
accessor imports in a source-level symbol closure. This narrows the remaining
combined C-VIDC/video/CCPU static boundary to 84 imports, but does not prove
the missing historical `ccpu486.lib` composition, storage lifecycle, or an
NTIO dynamic video dependency. It authorizes no runtime link or shim.
Authority:
`docs/research/softpc-source-matrix/NT4-CVID-PROVIDER-CLOSURE-AUDIT.md`.
The same audit cross-checks the live NTIO/DosInit prefix: it reaches the
`50h/0Fh` `demGetDrives` boundary, not video. The 84 imports therefore do
not admit video/device work; only separately caller-proven
`c_effective_addr` remains a reconstruction candidate, while
`c_sas_touch` and `c_VirtualiseInstruction` remain fail-closed generated
slots.

**NTIO resident-device supplement (2026-08-09):** the fixed source-built
`NTIO.SYS` first `CON` header, strategy, interrupt, `cdev` pointer lifecycle,
and `con_table` dispatch have been crosschecked against source, map, and
image bytes. The first `CON` initialization is a guest-to-guest device-chain
path requiring ordinary real-mode monitor semantics and the original A20
route; it does not authorize a console/device/BOP shim. Authority:
`docs/research/ccpu-bios-closure/S5-NTIO-CON-DEVICE-INIT-TRACE.md`.

**NTIO strategy execution supplement (2026-08-09):** a new default-disabled
fixture accepts a caller-selected source-built `NTIO.SYS`, maps its entire
33,792-byte image at `0070:0000`, and executes the original `strategy` bytes.
It proves the original `ptrsav` writes and deliberately stops before the
original `retf`; the only admitted S4 extensions were the exact
`CS: MOV [disp16],r16` form and the caller-proven, atomic plain `retf` form.
No device, console, BOP, or host behavior was added. Authority:
`docs/research/ccpu-bios-closure/S5-NTIO-STRATEGY-EXECUTION-FIXTURE.md`.

**Real-mode far-return supplement (2026-08-09):** `DEVIOCALL2` proves that
resident-device entry uses `CALL DWORD PTR [CALLDEVAD]`. S4 validates both
return words before committing CS:IP and SP, with a tested atomic memory-fault
outcome. The NTIO image fixture supplies no return frame, so it cannot skip
the original DOS caller or create device success.

**NTDOS image-address supplement (2026-08-09):** the fixed `NTDOS.SYS`
`PARASTART`/`stripz` mapping is now crosschecked through the actual
`DEVIOCALL2` bytes. `NTDOS.MAP` addresses are not final-file offsets; the
temporary DOSCODE/DOSDATA segment relation remains owned by original
`sysinit`. No device-call fixture may prefill `CALLDEVAD` or select the
temporary DOS segment. Authority:
`docs/research/ccpu-bios-closure/S5-NTDOS-IMAGE-ADDRESS-MAP.md`.

**Sysinit temporary-DOS gate (2026-08-09):** the temporary DOS segment cannot
be profile-fixed: original sysinit derives it from its live CS, `SI_end`, and
memory state, then adjusts it using NTDOS `PARASTART`; `DosInit` subsequently
relocates DOS data. A true NTDOS-to-NTIO device-call trace must first capture
that original state rather than entering `DEVIOCALL2` by map address. Authority:
`docs/research/ccpu-bios-closure/S5-SYSINIT-TEMPORARY-DOS-STATE-GATE.md`.

**Sysinit entry supplement (2026-08-09):** the source-built NTIO image now
has a fixed static entry trace from `sysinit` through `goinit`. Its first
runtime boundary is the original `BOP 12h` conventional-memory query, whose
direct historical owner is `BIOS[0x12] -> memory_size`; `MS_bop_0` remains the
separate selector-0 DOS/DEM path. This rules out a DOS/device shim or a
profile-selected prepared entry at this boundary. The static result does not
constitute a live trace and admits no synthetic BOP return. Authority:
`docs/research/ccpu-bios-closure/S5-SYSINIT-ENTRY-STATIC-TRACE.md`.

**CCPU BOP12 dispatch supplement (2026-08-09):** the original CCPU `C4 C4`
decoder is present in `ccpu386/c_main.c`: it consumes the selector byte,
advances IP, then invokes `bop(0x12)`, which the complete original `BIOS[]`
maps to `memory_size`. The next reconstruction unit is therefore the original
CCPU/BIOS/SAS composition and lifecycle, not a replacement BOP decoder or
memory-size handler. Authority:
`docs/research/ccpu-bios-closure/S5-CCPU-BOP12-DISPATCH-CONTRACT.md`.

**CCPU product-composition supplement (2026-08-09):** all 130 ordinary C
sources declared by historical `ccpu386/sources` are present. The remaining
gap is profile/product composition: `obj.vdm` expects `ccpu486.lib` under the
CCPU profile, alongside C-VIDC and the complete host/provider closure, but its
archive/map/generator assembly is not recoverable from the fixed tree. This
blocks clean composition, not a newly discovered missing CCPU instruction
source; no stub or reduced BIOS table is admitted. Authority:
`docs/research/ccpu-bios-closure/S5-CCPU-PRODUCT-COMPOSITION-INVENTORY.md`.

**DemGetDrives seam supplement (2026-08-09):** `DosInit`'s first early DOS
service is now closed to `demGetDrives`' original host algorithm. It writes
whole AX and clears CF while NTDOS consumes AL; A:/B: use the historical OEM
drive-type boundary and must support accessible physical, USB, and virtual
floppy volumes under explicit auto/disabled/host-drive/image-backed policy.
The contract admits no drive shim or service handler until original CCPU/DEM
execution reaches this caller. Authority:
`docs/research/ccpu-bios-closure/S5-DEMGETDRIVES-HOST-SEAM-CONTRACT.md`.

**DemGetDPBList supplement (2026-08-09):** the subsequent original DOS call
is now closed to its NTDOS-owned `ES:BP` allocation cursor and guest DPB-list
writes. Only removable/fixed classifications from the earlier original drive
scan produce entries; the packed far-pointer chain and returned BP remain
historical DEM/guest behavior, not loader or media-policy work. Authority:
`docs/research/ccpu-bios-closure/S5-DEMGETDPBLIST-GUEST-WRITE-CONTRACT.md`.

**Effective-address crosscheck (2026-08-09):** the sole admitted local
SoftPC semantic recovery, `c_effective_addr`, was rebuilt and re-executed in
the isolated i386 clang-cl/Ninja island after adding bounded assertions for
descriptor-limit separation and 32-bit linear-address wrap. Its manifest entry
matches the fixture bytes. The target remains `EXCLUDE_FROM_ALL`; root CTest
contains no OpenNT/SoftPC target. This confirms an address-formation contract,
not a complete CCPU, memory-access, or runtime admission. Authority:
`docs/research/softpc-source-matrix/NT4-CCPU-EFFECTIVE-ADDRESS-ORACLE-CROSSCHECK.md`.

**Historical library-input supplement (2026-08-09):** a recursive audit of
the four locally available historical source trees found no recoverable
CCPU/C-VIDC composite archive in the MVDM/SoftPC-adjacent static libraries.
The non-COFF archives are DOS/WOW16/toolchain inputs rather than candidates
for the missing 32-bit software-monitor product. This does not close the
historical-input search beyond local scope and authorizes no local-composition
shortcut. Authority:
`docs/research/softpc-source-matrix/NT4-SOFTPC-LIBRARY-ARTIFACT-AUDIT.md`.

**Monitor/host ABI correction (2026-08-09):** the existing v2 helper obtains
register state and its service-byte aperture from a CCPU probe profile. That
is not a source proof that NT4 x86 `MS_bop_0` requires CCPU ownership. The
original selector-50 entry itself owns the service-byte fetch, `DemDispatch`,
IP consumption and idle decision. Any future reconstructed-monitor facade may
only expose checked real-mode state and ordinary-RAM translation before one
original `BIOS[selector]` call; it may not decode a service, dispatch DEM,
advance IP, emulate V86, or solve the complete-table static closure. Authority:
`docs/research/ccpu-bios-closure/S5-MONITOR-HISTORICAL-HOST-ABI-CUT.md`.

The NT4 `host/src/sources` coverage audit is
`docs/research/ccpu-bios-closure/S5-HOST-SRC-COVERAGE-AUDIT.md`: nine
previously unrepresented historical source units now compile in two
default-disabled object inventories, while `fprt.c` remains source-only until
a matching historical CRT header/toolchain is recovered.
S5 independently reconfirms the already-recorded first post-load NTDOS
boundary: `C4 C4 50 0F` dispatches `SVC_DEMGETDRIVES`, rather than the
earlier date-only fixture service. Its current historical owner-closure intake
is `docs/research/ccpu-bios-closure/S5-NTDOS-FIRST-POSTLOAD-SVC.md`.
The following fixed `DosInit` sequence is now recorded through the first
COMMAND BOP and DPB creation, including `50h/1Bh`, `50h/32h`, `54h/05h`, and
`50h/46h`; authority:
`docs/research/ccpu-bios-closure/S5-NTDOS-DOSINIT-EARLY-SVC-LEDGER.md`.
The `54h/05h` owner boundary is now independently audited: it retains original
`BIOS[54h] -> MS_bop_4 -> CmdDispatch -> cmdSetInfo`, consumes only the three
original `GetVDMAddr` guest pointers, and introduces no host shim. Its complete
COMMAND dispatch table remains a later owner closure rather than a candidate
for local service-table reduction; authority:
`docs/research/ccpu-bios-closure/S5-COMMAND-BOP-OWNER-BOUNDARY.md`.
The paired `50h/0Fh` discovery and `50h/46h` DPB cache-consumption contract
is separately recorded in
`docs/research/ccpu-bios-closure/S5-DRIVE-SESSION-OWNER-CONTRACT.md`.
The reconstructed monitor now replays the fixed NTDOS `DosInit` prefix through
its first real `C4 C4 50 0F` request, preserving the original DOS-data copy and
BIOS communication-block read, then records a typed bridge refusal before any
`demGetDrives` service executes. This is CPU/memory mechanics evidence only;
authority: `docs/research/ccpu-bios-closure/S5-NTDOS-DOSINIT-PREFIX-EXECUTION.md`.
Its bounded bridge return contract now permits only `AX/CX/DX/CS/IP` and a
validated single `CF` bit; invalid responses commit no partial state. The
historical `demGetDrives` return shape is fixture evidence only because S4 RAM
and the historical SAS aperture are still distinct. Authority:
`docs/research/ccpu-bios-closure/S5-S4-BRIDGE-RETURN-CONTRACT.md` and
`docs/research/ccpu-bios-closure/S5-S4-SAS-APERTURE-AUDIT.md`.
The original `oemuni` source owner is now in the default-disabled no-SM0
closure diagnostics, resolving 18 OEM facade imports without a local
replacement; its remaining native/modern seam boundary is recorded in
`docs/research/ccpu-bios-closure/S5-ORIGINAL-OEM-OWNER-RECOVERY.md`. The
separate original `nt_mem.c` SAS owner compiles and removes its two allocator
imports from the same diagnostics, exposing `VdmSetPhysRecStructs` as the
private-VDM seam. Its only confirmed caller surface is non-i386 RISC
DIB/WinG mapping, so it is recorded but not scheduled as the NT4 x86 startup
blocker; authority:
`docs/research/ccpu-bios-closure/S5-ORIGINAL-INTEL-MEMORY-OWNER-RECOVERY.md`
and `docs/research/ccpu-bios-closure/S5-VDM-PHYSICAL-RECORD-SEAM-EVIDENCE.md`.

The direct original `demGetDrives` trace was re-built and re-run from the
current source island with exit code 0. It remains a trace-only oracle because
its fixture retains the archived SM0 lifecycle foundation; the only executed
modern behavior seam in its original drive-classification path is the
documented `RtlProcessHeap -> GetProcessHeap` compatibility entry. Authority:
`docs/research/ccpu-bios-closure/S5-DEMGETDRIVES-DYNAMIC-HOST-SEAMS.md`.

**Latest S5 evidence:** the exact NTIO prefix reaches `0070:0475 C4 C4 50 11`.
The default-disabled historical fixture completes original `MS_bop_0 ->
DemDispatch -> demLoadDos` after original `DemInit`; the SAS load target begins
`E9 A9 51`, matching BYOB `NTDOS.SYS`. The narrow F1 `CreateFileOem` facade
replaces only the obsolete NT4-private TEB buffer assumption, and the original
`IDLE_disk -> HostIdleNoActivity` owner returns. This is not a guest-execution
claim. Authority: `docs/S5-ORIGINAL-DEMLOADDOS-CLOSURE.md` and
`docs/S5-POST-LOAD-HANDOFF-AUDIT.md`. The CCPU `C4 C4` return contract is
source-derived, but its full-BIOS dynamic fixture currently has unresolved
historical owner imports; authority: `docs/S5-RETURN-TO-MONITOR-CONTRACT.md`.

The first post-load historical `50h/0Fh` DEM caller is now independently
executed by a default-disabled i386 fixture: original `BIOS[50h] -> MS_bop_0 ->
DemDispatch -> demGetDrives` returns `IP=0479`, `AX=nDrives=5`, `CF=0`, and
writes its original 26-slot `PhysicalDriveTypes` cache. The only newly proven
modern seams are x86 NTAPI declarations for the original RTL conversion calls
and a `GetProcessHeap()` definition for removed `RtlProcessHeap`; original drive
classification, cache and later DPB ownership remain intact. This is historical
host closure evidence, not a drive policy or runtime claim. Authority:
`docs/research/ccpu-bios-closure/S5-DEMGETDRIVES-DYNAMIC-HOST-SEAMS.md`.

The immediately following fixed `50h/1Bh` service is also independently
executed through original `BIOS[50h] -> MS_bop_0 -> DemDispatch ->
demSetDTALocation`. It advances IP to `0479` and retains the original DTA,
PDB, extended-error and SFT pointers in the same CCPU/SAS aperture. It exposes
no modern host seam and supplies no DOS-data substitute. Authority:
`docs/research/ccpu-bios-closure/S5-DEMSETDTA-GUEST-ADDRESS-OWNER.md`.

The next `50h/32h` guest-address retention and `54h/05h` COMMAND address
retention are also independently closed through their original tables. The
former preserves the VHE/device-chain pointers without triggering hard-error
behavior. The latter keeps the complete original `CmdDispatch` table and
recovers `pFDAccess` from its actual `nt_fdisk.c` storage owner; no disk/floppy
operation or drive policy is invoked. These are default-disabled trace facts,
not device or COMMAND runtime claims. Authority:
`docs/research/ccpu-bios-closure/S5-CMDSETINFO-HISTORICAL-STORAGE-CLOSURE.md`.

`50h/46h demGetDPBList` is now independently executed after original `50h/0Fh`
drive discovery in one DEM session, using the already-governed CCPU/SAS
`GetVDMAddr` reconstruction profile. The original owner writes seven 33-byte
DPBs, consumes `BP` to `00f7`, and terminates the final `Next` with `-1` while
the fixture merely supplies the destination aperture and asserts the result.
This does not reconstruct the historical V86 mapping, introduce a low-address
alias, or admit disk/media/drive policy; it is a default-disabled guest-write
trace fact. Authority:
`docs/research/ccpu-bios-closure/S5-DEMGETDPBLIST-CCPU-SAS-CLOSURE.md`.

The first post-`DosInit` command-host boundary is now classified as original
`BIOS[54h] -> MS_bop_4 -> CmdDispatch(0Ch) -> cmdGetConfigSys`. Its historical
owner reads the Windows profile and writes a temporary CONFIG path through
`DS:DX`; direct execution would violate BYOB containment. A future explicit,
session-local configuration projection must preserve the BOP/output/DOS-open
sequence without reading ambient Windows files or prewriting the guest buffer.
It is wrapper/mantle profile policy, not CPU, SoftPC device, or generic core
behavior. Authority:
`docs/research/ccpu-bios-closure/S5-SYSINIT-CONFIG-HOST-SEAM.md`.
The exact 64-byte `sysinitseg:config` return buffer, original `doconf` consumer,
and contained projection admission requirements are now recorded in
`docs/research/ccpu-bios-closure/S5-CONFIG-PROJECTION-ADMISSION-RECORD.md`.

The default-disabled S4 monitor now executes the fixed normal `DosInit` slice
after an already-completed historical `50h/0Fh`: it preserves the real guest
`A2 0046` write, executes `F6 /0` through the checked ordinary-RAM route,
branches on `SCS_ISDEBUG`, and reaches original `50h/1Bh` only to stop on a
bridge refusal. No service result is synthesized. This is CPU/memory evidence,
not completion of either historical service or transport. Authority:
`docs/research/ccpu-bios-closure/S5-NTDOS-POSTDRIVE-MONITOR-SLICE.md`.

**Next guest boundary:** the original near `call charinit` is now the first
unexecuted boundary after the proven `54h/05h` return. `charinit` builds a DOS
device-init packet and invokes the original `DEVIOCALL2`, which calls the
strategy and interrupt addresses in the NTIO/BIOS-supplied guest device header.
This is a prepared-entry/device-chain contract gap, not authorization for a
synthetic CON, keyboard, disk, request-status, or generic host shim. Authority:
`docs/research/ccpu-bios-closure/S5-NTDOS-CHARINIT-DEVICE-BOUNDARY.md`.

The same default-disabled S4 fixture now also executes the exact normal `DosInit`
bytes after a completed historical `54h/05h` through the original near transfer
to `charinit`. It proves only real-mode CPU/ordinary-memory mechanics: restore
of the three caller registers, DOS stack setup, code/data pointer writes,
instance-table segment writes, `initiret` vector storage, and checked atomic
near-call transfer. It neither executes `cmdSetInfo`/`charinit` nor supplies a
service result, BIOS, IVT policy, device, or host seam. Authority:
`docs/research/ccpu-bios-closure/S5-NTDOS-POSTDRIVE-MONITOR-SLICE.md`.

**Profile correction:** NT4 x86 used the V86 `MONITOR` profile. CCPU/C-VIDC is
a non-x86 source-evidence branch, not an x64 runtime composition waiting to be
linked. Its missing composite inputs remain worth recovering, but they no
longer gate the x86 historical-host closure or the reconstructed software-monitor
specification.

**Latest S1 evidence:** `ntdos64-opennt-original-bios50-date-fixture` now
builds as `COFF-i386` and exits `0` after directly invoking original
`BIOS[50h]`. The original entry reaches `MS_bop_0`, reads the service byte from
the historical SAS route, calls original `DemDispatch(0x14)`, and advances IP
from 3 to 4. It does not call `c_cpu_simulate()` or provide a local BIOS/BOP/DEM
handler. The complete BIOS[] table still uses trace-only `/OPT:REF
/FORCE:UNRESOLVED` for uncalled entries, so this is a bounded host-closure fact,
not a full runtime claim. Authority: `docs/S1-HISTORICAL-HOST-CLOSURE.md`.

**Latest S2 evidence:** `ntdos64-opennt-historical-bios-bridge-v1-fixture`
builds as `COFF-i386` and exits `0`. Its fixed-width v1 record rejects an
uninitialized session, bad ABI version and an unsupported selector before
touching historical register state; its only admitted record enters original
`BIOS[50h]` for the existing date trace and returns the post-handler register
snapshot. It remains a default-disabled i386 trace fixture, not an x64
transport, generic selector bridge or runtime host shim. Authority:
`docs/S2-HISTORICAL-BRIDGE-RECORD-CONTRACT.md`.

**Latest S4 evidence:** `refs/archive/reconstruction/monitor` is an explicitly admitted,
default-disabled independent CPU/memory mechanics slice, not an OpenNT overlay
or archived BOP prototype. Its MinGW test exits `0`; its i386 bridge fixture
also exits `0` after `C4 C4 50 14` reaches the original S2/BIOS/DEM date trace
and `D6 FE` terminates the invocation at IP 6. Every terminal stop returns a
redacted in-memory event containing only the pre-stop real-mode `CS:IP`, physical
fetch, up to four opcode bytes and bridge disposition. It has no x64-to-i386 transport,
NTIO boot, device, DOS or host-policy claim. Authority:
`docs/S4-RECONSTRUCTION-ADMISSION.md` and `docs/S4-STOP-EVENT-CONTRACT.md`.

**Governance correction:** Completed on 2026-08-08. OpenNT SoftPC/CCPU repair
is the sole execution route. `ntvdm64`, Bochs, PCjs, WHPX, NTVDMx64 and
core/mantle material are read-only comparison or downstream records only;
they cannot schedule, gate, link, or substitute the current route. Authority:
`docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`; audit:
`docs/governance/softpc-first-governance-correction-2026-08-08.md`.

**M92 governance reconciliation:** Completed. The root CMake graph has been
reduced to the modern CLI and
BYOB admission tests; external-core, WHPX, owned-monitor, handwritten DOS/BOP,
NT4 bootstrap and mantle probe targets/CTest entries are gone. The OpenNT
island no longer compiles an OpenNT-4.5 source. Its manifest verifies all 2841
entries. The physical files, manifest entries and CMake inputs have been
reconciled; every overlay has a recorded classification in
`M92-OPENNT-OVERLAY-AUDIT.md`. `c_effective_addr.c` is the only admitted
reconstruction; `config.c` and `nt_msscs.c` remain unadmitted host-seam
candidates pending separate caller-first runtime admission.

**Condition:** Recover or independently reconstruct the missing SoftPC/CCPU
semantics under the historical source boundary. `c_effective_addr` is already
an admitted, bounded reconstruction; `c_sas_touch` and
`c_VirtualiseInstruction` remain fail-closed until a live caller supplies a
semantic contract. The current live reset boundary is instead the missing
CCPU/C-VIDC composition input: CCPU owns a 5-slot `Video` storage object while
C-VIDC supplies an incompatible 81-slot provider table. Historical build or
object evidence must establish the composition storage and initializer before
any reconstruction is considered. Each reconstruction must be tied to original
callers, source evidence and bounded compatibility tests; it may not be a new
DOS SVC or BOP dispatcher.

**Local restriction:** `ntdos64` may repair the historical SoftPC stack and
add documented modern-host compatibility seams inside this repository. It
must not substitute a new DOS runtime, flatten the original dispatcher chain,
or claim a complete runtime before the reconstructed monitor path is tested.

**Current evidence:** `docs/CCPU-COMPLETION-RECOVERY-AUDIT.md` identifies the
three absent CCPU interfaces and the surrounding original owners. M89 records
the original host event/console boundary. M91 is retained only as a future
`ntvdm64/core` integration comparison; it is not an execution gate for this
repository.

`c_effective_addr` is now a manifest-tracked SoftPC reconstruction overlay and
passes both its ABI fixture and the selector-50 historical-chain fixture.
`c_sas_touch` and `c_VirtualiseInstruction` remain trace-only guards until a
live original caller defines their behavior. Authority:
`docs/SOFTPC-CCPU-MISSING-INTERFACES.md`.

**Latest R2 SAS evidence:** the default-disabled
`original-sas-lifecycle-trace-fixture` builds and runs as `COFF-i386` with
exit `0`. It proves only the original `ccpusas4.c -> sim32.c + nt_mem.c`
allocation/teardown path, 2 MiB ordinary SAS RAM access, and no unadmitted
callback. It enters no CPU, BIOS, BOP, DEM, device, or DOS execution path.
Its retained unresolved descriptor/private-VDM imports are uncalled object
closure diagnostics under `/FORCE:UNRESOLVED`, not new shims or a runtime
admission. Authority: `docs/R2-SAS-RESET-PROFILE-AUDIT.md`.

**Latest R1 composition evidence:** COFF inspection of the currently built
original `ccpu386/ntstubs.c` places `_Video` at `0x288` and the next data
symbol at `0x29c`, proving 20-byte short-ABI storage. The original C-VIDC
archive provides `_C_Video` but `ev_glue.c` retains an unresolved external
`_Video`, which requires separate 81-slot storage/initialization. The full
ABI cannot be copied into the CCPU object; conversely, the CCPU short-table
initialization remains independently unknown. Across local NT4, NT4.5 and XP
source snapshots, the relevant headers and build descriptors are identical,
yet all omit `ccpu486.lib`, its producer rule and `host/genPg`. The reset
fixture that exposed the null short-table call is deliberately
non-`C_VID`: its actual built overlay has `CPU_40_STYLE` and `CCPU`, while the
original video-provider objects additionally have `C_VID`. It is a mixed
portable-CCPU probe, not proof of the historical product composition. The
active task is therefore to compare original C-VID product-side and internal
CCPU call paths, then do source/build recovery or prepare a separately
reviewed composition specification. It is not a host shim or external
execution backend. Authority:
`docs/R1-CCPU-CVIDC-ABI-MATRIX.md`.

**R1 retained evidence:** static caller analysis finds no actual caller of the
five CCPU short-table macros in the fixed CCPU C/H inputs. In the original
`CPU_40_STYLE + C_VID` product profile,
`ega_prts.c::init_vga_globals -> SetMarkPointers(0)` is macro-rewritten to the
named `ev_glue.c::setMarkPointers(0)` call; that function internally needs
the complete C-VIDC `Video` slots `SetVideomark_byte`, `SetVideomark_word`,
and `SetVideomark_string` (58--60). R1 therefore first recovers or proves the
complete-table storage/initializer owner; short-table work is deferred until a
real original caller appears. Authority:
`docs/R1-CCPU-CVIDC-COMPOSITION-RECOVERY-SPEC.md`.

**R1 candidate withdrawal:** the former default-disabled 0--77 `C_Video`
provider mapping and 78--80 named-function tail mapping were removed from
overlay, CMake, and the source manifest. Product-side C_VID callers use named
`set*Pointers` functions, not the full table tail; neither mapping had evidence
for a historical initializer or lifecycle owner. This withdrawal does not solve
the internal `ev_glue.c` reference to complete `Video` storage.

**R1 external-reference boundary:** local NTVDMx64 patches confirm that a
later compatibility project solved the same full-`Video`/short-`Video`
collision by defining an initialized full `Video` in `vglfunc.c` and removing
the short definition. This is recorded solely as a candidate semantic and is
not NT4 historical-build proof or an implementation admission. Authority:
`docs/R1-CVIDC-REFERENCE-COMPARISON.md`.

**R1 composite-library evidence:** the original CCPU product link selects one
missing `ccpu486.lib`, not independent `ccpu386` and `Cvidc` libraries.
`vglob.c` is absent from the retained CCPU `sources` list, confirming that the
missing composite boundary contains more than the two independently buildable
source sets. Current combined-object fixtures are therefore diagnostics only,
not historical composition proof. Authority:
`docs/R1-CCPU-CVIDC-COMPOSITION-RECOVERY-SPEC.md`.

**R1 official-binary scope:** the local OpenNT-4.5 i386 `ntvdm.exe` retains
COFF symbols for the x86 BOP/DEM/host path, but no CCPU or C-VIDC symbols; its
product profile uses the V86 monitor. It is a host oracle only and cannot
recover the non-i386 `ccpu486.lib` composition. Authority:
`docs/R1-CVIDC-REFERENCE-COMPARISON.md`.

**Latest R1 function-boundary result:** a `/Gy` COFF inspection proves that
the original `ev_glue.c::setMarkPointers(0)` itself needs only `Video`,
`cevid_verbose`/`printf`, and original mark-provider data; it does not reach
`Gdp`, `EGA_CPU`, video-memory, BIOS character-write, or host-display state.
However, current LLD still requires full-translation-unit undefined closure
when that original object is linked, even with LTO enabled. The installed LLVM
does not provide `llvm-extract`, so no clean, reproducible single-function
artifact fixture is available. The attempted fixture/target were removed; this
is a recorded negative result, not a reason to add a local video or BIOS shim.
The historical complete `Video` storage/initializer and its lifecycle owner
remain unresolved. Authority: `docs/R1-CVIDC-VIDEO-STARTUP-CLOSURE.md`.

**Current R2 priority:** the self-owned, default-disabled R2 monitor has a
bounded C11 fixture for original D6 and C4 BOP instruction decoding, IP
consumption, `FE` invocation exit, unbound-table failure, and a NOP-to-BOP
execution slice. Its `BIOS[]` adapter is synthetic and same-address-space
only; it does not link historical OpenNT objects or prove the original BIOS,
MS_bop_0, DemDispatch, or DEM chain. The next admission requirement is a
versioned fixed-width bridge contract that preserves historical table
invocation within its own compatible ABI domain. Authority:
`docs/R2-RECONSTRUCTED-MONITOR-CONTRACT.md` and
`docs/R2-HISTORICAL-BRIDGE-BOUNDARY.md`.

**Historical-side R2 baseline:** a fresh Ninja/clang-cl i686 build now
reproduces and runs `ntdos64-opennt-original-selector50-trace-fixture` with
exit code 0; its PE is `COFF-i386`. The fixture retains original CCPU,
complete BIOS table, `MS_bop_0`, and DEM objects for the bounded selector-50
date-service trace. It still depends on the archived SM-0 foundation, so it
is evidence of the historical-side call chain only, not a runtime admission
or a cross-bitness bridge. The next implementation task is a versioned,
fixed-width bridge record with no archived adapter in its normal path.
The concrete ownership and negative-case gates are recorded in
`docs/R2-HISTORICAL-SELECTOR-BRIDGE-ADMISSION.md`.

**SAS/reset profile audit:** OpenNT's `ccpusas4.c` provides a concrete SAS
init/term and ROM ordering oracle, but its two `sim32.c` allocators belong to
the historical x86 MONITOR and non-x86 CPU_40_STYLE profiles respectively.
Neither is an x64 runtime drop-in. A fresh full original-owner link reaches
the expected missing CCPU vectors, C-VIDC generated vectors, and NT VDM
private-host surface; this is evidence for incremental caller-first recovery,
not a license for a broad shim. Authority:
`docs/R2-SAS-RESET-PROFILE-AUDIT.md`.

**Latest live-closure evidence:** a default-disabled startup-boundary fixture
reaches original `gfi_init()` after its original empty-floppy owner
`gfi_empty_active()` is linked from `gfi_mpty.c`. Two narrower fixtures prove
that `gfi_init()` returns both through a recording seam and through the
original NT `config.c::host_runtime_set()` owner. After the custom trace
harness title buffers were initialized, its former stage-2 wait became a
deterministic access violation in the harness prelude/`setjmp` path. The full
startup object set can call only `gfi_init()` and return. This is therefore not
a GFI/config host-state gap and not a reason to adopt an external CPU/core
backend; the harness is trace-only and no longer an admission gate. Authority:
`docs/SOFTPC-STARTUP-HOST-CLOSURE.md`.

**Original entry evidence:** the new default-disabled
`ntdos64-opennt-host-main-prefix-trace-fixture` links original
`base/support/main.c` and exits `0` after observing its first historical call
to `host_applInit`. It verifies the original `pargc`/`pargv` assignment and
does not replay or skip the original startup sequence. The existing original
`host_applInit` checkpoint-17 fixture also exits `0`. This joins the real
process entry to the established host-lifecycle prefix, but does not establish
UI, `gfi_init`, `config`, CPU, BIOS, BOP, DEM, or guest execution. Authority:
`docs/HOST-MAIN-PREFIX-TRACE.md`.

**Main-prefix continuation:** a default-disabled original `main.c` overlay
reaches post-`host_applInit` (60), pre-/post-`gfi_init` (61/62), and the
post-`init_virtual_drivers()` checkpoint (63). `verbose` is closed by its
original `base/support/xt.c` owner; the mouse initialization uses original
`mouse_io.c`, `virtual.c`, and the already retained original `nt_unix.c`
`host_memset` owner. Its complete bounded trace, including all HERC instance
field writes, returns successfully. No mouse, keyboard, input queue, media, or
console substitute has been admitted. In this full `main.c` composition, the
next call boundary at `config()` raises `0xC0000005` before checkpoint 64.
That result is confined to the composition fixture: the separate original
`config()` prefix fixture reaches original `sas_term()` and the SAS/ROM
extension reaches original `InitNtCpuInfo`. The active blocker is therefore
the non-x86 generated debugger-register bridge at `InitNtCpuInfo`, while the
composition-fixture fault remains a trace-harness integration investigation;
neither result is evidence of a usable configuration or guest runtime.
Authorities: `docs/HOST-MAIN-PREFIX-TRACE.md` and
`docs/R3-CONFIG-PREFIX-TRACE-LEDGER.md`.

**Original entry composition:** the default-disabled
`ntdos64-opennt-host-main-applinit-trace-fixture` runs original `host_main`
into the checkpointed original `host_applInit`; checkpoints 10 and 17 both
exit `0`. No local `host_applInit` implementation is present in this target.
This establishes the original process-entry composition; the later bounded
UI and event-thread-prefix observations are recorded immediately below. It
does not establish event-thread lifecycle, `gfi_init`, `config`, CPU, BIOS,
BOP, DEM, or guest execution. Authority: `docs/HOST-MAIN-PREFIX-TRACE.md`.

**Current asynchronous evidence:** the same composition reaches original
`nt_start_event_thread()` through its entry, `CreateMutex`,
`SetConsoleCtrlHandler`, suspended `CreateThread`, event/queue initialization,
and its return to `host_applInit` (checkpoints 40--50 and 31), each with exit
`0`. The initial stop after `CreateThread` was a missing original-data-owner
closure: `ThreadInfo` is defined by historical `host/src/nt_timer.c`; linking
that existing object resolves the state without calling timer initialization
or adding a local replacement. NT4 and NT4.5 retain the same unannotated
callback spellings, and their historical `sources` files provide no `/Gz`
evidence; modern x86 calling-convention warnings are evidence to preserve and
investigate, not permission to alter the historical callback ABI. No event
loop, keyboard, or console-event replacement has been admitted. Authority:
`docs/HOST-MAIN-PREFIX-TRACE.md`.

**Interpretation rule:** Completed M57-M80 entries retained below are
historical evidence records, not current execution authority where they name a
"governed backend", a core adoption path, or CCPU as oracle-only. Their active
interpretation is superseded by `docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md` and
the Current Task above.

## Newly Completed Task

**ID:** M91-NTVDM64-CURRENT-BACKEND-GATE-REAUDIT

**Status:** Completed as a read-only current-source recheck.

**Result:** Current M5 PC/AT controller migration does not add configured
decode observation or a paused ordinary-RAM multi-span transaction. This is a
future `ntvdm64/core` integration comparison only; it does not block the
historical SoftPC/CCPU reconstruction path in `ntdos64`. Authority:
`docs/M91-NTVDM64-CURRENT-BACKEND-GATE-REAUDIT.md`.

## Newly Completed Task

**ID:** M90-FROZEN-NVTDM-REFERENCE-RECONCILIATION

**Status:** Completed as a build-graph and documentation correction.

**Result:** `refs/archive/legacy-adapters/nvtdm.c` is reconfirmed to contain handwritten staging,
interpreter and DOS-service probes, and remains only a source listing in the
explicitly disabled `ntvdm-startup-reference` custom target. The default
MinGW build graph and CTest registry contain no `nvtdm` executable or source
reference. Legacy command spelling is now documented as retired, rather than
as a current engine capability. Authorities: `docs/ENGINE-CONTRACT.md` and
`docs/DEFAULT-BUILD-INSTRUMENTATION-ISOLATION.md`.

## Newly Completed Task

**ID:** M89-HISTORICAL-HOST-EVENT-THREAD-BOUNDARY-AUDIT

**Status:** Completed as a source-level host event/input boundary audit.

**Result:** The original event-thread owner compiles in the default-disabled
historical island. Its mutex, suspended-thread lifecycle, console-control
handler, synchronization events, key history and BIOS-keyboard transport are
now classified as one historical host state machine. A modern shim must offer
capabilities at that boundary; it may not replace the queue or enter a normal
execution path. The host lifecycle fixture now exposes a post-call checkpoint
while the original event thread remains suspended; interactive-console runtime
behavior remains unproven.
Authority: `docs/HOST-EVENT-THREAD-BOUNDARY.md`.

## Newly Completed Task

**ID:** M88-CURRENT-CORE-ARCHIVE-PROBE-REVERIFICATION

**Status:** Completed as a read-only same-toolchain archive-probe recheck.

**Result:** The current external core archives link and run the existing
bounded #UD/entry-plan software-machine probe, while a separately rebuilt
normal runner remains independent. This is not M70 admission. Authority:
`docs/M88-CURRENT-CORE-ARCHIVE-PROBE-REVERIFICATION.md`.

## Newly Completed Task

**ID:** M87-NTVDMX64-HAXM-ADAPTER-BOUNDARY-AUDIT

**Status:** Completed as a source-level host-adapter boundary audit.

**Result:** The staged HAXM monitor is confirmed as a historical modern-host
observation, not a portable backend or mantle input. Its lifecycle/event
taxonomy is usable only as design evidence; HAX IOCTLs, VDM_TIB/TEB state,
raw mappings and accelerator exit semantics are explicitly excluded. Authority:
`docs/M87-NTVDMX64-HAXM-ADAPTER-BOUNDARY-AUDIT.md`.

## Newly Completed Task

**ID:** M86-CORE-SECOND-CONSUMER-CANDIDATE-AUDIT

**Status:** Completed as a read-only second-consumer audit.

**Result:** Current NXVM firmware/reset, debugger, entry-plan and mantle-shape
test do not independently require configured decode observation or a paused
ordinary-RAM multi-span transaction. The M70/M79 gate remains closed without
an artificial core request. Authority:
`docs/M86-CORE-SECOND-CONSUMER-CANDIDATE-AUDIT.md`.

## Newly Completed Task

**ID:** M85-GUEST-CLOSURE-GOVERNANCE-RECONCILIATION

**Status:** Completed as a current-document reconciliation.

**Result:** The active artifact matrix and campaign/work-order documents now
distinguish the completed OpenNT-tools16 guest source closure from still-gated
runtime validation, and distinguish the 75-row whole-system descriptor census
from the 33-file direct DOS closure. Authority:
`docs/M85-GUEST-CLOSURE-GOVERNANCE-RECONCILIATION.md`.

## Newly Completed Task

**ID:** M84-REDACTED-TRACE-ENVELOPE

**Status:** Completed as a fixture-level, copied-value diagnostic contract.

**Result:** A bounded modern trace envelope now records only finite event
classes, result classes, byte counts and short identity prefixes. It is
unlinked from the runner and all historical islands; its focused test covers
copying, validation, bounded retention and reset. Authority:
`docs/M84-REDACTED-TRACE-ENVELOPE.md`.

## Newly Completed Task

**ID:** M83-NT4-ENUS-FIRST-PROFILE-CONTRACT

**Status:** Completed as the version-locked, non-implementation adapter
contract.

**Result:** BYOB identity, copied session policy, early service ownership,
guest-state discipline, core admission, deferred features, and bounded
acceptance evidence now have one authority. Authority:
`docs/M83-NT4-ENUS-FIRST-PROFILE-CONTRACT.md`.

## Newly Completed Task

**ID:** M82-NT4-EARLY-SERVICE-ABI-DETAILS

**Status:** Completed as a source-level early-service ABI ledger.

**Result:** The command BOP transport, one-based boot-drive result, full
DTA/DOS-data registration, packed DPB field writes, and packed SCSINFO
registration are now fixed without adding a new core gap. Authority:
`docs/M82-NT4-EARLY-SERVICE-ABI-DETAILS.md`.

## Newly Completed Task

**ID:** M81-OPENNT-SOURCE-SCOPE-RECONCILIATION

**Status:** Completed as a local source-root and inventory-tool audit.

**Result:** The staged historical input is proven to be a 79-descriptor,
43-declaration subset of the 194/75 full reference census. The inventory tool
now normalizes input roots correctly; reference-only sources may not enter a
future build without an explicit manifest-backed import. Authority:
`docs/M81-OPENNT-SOURCE-SCOPE-RECONCILIATION.md`.

## Newly Completed Task

**ID:** M80-NT4-STARTUP-SERVICE-CLOSURE

**Status:** Completed as a static source-reachable bootstrap closure.

**Result:** The NTIO-to-COMMAND startup spine now identifies the ordered
DEM/CMD services, distinguishes debug/WOW/console branches, and confirms that
DPB/environment/image publication reuses the two M70 backend needs rather
than requiring a third core extension. Authority:
`docs/M80-NT4-STARTUP-SERVICE-CLOSURE.md`.

## Newly Completed Task

**ID:** M79-NTVDM64-CURRENT-BACKEND-GATE-REAUDIT

**Status:** Completed as a read-only current-source verification.

**Result:** The current core retains useful foundations, including frozen
real-mode `#UD` transitions and stopped entry-plan preloads, but neither is
the generic configured-decode observation or paused ordinary-RAM transaction
formerly proposed for external wrapper integration. This is future comparison
evidence only and does not gate SoftPC reconstruction. Authority:
`docs/M79-NTVDM64-CURRENT-BACKEND-GATE-REAUDIT.md`.

## Newly Completed Task

**ID:** M78-RUNNER-BYOB-HANDOFF-POLICY

**Status:** Completed as a tested modern CLI boundary improvement.

**Result:** `ntdos64-run` now validates an explicit BYOB profile/root before
launching an external DOS/NE engine, passes that same validated profile/root
to the engine before the target separator, preserves direct PE handoff, and
remains free of trace/oracle execution code. Synthetic transport and
no-guest-input regression checks are part of the 13-case CTest suite.
Authority:
`docs/M78-RUNNER-BYOB-HANDOFF-POLICY.md`.

## Newly Completed Task

**ID:** M77-HISTORICAL-HOST-CLOSURE-EVIDENCE-AUDIT

**Status:** Completed as a host object-closure and runtime-boundary audit.

**Result:** Fixed historical inputs compile into the isolated provider archive
and support bounded BIOS/BOP/DEM oracle traces, but do not form an x86 host
executable. The former M57 V86/backend conclusion is retained as historical
source evidence only. Current execution instead repairs the CCPU/SoftPC
software-monitor path without kernel V86 or an external backend; see
`docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`. Authority:
`docs/M77-HISTORICAL-HOST-CLOSURE-EVIDENCE-AUDIT.md`.

## Newly Completed Task

**ID:** M76-NT4-GUEST-CLOSURE-EVIDENCE-AUDIT

**Status:** Completed as a current guest artifact and provenance reconciliation.

**Result:** The fixed direct DOS closure is verified as 22 unchanged-source
builds, seven static companions, and four explicitly non-source-built
`binplace` inputs; five WOW16 device drivers are independently source-built.
The wider 75-output MVDM inventory remains out of scope for this closure.
Authority: `docs/M76-NT4-GUEST-CLOSURE-EVIDENCE-AUDIT.md`.

## Newly Completed Task

**ID:** M75-NT4-EXECUTABLE-PREFIX-PREREQUISITES

**Status:** Completed as a source-to-execution prerequisite synthesis.

**Result:** The NTIO pre-entry ordering, virtual-interrupt reservation, image
publication boundaries, device/profile facts, and bounded stop conditions are
recorded. The former conclusion that CCPU was not an execution prerequisite is
superseded: the active route reconstructs the OpenNT SoftPC/CCPU monitor while
excluding only the unavailable kernel V86 route. Authority:
`docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`.

## Newly Completed Task

**ID:** M74-NT4-BOOT-INPUT-DISPLAY-CLOCK-GATING

**Status:** Completed as a source-derived EN-US device-gating record.

**Result:** NTIO installs keyboard/video-related guest state and NTDOS requires
the `CLOCK$` device-chain structure, but neither fact proves first `/C` use of
live input, visible display, PIT/IRQ, CMOS, or RTC. The historical Taiwan-only
temporary-vector path is excluded from EN-US evidence. Authority:
`docs/M74-NT4-BOOT-INPUT-DISPLAY-CLOCK-GATING.md`.

## Newly Completed Task

**ID:** M73-NT4-A20-XMS-HMA-FALLBACK-LEDGER

**Status:** Completed as a source-derived HMA branch and ownership record.

**Result:** `LoadDOSHiOrLo` is proven to fall back to low memory on every
failed HMA/XMS/A20 probe. A20 remains generic core machine semantics, whereas
XMS discovery, allocation, UMB policy, and HMA-selection are profile-capability
policy. This adds no core gap beyond M70. Authority:
`docs/M73-NT4-A20-XMS-HMA-FALLBACK-LEDGER.md`.

## Newly Completed Task

**ID:** M72-NT4-BOOT-MEMORY-BOP-ABI-LEDGER

**Status:** Completed as a source-derived early-memory service record.

**Result:** The NT4 startup's BOP `12h` conventional-memory and BOP `15h` /
`AH=88h` extended-memory calls are traced to the original BIOS table and
handlers. The fixed startup uses AX only, with no carry check on the second
call; it can historically receive zero extended memory. The result confirms
profile-owned memory answers and does not admit CMOS, RTC, or controller
extraction. Authority: `docs/M72-NT4-BOOT-MEMORY-BOP-ABI-LEDGER.md`.

## Newly Completed Task

**ID:** M60-NT4-BOP-SVC-DISPATCH-LEDGER

**Status:** Completed as a source-derived dispatch index and transport record.

**Result:** The original `BOP_DOS` transport sequence and all `49h` DEM SVC
table entries are indexed from the fixed source. The record distinguishes
delegated historical owners from original placeholders; it does not implement
or authorize a synthetic dispatcher. Authority:
`docs/M60-NT4-BOP-SVC-DISPATCH-LEDGER.md`.

## Newly Completed Task

**ID:** M61-DEMLOADDOS-BOOTSTRAP-ABI-LEDGER

**Status:** Completed as a source-derived bootstrap service record.

**Result:** The NTIO-originated `DI:0000` NTDOS load contract, historical
EOF-driven direct-write behavior, host path dependency, and terminate-on-error
policy are separated from the future contained backend/wrapper requirements.
Authority: `docs/M61-DEMLOADDOS-BOOTSTRAP-ABI-LEDGER.md`.

## Newly Completed Task

**ID:** M62-DEM-FILE-GUEST-BUFFER-ABI-LEDGER

**Status:** Completed as a source-derived local-file service record.

**Result:** The original open/read/write register ABI, raw handle exposure,
directional guest-buffer behavior, pipe exceptions, and DOS-result dependency
are recorded without adding a replacement service implementation. Authority:
`docs/M62-DEM-FILE-GUEST-BUFFER-ABI-LEDGER.md`.

## Newly Completed Task

**ID:** M63-DEM-DRIVE-CURRENT-DIRECTORY-ABI-LEDGER

**Status:** Completed as a source-derived namespace state record.

**Result:** Original drive-count semantics, CDS validation/reset behavior,
and ambient host current-directory/`=X:` environment coupling are separated
from the future contained per-session namespace contract. Authority:
`docs/M63-DEM-DRIVE-CURRENT-DIRECTORY-ABI-LEDGER.md`.

## Newly Completed Task

**ID:** M64-DEM-ERROR-HARD-RETRY-ABI-LEDGER

**Status:** Completed as a source-derived error and retry state record.

**Result:** The VHE registration, normal versus INT-24 hard-error split, and
register-plus-service retry snapshot are documented as session state, not as a
generic platform error callback. Authority:
`docs/M64-DEM-ERROR-HARD-RETRY-ABI-LEDGER.md`.

## Newly Completed Task

**ID:** M65-DEM-DEBUG-STREAM-EXIT-ABI-LEDGER

**Status:** Completed as a source-derived debug-channel and termination record.

**Result:** `SVC_OUTPUT_STRING`/`INPUT_STRING` are debugger-only channels, and
the actual `demExitVDM` ignores its stale message-pointer comment before
entering historical product shutdown. Neither is promoted to the contained
CLI boundary. Authority: `docs/M65-DEM-DEBUG-STREAM-EXIT-ABI-LEDGER.md`.

## Newly Completed Task

**ID:** M66-DEM-COMMAND-EXIT-AND-WIN32-HANDOFF-LEDGER

**Status:** Completed as a source-derived normal-exit and host-handoff record.

**Result:** PDB cleanup, COMMAND DX/CF/AL exit behavior, and the historical
direct Win32 process route are separated from BaseSrv, VDD, event-thread, and
ambient-handle product integration. Authority:
`docs/M66-DEM-COMMAND-EXIT-AND-WIN32-HANDOFF-LEDGER.md`.

## Newly Completed Task

**ID:** M67-COMMAND-BOOTSTRAP-ENVIRONMENT-STREAM-ABI-LEDGER

**Status:** Completed as a source-derived COMMAND bootstrap record.

**Result:** COMMAND's BaseSrv broker, raw guest-pointer caching, ambient
environment conversion, and raw host standard-handle exposure are separated
from the minimum contained CLI input and stream contract. Authority:
`docs/M67-COMMAND-BOOTSTRAP-ENVIRONMENT-STREAM-ABI-LEDGER.md`.

## Newly Completed Task

**ID:** M68-GUEST-COMMAND-CMDINFO-BOOTSTRAP-ABI-LEDGER

**Status:** Completed as a version-locked guest/host command ABI record.

**Result:** The packed `CMDINFO`/`SCSINFO` structures and permanent COMMAND's
COMSPEC, environment-capacity, and next-command call order are recorded
without turning their historical broker fields into a generic runtime API.
Authority: `docs/M68-GUEST-COMMAND-CMDINFO-BOOTSTRAP-ABI-LEDGER.md`.

## Newly Completed Task

**ID:** M69-NT4-ENUS-BOOTSTRAP-CAPABILITY-MATRIX

**Status:** Completed as a cross-ledger executable-admission synthesis.

**Result:** The fixed-profile operation categories are mapped to existing
foundations, former backend-gap hypotheses, and wrapper-only policy. The
matrix introduces no local executor or replacement dispatcher; its former
backend admission language is superseded by the SoftPC reconstruction
governance. Authority:
`docs/M69-NT4-ENUS-BOOTSTRAP-CAPABILITY-MATRIX.md`.

## Newly Completed Task

**ID:** M70-NEUTRAL-BACKEND-CAPABILITY-HANDOFF

**Status:** Completed as an external core-owner acceptance package.

**Result:** The two consumer-gated neutral capability candidates now have
bounded contracts, required test evidence, ABI prohibitions, and a return
admission checklist. Authority:
`docs/M70-NEUTRAL-BACKEND-CAPABILITY-HANDOFF.md`.

## Newly Completed Task

**ID:** M71-NT4-BOOT-MINIMUM-MACHINE-MATRIX

**Status:** Completed as a source-derived staged device classification.

**Result:** Startup references confirm generic low-memory/A20/memory-service
needs while leaving XMS, input/display, timer, controllers, and optional
devices trace-gated. No premature VM-to-core controller move is admitted.
Authority: `docs/M71-NT4-BOOT-MINIMUM-MACHINE-MATRIX.md`.

## Newly Completed Task

**ID:** M59-HISTORICAL-PROVIDER-ARCHIVE-PURITY

**Status:** Completed as an isolated x86 source-object rebuild and build-graph
isolation correction.

**Result:** The fixed historical provider archive rebuilt successfully after
the frozen CCPU register bridge was removed from its general host-stack target;
a second fresh clang-cl x86/NMake configuration also rebuilt it and its archive
response contains no bridge reference. The two archives are not byte-identical,
so this is configuration-level object-closure evidence only, not an executable
runtime or bit-for-bit reproduction.
Authority: `docs/M59-HISTORICAL-PROVIDER-ARCHIVE-PURITY.md`.

## Newly Completed Task

**ID:** M58-GOVERNED-BACKEND-READINESS-REAUDIT

**Status:** Completed as a current-source reconciliation.

**Result:** Existing prepared entry, frozen memory/ROM/A20, lifecycle, CPU,
and device foundations remain usable. Configured decode observation and paused
atomic multi-span ordinary-RAM publication remain absent and consumer-gated.
Authority: `docs/M58-GOVERNED-BACKEND-READINESS-REAUDIT.md`.

## Newly Completed Task

**ID:** M57-HISTORICAL-PROFILE-BOUNDARY-CORRECTION

**Status:** Completed as an architecture correction.

**Result:** `InitNtCpuInfo` is a non-x86 cross-profile fixture stop, not a
missing NT4 x86 input. The historical x86 profile uses V86; CCPU remains an
oracle for the x64 governed-backend route. Authority:
`docs/M57-HISTORICAL-PROFILE-BOUNDARY-CORRECTION.md`.

## Newly Completed Task

**ID:** M56-BYOB-VALIDATOR-REGRESSION-EXPANSION

**Status:** Completed as a deterministic fixture expansion.

**Result:** The default-disabled MinGW test now covers the manifest size bound
and missing/zero-length component objects in addition to M52's result-family
fixtures. It rebuilt and passed on 2026-08-08. Authority:
`docs/M51-BYOB-PROFILE-VALIDATOR-IMPLEMENTATION-AUDIT.md`.

## Newly Completed Task

**ID:** M55-EXTERNAL-BACKEND-HANDOFF-PACKAGE

**Status:** Completed as a neutral prerequisite handoff.

**Result:** The package isolates configured decode observation and paused
ordinary-memory transactions behind an independent-consumer gate, then an
opaque copied-value ABI and owned fixtures. Authority:
`docs/M55-EXTERNAL-BACKEND-HANDOFF-PACKAGE.md`.

## Newly Completed Task

**ID:** M54-CONTAINED-PROFILE-FIXTURE-IMPLEMENTATION-AUDIT

**Status:** Completed as a negative local-implementation audit.

**Result:** No M41-M46 behavior can be honestly implemented without the
missing neutral transition/transaction backend contracts or original DEM
reachability. Frozen local rewrite/namespace code remains non-runtime
reference only. Authority:
`docs/M54-CONTAINED-PROFILE-FIXTURE-IMPLEMENTATION-AUDIT.md`.

## Newly Completed Task

**ID:** M53-HISTORICAL-GENERATED-ABI-RECOVERY-AUDIT

**Status:** Completed as an independent missing-input reconfirmation.

**Result:** The apparent `GDP_PTR` / constraint dependency belongs to a
non-x86 cross-profile fixture, not the selected NT4 x86 runtime profile.
XP/CVIDC and MIPS/PPC lookalikes are incompatible and cannot be combined.
Authority:
`docs/M53-HISTORICAL-GENERATED-ABI-RECOVERY-AUDIT.md`.

## Newly Completed Task

**ID:** M52-BYOB-PROFILE-VALIDATOR-FIXTURE-CLOSURE

**Status:** Completed as a default-disabled MinGW fixture expansion.

**Result:** Owned fixtures now cover each public validator result family and
zero a failed selection. `byob-profile-test` rebuilt and passed on 2026-08-08;
it remains absent from `ntdos64-run`. Authority:
`docs/M51-BYOB-PROFILE-VALIDATOR-IMPLEMENTATION-AUDIT.md`.

## Newly Completed Task

**ID:** M51-BYOB-PROFILE-VALIDATOR-IMPLEMENTATION-AUDIT

**Status:** Completed as a modern-runner boundary and coverage audit.

**Result:** The validator is already a bounded, source-isolated metadata gate,
but its fixture coverage is incomplete. M52 may expand only owned tests and
correct discovered contract failures. Authority:
`docs/M51-BYOB-PROFILE-VALIDATOR-IMPLEMENTATION-AUDIT.md`.

## Newly Completed Task

**ID:** M50-HISTORICAL-HOST-PREFIX-RECONCILIATION

**Status:** Completed as a source-backed fail-closed boundary record.

**Result:** The apparent `config.c -> InitNtCpuInfo` prefix is a cross-profile
fixture path, not an NT4 x86 runtime prerequisite. The actual NT4 x86 profile
uses V86; no shim or startup bypass is allowed. Authority:
`docs/M50-HISTORICAL-HOST-PREFIX-RECONCILIATION.md`.

## Newly Completed Task

**ID:** M49-CORE-CONSUMER-EVIDENCE-AUDIT

**Status:** Completed as a negative second-consumer audit.

**Result:** Default-PC firmware construction, debugger one-span writes,
session lifecycle control, and host-media atomicity do not establish either
a configured decode transition or paused multi-span ordinary-RAM transaction.
No core extension is requested. Authority:
`docs/CORE-CONSUMER-EVIDENCE-AUDIT.md`.

## Newly Completed Task

**ID:** M48-BACKEND-ADOPTION-READINESS

**Status:** Completed as a current-source backend reconciliation.

**Result:** Entry plan, frozen memory/ROM topology, A20 and constrained `#UD`
responses are usable foundations, but generic decode-point transition and
paused atomic patch transactions remain missing and consumer-gated. Authority:
`docs/BACKEND-ADOPTION-READINESS.md`.

## Newly Completed Task

**ID:** M47-LANE-P-TRACE-FIXTURE-PLAN

**Status:** Completed as a bounded fixture and redaction plan.

**Result:** M41-M46 now have explicit pure, transaction, historical-oracle,
and future-runtime test classes. Integrated runtime fixtures remain forbidden
until the neutral backend contracts exist; frozen instrumentation stays
observation-only. Authority: `docs/LANE-P-TRACE-FIXTURE-PLAN.md`.

## Newly Completed Task

**ID:** M46-CONTAINED-DOS-NAMESPACE-DOSSIER

**Status:** Completed as a source-backed contained namespace dossier.

**Result:** The first profile has one immutable read-only drive and only the
selected system/config/command/target roles. Normal files use the original
four DEM operations through opaque tokens; NTDOS remains `demLoadDos`-only.
Authority: `docs/CONTAINED-DOS-NAMESPACE-DOSSIER.md`.

## Newly Completed Task

**ID:** M45-LANE-P-DIRECTORY-LIFECYCLE-DOSSIERS

**Status:** Completed as three source-derived Lane P lifecycle dossiers.

**Result:** Directory synchronization, console readiness, and one-command
completion have distinct owners. Completion records a zero-extended DOS result
only on the original return-exit service; an unqualified stop cannot be called
a CLI exit result. Authority: `docs/LANE-P-DIRECTORY-LIFECYCLE-DOSSIERS.md`.

## Newly Completed Task

**ID:** M44-LANE-P-FIRST-COMMAND-DOSSIER

**Status:** Completed as a source-derived first-command admission dossier.

**Result:** `CMDGETNEXTCMD` is a 36-byte shared record, not a string API. A
contained first-call overlay may atomically supply one verified `.COM`/`.EXE`
target and tail while leaving PIF, standard-handle pointers, host event loops,
and Win32 handoff unavailable. Authority:
`docs/LANE-P-FIRST-COMMAND-DOSSIER.md`.

## Newly Completed Task

**ID:** M43-LANE-P-CONFIG-AUTOEXEC-DOSSIERS

**Status:** Completed as two source-derived Lane P configuration dossiers.

**Result:** CONFIG and AUTOEXEC require separately declared immutable
resources and bounded OEM paths; both are one transaction to `DS:DX`, then
immediately consumed by original DOS/COMMAND. Empty/missing profile data and
historical PIF/temp-file policy are not silently emulated. Authority:
`docs/LANE-P-CONFIG-AUTOEXEC-DOSSIERS.md`.

## Newly Completed Task

**ID:** M42-LANE-P-ENVIRONMENT-DOSSIER

**Status:** Completed as a source-derived Lane P environment dossier.

**Result:** The environment image is immutable OEM profile data prefixed by
the selected COMSPEC. Capacity retry changes only BX; a successful path uses
one atomic ordinary-RAM transaction; host environment access is forbidden.
Authority: `docs/LANE-P-ENVIRONMENT-DOSSIER.md`.

## Newly Completed Task

**ID:** M41-LANE-P-SETINFO-COMSPEC-DOSSIERS

**Status:** Completed as two source-derived Lane P dossiers.

**Result:** Set-info uses opaque validated bindings with no guest mutation;
COMSPEC uses a bounded copied path, profile equality, and AL-only terminal
result. Both remain default-disabled design work pending M36/M37. Authority:
`docs/LANE-P-SETINFO-COMSPEC-DOSSIERS.md`.

## Newly Completed Task

**ID:** M40-LANE-P-SERVICE-DOSSIER-SCHEMA

**Status:** Completed as a reusable overlay-admission schema.

**Result:** Any Lane P overlay must now declare exact source ownership,
profile inputs, transaction effects, failures, trace identity, isolation, and
fixtures before code can be proposed. Authority:
`docs/LANE-P-SERVICE-DOSSIER-SCHEMA.md`.

## Newly Completed Task

**ID:** M39-HISTORICAL-FIDELITY-OVERLAY-DECISION

**Status:** Completed as a binding two-lane architecture decision.

**Result:** Exact host-operation and memory-transport bridges may support the
historical reference lane. Profile-specific service-body overlays are permitted
only in a clearly labeled contained adapter lane, never as evidence that the
historical host stack is restored. Authority:
`docs/HISTORICAL-FIDELITY-OVERLAY-DECISION.md`.

## Newly Completed Task

**ID:** M38-HISTORICAL-FIRST-CUT-OVERLAY-GRANULARITY-AUDIT

**Status:** Completed as a first-path obsolete-operation map.

**Result:** Only console acknowledgement and stop are narrow lifecycle seams.
Command, configuration, environment, keyboard, directory, and completion
services require explicit adapter body-overlay admission because their
historical bodies embed excluded product policy. Authority:
`docs/HISTORICAL-FIRST-CUT-OVERLAY-GRANULARITY-AUDIT.md`.

## Newly Completed Task

**ID:** M37-CORE-DELTA-HANDOFF-RECORD

**Status:** Completed as a consumer-gated neutral capability handoff.

**Result:** Both candidate capabilities have explicit invariants, prohibited
surface, future test evidence, and a second-consumer gate. The external
adapter does not implement or approximate them. Authority:
`docs/CORE-DELTA-HANDOFF-RECORD.md`.

## Newly Completed Task

**ID:** M36-CORE-BACKEND-ADOPTION-GATE-AUDIT

**Status:** Completed as a current-interface comparison.

**Result:** Existing entry plan, checked memory, frozen topology, and
constrained `#UD` response are adequate foundations, but neither a general
decode-point transition nor multi-span runtime transaction is present. The
external adapter prefix remains blocked without a local-executor workaround.
Authority: `docs/CORE-BACKEND-ADOPTION-GATE-AUDIT.md`.

## Newly Completed Task

**ID:** M35-NEUTRAL-CORE-MANTLE-DELTA-SYNTHESIS

**Status:** Completed as a neutral ownership and adoption-gate record.

**Result:** Existing core physical memory, entry plan, frozen mappings, and
`#UD` transition remain useful but insufficient for a historical adapter. M35
places multi-span commit and a bounded general transition context behind an
explicit second-consumer/adoption gate; DOS data and policy stay outside core
and mantle. Authority: `docs/NEUTRAL-CORE-MANTLE-DELTA-SYNTHESIS.md`.

## Newly Completed Task

**ID:** M34-HISTORICAL-COMMAND-OBJECT-BOUNDARY-AUDIT

**Status:** Completed as a preserved-source and COFF-import audit.

**Result:** All eleven original COMMAND units rebuild in the fixed
default-disabled x86 gate. `CmdDispatch` retains all seventeen selectors; its
full table cannot be replaced or pruned. The audited imports distinguish future
backend/mantle contracts from deferred console/PIF/stream/native-exec policy.
Authority: `docs/HISTORICAL-COMMAND-OBJECT-BOUNDARY-AUDIT.md`.

## Newly Completed Task

**ID:** M33-COMMAND-GUEST-RECORD-TRANSACTION-DESIGN

**Status:** Completed as a backend-neutral record-transaction contract.

**Result:** `CMDSETINFO`, profile paths, initial environment, and
`CMDGETNEXTCMD` require copied multi-span staging, full ordinary-RAM
preflight, deterministic overlap handling, atomic publication, and
transition-epoch invalidation. DOS packed structures remain adapter data;
core/mantle has no command-specific API. Authority:
`docs/COMMAND-GUEST-RECORD-TRANSACTION-DESIGN.md`.

## Newly Completed Task

**ID:** M32-COMMAND-SERVICE-FIRST-CUT-ADMISSION

**Status:** Completed as a source-derived command ABI and policy record.

**Result:** The command path needs more than `CMDGETNEXTCMD`: checked SCS
bindings, contained current-directory synchronization, terminal acknowledgement,
and explicit no-KEYB policy join the config/environment/command/completion
surface. Native handoff, raw stream handles, PIF, and old console threads stay
deferred. Authority: `docs/COMMAND-SERVICE-FIRST-CUT-ADMISSION.md`.

## Newly Completed Task

**ID:** M31-HISTORICAL-DEMLOADDOS-RESOURCE-BRIDGE-DESIGN

**Status:** Completed as a source-derived bridge design.

**Result:** NTDOS is supplied only on original `SVC_DEMLOADDOS`, using live DI,
transient verified BYOB bytes, and a transactional guest write. System
directory fallback and initial preloading are expressly rejected; failure keeps
the original terminate-VDM outcome. Authority:
`docs/HISTORICAL-DEMLOADDOS-RESOURCE-BRIDGE-DESIGN.md`.

## Newly Completed Task

**ID:** M30-BYOB-BOOTSTRAP-RESOURCE-OWNERSHIP

**Status:** Completed as a BYOB role/loader ownership record.

**Result:** NTIO alone is an entry preload. Original `demLoadDos` owns NTDOS
loading on live `DI`, while original NTDOS/DEM owns COMMAND open/EXEC. Earlier
NTDOS preload fixtures are explicitly trace-only. Authority:
`docs/BYOB-BOOTSTRAP-RESOURCE-OWNERSHIP.md`.

## Newly Completed Task

**ID:** M29-DEM-READ-GUEST-MEMORY-BRIDGE-ADMISSION

**Status:** Completed as a backend-neutral contract admission.

**Result:** `demRead` requires trap-scoped host staging, complete ordinary-RAM
preflight, and all-or-none commit through the checked machine route. It is a
future neutral core/mantle requirement, not permission to expose raw guest
memory or build another executor here. Authority:
`docs/DEM-READ-GUEST-MEMORY-BRIDGE-ADMISSION.md`.

## Newly Completed Task

**ID:** M28-HISTORICAL-DEM-TOKEN-BRIDGE-DESIGN

**Status:** Completed as a source-derived bridge design.

**Result:** Four original DEM functions can consume M27 tokens only by
replacing their direct host-operation calls. A fake `HANDLE` is rejected
because original hard-error logic queries native handle metadata. The design
keeps DEM dispatch, SFT/PSP/EXEC, and original error ownership intact.
Authority: `docs/HISTORICAL-DEM-TOKEN-BRIDGE-DESIGN.md`.

## Newly Completed Task

**ID:** M27-CONTAINED-DEM-FILE-TOKEN-TABLE

**Status:** Completed as a default-disabled modern-host fixture.

**Result:** The token table proves that an x64 native file object can stay
private while the historical 32-bit `AX:BP` transport carries only opaque,
per-session values. It is read-only and contained, and is not linked into the
runner or historical DEM objects. Authority:
`docs/CONTAINED-DEM-FILE-TOKEN-TABLE.md`.

## Newly Completed Task

**ID:** M26-HISTORICAL-DEM-FILE-SHIM-ADMISSION

**Status:** Completed as a source/build admission record.

**Result:** Original DEM keeps DOS file-service ownership, but its `AX:BP`
HANDLE transport cannot survive x64. The only admitted repair is a contained,
per-session opaque 32-bit token table at the original host-operation boundary;
the existing source overlays are compilation repairs rather than runtime
semantics. Authority: `docs/HISTORICAL-DEM-FILE-SHIM-ADMISSION.md`.

## Newly Completed Task

**ID:** M25-DOS-FILE-OPEN-EXEC-CONTAINMENT-LEDGER

**Status:** Completed as a source-derived containment record.

**Result:** The first command requires NTDOS's original path/SFT/PSP/arena and
EXEC behavior plus DEM open/read/seek/close. A contained 32-bit opaque token
bridge, rather than raw host handles or controller emulation, is the earliest
modern host seam. Authority: `docs/DOS-FILE-OPEN-EXEC-CONTAINMENT-LEDGER.md`.

## Newly Completed Task

**ID:** M24-CONTAINED-NAMESPACE-AND-COMMAND-SERVICE-LEDGER

**Status:** Completed as a source-derived ABI and ownership record.

**Result:** The first command uses explicit guest path buffers, paragraph-size
environment negotiation, a packed CMDINFO record, profile-local current
directory data, and bounded completion. Raw Win32 standard handles and
native-image execution remain deferred. Authority:
`docs/CONTAINED-NAMESPACE-AND-COMMAND-SERVICE-LEDGER.md`.

## Newly Completed Task

**ID:** M23-COMMAND-ARTIFACT-ROLE-RECONCILIATION

**Status:** Completed as an artifact/source role record.

**Result:** The first command artifact roles close with NTIO.SYS, NTDOS.SYS,
COMMAND.COM, and a profile-selected owned DOS command. The broader 75-entry
inventory is not an unfinished DOS guest count; configuration, locale,
peripheral, DPMI, development, and WOW artifacts remain on-demand. Authority:
`docs/COMMAND-ARTIFACT-ROLE-RECONCILIATION.md`.

## Newly Completed Task

**ID:** M22-SYSINIT-COMMAND-HANDOFF-DEPENDENCY-LEDGER

**Status:** Completed as an ordered static dependency record.

**Result:** The first command path requires a profile-owned configuration,
namespace, environment, command and stream policy while retaining DOS-owned
open/EXEC semantics. Keyboard, mouse, EMS, XMS/HMA, config device/install,
WOW, and native process handoff are not bootstrap prerequisites. Authority:
`docs/SYSINIT-COMMAND-HANDOFF-DEPENDENCY-LEDGER.md`.

## Newly Completed Task

**ID:** M21-DOSINIT-POST-DPB-DEPENDENCY-LEDGER

**Status:** Completed as an ordered static dependency record.

**Result:** The pre-DPB registrations are typed guest-address bindings, the
DPB transaction is M20, and the rest of `DosInit` is DOS-owned low-memory/IVT
setup before returning to BIOS/sysinit. XMS, serial, and hardware-controller
paths remain non-baseline. Authority: `docs/DOSINIT-POST-DPB-DEPENDENCY-LEDGER.md`.

## Newly Completed Task

**ID:** M20-DEMGETDPBLIST-GUEST-MEMORY-CONTRACT

**Status:** Completed as source evidence and neutral transaction requirement.

**Result:** The original service writes a minimal DPB list directly through
`ES:BP` and advances BP. The modern boundary must preflight and atomically
commit the complete ordinary-RAM range plus BP patch; no disk controller is
implied. Authority: `docs/DEMGETDPBLIST-GUEST-MEMORY-CONTRACT.md`.

## Newly Completed Task

**ID:** M19-DEMGETDRIVES-CONTAINED-NAMESPACE-CONTRACT

**Status:** Completed as source evidence and neutral contract.

**Result:** The historical service returns `AX=nDrives`, `CF=0`, and establishes
26 host-side drive-type slots. Its Win32/NT root scanning is excluded from the
modern route; mantle/profile requires one immutable contained namespace record
for both the count and later DPB consumers. Authority:
`docs/DEMGETDRIVES-CONTAINED-NAMESPACE-CONTRACT.md`.

## Newly Completed Task

**ID:** M18-NTDOS-POST-LOAD-FIRST-INSTRUCTION-DEPENDENCY

**Status:** Completed as static source/artifact evidence.

**Result:** NTDOS is copied and invoked through the historical BIOS/NTIO
handoff, with BIOS communication block, device list, and memory-size inputs.
Its first non-debug external request is `SVC_DEMGETDRIVES`, proving that the
next boundary is a contained logical namespace rather than a PC/AT controller.
Authority: `docs/NTDOS-POST-LOAD-FIRST-INSTRUCTION-DEPENDENCY-LEDGER.md`.

## Newly Completed Task

**ID:** M17-NTIO-IMAGE-LAYOUT-PROVENANCE-CORRECTION

**Status:** Completed as source/artifact reconciliation.

**Result:** The fixed artifact is `0x8400` bytes and derives `DI=08B0h` /
physical `0x8B00`; the former `0x83E0/08AEh` pair is explicitly limited to
archived staging or synthetic fixture evidence. The two affected owned
fixtures rebuilt with GCC and passed. Authority:
`docs/NTIO-IMAGE-LAYOUT-PROVENANCE-CORRECTION.md`.

## Newly Completed Task

**ID:** M16-SVC-DEMLOADDOS-HISTORICAL-CONTRACT

**Status:** Completed as static source/artifact evidence.

**Result:** The original path is `BIOS[50h] -> MS_bop_0 -> DemDispatch(11h)
-> demLoadDos`. `MS_bop_0` owns the trailing service byte/IP increment;
`demLoadDos` consumes live `DI` and historically terminates on host file I/O
failure. This proves NTDOS loading is post-entry transition work, not an
initial preload. Authority: `docs/SVC-DEMLOADDOS-HISTORICAL-CONTRACT.md`.

## Newly Completed Task

**ID:** M15-NTIO-FIRST-INSTRUCTION-DEPENDENCY-LEDGER

**Status:** Completed as static artifact/source evidence.

**Result:** The fixed artifact jumps from `0070:0000` to `init` at `0070:0370`,
consumes virtual-interrupt state at `0x714`, consumes the M9 IVT/BDA fields,
and first reaches `C4 C4 50 11` for `SVC_DEMLOADDOS`. No port I/O or BIOS
interrupt occurs before that boundary. Authority:
`docs/NTIO-FIRST-INSTRUCTION-DEPENDENCY-LEDGER.md`.

## Newly Completed Task

**ID:** M14-NTIO-PRE-INSTRUCTION-TRACE-ADMISSION

**Status:** Completed with an explicit trace boundary.

**Result:** Existing historical execution stops before the CCPU monitor ABI,
and neither frozen instrumentation nor current core can observe historical
state without defining it. A neutral future constructed-profile observation
contract is identified; no shim or runtime path was added. Authority:
`docs/NTIO-PRE-INSTRUCTION-TRACE-ADMISSION-REPORT.md`.

## Newly Completed Task

**ID:** M13-NTIO-ENTRY-REGISTER-BASELINE

**Status:** Completed as a source classification gate.

**Result:** `CS:IP=0070:0000` is source-locked; every other entry-plan field
has a concrete CCPU-reset candidate but remains trace-required because the
cold device-reset chain is not yet witnessed. Authority:
`docs/NTIO-ENTRY-REGISTER-BASELINE-LEDGER.md`.

## Newly Completed Task

**ID:** M12-MANTLE-PROFILE-COMPOSITION-ADMISSION

**Status:** Completed as a contract and evidence gate.

**Result:** Mantle may compose exactly five source-evidenced IVT/BDA fields
and two image preloads into seven ordinary-RAM writes, but it may not create a
core entry plan until the complete NTIO register baseline is proven.
Authority: `docs/MANTLE-PROFILE-COMPOSITION-ADMISSION-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M11-CONSTRUCTED-FIRMWARE-INITIAL-STATE-EXPORT

**Status:** Completed as a contract and evidence gate.

**Result:** The self-owned default PC/AT provider has a bounded,
stopped-boundary snapshot contract for the five M9 inputs. The record forbids
ROM/session/UI export and identifies the current missing neutral exporter as
mantle work, not adapter code. Authority:
`docs/CONSTRUCTED-FIRMWARE-INITIAL-STATE-EXPORT-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M10-FIRMWARE-PROFILE-INPUT-ADMISSION

**Status:** Completed.

**Result:** The self-owned default firmware is a candidate source; the old
OpenNT scaffold is expressly excluded. A generic constructed-state exporter
is required before M9 receives actual values. Authority:
`docs/FIRMWARE-PROFILE-INPUT-ADMISSION-REPORT.md`.

## Newly Completed Task

**ID:** M9-MANTLE-INITIAL-STATE-DESCRIPTOR

**Status:** Completed.

**Result:** Mantle initial-state validation now rejects unproven inputs before
machine construction while preserving legitimate zero-valued vectors and
copied evidence records. Authority:
`docs/MANTLE-INITIAL-STATE-DESCRIPTOR-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M8-NTIO-IVT-BDA-FIRST-LIVE-DEPENDENCY

**Status:** Completed.

**Result:** The first live IVT/BDA inputs are source-enumerated with ownership
and explicit scaffold rejection. No handler or default value was introduced.
Authority: `docs/NTIO-FIRST-LIVE-IVT-BDA-LEDGER.md`.

## Newly Completed Task

**ID:** M7-BOOTSTRAP-TRANSACTION-COMPOSITION

**Status:** Completed.

**Result:** The private NTIO/NTDOS preload transaction is now composed as two
ordered, non-overlapping borrowed views with full capacity revalidation. Its
fixture passed without adding a core, historical, or normal-runtime
dependency. Authority: `docs/NT4-BOOTSTRAP-TRANSACTION-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M6-NTDOS-LOAD-TRANSACTION-PREFLIGHT

**Status:** Completed.

**Result:** A pure `DI:0000` range planner creates one private NTDOS preload
declaration only after complete ordinary-RAM validation. It passed isolated
tests without a historical service or default-runtime dependency. Authority:
`docs/NTDOS-LOAD-TRANSACTION-PREFLIGHT-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M5-NTIO-BOOTSTRAP-PREFLIGHT

**Status:** Completed.

**Result:** A pure private-copy builder now models the original NTIO
virtual-interrupt preservation transaction without overlapping preloads or
guest mutation. It passed isolated tests and has no core/historical/default
CLI dependency. Authority: `docs/NTIO-BOOTSTRAP-PREFLIGHT-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M4-CORE-MANTLE-CONTRACT-DELTA

**Status:** Completed.

**Result:** The C1-C3 generic core changes, mantle boundary, and external
adapter responsibilities are captured in a no-source-change handoff packet.
Authority: `docs/CORE-MANTLE-NTDOS-DELTA-PACKET.md`.

## Newly Completed Task

**ID:** M3-OWNED-EXECUTION-BACKEND-ADMISSION

**Status:** Completed with a core contract delta.

**Result:** The `ntvdm64` software core is the preferred execution candidate;
the external archive probe passed without any source modification. The
remaining transition gaps are owned generic-core work, not wrapper shims.
Authority: `docs/OWNED-MONITOR-M3-EXECUTION-BACKEND-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M2-ORACLE-ADAPTER-ADMISSION

**Status:** Completed.

**Result:** A profile-local, fixed-snapshot date adapter maps one original
OpenNT service result through M1's constrained patch contract. It has no host
clock call, historical object dependency, DOS service table, or normal runtime
linkage. Authority: `docs/OWNED-MONITOR-M2-ORACLE-ADAPTER-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M1-GENERIC-TRANSITION-CONTRACT-DESIGN

**Status:** Completed.

**Result:** The owned M1 registry is finite, configuration-only, prefix-safe,
and immutable after freeze. Its fixture proves physical checked fetch,
constrained real-mode patching, fixed IP consumption, and atomic rejection of
invalid handler output. It has no DOS or historical source dependency.
Authority: `docs/OWNED-MONITOR-M1-WORK-ORDER.md`.

## Newly Completed Task

**ID:** M0-OWNED-USERMODE-MONITOR-CONTRACT

**Status:** Completed.

**Result:** A new, default-disabled M0 target implements zeroed finite RAM,
a shared checked read/write/fetch route, resettable visible real-mode state,
and typed failure results. Its fixture passed under the recorded MinGW-w64
toolchain. The normal CLI link response remains a single `ntdos64_run.c`
object. Authority: `docs/OWNED-MONITOR-M0-WORK-ORDER.md`.

## Newly Completed Task

**ID:** G0.4-FULL-NT4-ARTIFACT-INVENTORY-RECONCILIATION

**Status:** Completed at first-pass reconciliation.

**Result:** The fixed-source 75-row descriptor census is fully classified as
direct guest, VDM host/support, tool, sample/development-only, utility/debug,
or WOW dependency. Indirect guest install ownership resolves the direct NT4
EN-US closure to 33 entries. This is classification and provenance closure;
it does not claim that the blocked historical host products are rebuilt.

## Newly Completed Task

**ID:** R5-ORIGINAL-CHAIN-INITIALIZATION-CLOSURE

**Status:** Completed with a source-recovery boundary, not a restored
historical runtime.

**Result:** The isolated closure compiled the selected original CCPU, BIOS,
`nt_bop`, DEM, and provider object groups. A default-disabled original
`config()` trace reached `InitNtCpuInfo` after original SAS and ROM work.
Cross-tree evidence confirms that its x86 direct-monitor generated ABI
(`GDP_PTR`, generated global layout, and `ConstraintR*`) is absent from the
available NT4, NT4.5, Windows 2000, XP, and imported reference inputs.

**Disposition:** No no-op, cross-profile declaration, or CCPU-local `Gdp`
alias is admitted. The historical chain remains a bounded oracle. Any
execution route beyond this point must be explicitly an owned user-mode
monitor, not a claim of restored historical CCPU composition. Authorities:
`docs/R5-ORIGINAL-CHAIN-REBUILD-2026-08-07.md`,
`docs/INITNTCPUINFO-RECOVERY-DISPOSITION.md`, and
`docs/OWNED-USERMODE-MONITOR-ORACLE-PLAN.md`.

## Completed Task

**ID:** HISTORICAL-GUEST-TOOLCHAIN-REBASE-2026-08-07

**Status:** Completed with documented source-input blockers for `BACKUP.EXE`,
`QBASIC.EXE`, `EDIT.COM`, and `MSHERC.COM`.

**Request:** Rebuild guest artifacts from pristine manifested source using only
the checked-in historical Microsoft tools. Remove prior JWasm/UASM/OpenWatcom,
source-built-MASM, and host-port build paths from the normal build surface.

**Current deliverable:** Preserve verified unchanged-source rebuilds on the
OpenNT tools16 baseline, document the narrow MS-DOS 4.0 C-runtime fallback,
and remove superseded MASM 5.10 evidence without source overlays or substitute
compilers.

**Authority:** `docs/MASM386-FIRST-TOOLCHAIN-RECOVERY-WORK-ORDER.md`.

**Progress:** OpenNT 4.5 tools16 MASM 4.00 is now the default guest
assembler and has rebuilt NTIO, NTDOS, DOSX, COMMAND, device, keyboard,
redirector, utility, and mixed-language SETVER/MEM targets from clean stages.
MS-DOS 4.0 is retained only for its verified `CL.EXE` plus matching 16-bit C
runtime closure required by SETVER/MEM; all their assembly and link work uses
OpenNT tools16. NTDOS and DOSX are no longer build blockers. Evidence:
`docs/HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`.

The direct DOS guest payload/install closure is now reconciled as 33 files:
22 source-built with the historical toolchain, seven static companion/data
files, and four hash-verified original OpenNT `binplace` inputs with no
available source-build rule. `QUIT.COM` is now source-built; the four
prebuilt inputs are explicitly not represented as compiled outputs.

The adjacent WOW16 driver subset is deliberately tracked separately: historical
source rebuilds now exist for `MOUSE.DRV`, `VGA.DRV`, `KEYBOARD.DRV`,
`COMM.DRV`, and `SOUND.DRV`.  They bring the currently verified, controlled
guest/WOW payload count to 38 (27 source-built, seven static, four transparently
staged prebuilt inputs).  This completes the five declared WOW16 device-driver
families. The separate 75-entry MVDM inventory is a whole-system
build-descriptor census that includes host components, tools, samples, and
WOW runtime families; it is not a count of outstanding DOS guest payloads.
Broader WOW16 runtime families remain unprocessed.
The per-artifact scope and stage mapping is recorded in
`docs/NT4-GUEST-ARTIFACT-CLOSURE.md`.

**Cleanup:** Completed for the active build surface. Only local-tool historical
runners and product scripts remain in `toolchain-probe`; Win32 host ports,
external VS/LLVM launchers, source-built-MASM import, generic assembler probes,
and the obsolete DOSBox linker runner were removed. Historical reports are
retained as non-authoritative evidence.

The final generated-directory cleanup is complete. `toolchain-probe` now
contains only the fixed input directories `downloads`, `fixtures`, `reference`,
`third_party`, and `tools`; all 31 retired `*-stage`, CCPU probe, host-inventory,
and upstream-copy output directories were deleted after a no-reference audit.
A subsequent active-script scan found no JWasm, UASM, WLink, OpenWatcom, or
`masm386-managed` invocation.

## Earlier Completed Tasks

### Directory Governance, 2026-08-07

All root-level generated `build*` caches have been moved to
`artifacts/build/`; each migration batch has a retained inventory manifest and
was followed by the default build/test verification. `toolchain-probe/` is now
an input/script boundary, with new runs required to write beneath `artifacts/`.
The support-provider replay records the required x86 MSVC environment, NMake,
`clang-cl`, and `i686-pc-windows-msvc` target. Authority:
`docs/governance/directory-reorganization.md` and
`docs/governance/toolchain-probe-inventory.md`.

The nine legacy root Markdown research reports are retained under
`artifacts/reports/legacy-root-milestones/` with a SHA-256 migration manifest;
the root now contains only project entry points.

**ID:** DOC-GOV-2026-08-07

**Request:** Establish document governance for the NT 4 `ntvdm.exe` research project. Use local governance skills and the read-only `ntvdm64` governance structure as references. Do not write to `ntvdm64`.

**Scope:** Create a documentation entry point, name current authorities, separate current status from research evidence, record historical-source and source-change constraints, and classify existing documents without rewriting legacy research records.

**Acceptance criteria:**

1. Contributors can find purpose, architecture, source policy, workflow, coding rules, status, and document ownership from one entry point.
2. Every governance topic has one named authority.
3. The `ntvdm64` boundary is explicit and read-only.
4. Existing records are mapped instead of silently deleted or reclassified.
5. Links introduced by this task resolve within the repository.

**Result:** Completed. The documentation entry point, authority map, source and coding policies, execution workflow, architecture overview, audit record, and current-status ledger were added. Link verification passed. No file was written to `D:\home\repos.hobby\ntvdm64`.

## Baseline Findings

- Valuable research evidence exists, but policy, roadmap, current task state, toolchain findings, and milestone records were not separated by a clear authority model.
- The root contains both entry-point documents and legacy milestone records, making the reading order difficult to infer.
- This project directory has no Git metadata, so provenance must be recorded in document evidence and source paths rather than assumed from local commits alone.

## Verification Plan

- Inspect newly introduced Markdown links.
- Confirm every authority named in `docs/README.md` exists.
- Confirm no write targets `D:\home\repos.hobby\ntvdm64`.

## Next Queued Task

Classify and cross-link the highest-value existing evidence records without renaming or deleting them. Prioritize historical source gates, toolchain provenance matrix, artifact matrix, ABI research, and ROI roadmap.
