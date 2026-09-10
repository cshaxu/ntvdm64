# Historical DEM File-Shim Admission

## Decision

The first runnable file seam must retain the original NT4 DEM function and
dispatch ownership:

```text
NTDOS file/SFT/PSP/EXEC -> DemDispatch -> demOpen/demRead/
demChgFilePtr/demClose -> contained modern host-file adapter
```

It may not replace a DEM selector, fabricate an SFT result, or create a
second pathname, EXEC, or DOS error implementation.  The required adaptation
is a per-session, opaque 32-bit file-token table at the historical host-file
operation boundary.  It is an x64 ABI repair, not a new DOS runtime.

## Source Evidence

The fixed NT4 sources establish this register contract.

| Function | Guest inputs | Historical success result | Direct host action |
| --- | --- | --- | --- |
| `demOpen` | `DS:SI` path, `BL` access/share mode | token high word `AX`, low word `BP`; file size `BX:CX`; pipe flag `DX`; `CF=0` | `CreateFileOem`, then `GetFileSize` |
| `demRead` | token `AX:BP`, byte count `CX`, buffer `DS:DX`, optional position `BX:SI` and `ZF` | bytes read `AX`, `CF=0` | optional `SetFilePointer`, `ReadFile` |
| `demChgFilePtr` | token `AX:BP`, signed offset `CX:DX`, origin `BL` | resulting offset `DX:AX`, `CF=0` | `SetFilePointer` |
| `demClose` | token `AX:BP`, optional final position `CX:DX` | `CF=0` | optional `SetFilePointer`, `CloseHandle` |

`demfile.c:319-320` serializes the historical `HANDLE` directly into
`BP:AX`; `demhndl.c:51`, `124`, and `374` reconstruct it with `GETHANDLE`.
That transport was valid when both `HANDLE` and the guest-visible token were
32 bits.  It is not valid in a 64-bit process: truncation would turn an x64
handle into an unrelated value before `demRead`, seek, or close.

The narrow initial profile does not admit the named-pipe branch
(`LoadVdmRedir`, `Vr*`), CD-ROM fallback, write/create/delete/rename, file
time, commit, or direct device behavior.  They remain historical source
inputs and later policy decisions, not bootstrap host requirements.

## Minimum Adapter ABI

The next implementation task may introduce a private per-session table with
the following behavior only:

```text
open(contained path, DOS access/share) -> token32 + file-size metadata
read(token32, offset-or-current, guest-byte span) -> count or DOS-mappable error
seek(token32, signed offset, DOS origin) -> new offset or DOS-mappable error
close(token32, optional final offset) -> success or DOS-mappable error
```

Required invariants:

1. A token is nonzero, 32-bit, session-owned, and is never an x64 `HANDLE`,
   pointer, CRT descriptor, or native Windows object value.
2. `demOpen` remains the only producer of the token and continues to publish
   it through `AX:BP`; `demRead`, `demChgFilePtr`, and `demClose` remain the
   consumers through the existing register transport.
3. The adapter resolves only paths that the already-admitted contained
   namespace maps below its configured root.  It performs no ambient drive
   discovery or device/pipe opening.
4. Read destinations are first validated as writable guest memory by the
   selected backend.  A host read must not leave a partially committed guest
   range after a translation or bounds failure.
5. Host errors flow through the original `demClientError`/extended-error path.
   The adapter may map host errors into the historical error inputs, but may
   not invent SVC return behavior.
6. Token invalidation is atomic at close and at session teardown.  A token
   from another session, an invalid token, or a double close fails through the
   original error path.

The adapter may be source-local to a separately linked historical host island;
its ABI must use fixed-width C types and explicit ownership.  No C++ object,
exception, or CRT allocation crosses the historical/modern boundary.

## Overlay And Build Evidence

`src/opennt/CMakeLists.txt` currently compiles
`overlay/base/mvdm/dos/dem/demfile.c` and `demhndl.c` in
`ntdos64-opennt-dem-objects`, not the upstream `base` copies.  This is not a
runtime admission:

* The `demfile.c` overlay changes four VDD helper declarations to match the
  contemporaneous `nt_vdd.h` ANSI prototypes and makes one equivalent local
  cast.  It does not alter `demOpen` or the normal DEM file service flow.
* The `demhndl.c` overlay replaces the obsolete compiler-invalid `STOREWORD`
  lvalue cast at the packed `ExtendedError` field with an equivalent `USHORT`
  assignment.  It does not alter the normal read path.

These changes are documented in `HISTORICAL-SOURCE-INPUT-GATES.md` and
`OPENNT-HISTORICAL-HOST-STACK-BUILD-GRAPH.md`.  They remain compilation
overlays only.  A runnable file bridge must either use provenance-locked
equivalent source repairs or demonstrate a compiler that accepts the original
spelling; it must not silently convert an overlay into an unreviewed runtime
fork.

The historical targets are all `EXCLUDE_FROM_ALL` under `src/opennt`.
The root `CMakeLists.txt` keeps `ntdos64-run` sourced only from
`src/runner/ntdos64_run.c` and rejects attempts to nest OpenNT historical inputs in
the MinGW runner graph.  The trace-only self-authored sources are likewise
behind `NTDOS64_ENABLE_TRACE_INSTRUMENTATION=OFF`.  Other pre-existing default
research probes may build, but no default executable links historical DEM, a
self-authored file service, or the future adapter.

## Outcome

M26 is complete as an admission record.  The next ROI task is to design and
test the contained token table in isolation, then use it only as the direct
host-operation replacement required to make the original DEM functions
runnable.  That task remains blocked from normal runtime admission until an
execution backend can reach the original DEM chain.
