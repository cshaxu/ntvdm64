# M0 T273 S2 P1 Bochs source recovery

## Delivery

The audited, quarantined source candidates are now copied into their formal
owners without using `src.old/` as a build input:

- 205 non-README `bochs-core` candidates are restored under their matching
  upstream-relative paths.
- 15 private `bochs-core-overlay` implementation files and its exception
  register are restored. The overlay remains callable only by `bochs-core`.
- 10 non-README `adapter-bochs` assembly candidates are restored.

The recovered Bochs set is the S1-audited minimal closure candidate: 177 exact
upstream files and 28 registered differences, no path absent from the pinned
Bochs 2.6 baseline. The 28 differences retain their existing local
`DIVERGENCE` markers and `bochs-core-overlay` bodies; this delivery introduces
no new source divergence.

## Boundary review

`adapter-bochs` uses Bochs headers/types only. Its opaque callback and
machine-facade names denote mechanical copied-record or CPU/RAM/timer actions;
they do not decode a selector or contain OpenNT/DOS/WOW/SoftPC/CCPU/Win32
provider code. S2 P2 will reduce the admitted object list to the minimal
construction/reset/run subset before any fixture claims success.

## Deliberate limitation

This is source recovery, not a build or runtime completion. No legacy Ninja
graph, object, archive, executable or `src.old/` path was reused. P2 owns the
first new-root object/link closure and must report every unresolved object edge
rather than importing a product shell.
