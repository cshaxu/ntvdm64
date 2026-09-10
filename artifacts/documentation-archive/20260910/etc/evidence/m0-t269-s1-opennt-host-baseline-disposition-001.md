# M0 T269 S1 — OpenNT-host baseline and disposition

## Question

What is the source-identity and mirror disposition of every current production
unit in `src/opennt-host` and its private overlay, and which owner boundary
must be repaired before the component can be declared compliant?

## Inputs

- Current repository commit `5ad8f4c1` and `src/opennt-host{,-overlay}`.
- Selected original tree `O:\repos.external\OpenNT`, revision
  `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`, rooted at
  `base/mvdm`.
- The mirror-component review standard in `docs/rules/CODING.md`, the current
  host and overlay READMEs, and the T269 proposal.

## Procedure

1. Enumerated every non-README production file below both component roots.
2. Mapped each host unit to the same relative path below the selected original
   `base/mvdm` root; calculated SHA-256 and `git diff --no-index --numstat`.
3. Inspected every changed hunk for a retained original call shape, crop
   boundary and local `DIVERGENCE:` marker; reconciled those identifiers with
   the host README.
4. Searched current source, tools and tests for private overlay consumers and
   for host source/body consumers outside the component.

## Disposition ledger

| Current unit | Selected original / baseline hash | Current hash | Disposition | Difference | Registered divergence / next action |
| --- | --- | --- | --- | --- | --- |
| `inc/oemuni.h` | `base/mvdm/inc/oemuni.h` / `26c850...2bafb` | `26c850...2bafb` | Exact original | 0 / 0 | None. |
| `inc/vrnmpipe.h` | `base/mvdm/inc/vrnmpipe.h` / `06e77e...60e40` | `06e77e...60e40` | Exact original | 0 / 0 | None. |
| `softpc.new/host/inc/error.h` | same relative path / `40953f...16ab3` | `40953f...16ab3` | Exact original | 0 / 0 | None. |
| `softpc.new/host/inc/nt_pif.h` | same relative path / `a8069d...b8d09` | `a8069d...b8d09` | Exact original | 0 / 0 | None. Its direct textual inclusion by an OpenNT-BOP command composition is an S3 owner-edge finding. |
| `softpc.new/host/inc/nt_uis.h` | same relative path / `fc00f8...c71e` | `fc00f8...c71e` | Exact original | 0 / 0 | None. |
| `softpc.new/host/src/nt_error.c` | same relative path / `3305ea...93228` | `f84e42...49e17` | Same-shaped original subset | +18 / -1127; 81 retained lines of 1,190 baseline lines | `HOST-DIV-012/013/014/026` are registered; S2 must make each complete crop boundary and retained failure rule explicitly auditable. |
| `softpc.new/host/src/nt_pif.c` | same relative path / `4ae2c1...262bc` | `4ae2c1...262bc` | Exact original | 0 / 0 | S3 must replace its direct cross-component `.c` inclusion with an owner-correct composition boundary or transfer disposition. |
| `vdmredir/vrinit.c` | same relative path / `088ecf...48273` | `6a5f7f...000d9` | True original subset with same-shaped binding | +4 / -675; 77 retained lines of 748 baseline lines | `HOST-DIV-017/022` are registered. S2 reviews the crop as an independent, provider-only subset. |
| `vdmredir/vrnmpipe.c` | same relative path / `fbb098...60e40` | `b31bcc...435b5` | True original subset with same-shaped modifications | +97 / -2497; 1,012 retained lines of 3,412 baseline lines | `HOST-DIV-015/016/020/023/024/025` are registered. S2 must verify every deleted named-pipe/async block is completely unreachable under the declared synchronous provider profile. |
| `vdmredir/vrputil.c` | same relative path / `1440cd...6fe16` | `73dd82...f7be1` | Same-shaped original/subset modification | +12 / -16; 190 retained lines of 194 baseline lines | `HOST-DIV-018/019` are registered; the changes are limited to unavailable product includes/diagnostics and x64-safe equivalent expressions. |
| `vdmredir/vrputil.h` | same relative path / `39f3e5...3c84` | `39f3e5...3c84` | Exact original | 0 / 0 | None. |
| `../opennt-host-overlay/softpc.new/host/src/nt_error_bindings.h` | No standalone original; private binding for original `nt_error.c` | Project-authored | Private overlay | Not compared as a mirror | `HOST-DIV-012/014`; only included by the corresponding host mirror. |
| `../opennt-host-overlay/vdmredir/vrinit_bindings.h` | No standalone original; private binding for original `vrinit.c` | Project-authored | Private overlay | Not compared as a mirror | `HOST-DIV-017`; only included by the corresponding host mirror. |
| `../opennt-host-overlay/vdmredir/vrnmpipe_product_seams.h` | No standalone original; private binding for original `vrnmpipe.c` | Project-authored | Private overlay | Not compared as a mirror | `HOST-DIV-024/025`; only included by the corresponding host mirror. |

Hash abbreviations above retain the leading and trailing source identity from
the full SHA-256 values captured during the procedure. Exact identity can be
reproduced from the selected paths with `Get-FileHash -Algorithm SHA256`.

## Observations

- Seven of eleven host source units are byte-for-byte selected originals.
- All four changed host bodies preserve an identified original file; none is
  an untracked self-authored body. Their current markers and README entries
  agree by identifier.
- The three overlay fragments are private: current source consumers are their
  respective `opennt-host` mirrors only. The production-tree governance tool
  references the root for ownership enforcement; it is not an implementation
  consumer.
- `src/opennt-bop/command/opennt_pif_composition.c` directly includes
  `../../opennt-host/softpc.new/host/src/nt_pif.c`. This crosses the declared
  component build boundary even though the imported body is exact; S3 must
  give the parser a single owner and a non-textual composition boundary, or
  transfer that exact original under an approved owner disposition.

## Interpretation and confidence

The component has a complete source baseline, but it is not yet compliant:
the four non-exact units require S2's crop/boundary review and the `nt_pif.c`
textual inclusion requires S3 ownership repair. Confidence is high for file
identity and direct consumer facts because they are exact path/hash/diff
results; reachability of each deleted `vrnmpipe.c` async block remains an S2
source/ABI audit rather than a claimed conclusion.

## Follow-up

Admit T269/S2 only for the four changed files and three private bindings:
repair missing crop-register precision without changing reached semantics.
S3 then decides the `nt_pif.c` owner edge from original source and ABI facts.
