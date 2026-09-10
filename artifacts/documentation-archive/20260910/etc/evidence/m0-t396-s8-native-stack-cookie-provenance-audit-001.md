# M0 T396 S8 — native abort-dispatch provenance audit 001

## Question

Does the post-`Disa20_Xfer` normal-product `0xc0000409` result arise from the
one-megabyte process stack reserve, or can that historical composition limit
be excluded before attributing the native abort-dispatch boundary?

The preliminary debugger-only stack-cookie hypothesis is withdrawn. Normal
Windows Error Reporting (WER) maps the normal product's fault offset to CRT
`_abort()`'s `int 29h` and records fast-fail code `7`
(`FAST_FAIL_FATAL_APP_EXIT`), not code `2`
(`FAST_FAIL_STACK_COOKIE_CHECK_FAILURE`). Cookie imports below are only a
non-attributing static inventory.

## Static baseline

The current formal x86 process map contains the MSVC CRT cookie/report
machinery (`__security_check_cookie`, `__report_gsfailure`, and
`__security_cookie`).  A symbol scan of the 415 current formal object files
found 85 objects importing `__security_check_cookie`:

| Cohort | Objects |
| --- | ---: |
| Original host | 16 |
| Original DEM | 10 |
| Original COMMAND | 7 |
| Original DPMI32 | 6 |
| Original CCPU | 12 |
| Adapters and app | 22 |
| Other original/support/test cohorts | 12 |

This is a candidate inventory only.  It does not identify the caller that
entered the CRT abort path. The current normal image's PE header has a
`0x100000` stack reserve and `0x1000` initial commit.

## Control

The control relink used the exact current object/library closure, including
the current S7 `softpc-bindings` library, and differed only by
`/STACK:8388608`.  It was emitted under the task-local build output, not made
the formal graph or staged product.  Its PE header reports an `0x800000`
reserve and `0x1000` commit.

The existing corrected Console-owning non-debug observer then ran the same
short-root declaration:

```text
O:\t396\ntvdm32.exe -f -o --command system32\krnl386.exe
```

It exited after 6937 ms with `0xc0000409`, loaded 15 modules, and did not load
`WOW32.DLL`.  The control product hash was
`A5549124525CFEC6744F8A6FBAF42DBDEF0484352C729C1A7F4CA963CDAE9A48`.

## Normal-product dispatch boundary

The non-debug WER minidump's exception context has `EIP` at the above
`_abort()` `int 29h`. Its frame chain returns to the instruction after an
indirect original CCPU dispatch-table call, then to original
`c_cpu_simulate`, `host_start_cpu`, `host_main`, the original-entry bridge,
and the app shell.

The dump contains the live table page. At the original image base, the table
entry is `0x0048A4C0`; after the dump's relocation it is `0x001FA4C0`. Both
resolve to original BIOS `_reset`, and the on-disk/current-binding table has
the same index-zero entry. `_reset` contains no direct CRT `abort` call.
Therefore the CCPU table identifies the reset cohort, not an opcode handler
or `invalidFunction`, and the immediate provenance question is which
source-defined reset initialization call enters the abort path.

## Disposition

The prior general observation that some large nested-buffer composition tests
need an eight-megabyte reserve does not explain this runtime failure.  The
same failure survives the larger reserve, so S8 rejects stack-reserve growth
as a repair. The scan also rejects deriving a source caller merely from CRT
linkage: 85 objects are instrumented. The normal WER code rejects the
previous stack-cookie attribution.

Debugger-only observations are retained as diagnostic contrast, not
acceptance: attaching a debugger changed the terminal status. S8 therefore
remains open for a separately justified, finite, default-off reset-phase
observation at the identified original BIOS boundary. It must report only
pre/post phase scalars for existing reset calls; it must neither replace nor
skip any initialization, route a session, or alter guest state.
It must not disable or bypass `/GS`, alter app routing, guest media, BOPs,
CPU semantics, or WOW32 loading.

## Reset-phase observation disposition

The observer implementation is default-off and compiled into its three owning
objects (`reset.obj`, `mvdm_softpc_machine_observation.obj`, and
`report_configuration.obj`).  An initial direct-object test link was rejected
as evidence: it retained the surrounding S3 archives rather than rebuilding
their owning archives, and therefore changed the observed terminal result from
the calibrated `0xc0000409` to `0xc0000029`.  Its complete phase-1 through
phase-14 enter/return report is consequently **not** an attribution for the
current product.

The same Console-owning observer, fixed `O:\\t396` media manifest, command, and
timeout applied to the unmodified S3 product reproduced `0xc0000409` after
6469 ms (15 modules, `loaded-wow32=no`).  This establishes the difference as a
link-composition variable, not a reset repair result.  The phase observation
is not admitted to run again until a complete current-binding archive/product
link exists.  No reset, video, CPU, BOP, routing, or guest behavior change is
licensed by this result.

### Complete-archive retry

A complete-archive observation product was subsequently linked without
direct-object precedence. It starts from the S3 formal archive graph and
rebuilds four replacement archives by removing and reinserting only their
already recompiled same-named members:

| Archive | Reinserted members |
| --- | --- |
| `original-softpc-bios.current.lib` | `obj\\bios\\reset.obj` |
| `softpc-bindings.current.lib` | `obj\\adapter-softpc\\mvdm_softpc_machine_observation.obj` |
| `app-machine-shell.current.lib` | `obj\\app\\report_configuration.obj` |
| `original-ccpu386.current.lib` | `obj\\ccpu\\ccpusas4.obj`, `obj\\ccpu\\ret.obj` |

Its map binds `_reset`, the reset-phase record function, report configuration,
and the two CCPU members to those replacement archives. The generated product
SHA-256 is `30C68E1178096A895381040CF84E3681600C743092F10639146E790818B8B796`.

Two fixed-container, Console-owning runs of that exact product used the same
`system32\\krnl386.exe` command and 10-second bound. With no report variable
it exited `0xc0000029` after 4547 ms. With only
`MVDM_RESET_PHASE_REPORT_PATH` enabled it exited `0xc0000029` after 4281 ms
and recorded enter/return pairs for every phase 1 through 14. The observer
therefore does not cause the terminal result, and original BIOS `_reset`
returns completely in this current archive composition. This result narrows
the active provenance boundary to post-reset CPU/guest execution; it does not
attribute the `0xc0000029` result to a specific callee or authorize a repair.

## Exact post-reset owner

The existing session-termination report was then enabled for the same
complete-archive product. It recorded exactly:

```text
MVDM-SESSION-TERMINATION origin=wow:private-hard-error code=0x00000078
```

`0x78` is `ERROR_CALL_NOT_IMPLEMENTED`. The owner is not inferred from the
exit code: `adapter-mvdm-host-out/win32/source/wow_private_unavailable.c`
sets the same origin and invokes `session_termination_current` with that exact
code from its `NtRaiseHardError` unavailable implementation. The resulting
`0xc0000029` is `STATUS_INVALID_UNWIND_TARGET`; it follows the deliberately
selected session longjmp path and is not a BIOS or CCPU instruction failure.

The original caller is `WOWpSysErrorBox` in original
`softpc.new/host/src/nt_error.c`. It supplies an NT4 `STATUS_VDM_HARD_ERROR`,
four parameters (two button words and two Unicode strings), and expects the
returned response after the historical CSRSS hard-error broker call. Public
modern Win32 does not directly supply that compound CSRSS contract. Therefore
S8 closes with an exact next owner: S9 must audit and select a source-shaped
private hard-error response/broker boundary. It must not turn the unavailable
stub into an invented success response or alter session termination merely to
hide the result.
