# T484 S3 P67 frontend library producer

B0076 uses CMD target frontend-library and the existing worker-owned library
and keyboard test images. The runner no longer invokes cl/link or the font
producer itself. Native source manifests, profiles, library ownership and font
dependencies remain authoritative; no second test source graph is introduced.
The runner retains import membership/license hashes and all test assertions.
The obsolete requirement for a preconfigured developer shell is removed:
the CMD entry owns toolchain initialization and rejects x64 for this target.

The owned initial run passed both images, all82 imported file hashes and the
18 selected library units. Tests cover text/graphics frames, dual font banks,
CAF key records, source retirement, failed input sinks and Unicode handling.
These bounded tests do not replace actual Window17 guest input acceptance.
Raw logs and process identities reside in build/t484-s3-frontend-library;
concise delivery receipts are under artifacts/build-migration/t484/s3-frontend-library.
No original guest, production source or package configuration is modified.

Cached full build passed. No-op image hashes and timestamps remain stable;
x64 target rejection returns2. All11 host input and22 published image hashes
remain unchanged, so no recopy was necessary. Documentation governance passed.
Inventory:355 entries,139 completed dispositions,216 remaining (53 pending
producer chains and163 awaiting reconciliation). No new guest runtime pass is claimed.
