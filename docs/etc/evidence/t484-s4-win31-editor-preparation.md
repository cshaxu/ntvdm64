# T484 S4 P37 Win31 editor and matrix preparation

## Scope and ownership

Continue the first DIR-ordered remaining caller, verify-win31-editors.ps1,
through its shared win31_runtime_matrix_test.ps1 dependency. No original
guest, production source, addon image or winnt.zip was changed.

Reuse guest-ini for literal set/append operations on one unique section,
with CREATE_NEW candidate output. Set requires one existing key; append
retains repeated device keys. Preserve unrelated bytes and the target line
terminator; reject ambiguous/missing sections or keys and newline injection.
No new native executable or second INI parser owner was added.

prepare-win31-editors.cmd owns PIF.EXE calls, COMMAND copy, optional MCALL,
empty save inputs and INI candidates. It now uses the current single WIN.PIF
instead of requiring historical WINSTD/WIN386 launch files. The real source
O:/w31 contains only WIN.CMD/WIN.PIF. DOSONLY selection reuses the matrix CMD.

prepare-win31-matrix.cmd owns authored witness binding/copy, configuration
snapshots, WIN.INI run, AUTOEXEC append, DOSONLY selection and byte-exact
restoration. CONFIG.NT is DOS text: FINDSTR selects standalone DOSONLY
lines and emits DOS line endings; snapshot restoration remains byte exact.
Invoke-Win31ProbeBuild delegates witness copies to prepare-test-inputs.cmd.
PowerShell retains input assertions, runtime observations, release signals,
evidence capture and owned cleanup. Its NASM path lookup is retained because
the subsequent matrix producer still needs that compiler environment.

## Evidence

Root: build/t484-s4-editor-preparation. Background process receipts retain
commands, start/finish times, logs and result artifacts.

- PID30800, 2026-11-24 21:56:39Z–21:56:47Z, exit0: expanded guest-ini build
  and contracts. CRLF/LF, Latin1 bytes, repeated device entries, literal keys,
  existing-output refusal, missing/duplicate sections and keys all pass.
- PID29052, 22:03:47Z–22:04:31Z, exit0: initial real-copy preparation contracts.
- PID31944, 22:05:59Z–22:08:26Z, exit0: final preparation contracts plus
  two real Win31 enhanced-mode runs, DOSONLY=0 and 1. Both report started,
  desktop-and-mode, normal-exit and pass true. This is the standard matrix
  witness, not the diagnostic two-editor controller. Normal Userenv and the
  existing private-desktop observer were used; no Computer Use.
- Final preparation asserts the existing editor INI transformations, COMMAND
  identity, A/B PIF targets, empty save witnesses, DOSONLY 0/1/0 selection and
  exact restoration of WIN.INI, CONFIG.NT and AUTOEXEC.NT. Original source
  files remain unchanged; owned O:/tmp/e31 and O:/tmp/r31 copies are removed.
- PID12784, 22:09:13Z–22:10:27Z, exit0: cached full product build, final
  guest-ini target and all 22 dual-location host release hashes. Unchanged
  product inputs/images justify reuse of the coherent published host package;
  test-only utilities are not product release artifacts.

Checked-in tests: test/observation/verify-guest-ini.ps1,
test/build/verify-win31-preparation.ps1 and the real shared runtime matrix.

## Limits and remaining work

This is a verified partial migration, not S4 closure. Keep the census at
348/355 disposed, 7 pending, with 5 overlapping supplemental chains.
The shared matrix still prepares/stamps its optional monitor through its
old direct PowerShell path. The diagnostic two-editor controller has not run
against a current compatible diagnostic package; ordinary product matrix
passes cannot substitute for that contract. These two callers remain open.
The other frozen callers and S5 tool consolidation/S6 clean acceptance/S7
mouse-dialog work remain in the execution plan. Do not count these shared
preparation primitives as complete caller migrations.
