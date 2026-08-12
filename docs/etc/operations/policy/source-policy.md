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
alternatives, and validation. Prefer isolated adapters and
build shims to editing historical code.

User-approved binary research artifacts must live below `artifacts/`, include
a source/role/hash/size/redistribution manifest, and remain outside source,
default build inputs, runtime discovery, and release packaging.
