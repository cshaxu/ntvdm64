# T95 S7 Contained Boot-File Materialization 001

Status: implemented adapter-local namespace increment, 2026-08-11.

## Decision

The first `ntdos64-byob-profile-v2` profile explicitly selects all three files
that its early guest boot can open:

| Role | Canonical DOS path | Bytes | Owner |
| --- | --- | --- | --- |
| `command` | `\COMMAND.COM` | independently identity-verified BYOB image | CLI/BYOB admission |
| `config` | `\CONFIG.SYS` | fixed `minimal-comment-v1`: `REM NTVDM64\r\n` | adapter profile materialization |
| `autoexec` | `\AUTOEXEC.BAT` | fixed `empty-v1` | adapter profile materialization |

The profile carries `guest_command_placement` (including its DOS drive index)
and a required `guest_boot_files` pair.  Both boot-file paths and both
materialization identifiers are closed values.  They are not host paths,
environment-derived strings, or file names to reopen.  The pair shares the
selected command drive; no separate drive-mapping policy is introduced.

`minimal-comment-v1` deliberately contains no `SHELL=`, `DEVICE=`, `INSTALL=`,
`DOS=HIGH`, `COUNTRY=`, environment import, or device request.  It is only a
nonempty contained CONFIG resource.  `empty-v1` is an existing zero-length
contained file, so COMMAND follows its ordinary DOS `OPEN`/no-batch behavior
rather than treating an empty service result as a magic shortcut.

## Source Relation And Deliberate Difference

OpenNT `cmdGetConfigSys`/`cmdGetAutoexecBat` (`base/mvdm/dos/command/cmdconf.c`)
historically derives temporary files through PIF, system-directory, locale and
environment policy.  That policy is excluded.  What remains source-shaped is
the BOP service's pathname result and the consumer order: NTDOS calls the
CONFIG service before ordinary DOS open/parse (`dos/v86/doskrnl/bios/sysinit1.asm`),
and COMMAND calls the AUTOEXEC service before ordinary DOS open
(`dos/v86/cmd/command/init.asm`).

This record therefore does **not** claim byte-for-byte restoration of
`cmdconf.c`; it records the explicit CLI/profile replacement permitted by S7.
No BOP is dispatched by this increment.

## Namespace Contract

`bx_ntvdm_readonly_namespace_v1` is adapter-local and owns one open operation
at a time.  A successful canonical open returns a private generation token and
the fixed byte count.  Seek permits an offset in `[0, size]`; read copies at
most 64 KiB into adapter-owned staging and advances the local offset; close
invalidates the token by advancing the generation.  An unknown path, wrong
drive, concurrent open, stale token, offset beyond EOF, or oversize read is
rejected without a host operation.

The namespace stores no Win32 handle, host pathname, guest pointer, BOP
selector, or Bochs object.  Its immutable image pointer is same-island backing
storage only; a future service must convert read bytes through the already
declared generic bulk-result transaction, not export this structure.

## Verification

The local MinGW build root `artifacts/build/current/adapter-cli-r1` rebuilt and
ran on 2026-08-11:

```text
byob-profile-test: owned BYOB profile acceptance and rejection families verified
bx-ntvdm-readonly-namespace-test: canonical read-only file lifecycle verified
bx-ntvdm-adapter-runtime-test: identity-to-pending-transaction path verified
```

The namespace test covers wrong-drive/path rejection, command sequential read
and seek, CONFIG nonempty materialization, AUTOEXEC zero-byte materialization,
close, and stale-token rejection.  It does not yet prove DEM request decoding,
guest RAM commits, or a Bochs runtime trace; those remain S7's next increment.

## O/S/R/C Adapter Completion Increment

`bx_ntvdm_dem_readonly_file_service` now consumes the namespace through the
four source-selected service identities, but is not yet registered with the
runtime dispatcher:

| Service | Increment behavior |
| --- | --- |
| `50:12` | asks for a bounded copied `DS:SI` 128-byte path and accepts only `AL=0`, `BL=0`, explicit `C:\...` canonical paths; success returns the private token in `AX:BP`, size in `BX:CX`, and `DX=0` |
| `50:00` | applies methods 0/1/2 as signed displacement over the private offset; invalid method remains pass-through |
| `50:16` | honors the source ZF distinction: ZF clear applies `BX:SI` before read; ZF set continues private offset. It stages data through one generic bulk-result transaction; EOF is a zero-payload, zero-address resume. |
| `50:02` | closes a nonzero private token or honours source-defined zero-token success; stale nonzero tokens produce `ERROR_INVALID_HANDLE`/CF. |

The focused test target
`bx-ntvdm-dem-readonly-file-service-test` passes its open, seek-to-end, EOF,
explicit-offset read, close and stale-close cases.  This validates only the
adapter-local closure.  No service is currently enabled in
`bx_ntvdm_cpu_result_bridge_v1`; path/error-family coverage and the matching
generic pending-bulk consumer remain required before any real BOP dispatch.

## Runtime Session Installation Increment

The runtime now copies the three canonical namespace paths into session-owned
storage at install time.  It does not retain pointers to the transient parsed
profile.  The existing v3 generic copied-read channel classifies this one
consumer as `DEM_FILE_OPEN`; its completion opens only the installed namespace
and returns a generic CPU result.  The runtime test installs a v2 profile,
requests `50:12`, supplies copied `C:\COMMAND.COM` bytes, and verifies a
successful two-phase completion.  A generic bulk-result take wrapper is also
available for a later completed read operation; it contains no service or file
vocabulary.

This reaches only adapter runtime composition.  The Bochs #UD path has not
been changed to invoke v3, no generic bulk consumer has been enabled there,
and `50:16` remains unregistered pending that generic bulk consumer.

`50:00` (seek) and `50:02` (close) are now registered in the runtime's v2
adapter path because they have no guest-memory payload.  The regression opens
the fixed COMMAND resource through v3, then proves seek-from-end and close
return source-shaped CPU results.  This remains a runtime-adapter result, not
a claim that Bochs has invoked these BOPs in a guest execution trace.

## Generic Bulk Consumer Admission

Exception `BX-ABI-041` records the one Bochs-internal mechanical extension
needed for a future `DEMREAD`: after the existing generic result is ready,
the CPU seam may take one opaque pending bulk transaction, verify its result
matches the same resume, preflight the complete single ordinary-RAM range, and
copy it once if nonempty. A zero-length result has no RAM address or copy.
The code does not name, inspect, or branch on a BOP selector, file, path,
handle, token, payload contents, DOS or OpenNT condition.

`Test-BochsUdDefaultOffBoundary.ps1` and
`Test-BochsOrdinaryRamBoundary.ps1` pass after the addition. This is only a
registered static seam; it does not prove a Bochs build or a guest execution.

## DEMREAD Runtime Registration

`50:16` is now registered in adapter runtime v2. A successful bounded read
queues the opaque bulk result and returns its already-validated CPU result;
the generic Bochs seam remains the sole potential RAM consumer. EOF queues a
zero-payload result. Invalid range or stale token returns a direct ordinary
CPU error (`CF`, `ERROR_INVALID_PARAMETER` or `ERROR_INVALID_HANDLE`) and
does not queue a transaction. The runtime regression proves a read of the
identity-backed COMMAND bytes through queue/take, and the focused service test
proves stale-token read rejection.

This is not a guest-execution claim: a Bochs build and a real trace consuming
the queued result are still required before declaring `50:16` runnable.
