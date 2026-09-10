# T224 S3 DEM Pipe/SFT Continuation State, ABI And Failure Map

## Result

`50:47` and `50:48` are one paired DEM continuation provider, not two file
services.  In the current CLI profiles no COMMAND pipe/SFT producer exists, so
all four profile dispositions retain the source-derived **absent pipe handle**
terminal.  There is no safe implementation seam until the later complete
`54:08/0A` child-redirection provider owns creation, identity and teardown of
one private pipe session.

## Original Owner Graph

| Layer | Original source | Relevant state/contract |
| --- | --- | --- |
| guest DOS consumer | `v86/doskrnl/dos/handle.asm:537-553`, `765-776` | Places the SFT `sf_NTHandle` in `AX:BP`; `50:47` loops while ZF is set; `50:48` loops while CF is clear. On EOF it copies returned `AX:BP` file size to SFT and clears `sf_nt_pipe_in`. |
| DEM dispatch | `demdisp.c:170-171`, `dossvc.h:164-165` | Selects `demPipeFileDataEOF` / `demPipeFileEOF`. |
| DEM implementation | `demhndl.c:562-605` | Converts the 32-bit guest value with `GETHANDLE`, calls COMMAND helper, and gets real file size with `GetFileSize`. `50:47`: `CF=EOF`, `ZF=!new_data_or_EOF`; `50:48`: `CF=EOF`; EOF returns size in `AX:BP`. |
| COMMAND owner | `command/cmdredir.c:602-656`, `cmd.h:127-164` | Looks up the same real handle in global `cmdPipeList` (`PIPE_INPUT`). State includes read/write/pipe/event/thread handles, filename/buffer, EOF, wait flag and critical section. `50:47` waits up to `PIPE_INPUT_TIMEOUT` on an event; `50:48` observes the same protected EOF flag. |
| producer/cleanup | `cmdredir.c:405-590`, `20-96` | Worker threads publish input/EOF and cleanup closes handles/files/threads. This is coupled to child redirection and cannot be recreated at one DEM selector. |

## Exact Register Outcomes

| identity | original input | original non-EOF | original EOF | required current no-pipe terminal |
| --- | --- | --- | --- | --- |
| `50:47` | `AX:BP` real `HANDLE` | `CF=0`; `ZF=1` when still no data, `ZF=0` when data arrives | `CF=1`, `ZF=0`, `AX:BP=file size` | `CF=1`, `AX=ERROR_INVALID_HANDLE (6)`; no host handle lookup |
| `50:48` | `AX:BP` real `HANDLE` | `CF=0` | `CF=1`, `AX:BP=file size` | `CF=1`, `AX=ERROR_INVALID_HANDLE (6)`; no host handle lookup |

The current adapter `redirector_deferred` route in
`bx_ntvdm_dem_package_session_v1.c` already emits that no-pipe terminal for
both identities.  The readonly namespace failure provider independently keeps
the same terminal because it cannot project an SFT pipe read handle.

## Profile Disposition

| profile | disposition now | reason |
| --- | --- | --- |
| Direct/default | explicit deferred no-pipe terminal | S2 captures only standard streams; it does not create a private COMMAND pipe/SFT session. |
| Readonly | explicit deferred no-pipe terminal | no host-global capture/pipe session is admitted. |
| Overlay | explicit deferred no-pipe terminal | file-overlay state does not supply redirection producer, event or SFT identity. |
| Virtual | explicit deferred no-pipe terminal | virtual namespace/device work cannot substitute a COMMAND child pipe lifecycle. |

## Rejected Implementations

- Passing a raw or truncated Win32 `HANDLE` through `AX:BP`.
- Making `50:47` immediately return EOF, which changes the guest retry and SFT
  transition semantics.
- Adding a worker/event/temporary-file subsystem directly to DEM.
- Implementing one selector without the sibling or without the COMMAND pipe
  creation and cleanup owner.
- Falling through from Overlay, Virtual or Readonly to Direct pipe behavior.

## Admission Decision

S3 is closed as a source/ABI/failure boundary, with **no leaf route change**.
The next T224 owner package is one COMMAND child-execution/redirection
provider for `54:08/0A` plus the shared pipe session lifecycle it necessarily
creates.  Only after that package admits a typed private pipe/SFT identity may
a later paired DEM subpackage bind `50:47/48` to stateful success paths.
`54:07` remains its separate classifier edge.