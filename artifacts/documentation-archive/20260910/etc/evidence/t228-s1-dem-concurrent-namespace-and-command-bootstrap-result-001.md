# T228 S1 DEM Concurrent Namespace and COMMAND Bootstrap Result

## Scope

This record captures one complete DEM namespace repair and corrects a
misclassification made while observing the hash-locked T228 source-built
guest inputs.  It does not claim child execution, normal `4Ch`, `50:3C`,
`54:0B`, or T228 closure.

## Repair

`bx_ntvdm_readonly_namespace_v1` previously represented only one active
immutable file stream.  Its `open`, `seek`, `read`, `close`, file-time and
token checks now operate on a fixed eight-entry opaque-handle table.  Each
entry has an independent generation, file index and offset.  The DEM outer
file session continues to own DOS-visible tokens; the namespace owns only its
immutable backend tokens.  The fixed-width table is adapter-local and neither
changes Bochs semantics nor adds a BOP-specific route.

`50:00` now obtains the current offset and end position through the namespace
position query, so seek origin semantics remain per-handle rather than using
the obsolete global diagnostic summary.

## Local verification

The fresh formal CPU5 / MSVC x64 `/MT` Ninja graph at
`build/M0-T228-S1/p32-dem-concurrent-namespace-20260818a` completed 444 of
444 compile, archive and link edges.  Its final incremental rebuild completed
after restoring the diagnostic sampling rule.

Two focused fixtures passed from that graph:

- `t228-s1-dem-concurrent-namespace-fixture.exe` proves simultaneous
  `COMMAND.COM` and `TARGET.COM` reads retain independent offsets, reject a
  stale token after close, and leave no active handle after both closes.
- `t226-s2-dem-readonly-file-conformance-fixture.exe` continues to prove the
  bounded open/seek/read/close (`O/S/R/C`) service contract.

## Source-built integration correction

The hash-locked input manifest remains
`c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`.
Direct and Readonly bounded observations both reach the initial `50:36` and
then a controlled terminal; they are not child-completion evidence.

OpenNT `get_binary_type` in
`src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm` opens a DOS image and
reads `Exec_Header_Len` (0x3e) to select EXE versus COM.  Default-off adapter
observations establish the following facts:

- The observed header read uses DOS handle `AX:BP=0003:0001` and returns
  `e9 5d 15 00`.
- The namespace record for that same handle resolves to declared slot 1,
  `COMMAND.COM`.
- A separately observed declared TARGET open resolves to slot 3 and returns
  `AX:BP=0004:0001`.

Consequently the first COM transfer at `50:36` is the normal initial
`COMMAND.COM` bootstrap, not an erroneous COM interpretation of
`TARGET.EXE`.  The earlier attribution of that read to TARGET is withdrawn.
The next integration diagnosis, if any, belongs to COMMAND's later lifecycle;
it must not treat this bootstrap fact as a target-loader or single-BOP defect.

## Limits

The temporary ordinal/handle sampling used to obtain the attribution was
restored.  The retained CLI observations are default-off and copy only typed
metadata or four returned bytes; they do not alter guest memory, CPU state,
BOP selection or provider behavior.
