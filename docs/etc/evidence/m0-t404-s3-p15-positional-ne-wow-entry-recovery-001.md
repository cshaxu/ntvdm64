# M0 T404 S3 P15 — positional NE-to-WOW entry recovery

## Question

Can the sole public command `ntvdm32.exe system32\WRITE.EXE` preserve the
original OpenNT Win16 classification choice through the current standalone
app/BaseVDM boundary, without exposing worker flags to the user?

## Source-recovery ledger

1. **Original source reuse.** OpenNT
   `opennt-host/base/win32/server/srvvdm.c:BaseSrvCheckVDM` dispatches
   `BINARY_TYPE_WIN16` to `BaseSrvCheckWOW`, not `BaseSrvCheckDOS`. Original
   `mvdm-host/dos/command/cmdmisc.c:GetWowKernelCmdLine` obtains the WOW
   bootstrap target from the worker process command line in the exact
   ` -a <fully-qualified KRNL386.EXE>` form. Both source bodies remain
   selected mirrors.
2. **Smallest same-shaped facade.** Full CSR/CSRSS BaseSrv transport and its
   child-process creator are explicit stopping boundaries. The existing local
   copied BaseVDM record is the finite replacement seam. P15 retains the
   original DOS/WOW record distinction by copying `command_owner` as a
   fixed-width private field, then makes app map already-classified NE only to
   the WOW owner. It preserves VDM-state values, payload copy order and the
   existing `GetNextVDMCommand` facade.
3. **External intrusion.** None.
4. **New mechanics.** The app derives the original `-w -a <short package
   KRNL386 path>` token sequence only in its private SoftPC argv. This is not
   a replacement WOW loader and does not enter guest memory. Runtime proved
   that this in-memory argv is insufficient for the original direct
   `GetCommandLine()` consumer; the missing actual-worker command-line bridge
   remains a new, unadmitted process-boundary decision.

## Changes and focused verification

- The fixed-width broker record advances from version 2 to 3 and preserves a
  copied DOS (`0`) or WOW (`ASKING_FOR_WOW_BINARY`) owner alongside its source
  VDM request state.
- The local broker no longer rejects the existing WOW owner and delivers the
  same copied owner to `base_vdm_local`.
- The app retains its resolved image kind. Only a Win16 image derives the
  package-relative short `KRNL386.EXE` bootstrap token; DOS gets no WOW token.
  User-provided `-w`/`-a` are rejected because they identify a system-created
  worker, not a public CLI mode.
- Fresh selected x86 objects were built in
  `build/M0-T404/S3/r011-p15-ne-wow-entry/`. The P14 x86 object/library cache
  was copied only after its source-manifest confirmed the same CCPU40/MSVC
  `/MT` catalogue; every P15-changed production object was explicitly
  recompiled and affected libraries relinked.
- `broker-base-vdm-record-test.exe`,
  `basesrv-base-vdm-broker-test.exe`, and
  `app-launch-declaration-wow-entry-test.exe` each exited `0`. The last proves
  the exact internal Win16 argv shape `exe, -f, -w, -a, <KRNL386>` and the
  DOS `exe, -f` non-regression.
- The formal x86 product link succeeded. Candidate SHA-256 was
  `aa1e57cecaa7b8a497d006f6ceb9be3a0d9cb7bd27645d2426a81e7d49d5d06c`.

## Positional runtime observation

The candidate was temporarily staged and started only as:

```text
ntvdm32.exe system32\WRITE.EXE
```

with the default-off `MVDM_BASE_VDM_REPORT_PATH` and
`MVDM_DEM_OPEN_REPORT_PATH` observers. It exited within the bounded interval.
The BaseVDM observer recorded:

```text
MVDM-BASEVDM state=0102 available=1 owner=2 dos-state=0 pending=0 pif-bytes=42 status=00000000
```

`0102`/owner `2` is the WOW request/record, replacing the earlier positional
DOS record (`state=0104`, owner `0`). This is direct proof that the app and
broker classification boundary is now correct.

The DEM-open observer reached initial DOS media but did **not** open
`KRNL386.EXE` or `DOSX.EXE`. The explanation is source-defined: P15 appends
the bootstrap tokens only to `softpc_argv`, while original
`GetWowKernelCmdLine` reads the real Windows process command line through
`GetCommandLine()`. The current one-process composition cannot make those two
carriers equal. Therefore P15 is not evidence of a KRNL386/DOSX/WRITE run and
must not be published as an owner test candidate.

Raw files are retained under `O:\ntvdm64\logs\` as
`t404-p15-positional-write-basevdm.txt`,
`t404-p15-positional-write-dem-open.txt`, and matching stdout/stderr files.
After the bounded check, the package was restored to P12 SHA-256
`3dc9d7f0e8a7836653dca50cf5d32ddacc316e85e4af14b9546a94051498b3aa`.

## Follow-up boundary

The next packet must choose and source-audit an actual worker-process launch
and command-record handoff, or a narrowly equivalent `GetCommandLine` seam.
It is a material process/bootstrap boundary and was intentionally not
implemented under P15. It must preserve the single public positional command,
then demonstrate `KRNL386.EXE` and `DOSX.EXE` before claiming WRITE recovery.
