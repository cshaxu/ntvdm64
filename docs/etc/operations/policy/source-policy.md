# Source And Research Policy

OpenNT NT4 MVDM/NTDOS is the primary historical source base. Later source
trees, `ntvdm64`, Bochs, PCjs, and similar projects are comparative evidence
only. They cannot be imported wholesale, built, linked, generated from, or
used as runtime/acceptance dependencies.

Every imported file, copied fragment, ABI conclusion, or historical-source
change records its source tree/path/revision, role, license or redistribution
status, rationale, alternatives, and validation. Prefer isolated adapters and
build shims to editing historical code.

User-approved binary research artifacts must live below `artifacts/`, include
a source/role/hash/size/redistribution manifest, and remain outside source,
default build inputs, runtime discovery, and release packaging.
