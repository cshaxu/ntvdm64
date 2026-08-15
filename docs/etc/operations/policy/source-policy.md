# Source And Research Policy

OpenNT NT4 MVDM/NTDOS is the primary historical guest/service source base.
Bochs 2.6, from `O:\repos.external\bochs-2.6-compat\bochs-2.6`, is the
approved third-party guest-machine backend, subject to the pinned-import and
source-identity requirements in `design/CODING.md` and the adoption record.
This is internal research; distribution/license review is deferred until a
release is considered. Later
source trees, `ntvdm64`, PCjs, and similar projects remain
comparative evidence only and cannot become runtime/acceptance dependencies.

Every imported file, copied fragment, ABI conclusion, or historical-source
change records its source tree/path/revision, role, notices present, rationale,
alternatives, and validation. The mandatory recovery ladder is: reuse
independently composable OpenNT source; reuse it through the smallest adapter,
compatibility seam or build shim; use a registered exceptional intrusion into
adopted external code only when essential mechanics cannot otherwise cross the
boundary; and author new behavior only when no usable source remains. The
record must demonstrate the preceding rung was attempted or name its specific
unavailable dependency; a trace hit, a convenient Win32 API, a temporary
buffer limit, or an easier rewrite is not such a demonstration. A
source-derived rehost belongs to the adapter/shim step, not to free-standing
new behavior: it must identify the original owner, source path, retained
layout/order/failure rules and the reason the original translation unit cannot
be composed. Owner-approved product requirements may explicitly constrain this
order. Prefer isolated adapters and build shims to editing historical code.

User-approved binary research artifacts must live below `artifacts/`, include
a source/role/hash/size/redistribution manifest, and remain outside source,
default build inputs, runtime discovery, and release packaging.
