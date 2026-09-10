# Historical DEMLOADDOS Resource Bridge Design

## Historical Contract

`demmisc.c:152-207` defines the original NTDOS transition:

```text
NTIO executes SVC_DEMLOADDOS
  -> MS_bop_0 consumes the service byte
  -> DemDispatch(0x11)
  -> demLoadDos
  -> live DI supplies segment, offset is zero
  -> NTDOS bytes are copied there
  -> failure terminates the VDM
```

`demLoadDos` returns no DOS success register value.  Its documented failure
behavior is `TerminateVDM()`, not `demClientError`.  The bridge must preserve
that lifecycle distinction.

## Admitted Modern Replacement

The future bridge is a named, default-disabled overlay of the one original
`demLoadDos` function.  It preserves the function signature, the original
dispatcher table entry, and the live `getDI()` input.  It replaces only these
obsolete host operations:

| Historical operation | Modern equivalent |
| --- | --- |
| `GetSystemDirectory` / mutable `pszDefaultDOSDirectory` | immutable selected `ntdos` BYOB descriptor and root-local resource request |
| `CreateFileOem("...\\ntdos.sys")` | descriptor-bound `byob_image_load_exact` or equivalent same-handle identity read |
| `GetVDMAddr(DI,0)` + repeated `ReadFile` directly into guest pointer | M29 `begin_guest_write(segment=DI, offset=0, byte_count)` plus host-owned transient bytes, then one commit |
| host file/read failure | release transient resources, abort transaction if opened, then original `TerminateVDM` lifecycle path |

This is not an initial preload, a synthetic SVC response, a pathname rewrite,
or a new DOS loader.  The historical BOP/DEM call reaches the original owner;
only its obsolete host-file and raw-pointer internals are adapted.

## Required Operation Sequence

1. At the returned transition boundary, capture `DI` from copied CPU state.
   The bridge must not cache a load segment before NTIO executes.
2. Acquire the previously selected NTDOS descriptor, rooted under the validated
   BYOB root.  Revalidate the file identity and obtain a transient complete
   host buffer.  Do not infer a system-directory path or search a drive.
3. Begin an M29 guest-write transaction for `DI:0000` and the exact verified
   byte count.  The backend applies its own mode/A20/paging translation and
   proves the entire target is ordinary writable RAM before any mutation.
4. Copy the complete transient buffer into the transaction's staging storage.
   This is a host-to-host copy; it is not an incremental guest write.
5. Commit the exact byte count once.  On success release the transient image
   and return normally from the original `demLoadDos` function.
6. On every failure before commit, abort the transaction if one exists, release
   every transient resource, emit only role/hash-prefix/length diagnostics,
   and invoke the original termination outcome.  On a commit failure, guest
   memory is still unchanged by M29's contract.

The bridge does not implement the debug-only `pszBIOSDirectory` symbol-path
side effect.  That is a debugger policy branch, not part of the non-debug
direct-CLI command cut.  If debugger support is later admitted, it requires a
separate symbol-notice capability and must never retain guest paths or bytes.

## Fixture Requirements

The bridge cannot enter a normal executable yet, because no adopted backend
reaches the original SVC.  Its eventual bounded fixtures must prove:

1. a selected synthetic NTDOS descriptor reaches an observed live `DI:0000`
   transaction request exactly once;
2. an identity mismatch, missing component, range rejection, allocation
   failure, and commit failure each issue no guest write and select termination;
3. a successful request releases its transient buffer and reports no pathname
   or guest byte content;
4. no test calls `DemDispatch`, fabricates SVC completion, or changes CPU mode;
5. a runtime-link audit proves the overlay is absent from `ntdos64-run` and
   default targets.

## Outcome

M31 is complete as a source-derived bridge design.  The next execution
question is the command-side host protocol: the selected `COMMAND.COM` must
receive the historical configuration/environment/command/stream services
identified in M22 and M24, without becoming a native process handoff.
