# Original loader frontier and current ownership

This S10 source review follows the six edition-specific NTDLL records in
[the external inventory review](shard-2/external-inventory-obligation-review.md).
It does not import a loader, modify product source, or establish DLL lifecycle
acceptance. The [input receipt](external-loader-owner-inputs.json) verifies
each original working file against its frozen Git blob identity, permitting
only recorded CRLF-to-LF checkout normalization, and records the actual
working SHA-256, size and source revision. Both editions remain comparison
evidence. An initial byte-only identity check rejected CRLF checkout bytes;
the corrected check records both hashes rather than claiming byte identity.

Procedure: join the six exact frozen IDs to `external-obligation-triage.csv`,
read each file from the edition root/revision in `seam-scope.json`, calculate
SHA-256 over its actual bytes, and calculate the Git blob SHA-1 over
`blob <byte-count>\0<bytes>` both before and after CRLF-to-LF normalization.
Require one to equal the frozen blob ID. Inspect the function bodies named
below, rather than treating their lexical presence as execution evidence.
The background source check passed for all six files; documentation
governance and `git diff --check` also passed. No runtime tests were run for
this source-only checkpoint.

## Actual chain

1. Original `softpc.new/host/src/nt_bop.c` loads WOW32 through
   `SafeLoadLibrary`, then resolves `W32Init` and `W32Dispatch`. Its load and
   export failures retire the VDM; the original algorithm is not a PE loader.
2. The registered package-path adaptation delegates the load expression to
   `src/ntvdm-exe/softpc/mvdm_softpc_firmware.c::mvdm_softpc_load_library`.
   WOW32 and VDMREDIR select product-owned absolute paths and call
   `LoadLibraryW`; other names call `LoadLibraryA`. The host loader owns real
   module mapping, import resolution and loader notifications. No NT4 PEB or
   private loader-list structure is supplied by this adapter.
3. Original `wow32/sources:24` selects `DLLENTRY=W32DllInitialize`.
   `New-T404S5Wow32ProviderNinja.ps1:338` instead retains modern static CRT
   initialization and generates `DllMain` forwarding to that original entry.
   The provider remains original `wow32.c::W32DllInitialize`; the bridge is a
   project-owned ABI boundary, not an original loader implementation.

## What the original three-file frontier actually supplies

| Original file in each edition | Observed responsibility | Current owner and disposition |
| --- | --- | --- |
| `ldrapi.c` | `LdrpLoadDll` maps/references dependencies, invokes `LdrpRunInitializeRoutines`, and calls `LdrUnloadDll` when initialization fails; unload dispatches process-detach to the stored entry | Modern Windows loader behind the public library API. Original source is contract evidence, not a proposed runtime import. |
| `ldrsnap.c` | Import/export resolution and `LdrpRunInitializeRoutines`; the latter calls the PE entry, consumes its byte return, and rejects failed initialization | Modern loader plus correctly typed CRT/original-entry bridge. The demonstrated bridge return mismatch remains a finite S11 repair. |
| `ldrinit.c` | Process/thread loader initialization, loader database/TLS and initialization-failure propagation | Modern Windows loader/CRT. The selected original WOW initializer still owns its own WOW state, not the host loader database. |

The byte-return evidence is explicit in both editions: OpenNT
`ldrsnap.c:647` declares `BOOLEAN InitStatus`, and line 769 stores `al` after
the entry call; OpenNT-4.5 has the corresponding declaration at 637 and store
at 759. A false result leads to `STATUS_DLL_INIT_FAILED`. Thus reading all
of EAX in the generated bridge is not justified by the original loader's
behavior. The [controlled actual-object witness](dll-entry-boolean-return.md)
demonstrates this ABI mismatch without asserting that an original WOW heap
failure has been observed.

## Recovery decision and remaining proof

Do not recursively import these NTDLL files: their loader database, mapping,
TLS, dependency and process initialization machinery belongs to the explicit
source-policy stopping boundary. This is a source/ownership conclusion, not a
claim that all old and modern loader behavior is identical. In particular,
the original x86 code's stack-mismatch diagnostic/repair is not reproduced or
approved as a new general compatibility mechanism here.

Restore the byte return at the existing bridge. Preserve original initializer
ordering, current CRT initialization and explicit-unload cleanup. S11 must
verify actual DLL attach failure, thread notifications and unload as well as
normal/restart WOW lifecycle; controlled return probes alone do not close
those paths. No additional private loader state or adapter API is required
for the demonstrated defect. Expected mirror and guest diff is zero.

The six external rows remain pending overall contract acceptance in the
external ledger, with this narrower ownership/source question resolved.
The 4,684 MVDM classification counts are unchanged. Concurrent changes in
`nt_bop.c` are observed source only, not compiled or published by this review.
