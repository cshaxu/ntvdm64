# DOS core, device and export-source checkpoint

This S10 checkpoint follows `d88b0fff8`. Its
[181-path disposition](dos-core-source-role-dispositions.csv) contains 179
historical guest build-input file roles and two partial export-source rows.
It does not claim 181 newly working functions or programs.

| Shard | Guest role exclusions | Source/build evidence |
| --- | --- | --- |
| 0 | 55 | [NTDOS, NTIO, ANSI, keyboard, COUNTRY, HIMEM and REDIR](shard-0/dos-core-guest-owner-review.md) |
| 1 | 52 | [Named objects, actual includes and supplemental ANSI messages](shard-1/dos-core-guest-owner-review.md) |
| 2 | 42 | [DOS/device/REDIR/HIMEM/XMM source and direct include chains](shard-2/dos-core-guest-owner-review.md) |
| 3 | 30 | [Actual guest link responses and supplemental CL2 message input](shard-3/dos-core-guest-owner-review.md) |

Every excluded current file matches a checked primary original or an
explicitly registered supplemental guest artifact. The
[supplemental provenance review](supplemental-original-provenance.md)
reconciles 1,013 current-only paths and does not grant runtime admission.
Four ANSI files require actual macro expansion chains rather than just
literal filename searches. MASM COMMENT blocks and commented keyboard layout
rules are not treated as active inputs. LANG/COUNTRY/layout, debug and source
conditions remain conditional.

Independent review rehashed all initial 175 source pairs and checked the
main-session include, target and inference edges. It confirmed ANSI's
`MSG_UTILNAME` invocation behind the macro-contained MSGSERV include.
Further per-owner review and the historical manifest/Git check established
the four additional ANSI artifacts. A real evidence error was corrected:
the NTIO recipe links `msbio_4.lnk`, whose mouse object differs from the
nearby `msbio.lnk`. All seven affected shard-3 rows were rechecked against the
actual response; no classification relies on the old wrong response.

These exclusions concern host implementation-input roles only. Guest DOS,
interrupt, XMS, device, input, locale and redirector behavior and actual image
provenance remain in S12–S17 acceptance. Shared MVDM-level DBGSVC/RDRSVC
declarations are not folded into this DOS-only group. Existing binaries,
unproved headers, unsupported/commented variants and remaining message chains
stay pending.

The [export-source review](export-source-recovery-review.md) identifies one
real supplemental mirror difference that is active configuration input and
one incompletely integrated primary-original template. The current-profile
name ratio is 138/140; it is not code or behavioral coverage. These rows are
partial and feed the finite S11 recovery group.

Current totals are 52 full, 228 partial, 1,932 excluded file roles and 2,472
pending. The frozen 4,684-path inventory is unchanged. Current-only identity
resolution does not by itself promote other files or close capability tests.
The existing checkpoint verifier is reused with mixed partial/excluded
transitions and admitted supplemental identity checks:

```powershell
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/dos-core-source-role-checkpoint-result.json --base d88b0fff8 --prefix dos-core-source-role- --expected-count 181
```

The [checkpoint result](dos-core-source-role-checkpoint-result.json) verifies
the exact delta, all shard joins, original identities, observed host-input
absence for excluded guest roles, source drift and unchanged unrelated rows.
Expected-count 180 is a deliberate negative and fails at the count assertion.
No original guest was modified/rebuilt, no product package was changed or
published, and S10 remains open.

Documentation governance and `git diff --check` passed. The
[background run record](dos-core-source-role-runs.json) retains owned process
IDs, commands where recorded and exact positive/negative exit results.
