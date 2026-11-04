# T475 S2 — NTCON implementation ownership

The owner subsequently expanded S2 to all product components; see the
[component dependency inventory](m0-t475-s2-component-dependency-inventory.md).

## Admission and initial inventory

The owner closes T475 S1 locally and directs the next startup-order component
review. This S audits other production components' dependencies on NTCON-owned
implementation; common runtime protocol edges are not static source ownership.
Its baseline is commit `6efc36043` and local package
`2b1325bd-17fb-4435-b2be-55995de325f6`. Main integration and Full remain pending.

The preceding T474 S1 Run16 review retained launcher-specific implementation,
removed the unused NTCON launcher link and Hook32 Run16 classification link,
and placed Hook-specific classification with Hook. Focused builds, tests and
ownership negative controls passed. Local ten-image package
`4249ef52-c3d3-4954-bfe9-c375ef046e1f` had matching release hashes; three
images were freshly built and seven unchanged verified baseline images reused.

On the committed baseline, `ninja -C <graph> -t inputs <target>` was inspected
for NTCON source paths in thirteen targets: worker-x86 NTVDM, Hook32, VDMREDIR,
NTVWM, Run16, NTSRV and NTMON source closures; the native NTVWM, Run16, NTSRV,
NTMON and Hook64 graphs; and the WOW32 graph. All thirteen had zero leaf inputs
under `src/ntcon-exe`. This is an initial build-graph observation, not a complete
dependency or behavioral closure: generated sources, headers, selected archive
members, alternate graph variants and actual use still need review.

The native worker graph includes `ntcon-exe/console_video.c` in
`ntvwm-presentation-test.exe`, while the native launcher graph includes
`ntcon-exe/session_service.c` in `frontend-scope-lifetime-test.exe`. These are
test targets, not proof of production links. NTVWM `text_frame.c` documents a
historical glyph conversion recovered from NTCON; its present source does not
establish an active NTCON source edge. Classify its current behavior and any
real overlap separately before proposing a shared owner.

Continue the source/header, production-symbol, alternate-profile and runtime
protocol inventory before concluding the S or changing implementation.
