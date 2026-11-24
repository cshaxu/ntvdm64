# T484 S3 P167 current external-message package contract

Retire test/observation/verify-wow-callback-image-root.ps1 (36 lines). It has
no current test/tool caller, points to a nonexistent observation-local
verify-wow-callback-cost.ps1, omits its required Observer argument, and expects
a historical13-image error string and a distinct P9 candidate. This is a
stale diagnostic-input self-test, not the callback recorder or measurement
implementation. Those remain. No claim is made that current tests reproduce
all four deleted historical rejection cases.

The retained verify-wow-external-native entry also had obsolete13-image and
P13 ZIP identity constants. Read the approved guest archive hash from
assets/winnt-guest-manifest.json and derive product names/count from the
current release manifest. JSON candidates require the exact current name set,
uniqueness and per-image hashes. Existing CSV root/hash checks remain, with
the current component count replacing13. No original guest/archive bytes or
product sources change.

Actual run: build/t484-s3-external-current, supervisor20400, child13924.
Normal-user-environment native reader; no environment values recorded.
The same current11-image package passed native x86/x64 compilation and actual
local, external-x86 and external-x64 Win16 message groups (nine reported cases),
including exact independently authored default NE identity checks. This run
does not exercise CrossWorkers or PendingSend; their assertion paths are
unchanged and are not claimed newly passed. Exact steps, inputs, guest reports
and exit codes are retained in external/steps.json and result.json.

The parent run also executes cached full and11 raw input/22 publication hash
checks. No changed product image requires republication. Temporary source
package is removed after termination; scoped guest stages are removed by their
owner. Documentation verification is in closure-result.json.

B0058 remains pending: default archive extraction/package assembly still needs
S4 native staging integration. The supplied-package path is now actually
verified; this checkpoint does not count it as a fully migrated entry. Original
355-candidate completion remains304, pending51. The retired self-test was not
one of those355 producer candidates; do not inflate completion counts.
