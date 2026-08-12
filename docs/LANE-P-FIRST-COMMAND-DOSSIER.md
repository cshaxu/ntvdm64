# Lane P First Command Dossier

Status: M44 design dossier, 2026-08-08. This is a Lane P-only, first-call
admission record under M40. It does not implement `cmdGetNextCmd`, replace the
original command dispatcher, or admit an arbitrary host-process launcher.

## Decision

An executable first-command cut is admissible only as a fixed contained DOS
launch request. The historical `cmdGetNextCmd` body is not a general command
transport: it invokes `GetNextVDMCommand`, PIF handling, environment and
directory merging, host event threads, console behavior, standard-handle
redirection, and raw 32-bit host pointers. Reusing that body under modern x64
would either recreate excluded historical product policy or carry invalid
pointer-sized state into the guest.

The admitted Lane P body preserves the original selector and guest record
contract, but replaces only the irreducible host policy with one profile-owned
launch request. It is not historical-host restoration and must trace as
`lane_p`.

## Source Evidence

| Fact | Evidence |
| --- | --- |
| `CMDINFO` is `pack(2)` and exactly 36 bytes. | `base/mvdm/inc/cmdsvc.h:96-124`; `base/mvdm/inc/CMDSVC.INC:40-58` |
| COMMAND constructs the record, calls the original selector, retries only when carry reports insufficient environment space, then processes returned redirection and code-page fields. | `dos/v86/cmd/command/tcode.asm:500-700` |
| On a returned `ExecPathSize`, COMMAND treats `EXECPATH` as already fully qualified and dispatches by the returned extension type without DOS PATH search. | `dos/v86/cmd/command/tmisc1.asm:386-440` |
| The historical body reads and writes guest record/buffers through raw `GetVDMAddr`, calls `GetNextVDMCommand`, checks PIF, exports directories, and creates redirection state. | `base/mvdm/dos/command/cmdmisc.c:25-364` |
| The original command buffer is `COMBUFLEN=128`, while its backing `UCOMBUF` is 131 bytes; `EXECPATHLEN=273`. | `dos/v86/cmd/command/comequ.asm:25-27`; `spc.asm:255-284` |

## Record Contract

The adapter copied-reads and copied-writes the complete 36-byte `CMDINFO` at
`DS:DX`, all in the same transition epoch. Its fixed offsets are:

| Offset | Field | First-cut use |
| ---: | --- | --- |
| 0 | `EnvSeg` | validates the already-established M42 environment segment |
| 2 | `EnvSize` | reads current capacity; first cut does not resize it |
| 4 | `CurDrive` | output: profile current drive, zero-based |
| 6 | `NumDrives` | output: declared contained drive count |
| 8 | `CmdLineSeg` | input pointer to `UCOMBUF` |
| 10 | `CmdLineOff` | input pointer to `UCOMBUF` |
| 12 | `CmdLineSize` | input capacity declaration; must be `128` for this guest build |
| 14 | `ReturnCode` | copied for trace only; first-call launch ignores it |
| 16 | `bStdHandles` | output zero: no legacy raw-handle redirection |
| 18 | `pRdrInfo` | output zero: no 32-bit host pointer crosses the boundary |
| 22 | `CodePage` | output: fixed profile code page |
| 24 | `fTSRExit` | must be zero for first call |
| 26 | `fBatStatus` | output zero: no batch source |
| 28 | `ExecPathSeg` | input pointer to 273-byte `EXECPATH` receiver |
| 30 | `ExecPathOff` | input pointer to `EXECPATH` receiver |
| 32 | `ExecPathSize` | input capacity 273; output exact path byte count including NUL |
| 34 | `ExecExtType` | output `4` for `.EXE` or `8` for `.COM` |

The complete record, the 273-byte path receiver, the 130-byte used prefix of
`UCOMBUF`, and the M41 bindings for `SCS_ToSync` and `IsDosBinary` must all be
ordinary RAM at one copied epoch. No persistent host pointer is retained.

## Profile Grammar

The profile declares exactly one first-call request:

```text
target    = canonical absolute OEM DOS path ending in .COM or .EXE
arguments = OEM bytes excluding NUL, CR, and LF
drive     = one declared mounted drive, zero-based
code_page = fixed profile code page
```

`target` uses the LP-03 canonical DOS-path grammar and must resolve to one
read-only contained namespace resource before the service transaction begins.
The profile explicitly rejects `.BAT`, `.PIF`, an unknown extension, host path
syntax, PATH searching, COMSPEC substitution, ambient current-directory
lookup, and any target that is not a declared DOS program role.

The adapter computes an uppercase profile path only under a declared OEM
case-mapping rule. It does not call host `_strupr` on guest memory. The
basename before the final extension, an optional single space plus `arguments`,
then CR/LF/NUL form the COMMAND tail. The resulting count stored in
`UCOMBUF[1]` must be at most 127, exactly matching the historical assertion.

## Atomic First-Call Result

After complete validation, one M33 transaction publishes all of the following:

1. `EXECPATH` receives the uppercase absolute target and its NUL; `ExecPathSize`
   receives its exact byte count including that NUL.
2. `UCOMBUF[1]` receives the command count; `UCOMBUF[2..]` receives basename,
   optional space and arguments, CR/LF/NUL. `UCOMBUF[0]` is preserved because
   this historical path does not assign it.
3. `CurDrive`, `NumDrives`, `CodePage`, `fBatStatus=0`, `bStdHandles=0`,
   `pRdrInfo=0`, and the extension type are written to the record.
4. The M41 `IsDosBinary` byte becomes `1`; M41 `SCS_ToSync` becomes `0xff`.
5. Carry is cleared. No other register, FLAGS bit, segment, CPU mode, event,
   session state, or device state is modified.

The command line does not request an environment retry. A record whose
environment is inconsistent with M42 is a profile/guest-contract failure, not
permission to invoke the original `GetNextVDMCommand` retry path.

## Failure And Isolation

Malformed record fields, bad/mapped/stale destinations, invalid M41 bindings,
invalid target or argument grammar, a target absent from the contained
namespace, unsupported extension, capacity overflow, cancellation, or failed
commit produces the Lane P fault/stop outcome. It writes no record field,
buffer byte, binding byte, register, or flag.

The first cut deliberately excludes `GetNextVDMCommand`, `cmdCheckForPIF`,
`cmdSetDirectories`, `cmdCheckStandardHandles`, legacy `pRdrInfo`,
`cmdGetStdHandle`, printer flushing, event threads, host console/title state,
process creation, batch execution, second command requests, and all Win32
binary handoff. `bStdHandles=0` makes original `tcode.asm` take its `no_rdr`
branch; future CLI stream support needs a separate pointer-free contract.

## Required Fixtures

- valid `.COM` and `.EXE` profile requests write exact path, command-tail,
  record fields, bindings, and carry-clear result in one commit;
- path/argument combinations yielding count 127 are accepted; 128 are
  rejected with no mutation;
- each receiver or binding routed through ROM/provider/hole, or made stale,
  rejects the whole transaction;
- `.BAT`, `.PIF`, unknown extension, path escape, missing namespace resource,
  nonzero `fTSRExit`, malformed capacities, and non-first invocation all fail
  closed;
- no fixture imports `GetNextVDMCommand`, allocates a `PREDIRCOMPLETE_INFO`,
  creates a process/thread/temp file, or consumes an ambient host directory;
- the trace identifies original selector/record layout, profile identity and
  redacted lengths, and carries the `lane_p` marker.

## Deferred

Later command delivery, batch semantics, first-command exit/reentry,
environment growth, current-directory mutation, standard streams, PIF, WIN32
handoff, WOW, directory synchronization, and command error dialogs require
their own dossiers. None may be inferred from this first-call contract.
