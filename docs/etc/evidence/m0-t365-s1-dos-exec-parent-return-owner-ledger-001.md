# M0 T365 S1 — DOS EXEC and parent-return owner ledger

## Question

What is the complete, source-owned immediate path for a low-dependency DOS
`.COM` or MZ `.EXE` execution request, from the original guest `INT 21h`
`AH=4Bh` entry through its host boundaries and back to the original COMMAND
continuation; and which finite source-shaped cohort must be recovered first?

## Inputs

- Selected byte-exact DOS/V86 guest mirror and provenance manifests recorded
  by [`src/mvdm-guest/README.md`](../../../src/mvdm-guest/README.md):
  `dos/v86/doskrnl/dos/{msproc.asm,misc.asm,msctrlc.asm,file.asm,handle.asm}`
  and `dos/v86/cmd/command/{command1.asm,tcode.asm}`.
- Selected original MVDM host mirror and provenance manifests recorded by
  [`src/mvdm-host/README.md`](../../../src/mvdm-host/README.md):
  `dos/dem/{demdisp.c,demfile.c,demhndl.c,demmisc.c,demsrch.c}` and
  `dos/command/{cmddisp.c,cmdexec.c}`.
- Current same-shaped host-resource mapping facade
  `adapter-mvdm-host-out/softpc/mvdm_host_identity.c`, the session guest-memory
  lease and the retained source-shaped guest-location facade.
- T364 closed COMMAND-delivery evidence.  No new product run, guest-media
  change, host mutation or source/ABI change was made for this ledger.

## Original control and data flow

### Entry, classification and common file path

1. Guest `COMMAND.COM` issues the normal DOS `INT 21h AH=4Bh` request.  Its
   resident recovery path in `command1.asm::Ext_Exec/Exec_Ret` preserves the
   DOS carry/error result; a successful child later uses `AH=4Dh` to collect
   the child status before it reloads the transient command body.
2. Guest NTDOS `msproc.asm::$Exec` accepts only function values `0`, `1`, `3`
   and `5`; values `0` (load/execute) and `1` (load only) carry the original
   environment, command-tail and FCB parameter-block shape.
3. `$Exec::get_binary_type` sets the original `EXECOPEN` flag, invokes `$OPEN`,
   rejects devices, reads the header through `$READ`, and chooses `.COM`, MZ,
   or an unknown NE/PE/LE type.  The unknown route is the historical
   `CMDSVC SVC_CMDCHECKBINARY` handoff; it is *not* required by a known
   low-dependency DOS `.COM`/MZ workload and remains outside the selected
   cohort.
4. `$OPEN` reaches `file.asm`, which calls `HRDSVC SVC_DEMOPEN` only for a
   regular file/UNC path; it writes the returned split opaque handle and file
   size into the original SFT.  No native `HANDLE` is guest data.
5. `$READ`, `$CLOSE` and the MZ-only header/relocation seeks remain in the
   original JFN/SFT route.  `handle.asm` selects slow `SVC_DEMREAD` when its
   fast path declines; it calls `SVC_DEMCHGFILEPTR` for a seek that must reach
   the host.  The `.COM` body needs open/read/close.  The MZ body shares those
   edges and additionally requires the original seek/relocation portion.

### Original guest process construction and entry

6. After a successful image load, `$Exec` owns arena allocation, environment
   copy/ownership, `MCB` naming, `PSP` creation, inherited JFN/SFT references,
   FCB and command-tail copy, and the child return vector.  `misc.asm` records
   the parent in `PDB_Parent_PID` and changes `CurrentPDB`; `DOS_DUP` retains
   the original inherited-SFT reference-count rule.
7. `$Exec` emits `SVC_DEMLOADDOSAPPSYM` only under the original debuggee
   condition, then emits `SVC_DEMENTRYDOSAPP` immediately before `retf` to the
   child.  These are debugger/VDD notifications, not executable loading or a
   substitute CPU route.  The original host implementations are
   `demLoadDosAppSym`/`demEntryDosApp`; the normal non-debug/no-user-hook
   profile is an original no-observable-effect route.

### Original termination, parent restoration and COMMAND continuation

8. Guest `$Exit` enters `$Abort`, which saves the termination result and then
   transfers to `msctrlc.asm::reset_environment`.  That original routine:
   restores terminate/CTRL-C vectors; invokes `SVC_PDBTERMINATE`; frees the
   child arena; runs `DOS_ABORT`; restores `CurrentPDB` to
   `PDB_Parent_PID`; restores the parent stack; and returns to the saved guest
   termination address.
9. `SVC_PDBTERMINATE` reaches original `demTerminatePDB`, which invokes the
   original VDD termination hook, SoftPC `HostTerminatePDB` (floppy/fdisk
   owner cleanup), and removes the per-PSP find-search state.  No app-owned
   process lifecycle is permitted here.
10. The original guest COMMAND paths in `tcode.asm` use
    `CMDSVC SVC_RETURNEXITCODE` only for a shell-out/native 32-bit return
    contract.  A normal DOS child returns through the guest PSP/terminate
    address and `command1.asm::Exec_Wait` (`AH=4Dh`); it is therefore not
    correct to substitute `cmdReturnExitCode` for normal DOS EXEC parent
    restoration.

## Immediate host dispositions

| Source edge | Owner and required disposition | Current evidence | S2 relevance |
| --- | --- | --- | --- |
| `SVC_DEMOPEN` | Original `demOpen`; public `CreateFileOem` semantics; host `HANDLE` published only as a session host-resource surrogate in `AX:BP`. | `demfile.c` already calls `mvdm_host_identity_publish_words`; the guest SFT retains only its original 32-bit field. | Required. |
| `SVC_DEMREAD` | Original `demRead`; a bounded writable guest-memory lease around the original `ReadFile` result, then original CF/AX result. | `demhndl.c` still obtains its data pointer through the historical `GetVDMAddr` macro before its legacy flush/free calls. | Required: this is the first unsafe direct-alias seam. |
| `SVC_DEMCLOSE` | Original `demClose`; resolve the session host-resource surrogate, close, then retire its identity. | `demhndl.c` resolves/releases through the shared session host-resource manager. | Required as the same finite file-lifecycle cohort. |
| `SVC_DEMCHGFILEPTR` | Original `demChgFilePtr`; resolve surrogate and preserve seek result. | Same handle facade is already the owner. | Static MZ dependency; not required to establish the first `.COM` workload. |
| `SVC_DEMENTRYDOSAPP`, `SVC_DEMLOADDOSAPPSYM`, `SVC_DEMFREEDOSAPPSYM` | Original DEM/VDD/debugger notification contracts. | Original bodies and host VDD hook chain are linked; their debug/user-hook branches are not a loader dependency. | Verify source disposition only; do not add a VDD/debugger product shell. |
| `SVC_PDBTERMINATE` | Original `demTerminatePDB → HostTerminatePDB`; retain guest PDB scalar and original cleanup ordering. | Original host units exist; SoftPC host source contains floppy/fdisk cleanup. | Required as the parent-return endpoint; no new process manager. |
| `SVC_CMDCHECKBINARY` | Original COMMAND non-DOS-image classifier. | `cmdCheckBinary` is an original COMMAND owner; it is not part of known DOS `.COM`/MZ closure. | Explicitly excluded. |
| `SVC_RETURNEXITCODE` | Original COMMAND shell-out/reentry contract. | T364 records the original source-shaped local Base VDM transport. | Explicitly excluded from ordinary DOS child return. |

## Mapping and ownership conclusion

- Guest real-mode locations (`DS:SI`, `DS:DX`, SFT/PSP/arena fields) remain
  numeric guest addresses and normal guest data.  They are not mapping-manager
  tokens.
- A native host file `HANDLE` is identity, not guest numeric data.  It must
  continue to use the one session-owned host-resource mapping-manager instance
  and the original split `AX:BP` transport.
- A host operation needing guest bytes must acquire a bounded synchronous
  guest-memory lease through the existing `mvdm_guest_location` facade.  It
  cannot retain a `GetVDMAddr` alias, a guest pointer or a lease after the
  original provider returns.
- `$Exec`, PSP/MCB/arena, relocation and parent restoration are guest code;
  they are load-only mirror inputs, never host compile/link inputs and never
  candidates for an app-defined loader.

## Selected S2 cohort

S2 is the complete **original low-dependency `.COM` DEM file-lifecycle and
guest-memory binding cohort**:

- retain the selected guest `$Exec → $OPEN/$READ/$CLOSE → SFT` flow unchanged;
- retain original `demOpen`, `demRead` and `demClose` algorithm/order and the
  current shared host-resource identity transport;
- replace only the direct guest-byte alias at the source-shaped provider seam
  with the existing bounded guest-location lease, through the smallest
  registered mirror overlay hook if more than three new executable lines are
  necessary;
- preserve original DOS error/carry direction for invalid path, mapping-lease,
  read and close failure; and
- exercise one immutable ordinary `.COM` input plus local positive/negative
  provider evidence.  It may not introduce a host executable loader, native
  child, parser, new BOP provider, or guest-media change.

MZ header/relocation remains recorded as the immediate next branch: its host
file dependency is the same cohort plus `demChgFilePtr`; it is not silently
claimed complete by a `.COM` result.

## Confidence and follow-up

Confidence is high for the original owner and control ordering because all
edges above come from selected guest/host definitions and current formal
source-unit manifests.  Runtime behavior is deliberately unclaimed in S1.
S2 must verify the finite `.COM` I/O binding before T365 can advance to formal
link closure and exactly one fixed workload observation.
