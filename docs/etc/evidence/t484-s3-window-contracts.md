# T484 S3 P29: native window contract producers

Seven interpreted builders are consolidated into CMD window-contracts and one
runtime-only runner. Eight native targets preserve their compiler/stack/LTCG
requirements, source owners and bounded runtime assertions. Font data uses the
existing native font producer and original ROM as read-only input, with declared
build dependencies. The frontend integration caller now selects Console state
through this runner. No product, mirror or original guest is changed.

The current presentation input function's old/new extraction is
[LF-identical](../../../artifacts/build-migration/t484/s3-window-contracts/projection.json).
Both [x64](../../../artifacts/build-migration/t484/s3-window-contracts/final-x64.json)
and [x86](../../../artifacts/build-migration/t484/s3-window-contracts/final-x86.json)
pass all eight runtime cases: input geometry, native sizing, carrier guard/order,
close priority, Console state, observed palette and ordinary Console capture.
These are native/bounded contracts, not real guest acceptance or universal
pixel-size claims.

Initial capture failed its zero-input-events assumption. Flushing stdout before
fail-fast preserved the child assertion, then event diagnostics established an
asynchronous120x9001 startup resize event, unrelated to the80x300 test buffer.
The [initial run](../../../artifacts/build-migration/t484/s3-window-contracts/x64.json),
[diagnostic run](../../../artifacts/build-migration/t484/s3-window-contracts/capture2.json)
and [actual event](../../../artifacts/build-migration/t484/s3-window-contracts/startup-event.txt)
remain recorded. No retry or delay was added to hide that race. Both variants
now check unchanged real geometry; the observed variant directly requires zero
size/window/InfoEx setter calls during four unchanged applications. This retains
the intended no-resize-feedback requirement without attributing unrelated
asynchronous startup input to those calls.

[Cached full build](../../../artifacts/build-migration/t484/s3-window-contracts/build-result.json)
and [11 raw/22 published hashes](../../../artifacts/build-migration/t484/s3-window-contracts/publication.json)
pass unchanged; no publication recopy is needed. The integration caller's
[single-case selection](../../../artifacts/build-migration/t484/s3-window-contracts/selected-console.json)
also passes with explicit script exit0.
S3 remains active; S4/S5 are pending.
