# T474 S3 — generic RPC and I/O libraries

## Ownership audit

`lib/rpc` owns local RPC binding and asynchronous call completion mechanics.
They use Windows RPC primitives and explicit caller-provided state, not the
NTVDM64 protocol. `common/rpc` retains protocol-specific request calls and
connection views. The async header and callable binding retain their existing
behavior and API spelling.

`lib/io` owns bounded overlapped pipe transfers and Console process-list
sampling and final native file-path resolution. The NTCON-worker request/reply format, frames, input decoding,
endpoint ownership and lifecycle remain in `common/console` and the product
components. Both generic libraries depend only on platform APIs, not `common`
or an executable. Tests check the source/build direction with negative cases.

`common` retains project-specific application search, guest environment,
product root, native image classification, native process packet semantics,
and the BaseSrv protocol client. Source/ABI behavior is unchanged by relocation.

## Verification and local publication

The ten product images were freshly built in this worktree. The staged
package and each corresponding input were independently hash-checked, as
were all ten images published to this worktree's `assets/release`. Package ID:
`e05f16a8-e865-40e1-8a6e-d86bc5d25809`. The original 63 guest files and
274 NTVDM export ordinals passed their immutable-media and ABI checks.

The RPC binding, console membership, pipe transfer and native path tests passed
42, 66, 117 and 227 checks respectively; Hook installation passed 118 checks.
Frontend and BaseSrv ownership graph checks and their negative controls passed,
as did the Base RPC client split checks. The complete Control suite passed all
12 selected gates against the identical ten-image package, including real
NTCON/worker communication and lifecycle scenarios. The optional external
DOS4GW/NESticle fixture was unavailable and skipped. Full product verification
was not run; the previously documented original guest long-PATH issue remains.

## Follow-up P — relocate the NTCON library tree

The owner directed the whole `src/ntcon-exe/lib` tree to `src/lib` without
a new suitability audit. All 55 tracked files, including the four SoftPC
modules, both license notices and the libvterm archive, match their previous
Git blob IDs byte for byte. The nxvm import manifest retains every source hash;
the libvterm manifest points to the relocated archive and notice. NTCON's
product-specific wrappers and import manifests remain in its component.

The x86 reference graph was regenerated using
`tools/build/New-T310OriginalSoftpcNinja.ps1`; the x64 frontend graph was
derived using `tools/build/New-NativeWorkerNinja.ps1 -Component Frontend`.
The x64 `ntcon.exe` linked successfully. The initial wrapper then failed because
it named the x86 Ninja script `run-ninja.cmd` rather than
`run-ninja-parallel.cmd`; the corrected invocation built and passed the six
`frontend-window-{library,controller,keyboard,mouse,lifetime,payload}-test.exe`
fixtures and `test/component-integration/verify-frontend-link-ownership.ps1`
including its negative controls. Earlier diagnostic graph generation with x64
as the reference failed because that generator expects x86; no product failure
was inferred. Run logs, PIDs, times and exit codes are in `build/t474-s3-p2`.

The candidate package uses the newly built NTCON and nine S3 build outputs;
each manifest input and stamped output hash was verified. The 63 immutable guest
files and 274 NTVDM export ordinals passed their existing checks.

The complete Control suite passed all 12 selected gates on the same ten-image
package, including real Console input, window, worker failure and nested
handoff. The external DOS4GW/NESticle fixture was unavailable and skipped;
Full was not run. Package `e719bbee-ba87-42c4-8438-80f0d62fdd0a` is
published only to this worktree's `assets/release`, with all ten output hashes
verified. The previous original guest long-PATH limitation remains.

## Rebase onto main

Owner directed rebase onto main commit `b919dd182`, retaining this worktree's
status record in conflicts and replacing both pre-rebase release packages with
a fresh build of the combined committed sources. Main's uncommitted work is
not included. The six local commits were replayed and the active S3 retained.

Rebase exposed an old-path `.gitattributes` entry: Git converted line endings
of the moved nxvm sources and the manifest hash gate rejected the first graph
generation. The exact Git blobs were restored and attributes now preserve the
new imported paths on future checkouts. All 55 imported files remain byte-exact.
The first orchestration attempt also misclassified already-exited processes as
failures; a corrected owned-process wait verified all six native exits as zero.
An x86 target spelling mismatch was corrected before compilation. Full run
records, commands, start times, process IDs and exit codes are retained under
`build/t474-s3-rebase`.

Fresh x64 worker, frontend, monitor, launcher, service and Hook64, plus x86
NTVDM, VDMREDIR, Hook32 and WOW32 were built from the rebased sources. Frontend
and BaseSrv ownership gates, their negative controls and the window library
fixture passed. All ten stage input and output hashes match the fresh build;
63 immutable guest files and all 274 NTVDM exported ordinals passed checks.

Control passed all 12 selected gates on this exact ten-image package, including
actual DOS and native command interaction, worker loss and nested-window
handoff. The optional DOS4GW/NESticle fixture was unavailable and skipped;
Full was not run. The original guest long-PATH limitation remains. Fresh
package `9f45db63-8a83-44c4-84b7-f4a45fd9297a` was published to this
worktree's `assets/release`; all ten release hashes matched the manifest.
