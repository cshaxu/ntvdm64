# DOS command source, build inputs and guest package identities

This shard-2 group covers the 13 pending DOS command paths recorded in the
[observer](dos-command-package-observer.py) and [receipt](dos-command-package-observations.json).
Each current source is verified against the original edition(s) recorded in
the frozen DOS16/WOW32 reconciliation. `append.exe`, `mem.exe` and
`nlsfunc.exe` are recorded only in OpenNT-4.5, not both editions; do not
invent an OpenNT match. All retain 100% bytes of the matching original, but
guest behavior restoration cannot be measured from these identities.

| Path | Proposed status and evidence | Remaining boundary |
| --- | --- | --- |
| `dos/v86/cmd/loadfix/LOADFIX.COM` | partial candidate; source-tree, ZIP and installed binaries match | Actual command behavior, error output, memory placement and runtime image selection. |
| `dos/v86/cmd/mem/mem.exe` | partial candidate; OpenNT-4.5, ZIP and installed bytes match | Actual DOS memory calculation, output, corner cases and runtime identity. |
| `dos/v86/cmd/qbasic/edit.com` | partial candidate; source-tree, ZIP and installed bytes match | Editor startup, input, files, shell transitions, error handling and normal exit. |
| `dos/v86/cmd/qbasic/edit.hlp` | partial candidate; source-tree, ZIP and installed resource bytes match | Actual help resolution, encoding, navigation and lifecycle. |
| `dos/v86/cmd/keyb/kb16.exc` | excluded candidate: historical guest build intermediate | Original makefile links KB16.EXC then converts it to KB16.COM; the latter's guest keyboard behavior remains in scope. |
| `dos/v86/cmd/mem/mem.skl` | excluded candidate: historical guest build input | Original makefile builds `mem.ctl` from skeleton and locale messages; actual MEM output remains in scope. |
| `dos/v86/cmd/command/readme.txt` | excluded candidate: historical source documentation | Describes source layout, not an executable owner; COMMAND behavior stays in scope. |
| `dos/v86/cmd/exe2bin/exe2bin.tag` | excluded candidate: historical change log | Records original code history, not the EXE2BIN implementation. |
| `dos/v86/cmd/keyb/keyb.tag` | excluded candidate: historical change log | Records original keyboard changes; it is not a keyboard capability exclusion. |
| `dos/v86/cmd/setver/setver.tag` | excluded candidate: historical change log | Records original SETVER changes; version compatibility remains in scope. |
| `dos/v86/cmd/append/append.exe` | pending | Original OpenNT-4.5 image is retained; current ZIP has no same-name member. Other search paths and intended packaging/behavior remain unresolved. |
| `dos/v86/cmd/nlsfunc/nlsfunc.exe` | pending | Original OpenNT-4.5 image is retained; current ZIP has no same-name member. Locale functionality, alternate files and supported packaging require proof. |
| `dos/v86/cmd/qbasic/gorilla.bas` | pending | Historical QBasic sample; current ZIP has no same-name member. Decide sample/fixture scope without inferring QBasic or guest capabilities are absent. |

Same-name archive searches apply only to the captured ZIP, not arbitrary
media, installations, renamed files or dynamic lookup. Byte matches do not
prove source-to-binary derivation, actual guest execution or success across
platforms. No historical makefile was executed; some have destructive clean
targets. The owner prohibits modifying or rebuilding guest. The coordinator
must independently replay and review candidate dispositions before merging;
this checkpoint changes no shared ledger, product, guest or user session.
