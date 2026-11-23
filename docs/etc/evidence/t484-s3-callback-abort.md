# T484 S3 P72 native WOW callback abort diagnostic

B0099 no longer scrapes old Ninja command syntax or rewrites cl/link strings.
The CMD target wow-callback-abort uses the current original_wcall16 compile
profile and substitutes exactly that object in the actual WOW32 object order.
It uses the product import libraries, export definition and link libraries.
The existing diagnostic wrapper remains byte-identical and includes the full
original callback TU. The normal product DLL is never replaced or published
with diagnostic contents. Output copies retain the old runner contract.

The runner keeps provider/root validation, hashes, diagnostic-only manifest,
input hashes and failure receipts; CMD child ownership/deadline is explicit.
The historical Ninja parameter is accepted but unused. Both default native
provider and explicitly supplied provider build roots use the same target.
No mirror/guest/production source changes are part of this delivery.

Raw evidence: build/t484-s3-callback-abort. Concise receipts:
artifacts/build-migration/t484/s3-callback-abort. Product/diagnostic object
inventories prove the single replacement; compiled callback-frame unit tests
are separate from the SHLPROBE Preparation/Returned guest fault scenarios.
The latter are not run in this producer-only migration, and no fresh fault
runtime acceptance is claimed.

Final checks passed:229 objects in identical order with exactly one callback
object replacement, stable diagnostic hash/timestamp on no-op build, x64
rejection, callback-frame fixture, cached full build and governance. All11
raw host inputs and22 published images remain unchanged; no recopy required.
Fixed355 inventory:149 completed,206 remaining (50 pending producer chains,
156 awaiting disposition reconciliation).
