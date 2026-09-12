# Original-owner replacement audit, initial pass

## Question and method

Which selected COMMAND/EDIT host providers replace existing OpenNT behavior?
Read-only source and build-generator inspection on 2026-09-12, against HEAD
`05f3859da` plus retained WIP. Inspected `base_vdm_local.c`,
`base_vdm_client.c`, `console_compat.c`, `demfastio.c`, mirrored `srvvdm.c`,
and external OpenNT `windows/core/ntcon/server/private.c`. Source research
follows the project source policy. No build, guest run or product change.

## Rechecked initial findings

| Functional unit | Current owner | Original owner | Disposition |
| --- | --- | --- | --- |
| Next-command service | adapter BaseSrv `get_next_command` | `base/win32/server/srvvdm.c:BaseSrvGetNextVDMCommand` | Rehosted queue/ownership/wait/capacity policy; recover original service slice before deleting replacement. CSR transport is a separate excluded dependency. |
| PIF response | adapter BaseSrv `fill_pif_info` | `srvvdm.c:BaseSrvFillPifInfo` | Rehosted response policy, not merely transport; compare field and failure order. |
| First-VDM state | adapter BaseSrv `base_vdm_local_is_first` | `srvvdm.c:BaseSrvIsFirstVDM` | Original global policy replaced by session state; explicitly resolve cardinality before migration. |
| Current directories (set/get grouped) | adapter BaseSrv directory storage | `srvvdm.c:BaseSrvSetVDMCurDirs/BaseSrvGetVDMCurDirs` | Recover original ownership and consumption rules through bounded storage binding. |
| DOS task exit | adapter BaseSrv exit | `srvvdm.c:BaseSrvExitDOSTask` | Original teardown policy partly replaced; enumerate unavailable console/CSR lifecycle operations. |
| Fast read | mirror overlay `dos/dem/demfastio.c` | `base/ntos/vdm/x86/rdwr.c:NTFastDOSIO` | New user-mode seek/read/guest-copy route. Compare finite original slice and original carry/fallback path; not proven corrupting writer. |
| Cursor visibility | Win32 adapter `ShowConsoleCursor` | `windows/core/ntcon/server/private.c:SrvShowConsoleCursor` | Confirmed unequal contract: original mouse-pointer display count versus current text-cursor visibility and fixed 0/-1 result. Not proof of prior false clicks. |

The generator `tools/build/New-T310OriginalSoftpcNinja.ps1` explicitly selects
the BaseSrv adapters, `console_compat.c` and the fast-I/O overlay. Mirror
presence alone does not prove an original function enters the product link.
These seven grouped findings are not seven proved removable implementations:
some contain essential unavailable-interface bindings. No safe deletion-line
total has been established.

## Additional scope, not included in the seven

- `base_vdm_client.c:GetNextVDMCommand` also implements capture/wait/retry;
  compare original `base/win32/client/vdm.c`, not just server routines.
- `ReadConsoleInputExW` filters Alt+Enter key-down but not key-up, and its
  NOREMOVE branch bypasses that filter. This is policy beyond API forwarding;
  compare the original input contract before judging intentional asymmetry.
- CPU40 descriptor transitions, BOP/interrupt return, keyboard, mouse,
  text video, memory and environment services remain incomplete audit areas.
  Include mirror-internal special cases and diagnostics, not just adapters.
- Architecture/source policy now places external kernel VDM in `opennt-host`,
  but `docs/design/CODING.md` and `docs/rules/CODING.md` still retain older
  kernel-vdm placement text. Record this governance inconsistency; no source
  move is justified solely by the directory label.

## T405 overlay follow-up: source contracts, not removal approval

Rechecked against the unchanged product-source baseline on 2026-09-12.
This adds bounded findings to the initial pass; it is not whole-project
audit completion. No product code or build selection changed.

| Overlay unit | Original counterpart | Current finding and remaining proof |
| --- | --- | --- |
| `dos/dem/demfastio.c:demFastRead` | OpenNT `base/ntos/vdm/x86/rdwr.c:NTFastDOSIO` | Confirmed duplicated seek/read service. The original uses trap-frame registers, VDM TIB scratch, native file APIs and pending-I/O completion; the overlay uses CPU accessors, a guest-memory lease and synchronous Win32 file APIs. These are substantial implementation substitutions, not a symbol alias. |
| `dos/dem/demfastio.c:demFastWrite` | Same original handler plus NTDOS slow-write fallback | Carry-only refusal, not a second write implementation. Do not count this as duplicated file-write logic or delete the fallback contract. |
| `softpc.new/base/ccpu386/localfm.c` | Original same-named global carrier; selected `ccpusas4.c` SAS owner | Retains Gdp/Cpu/Video and avoids a second Sas definition, with explicit C-VID type selection. Composition difference; no newly implemented execution algorithm found in this body. |
| `softpc.new/base/ccpu386/sas_overwrite_memory.c` | Original `ntstubs.c:sas_overwrite_memory` | Direct call to `c_sas_overwrite_memory`; extracted wrapper, not a replacement invalidation algorithm. Whole-file restoration must avoid alternate global/provider collisions. |
| `softpc.new/base/cvidc/mvdm_cvidc_vector_binding.c` timing accessors | Original `base/cvidc/accessfn.c` CPU-vector accessors and `base/system/qevnt.c` consumers | Autonomous timing state requires priority review. `GetJumpCalibrateVal` and `GetJumpInitialVal` both return one local value initialized to 100; `SetJumpInitialVal` changes it. Separate original accessor slots do not prove these values interchangeable. No equivalent original backing provider has yet been established. |
| Same C-VID file, vector assignments | Original Cpu/Sas/Video vectors and generated binding inputs | Composition repair distinct from the timing policy. Requires generated-slot audit; neither the whole file nor every binding is established as removable. |
| `softpc.new/host/src/mvdm_ica_eoi_bridge.c` | Original `ica_eoi` PIC leaf | Typed forwarding with a LONG temporary, null guard and rotate=0. No PIC arbitration algorithm is duplicated in this wrapper. Call-site contract still needs review. |

The C-VID header and EOI header supply declarations, not separate functional
findings. Together the rows cover the five overlay C bodies and their two
headers, but do not close their transitive/generated dependencies.

### Fast-read contract details needing end-to-end checks

The original handler sets VDM_IDLEACTIVITY, advances EIP past its kernel BOP,
rejects encoded console/pseudo-standard handles, and converts the file offset
as unsigned. The overlay rejects current standard handles and non-disk files,
passes an explicitly zero high word to SetFilePointer, and delegates BOP
dispatch and guest-memory ownership elsewhere. Do not transplant kernel EIP
advancement into the CPU40 callback: it could double-advance the instruction.
The original rejects a successful seek whose low position equals 0xffffffff;
the overlay accepts that position when GetLastError is NO_ERROR.

The overlay can return carry after a successful ReadFile if lease release
fails. Whether a subsequent slow read can then observe an advanced file
position depends on the caller's ZF/seek behavior and lease failure contract;
this is an investigation target, not a demonstrated DOSX corruption.
Likewise the absent local idle-activity assignment is not proof that the
surrounding dispatch omits it. Original pending-status/EOF handling and these
side effects must be compared before proposing a replacement.

### Reviewer limits

Source bodies and generator binding names were re-read separately from their
divergence comments. The comments are provenance hints, not equivalence
proof. Searching the original ccpu386 and host C sources for the three timing
accessor names produced no backing implementation; that bounded negative
search does not prove one is absent from other generated/source packages.
The timing row is therefore unproven autonomous behavior, not yet a confirmed
duplicate original algorithm. The existing seven grouped replacement
findings must not be inflated by counting this candidate as proven.

## Full-audit completion requirements

For every selected functional unit, record original path/function, current
provider, actual build selection, unavailable outgoing interface, semantic
differences, retain/replace/delete decision and regression workload. Count
functional units separately from files and physical added/deleted lines.
Compare selected upstream against both mirror and overlay; do not label all
adapter lines hacks. This initial pass is not a full COMMAND/EDIT closure
audit and supplies no whole-project total.
