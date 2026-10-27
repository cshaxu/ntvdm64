# Independent review of guest source roles

The completed shard-1 auditor performed a separate read-only review of the
main-session 187-path disposition, scope statement and shard-0 edge table.
This review did not change product, guest or another shard's classification.

The reviewer independently recalculated all 187 current and referenced
original hashes and found them equal to the proposed table. No
mirror-different file was included among the exclusions. The reviewed
COMMAND/WRITE chains and shard-0 direct include statements were consistent
with a historical guest build-input role, rather than merely a nearby
filename or commented reference. This does not establish all configuration
branches, binary provenance or runtime functionality.

One precision issue was found and corrected before delivery:
`wow16/inc/windows.h:5691-5696` includes `windowsx.h` only when both
`NOWINDOWSX` and `RC_INVOKED` are undefined. The exact condition is now
recorded on the `windowsx.h` source-edge row and the derived disposition.
It leaves the historical file role unchanged but prevents interpreting the
include as active under every configuration. Other source/resource condition
limits remain in the per-shard evidence and the combined scope statement.

The [checkpoint verifier](guest-source-role-checkpoint.py) compares all 4,684
merged rows and all four shard ledgers to `d050fa4b4`, requires exactly these
187 pending-to-excluded changes, and verifies unchanged unrelated rows,
original bytes, recorded source tables, observed host inputs, full-file
evidence and the six previously recorded live source drifts. Its
[result](guest-source-role-checkpoint-result.json) is ledger/source evidence,
not a guest execution or capability test.
