# M0 T280 S14 — find-list identity binding evidence

The original code stores each `PFFINDLIST` twice: an original `FFindId` search
identity and a raw pointer in DOS DTA/FCB memory. `FFindId` remains original
numeric data. The raw pointer is an opaque host identity and must be replaced
by a surrogate from the existing session `host_resource` mapping manager.

The ledger records all creation, validation and cleanup sites. A later
implementation changes only the source-facing storage/validation seam: it
publishes the native `PFFINDLIST`, stores its 32-bit surrogate, resolves it
before comparison, releases it before free, and treats stale/missing identity
as the original search failure. DTA/FCB copying is separately a bounded
guest-memory lease; no mapper instance is shared or newly created.
