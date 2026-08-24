# Legacy-code evidence

This tree retains source-shaped historical material that is useful for review,
provenance, or a later owner-approved recovery task but is **not** part of any
current production component or formal Ninja input.

It may contain immutable OpenNT/SoftPC originals, obsolete overlays,
uncompiled mirror fragments, import manifests and architecture-specific header
sets excluded from the CPU5/x86 closure.  It is never an include root, link
input, runtime provider or test source root.  The authoritative upstream
comparisons remain under `refs/opennt/` and `refs/bochs/`.

When a later task promotes an item from this tree, it must move it with Git
history into its single owning production component, identify the upstream
definition, mark every required source edit as `DIVERGENCE:`, and add one
corresponding row to that component's `README.md` register.
