# M0 T236 S6 P1 — `VdmQueryDir` Fallback Closure

## Question

Can the imported OpenNT pathname/FCB search owner preserve its own recovery
semantics on a modern host after the historical
`NtVdmControl(VdmQueryDir)` fast reset is unavailable?

## Source and disposition

`src/opennt/base/mvdm/dos/dem/demsrch.c` owns both alternatives in
`FileFindReset`:

1. For a non-dot remembered result with `SupportReset`, it calls the old
   VDM-only `NtVdmControl(VdmQueryDir)` fast locator using an open directory
   handle, last `FileIndex`, last name, and an NT directory-information
   buffer.
2. If that call does not return success, end-of-directory, or no-such-file,
   it retains the original slow path: restart `NtQueryDirectoryFile`, compare
   the remembered `FileIndex` and name, then continue from the next result.

There is no public Win32 operation that seeks a directory enumeration to a
previous `FileIndex`; `FindFirstFileW`/`FindNextFileW` can restart and scan,
but cannot set an enumeration cursor. Recreating `VdmQueryDir` would require
the removed VDM kernel protocol and would add an adapter-owned directory
service. It is therefore rejected.

The existing smallest seam in
`src/bx-vdm/bop/shim/demsrch_fcb_shim.c` retains the source failure decision:
`NtVdmControl` returns `STATUS_NOT_IMPLEMENTED`. This is an explicit
unavailable result, not a success-shaped substitute. The original slow reset
remains the active implementation.

## Procedure

The existing direct-import fixture creates two matching temporary files and
performs original `50:09` FindFirst. It then calls the original `DemHeartBeat`
8640 times, which causes the imported source to close the idle search handle
and buffer. The next `50:0B` must reopen, invoke `FileFindReset`, receive the
unavailable fast-path status, scan through the original slow reset, and return
the second file. A final `50:0B` must return `ERROR_NO_MORE_FILES` with carry.

The fixture also retains its existing FCB first/next, opaque FCB token,
read/date/info, rename/delete/create/close, and PDB cleanup checks.

## Verification

A fresh formal MSVC x64 `/MT` Ninja graph was generated under:

`build/M0-T236-S6/p1-vdmquerydir-fallback-r1`

The focused fixture rebuilt and exited zero:

```text
T230 S5 direct OpenNT demsrch/demfcb import: slow-reset search lifecycle, tokenized FCB operations and PDB cleanup verified
```

An immediate Ninja dry-run reported no work.

## Result

S6 is closed. Ordinary pathname and FCB search remain imported OpenNT code;
the removed fast locator has one explicit unavailable disposition; the
original fallback has positive continuation and exhaustion evidence. No guest
pointer, host handle, `FileIndex`, directory buffer, selector policy or new
directory protocol crosses the bx-vdm boundary.

## Follow-up

Directory mutation during a reset retains the original slow-path limitation:
if the remembered item has disappeared, the source may report exhaustion. It
is not a correctness reason to invent a replacement locator. T236 resumes S5
for COMMAND WOW source disposition.
