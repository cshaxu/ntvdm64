# Current bitmap binding resource lifecycle

The existing `verify-wow-bitmap-bindings.ps1` suite rebuilt against the
current selected WOW32 bitmap object and passed. The real GDI fixture covers
30 nested uses, four threads with 50 conversions each, named resources,
missing resources, GDI object counts before/after, and two shutdown calls
after initialization. Its failure fixture checks injected acquisition and
lock failures; the registration-surface check is an ABI check only. Source,
object and fixture hashes are in `inputs.json`; output and suite exit are
retained here.

This complements the real worker's [early empty-state unload](../wow32-empty-cleanup/review.md).
It does not prove a live worker explicitly unloads with initialized bitmap
resources, or cover populated registry-token cleanup. It changes no product or
guest file and does not replace the product Full gate.
