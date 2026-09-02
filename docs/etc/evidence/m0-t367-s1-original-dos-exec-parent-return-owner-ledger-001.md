# M0 T367 S1 — Original DOS EXEC and parent-return owner ledger

Status: closed source/owner ledger; no new product behavior or runtime
observation is claimed by this document.

## Boundary

T366 proved that the original staged `COMMAND.COM` reaches the original DEM
open path.  It did **not** prove that DOS executes the declared image.  The
next owner is the immutable guest DOS kernel, not a new host loader or an
adapter-side process substitute.

The selected guest kernel source is
`src/mvdm-guest/dos/v86/doskrnl/dos/msproc.asm`.  Its `$Exec` procedure begins
at line 283.  It owns the original EXEC parameter block, environment choice,
memory allocation, PSP construction, FCB/command-tail setup, `.COM` and MZ
loading, relocation, initial register/stack setup, and transfer to user code.
The same source owns `$EXIT` at line 1673 and `$ABORT` at line 1705; the latter
restores saved terminate/CTRL-C vectors and transfers to `reset_environment`.

`src/mvdm-guest/dos/v86/doskrnl/dos/handle.asm` is the first host-facing I/O
owner on this path.  Its `$Read` contract is documented at line 373, the
original procedure begins at line 393, and its normal file branch invokes
`SVC_DEMREAD` at line 466.  The corresponding original host
provider remains `src/mvdm-host/dos/dem/demhndl.c:demRead`, selected by the
original `demdisp.c` service table.  T365 already recovered its bounded
guest-buffer/host-file identity binding without changing the original
`ReadFile`, carry-flag, or DOS-error ordering.

The immediate BOP crossings are already source-defined: file open is `50:12`,
normal read is `50:16`, close is `50:02`, and COMMAND's later return-result
consumer is `54:0B`.  `SVC_DEMFASTREAD` (`50:42`) is an optional kernel-era
fast path; its explicit CF-failure route returns unchanged `$Read` to normal
`50:16`, so it is not a new EXEC owner and is not a substitute for this chain.

## Source-shaped lifecycle

```text
Base VDM command record (T366)
  -> original COMMAND uses the staged COMMAND.COM image
  -> guest NTDOS msproc.asm:$Exec
       -> original $Open / JFN / SFT state
       -> handle.asm:$Read -> SVC_DEMREAD -> original demRead
       -> original arena allocation + PSP/environment + FCB/tail
       -> .COM load at Exec_Read_Com / Exec_Read_Block, or MZ load/relocation
       -> Xfer_To_User
  -> guest program terminates by INT 21h / $EXIT / $ABORT
       -> original parent PSP/vector/environment restoration
       -> original COMMAND continuation / 54:0B result path
```

The first source-visible executable load operation is not an external
`CreateProcess` operation.  `$Exec` uses the DOS file/JFN/SFT path and reads the
guest image into guest memory.  Therefore no `app`, `session`, or host adapter
may synthesize a child process, PSP, return frame, or parent restoration.

## Cohort disposition

| Cohort | Original owner | Present disposition | T367 decision |
| --- | --- | --- | --- |
| command record and staged `COMMAND.COM` | BaseClient/BaseSrv + COMMAND | T366 source-shaped route and one observation | input already available; do not alter it |
| JFN/SFT open/read/close | guest `open.asm`/`handle.asm`; host DEM providers | Original source plus T365 bounded host binding | first formal boundary to prove through the real guest path |
| `.COM` image load and transfer | guest `msproc.asm` | Original guest code only; no runtime proof | recover as one original EXEC cohort |
| MZ parse/relocation | guest `msproc.asm` | Original guest code only; no runtime proof | retain in same cohort; do not implement a host-side MZ loader |
| PSP, arena, environment, FCB/tail | guest `msproc.asm`, `alloc.asm`, `handle.asm` | Original guest code only; no runtime proof | prove through the selected `.COM` first; retain MZ source path unchanged |
| `$EXIT`/`$ABORT` parent return | guest `msproc.asm` | Original guest code only; no runtime proof | prove only after guest instruction transfer/termination exists |
| COMMAND `54:0B` result consumption | original COMMAND host package | existing source route; no DOS parent-return proof | consume only the guest-derived return result; no synthetic completion |

## Explicit non-owners

- `app` may publish the already declared initial command record and stage media;
  it does not load or run a DOS child.
- `session` may hold neutral completion state and controlled-stop lifecycle;
  it does not own DOS PSP/JFN/SFT/arena semantics.
- `adapter-mvdm-host-in` routes a mechanical BOP ingress only; it does not
  interpret `$Exec`.
- `adapter-mvdm-host-out` retains the original DEM provider binding and bounded
  guest-memory lease; it does not construct DOS execution state.
- No BOP selector patch, synthetic child loader, changed guest binary, or
  `src.old` input is admitted by S1.

## S2 admission target

S2 must select the **smallest complete original source cohort** that can move
the real guest path beyond T366's successful file-open point.  It must cover
the source-visible JFN/SFT read contract end-to-end (including exact guest
buffer and carry/AX result behavior), rather than add a standalone BOP handler.
It must not claim `.COM` execution, MZ execution, `$EXIT`, `54:0B`, or parent
return until each is separately evidenced.

## Evidence references

- `docs/etc/evidence/m0-t365-s2-dem-com-file-lifecycle-lease-closure-001.md`
  records the original DEM open/read/close provider and bounded lease contract.
- `docs/history/m0-t366-declared-dos-workload-command-delivery-closure-20260902.md`
  records that T366 reached the file-open phase only, with no dem-read sidecar
  nor child/parent completion claim.
- `docs/etc/operations/m0-t367-original-dos-exec-parent-return-001.md` is the
  governing packet plan.
