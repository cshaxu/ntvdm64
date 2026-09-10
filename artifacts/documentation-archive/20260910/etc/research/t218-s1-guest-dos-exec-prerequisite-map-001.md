# T218 S1 — Guest DOS `$Exec` Structural Prerequisite Map

## Decision

The first-profile DOS `INT 21h, AH=4Bh` path is a **guest NTDOS process
operation**, not an adapter service and not a Bochs feature request. The next
work must preserve the guest-owned loader, MCB, PSP, environment and
real-mode transfer sequence. `bx-vdm` may supply only the already-defined
host boundaries which that guest code reaches.

T217 closes the declared-image loader *handle family*
(`50:12 -> 50:00 -> 50:42 -> 50:16 -> 50:02`) as a focused x64 fixture. It
does not prove a guest EXEC, target instruction execution, child return, or a
COMMAND lifecycle. This map supplies the missing package-level distinction.

## Scope and inputs

- Original guest owner: `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm`.
- Guest teardown owner:
  `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msctrlc.asm`.
- Original DEM order and handlers:
  `src/opennt/base/mvdm/dos/dem/demdisp.c`, `demmisc.c`, and `demsrch.c`.
- BOP identities: `src/opennt/base/mvdm/inc/dossvc.h`, `cmdsvc.h`, and
  `bop.h`.
- Existing disposition baseline: T217 S12--S14 and the
  [T200 S31 host-integrated BOP ledger](t200-s31-host-integrated-bop-provider-audit-ledger-001.md).

This is source/ABI/failure evidence only. It changes neither guest bytes,
host capability policy, Bochs/mantle code, nor a BOP implementation.

## Original control flow

```text
DOS $Exec (AH=4Bh, AL=0/1/3)
  -> get_binary_type: DOS Open / IOCTL / Read [/Seek]
  -> COM or MZ image load: DOS Seek / Read / Close
  -> guest MCB allocation, environment copy, PDB/PSP construction
  -> guest CS:IP and SS:SP construction
  -> DEM 50:36 entry notification
  -> guest far return into child
  -> child INT 21h exit/abort/TSR
  -> guest vector/PDB/environment teardown
  -> DEM 50:3C PDB termination notification
  -> guest return vector to COMMAND
  -> COMMAND 54:11 return-exit-code lifecycle (when COMMAND owns the parent)
```

The fixed-profile path selects a declared DOS COM image. The MZ branch is
nevertheless included because `$Exec` owns it and it shares most of the
normal file/handle contract. NE/PE/LE recognition is not silently a DOS-EXE
fallback: `get_binary_type` returns `AL=FFh`, after which `$Exec` calls
COMMAND `54:07` (`SVC_CMDCHECKBINARY`).

## Structural prerequisite and ownership map

| Stage | Original owner and source | ABI/order/failure rule | Present disposition | Required package result |
| --- | --- | --- | --- | --- |
| Call form | `msproc.asm:$Exec` | `AL=0` load/execute, `1` load only, `3` overlay; invalid `2`/`4` returns `ERROR_INVALID_FUNCTION`. The EXEC parameter block contains environment segment, command-tail pointer and two FCB pointers. | Guest source-built NTDOS only. | Retain as guest behavior; adapter must not decode or implement it. |
| Binary classification | `msproc.asm:get_binary_type`; DOS file/handle layer; DEM `demfile.c`/`demhndl.c` | Set `EXECOPEN`, open, IOCTL, read DOS/MZ header; MZ then optionally seek/read new-header signature. Open/read/device/bad-header failures return through the DOS carry/error path. | T217's shared declared-image loader family is the prerequisite, not proof of all classification branches. | Test COM and MZ paths through the shared DEM file/handle package; do not add a `50:12` leaf. |
| Unknown executable | `msproc.asm:go_checkbin`; `cmdsvc.h:54:07`; COMMAND `cmdexec.c` | Only `AL=FFh` reaches `SVC_CMDCHECKBINARY`; it may update `DS:DX` and fail, including insufficient command-tail room. | Deferred COMMAND executable-classification package. | Keep explicit unavailable/deferred result until that package has source/ABI/failure closure. Do not treat `CreateProcess` as a substitute. |
| COM load | `msproc.asm:Exec_Com_File` through `Exec_Set_PDB` | Allocate the largest usable MCB block, reserve PSP/stack, seek zero, read, require exact size, set `CS=DS=ES=SS`, `IP=100h`, and put a zero return word on the child stack. Allocation/short-read failure follows the guest DOS error path. | Guest-owned; needs ordinary `50:00/16/02` continuity only. | First executable profile should start with a bounded declared COM child, not an adapter COM loader. |
| MZ load and relocation | `msproc.asm:Exec_exe_file` through `exec_do_reloc` | Validate header-derived resident size, allocate MCB, seek/read resident chunks, reject materially short reads, seek/read relocation records and relocate words; then calculate entry CS:IP and SS:SP. | Guest-owned; same normal file/handle provider plus guest RAM is required. | Defer runtime admission until the COM child/termination contract is proved; no new machine feature follows from this source alone. |
| Environment | `msproc.asm:Exec_Check_Environ` | Inherits or uses supplied environment; scans for double-NUL with `CX=8000h` (32 KiB maximum), allocates/copies it, appends word `1` and executable name. Bad scan returns `ERROR_BAD_ENVIRONMENT`; allocation returns `ERROR_NOT_ENOUGH_MEMORY`. | T217 initial COMMAND environment supports initial shell construction, not arbitrary EXEC environments. | Record a 32-KiB guest EXEC environment limit in the first-profile admission; do not infer acceptance from the host-side 65,535-byte capture bound. |
| PSP/PDB and return frame | `msproc.asm:Exec_Build_Header` and `Exec_Set_Return`; guest MCB/PDB sources | Assign MCB owners, duplicate parent PDB, copy FCBs/tail, save parent return CS:IP in PDB and IVT termination location, and set DTA. `AL=1` returns constructed CS:IP and SS:SP without executing. | Guest-owned. | Retain guest source and verify it by guest-visible state only; no host pointer or Bochs CPU structure may cross this boundary. |
| Real-mode transfer | `msproc.asm:Xfer_To_User`; Bochs CPU/RAM and bx-mantle mechanics | `$Exec` publishes entry as `DS:SI`, stack as `AX:DI`, PDB in `DX`, default AX in `BX`; then executes `SVC_DEMENTRYDOSAPP`, changes guest SS:SP/registers and far-returns. | Existing minimal machine owns ordinary real-mode execution. No missing Bochs device, BIOS, PIC or firmware requirement is proven. | Verify a bounded child entry after lifecycle prerequisites; reject Bochs intrusion unless a reached source-owned mechanical requirement proves it. |
| Entry notification | `demmisc.c:demEntryDosApp`; `dossvc.h:50:36` | Reads PDB from DX; after first call only invokes `VDDCreateUserHook`, otherwise returns `VOID`. It has no loader result, memory copy, or launch decision. | Current source-derived `50:36` resume/no-VDD disposition exists, but is endpoint evidence only. | Migrate/retain it within the DEM lifecycle package with an explicit no-VDD profile; never call it EXEC closure. |
| Debug symbol notices | `demmisc.c:demLoadDosAppSym` / `demFreeDosAppSym`; `50:34/35` | Guest calls only under debuggee guards; original path notifies debugger using guest address conversion. | Deferred no-debug profile. | Default profile must retain no-debug disposition; debugger recovery is a separate package. |
| Child teardown | `msproc.asm:$Exit/$Abort`; `msctrlc.asm:reset_environment` | Guest restores parent vectors, environment and process state. `reset_environment` issues `SVC_PDBTERMINATE` with terminating PDB in `BX`. | Guest core is source-built; the host notification is not yet a complete direct-host lifecycle. | Establish a single DEM child/PDB lifecycle that closes child-owned opaque handles/search state and preserves no-VDD behavior. |
| PDB notification | `demsrch.c:demTerminatePDB`; `dossvc.h:50:3C` | Original code notifies VDD, invokes `HostTerminatePDB`, frees the PDB's find-first list, and returns `VOID`. | Current generic/session fallback is not proof that direct host handles and searches are cleaned up. | Required before a normal child-return claim; source-derived seam must own only its session records, not reimplement DOS teardown. |
| COMMAND return | `cmdsvc.h:54:0B`; COMMAND `cmdexec.c` | Reached after guest child teardown returns to a COMMAND parent; historical host code includes directory/redirection/event-thread/next-command machinery. | Deferred COMMAND lifecycle/return package. | Do not synthesize a CLI exit result per BOP. A one-shot profile needs one source-derived COMMAND-return contract; interactive and multi-program remain later packages. |
| Other formats | `get_binary_type`, COMMAND/WOW/DPMI source packages | NE/PE/LE delegates via the unknown-binary path; protected-mode/WOW cannot be made a COM/MZ compatibility fallback. | Deferred. | Preserve explicit profile rejection until their whole owner packages are admitted. |

## Required host/BOP capability set

The table identifies *which existing owner package must eventually be
complete*, rather than authorizing an endpoint patch.

| Capability family | Required for | Original route | Current conclusion |
| --- | --- | --- | --- |
| DEM normal namespace and opaque handle lifecycle | classification and COM/MZ bytes | `50:12`, `50:00`, `50:16`, `50:02` | T217 proves one declared-image family fixture. Direct/readonly/overlay/virtual provider selection and normal target bytes need owner-package proof. |
| DEM entry/PDB lifecycle | enter and leave a child | `50:36`, `50:3C` | Must be a DEM lifecycle package. `50:36` is no-VDD notification; `50:3C` must reconcile host session cleanup with original PDB/find-list semantics. |
| COMMAND executable classification | only NE/PE/LE/unknown branch | `54:07` | Deferred, source-derived COMMAND package; no modern Win32 shortcut is justified. |
| COMMAND return lifecycle | successful child return to COMMAND | `54:0B` | Deferred; separates one-shot normal return from later interactive/multi-program work. |
| Debugger/VDD/WOW/DPMI | guarded or alternate branches | `50:34/35`, VDD hook, `51`, `53` | Explicitly absent/deferred by first profile. Their original branches must fail or remain unreachable according to the named package, never be faked in bx-core. |
| Bochs mechanics | ordinary guest code execution | CPU/RAM/real-mode transfer only | Existing bx-core/bx-mantle owner is sufficient as a structural prerequisite. No new device or Bochs semantic intrusion is admitted by this map. |

## Recovery-ladder disposition

| Rung | Result |
| --- | --- |
| 1 — direct original source | The source-built NTDOS `$Exec`, MCB/PSP/teardown and COM/MZ logic remain the mandatory implementation. DEM and COMMAND translation units are not independently composable in the x64 CLI because they depend on CCPU/SAS guest-address macros, VDD and the NT4 host-product shell. |
| 2 — smallest adapter/shim | Retain the existing fixed-width guest-memory/CPU transaction seam and recover the DEM lifecycle and COMMAND return contracts from their named original sources. Any rehost must preserve service ordering, void-vs-result status, PDB/handle ownership and DOS failure mappings. |
| 3 — external intrusion | Rejected. The map finds no missing Bochs mechanics; a lifecycle or host-composition gap is not authority to change bx-core. |
| 4 — new behavior | Rejected. No adapter EXEC, COM/MZ loader, synthetic PSP/MCB, host-process launch, arbitrary target dispatcher or leaf BOP handler is justified. |

## Admission consequence

The smallest coherent behavior package after T218 is **first-profile guest
child lifecycle**, not a trace-selected BOP:

1. source/ABI/failure-map the complete DEM `50:36/3C` lifecycle together
   with session-owned opaque handle/search cleanup;
2. source/ABI/failure-map the one-shot COMMAND `54:0B` parent-return contract;
3. implement the two packages only through their original provider structure
   and the shared host-mutation/drive-view ABI; and
4. after their focused family regressions pass, run one bounded native trace
   to observe the composed guest COM child entry and normal return.

This ordering is deliberately not an instruction to implement `50:36`,
`50:3C`, or `54:0B` individually. Each is a member of its owner package.
COM is the first bounded runtime candidate; MZ, unknown/32-bit, WOW,
interactive COMMAND and guest multi-program execution remain separate
admissions.

## Evidence procedure and limits

The source files above were read directly in this worktree. No build, probe,
native trace, host mutation, external import, or code change was performed.
The conclusions about current code are bounded to the cited current session
and T200/T217 evidence; they do not claim that a current no-op, typed resume,
or mapped/deferred ingress is a complete historical provider.

