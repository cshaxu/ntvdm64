# M0 T237 S3 — XMS regression and v1 retirement result

## Scope

S3 verifies the recovered OpenNT XMS package as a complete `52:00..0B`
owner package and removes the superseded handwritten XMS v1 provider.  It
does not make the one-MiB default machine claim an extended-memory aperture,
and it does not invent UMB or keyboard/INT15 behavior.

## Regression evidence

| Surface | Witness | Result |
| --- | --- | --- |
| Source allocation/free/reallocate/query | `t237-s2-xms-source-mirror-fixture` | Original `xmsblock.c` plus the original suballocator returns the source register results. |
| Checked descriptor move | same fixture | Original `xmsMoveBlock` control point uses the only x64-required divergence: checked guest-RAM read/copy/write. |
| Page-size and configured-memory query | same fixture | `52:04` and `52:05` execute the mirrored source bodies. |
| UMB failure semantics | same fixture | With no mantle UMB reservation map, original request/release return `B1`/`B2`; no bx-vdm UMB allocator exists. |
| Generic ingress | same fixture | Copied `C4 C4 52 05` resumes at `RIP+4` with source `AX=8192`. |
| A20 source success | `t237-s3-xms-a20-source-mirror-fixture` | Under a real 16 MiB minimal-machine lifecycle, `52:00` invokes original `xmsa20.c` for off/query/on/query and the mantle state matches each source result. |
| Unavailable source prerequisites | source fixture | `52:06` declines pending checked durable HIMEM-state/UMB mechanics; `52:09` declines pending the keyboard/BIOS-owned `UpdateKbdInt15` mechanism. |

`build/M0-T237-S3/002` was generated afresh from the formal full-module
manifest.  A selected-target build initially reused the A20 fixture object;
the subsequent clean full `all` build correctly exposed that the fixture had
relied on a transitive definition of `BX_NTVDM_CPU_EXECUTION_REAL`.  S3 added
the owning CPU-state ABI include, then the full formal `all` graph completed
and both XMS fixtures returned zero.  The prior fresh S3 graph likewise built
and ran the complete source-mirror fixture.  Thus the final claim rests on a
full graph, not on an incrementally reused target object.

## v1 retirement

The XMS v1 provider/session, XMS/DPMI combined classifier, their direct test
inputs, composition fixtures and obsolete probe have been removed.  DPMI's
still-needed historical classifier was rehomed under a DPMI-only name.  A
repository scan for `xms_package_session_v1` and `xms_dpmi_plane_v1`, outside
documentation and disposable build roots, returns no executable/test input.
`tools/governance/Test-T237S3XmsV1Retirement.ps1` makes this a repeatable
gate: it asserts the old provider/test/fixture paths are absent, rejects any
remaining v1 XMS provider identity, and verifies that the formal bx-vdm
manifest contains the complete source mirror but no `bop-v1` input.

## Independent mirror audit

The independent close review compared the original and mirrored function
declarations file by file.  The counts match: `xms.c` 1/1, `xmsa20.c` 6/6,
`xmsblock.c` 5/5, `xmsdisp.c` 1/1, `xmsmisc.c` 3/3, and `xmsumb.c` 5/5.
The original source dispatcher still owns all twelve service slots.  The only
non-owner references to selector `52` are generic historical catalog/ingress
metadata (excluded from the formal manifest) and selector-neutral observation;
the only executable product recognizer is `bop/xms_v2_generic_ud_bridge.c`.

## Result and transfers

T237 is source/package complete: all twelve slots have either the original
source body through an admitted mechanical seam or a visible owner-specific
decline.  The remaining work is deliberately transferred, not hidden:

- `52:06..08` success waits for a bx-mantle UMB reservation/lifecycle owner;
- `52:09` waits for the bx-mantle keyboard/BIOS INT15 owner;
- a real extended-RAM guest aperture and continuous NTDOS execution remain
  machine/guest integration work.

No statement here claims that an arbitrary guest program has reached or
completed XMS execution.
