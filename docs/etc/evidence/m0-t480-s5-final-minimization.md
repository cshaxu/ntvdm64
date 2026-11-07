# T480 S5 final host mirror minimization

## Scope and findings

Owner admits and authorizes S5 through closure after465ba5dcb. Admission is
7f683ca81. Scope remains the1060 tracked C/H files in mvdm-host and non-mvdm;
no guest source, ASM, unsupported configuration or new capability is admitted.
Earlier T478 correctness repairs and completed S2/S3 restorations are not undone.

The final inventory verifies every pinned original SHA-256, exact tracked-file
coverage, current source identity, actual consumers and all9376 frozen S1 IDs.
There are581 text-different files;479 have no normalized-line difference. All
current host-source changes are accounted for by S2/S3/S5 receipts:40 files in
the union. No unaccounted drift or guest change was found.

## Six further original restorations

| File | Restoration | Preserved boundary |
| --- | --- | --- |
| public/sdk/inc/ntexapi.h | Original terminal `_NTEXAPI_` comment. | Same outer guard, directives and lines. |
| public/sdk/inc/ntregapi.h | Original terminal `_NTREGAPI_` comment. | Same outer guard, directives and lines. |
| public/sdk/inc/ntrtl.h | Original terminal `_NTRTL_` comment. | Same selected declaration cohorts and lines. |
| public/sdk/inc/nturtl.h | Original terminal `_NTURTL_` comment. | Same selected declarations and lines. |
| windows/core/ntuser/kernel/userk.h | Original terminal `!_USERK_` comment. | Actual matching outer guard was checked, not inferred from filename. |
| ds/netapi/rap/strucsiz.c | Original `RapStructureSize` closing comment. | No original blank line inserted; diagnostic mapping stays unchanged. |

All paths above are relative to src/non-mvdm. The
[restoration ledger](../../../artifacts/mvdm-audit/t480-s5-restorations.json)
contains exact line/text, original hash and before/after hashes. The applicator
checks all inputs before any mutation and preserves every other byte. These
six fragment restorations were not previously completed by S2/S3.

## Rejected mechanical leads

A separate non-greedy text alignment scan found5 one-line leads. All were false
owner matches: wuman's dereference was mistaken for a comment, and clres,
usercli, profile and taskman comment/history lines belonged to different
functions or declarations. Complete function context resolves them; original
xxxUserYield history already matches the current function.

A logical-line spacing probe found118 leads in44 files. These are not118 proved
safe edits. Repeated braces/returns and inserted adapter comments can share an
original logical coordinate without being the same original statement. Keep
current scope indentation for guest leases, callback lifetime, cleanup/finally,
native-child setup and corrected device paths. Examples:

- cmdexec SetStdHandle statements now sit inside the validated-handle block.
- text.c's extent calls now sit inside the existing cleanup-protected scope.
- nt_unix's strerror fallback is outside the original error-index conditional;
  restoring that conditional's indentation would describe a removed scope.
- sim32's matching comment terminator closes a new CPU40-boundary explanation.
- nt_sound's shifted introductory comment has no original mapping anchor there.
- vga_prts's assignment retains DIV-358's corrected scope; nt_fulsc retains
  selected scope around the explicitly excluded X86GFX body.

The [layout ledger](../../../artifacts/mvdm-audit/t480-s5-layout-dispositions.json.gz)
retains current/original contexts and conservative decisions; the
[alignment ledger](../../../artifacts/mvdm-audit/t480-s5-alignment-dispositions.json.gz)
records all5 false alignments. Preserving these candidates is not an assertion
that every whitespace choice is mathematically minimal. This task preserves
coherent code structure instead of aligning unrelated braces to reduce a metric.

## Final accounting and proof limits

[Complete final inventory](../../../artifacts/mvdm-audit/t480-s5-final-inventory.json.gz)
uses the same normalized-line SequenceMatcher(autojunk=True) method as S1:

| Measure | S1 | Final |
| --- | ---: | ---: |
| Files |1060|1060|
| Text-different files |581|581|
| Lexical edit blocks |9376|9330|
| Added lines versus original |33207|34899|
| Removed original lines |161677|163197|

The block count falls46, but total added+removed lines increases3212. The
restored and relocated Base environment owner changes the greedy alignment:
vdm.c alone rises3390 by that measure; other files together fall178. S5's six
same-line comments reduce changed-line accounting by12 while splitting one
block. These are honest text metrics, not defect counts or semantic distance.
The secondary autojunk=False scan reports12008 pre-S5 blocks and cannot be
numerically compared with S1. Do not choose whichever algorithm looks better.

S2 restored53 whitespace fragments in30 files. S3 restored3 expressions,
10 comment/spelling fragments, the RTL1970 owner and3 environment function
structures in13 files. S5 restores6 more comments. Union:40 files. No whole
unused differing file was established: every differing file has real consumers.

All frozen categories remain covered:3418 diagnostic-only,146 S2 whitespace,
2132 substantive and3680 mixed exclusion blocks. The5812 substantive/mixed
rows retain their S3 dispositions;3,641 remain conservative retentions without
an independent proof that each difference is necessary. S5 does not relabel
those rows as semantic PASS. Whole-owner review and measured safe reductions
support bounded closure, not a claim of global mathematical minimum diff.

Remaining difference families preserve actual contracts: selected CPU40 rather
than NT4 kernel/CPU30 execution, x86/x64 widths and calling conventions, bounded
guest-memory leases, callback/task ownership, modern native USER/CSR boundaries,
previously repaired original bugs, and explicit unsupported-branch errors/line
mapping. Missing runtime hits, a registry ID or a passing suite is insufficient
authority to undo them. Broader autonomous implementation redesign remains its
separate queued package.

## Historical verifier applicability

S2/S3/S4 source-hash checks intentionally bind their own accepted revisions.
S5 supersedes five previously checked file identities and adds nturtl.h to the
restored-file union. Replaying an old verifier directly on the new source can
therefore reject its hash, correctly. Do not rewrite old receipts or weaken
those assertions. The S5 inventory composes the exact stage chain and supplies
the final current-source acceptance; historical stage replay uses its recorded
revision. Runtime regression still exercises the current coherent package.

## Verification and delivery

The final-inventory verifier passed all1060 identities, exact baseline/current
coverage and historical classification coverage. All819 before/after consumers
have identical emitted text, tokens and logical source locations. Both coherent
architecture builds passed, followed by all16 Full gates and WINMINE interaction.
All11 tested image hashes match this worktree assets/release after publication.
The final package retains version0.0.480 and protocol67. No O:/winnt write or push.

`artifacts/mvdm-audit/t480-s5-delivery.json` and `t480-s5-verification.zip` retain
exact commands, supervisor identities, deadlines, results, package hashes and
source/consumer evidence. Final documentation governance and staged diff checks
are required before the local P commit. This closes the bounded T480 program;
its conservative proof limits above remain explicit rather than being promoted
to a claim of complete semantic minimality.

Checked-in entrypoints:

- `tools/audit/restore-t480-s5-comments.py` applies the sealed six-fragment plan
  only after successful baseline captures; pinned originals must be available.
- `tools/audit/verify-t480-s2-preprocessing.py` accepts the S5 plan/output root
  and runs each frozen real consumer under its matching VS environment.
- `tests/build/verify-t480-s5-comments.py build/t480-s5` requires exact consumer
  sets, commands, emitted text, tokens, logical locations and final hashes.
- `tests/build/verify-t480-s5-inventory.py --output build/t480-s5/<fresh-dir>`
  checks the final inventory against the pinned originals, Git and stage receipts.

All compiler/test invocations run through owned background supervisors with
PID, start time, logs and deadlines. No guest source or firmware is read by the
inventory. Product tests use only immutable-media copies. DOS4GW/NESticle remains
the previously approved exclusion, never an invented pass.
