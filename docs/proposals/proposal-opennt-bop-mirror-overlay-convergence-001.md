# Superseded Proposal — OpenNT BOP mirror and overlay convergence

This candidate is subsumed by **M0 T270 S4**, within the owner-directed
mirror-component conformance reconciliation task. It remains as evidence of
the prior BOP-specific scope and must not be admitted independently.

## Objective

Bring `opennt-bop` into conformance with the mirror-component review standard
before new BOP owner-package recovery resumes. Every retained production file
must be an exact original, an explicit true subset, or a same-shaped local
modification; project-authored bodies that exceed the permitted mirror
threshold belong in `opennt-bop-overlay`, not in the original-code mirror.

## Scope

1. Inventory all live `opennt-bop` production source, headers and build inputs
   against their pinned OpenNT originals.
2. Measure non-exact mirror files and audit their local `DIVERGENCE:` markers
   and README registrations.
3. Move every body exceeding 50-percent divergence, or each new semantic
   intrusion longer than three executable lines, to the matching path under
   `opennt-bop-overlay`.
4. Leave the smallest source-first, same-shaped call boundary in the mirror,
   preserving original service/provider ABI, ordering and reached failures.
5. Remove production reference/example/test material from component roots,
   assigning it to tests or `docs/etc/legacy_code`.

## Overlay boundary

`opennt-bop-overlay` is private to `opennt-bop`. Only `opennt-bop` may include,
link, or invoke it. It may not become a generic adapter, host-capability, BOP
ingress, or machine-mechanics owner. An unavailable historical Win32 or
SoftPC/CCPU interface must first use the same-shaped `adapter-win32` or
`adapter-softpc` route; an overlay body records why that route cannot express
the retained original logic directly.

## Exit criteria

- A committed, source-cited disposition exists for every live production file.
- Every non-exact file has local `DIVERGENCE:` markers and an `opennt-bop`
  README register entry, or has been relocated to the private overlay.
- No oversized self-authored algorithm remains in `opennt-bop`.
- Existing provider-route and negative-failure fixtures pass in the formal
  Ninja graph; the task adds no BOP feature or guest behavior.
- Documentation governance and `git diff --check` pass.

## Non-goals

- No new BOP service/provider implementation, selector interpretation change,
  or trace-driven endpoint work.
- No change to Bochs mechanics, OpenNT host capability semantics, guest images,
  or adapter ownership.

## Admission evidence

This is an unnumbered candidate task. It must be separately admitted as the
sole active numeric T/S packet after the preceding Bochs component convergence
task closes.
