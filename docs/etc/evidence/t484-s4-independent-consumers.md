# T484 S4 independent consumer reconciliation

P8 closes two remaining inventory classifications without rewriting their
implementations. Scope follows the active plan: standalone assertions and
maintenance may retain their language, but required producers may not.

## Clear-BuildOutput

The complete script inventories and previews exact owned output, verifies an
ownership manifest and quiescence attestation, then optionally deletes reviewed
build children. It does not compile, generate configuration, stage packages or
publish images. Caller review across src/tools/test, CMD, CMake and PowerShell
finds only Test-BuildHygiene as an executable caller. Execution rules name it as
the manual maintenance owner; ordinary or fresh-directory CMD construction does
not invoke it. Keep this existing safety boundary as independent maintenance,
not a CMD wrapper around an interpreter. It is not obsolete and is not a
prerequisite for S5 interpreter-free construction.

## Broker I/O handoff

verify-broker-io-handoff has no local compiler or runtime artifact producer.
Its one package preparation call is New-PhysicalRuntimeStage, migrated to CMD
in P6. Remaining operations observe PE identities, set the test process
environment, start the existing observer, assert actual CMD/DOS output and exit
status, write evidence and clean owned processes. This is an existing Full gate;
it is retained rather than duplicated or removed.

## Evidence

build/t484-s4-independent-consumers records PID21876,2026-11-24T16:39:19Z to
16:39:41Z,exit0. The existing Test-BuildHygiene passes28 assertions, including
all-target preflight, traversal/cache/Git/media/junction refusals, untouched
junction target, reviewed deletion, native cache policy and owned-prefix cleanup.
The three real I386 broker cases all pass: native, nested-console and
nested-window. They assert delivered input, return code23, actual DOS/MEM output
and parent resumption; the window case also verifies CAF ownership. Tests use
the full Userenv current-token environment and private desktop, not Computer Use.
These results do not claim the separate owned-visible-console variant or AMD64
CMD branch was rerun. No product or test source changed in this classification
batch; no new build, publication or Full runtime result is claimed.

## Accounting

Clear-BuildOutput becomes retained-standalone-audit (maintenance); broker handoff
becomes retained-native-delegate. The frozen census becomes332/355 disposed,
23 pending:22 producer entries and1 reconciliation entry (ProductVerification).
The24 overlapping supplemental chains remain pending. S4 remains active and
S5 clean interpreter-free construction is not yet complete.
