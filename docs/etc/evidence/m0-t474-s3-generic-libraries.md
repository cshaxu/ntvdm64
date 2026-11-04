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
