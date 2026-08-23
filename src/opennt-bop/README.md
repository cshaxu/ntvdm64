# opennt-bop

Minimal-change mirrors of original OpenNT BOP providers, selector/service
routing and source-owned provider control flow. Preserve original names,
parameters, structures, ordering and observable failures.

It may call `opennt-host`, `adapter-win32` and `adapter-softpc` through their
declared interfaces; it must not touch Bochs objects, `bx-mantle` or `bx-core`
directly.

## Local intrusion register

Every edit to an imported BOP mirror is registered here with original path and
identity, exception identifier, necessity and focused verification. Each edited
source expression additionally carries a `DIVERGENCE:` comment naming the
replacement and reason. The future static library name is `opennt-bop`; its
public include surface is the BOP ingress/provider ABI consumed by `app`.
