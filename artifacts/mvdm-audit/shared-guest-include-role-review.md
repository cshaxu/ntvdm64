# Shared guest declarations: exact include ownership

This S10 checkpoint compares against `8197f9de3`. It reviews all **104**
then-pending `.h`/`.inc`/`.mac` files in `wow16/inc`, `dos/v86/h` and
`dos/v86/inc`, across four fixed disjoint shards. The proposal tables are
[shard 0](shard-0/shared-guest-include-owner-review.csv),
[shard 1](shard-1/shared-guest-include-owner-review.csv),
[shard 2](shard-2/shared-guest-include-owner-review.csv) and
[shard 3](shard-3/shared-guest-include-owner-review.csv).

## Accepted physical file roles

The [44 dispositions](shared-guest-include-role-dispositions.csv) resolve
14/11/7/12 paths respectively. Each has a positive original include or
explicit prerequisite, a resolved original include root and a guest target.
They are excluded as physical historical guest build inputs, not as APIs,
data contracts, algorithms or runtime capabilities.

For DOS this includes actual MEM/SETVER C consumers through CL16's shared
header root, and original MASM prerequisites in COMMAND, KEYB and NTIO.
WOW chains include DDEML, TIMER, USER, kernel31, WRITE and WFWNET. A default
or conditional recipe can establish a file role without proving a particular
installed image was built that way. KOREA, debug, !MSDWP, external INCLUDE,
resource and assembly conditions remain explicit, not silently removed.
An explicit build prerequisite is recorded as such; it does not masquerade
as proof of an executed preprocessor branch.

Native code often spells the same SDK/CRT basename. The selected worker/WOW
graphs' explicit `/I` and `/FI` roots do not name these three guest include
directories, and the fresh dependency snapshots contain none of the 44 exact
paths. That distinguishes the observed native SDK resolution from the
positive guest chains. It does not prove every possible qualified include,
alternate environment or future profile is unable to use a guest file.
Snapshot absence alone never supplies an exclusion.

## Concrete boundaries retained pending

The other **60** files stay pending in S10. There are real local shadows:
WRITE has its own `dbcs.h` and `debug.h`; MMSYSTEM has a local `int31.inc`.
The shared `int31.inc` is instead proved through TIMER/startend, whose local
copy is absent. WRITE's quoted DBCS includes cannot prove use of the shared
copy. A ToolHelp CLASS1 reference to `testing.h` also does not prove a selected
target when the matching rule is commented and the active object list omits it.

Other unused CRT/MASM declarations need a positive original SDK/tool role or
an actual resolved consumer. Local/shared name collisions, unresolved library
owners and alternate resource/profile edges remain per-file questions.
These are not demonstrated runtime defects and do not authorize replacing
original guest declarations or weakening their host ABI counterparts.

## Verification

The [observation manifest](shared-guest-include-role-observations.json)
binds all four proposal tables, fresh worker/WOW input and dependency outputs,
and the actual graph identities. All four read-only Ninja observations
(PIDs 33468, 26968, 5164 and 12688) exited 0. The reused source-role checker
verifies the exact 44-row delta, all 104 reviewed paths and fixed shard
membership, original/frozen identities, unchanged unrelated records, full
source invariants and the existing six source drifts.

```text
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py build/t473-s10/shared-guest-include-owner/checkpoint.json --base 8197f9de3 --prefix shared-guest-include-role- --expected-count 44
```

Run in the required owned background context. The
[result](shared-guest-include-role-verification.json) is role/identity evidence,
not guest execution or a restored-code percentage. No source or guest bytes
change; no new mirror exception is introduced. Ledger counts become
**1,890 pending, 2,506 excluded, 236 partial and 52 full**. S10 remains active.
The separate current-runtime build/product verification does not turn any
pending source row into full, and this audit does not certify that delivery.
