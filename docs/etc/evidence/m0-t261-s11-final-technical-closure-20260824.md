# M0 T261 S11 technical closure — OpenNT-host original-layout recovery

## Outcome

S11 completes the admitted OpenNT-host original-layout recovery and
composition work. `opennt-host` now contains only re-rooted OpenNT host source or
explicitly registered, source-shaped recovery seams; source-owned BOP logic,
session composition and mechanical conversion no longer masquerade as host
providers.

## Delivered closure

- The byte-identical PIF header/source triplet is retained at its original
  relative `softpc.new/host/...` paths.
- Reached original host bodies are re-rooted: `nt_error.c` and the VDMREDIR
  `vrinit.c`, `vrnmpipe.c` and `vrputil.c` helpers. Every required host change
is registered once as `HOST-DIV-012..023` in `opennt-host/README.md` and
locally source-marked.
- Drive filtering/private namespace, generic host configuration, XMS,
  Redirector and top-level shims were deleted or moved to their original BOP,
  adapter or session owner. `opennt-host` has no `*_shim` production path.
- `adapter-bop` and `adapter-softpc` are selector/service/family/DOS/WOW
  blind. The last INT06 interpretation moved to `opennt-bop`; test-only
  controls reside below `tests/`.
- The reached `UMBNotify(UCHAR)` fragment has its original empty notification
  result. Its sole modern-build adjustment is the registered `BOP-DIV-081`
  `(void)code`; it does not claim UMB allocation or device initialization.

## Evidence

- [S8 final host-exception audit](../etc/evidence/m0-t261-s8-final-opennt-host-exception-closure-001.md)
- [S9 adapter-residue closure](../etc/evidence/m0-t261-s9-final-adapter-residue-closure-001.md)
- [S10 residual-source audit](../etc/evidence/m0-t261-s10-configuration-residual-source-truth-audit-001.md)
- [S11 manifest/exception audit](../etc/evidence/m0-t261-s11-opennt-host-final-manifest-exception-audit-001.md)

The existing formal Ninja graph executed its affected compile/link closure and
ended with `ninja -n all` reporting no work. No `bx-core` source was compiled
by the final S10/S11 closure. Documentation governance, component manifest,
production-tree and generic-UD boundary gates passed.

## Explicit boundary and task state

The current `t245` configuration fixture reaches its subsequent original
`demDasdInit` FDC/INT13 tail and exits `2`; the current `t251` fixture exits
`8` in its later BOP-owned mailslot/guest-memory tail. Both are retained,
separately owned `opennt-bop`/machine work—not `opennt-host` source-layout
failures and not claims of passing functionality.

The documentation gate requires an active `M/T/S` packet, so S11 remains the
active technical-closure packet until the owner admits the next Queue
candidate. This evidence does not allocate that next T.
