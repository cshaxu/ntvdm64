# T481 S21: launch tool consolidation and final audit

## Contract and implementation

Owner requires deleting standalone win-multitask, merging its responsibilities
into win95-launch using the existing win31-setup dynamic WIN.CMD model,
without unrelated Win31/Win101 changes, then delivering and closing T481.
Baseline: 843cae6b4. No product ABI, guest binary or mirror-source changes.

The old installer is superseded by the existing APPLY.CMD -> deployed
WIN.CMD --install route. Driver selection continues through that same WIN.CMD;
UNAPPLY uses --remove. Existing MULTITASK.INI.BAK from old installations is
restored locally before removal; no new legacy records are generated and no
second configuration parser is added. Normal backup restoration matches the
current UNAPPLY policy; no old HASH.EXE dependency remains.

Three standalone files are deleted. The old installer-only package test and
Win31 installer witness are retired with their obsolete interface; maintained
Win31 setup/profile tests are unchanged. The Win95 identity witness retains
InstallViaLaunch and drops the redundant InstallViaTool path. No active
script refers to tools/win-multitask or InstallViaTool afterward. Historical
investigation references remain historical, not executable consumers.

## Verification and limitations

Receipts: build/t481-s21/job. Cached x86/x64 compilation passed.
Focused tests passed: four-profile generation and repeated toggling, dynamic
removal with byte-equivalent unrelated INI content, existing restoration,
legacy original-INI restoration, failed preparation rollback and command safety.
These script fixtures use a Run16 shim and do not prove guest execution.
The first profile attempt used an overlong generated PIF path and failed;
rerunning with the established short build/s21p fixture convention passed.
The failed receipt is retained, not counted as a product pass.

Full passed all 16 selected gates in 347727 ms (job/full-2.out,
full/timings.json); external DOS4GW/NESticle remained an explicit fixture skip.
Final documentation governance and self-test passed (job/final.out). All 11
published image hashes match the frozen package at both destinations.
Win31-setup and Win101-setup trees are identical to the baseline. Original guest, assets/winnt.zip and installed Windows trees
are not changed. The Win95 Setup proposal is queued, not implemented.

The final T closure preserves the confirmed wrong-waiter identity defect,
Win95 exit code 1 boundary and original guest limitations. No repair is
claimed for these by deleting a tool or its obsolete installer tests.
