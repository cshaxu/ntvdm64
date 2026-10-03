# T423 S31 shared Console handoff

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Question and source boundary

Returning from DOS to an outer Win32 CMD left the Console at the DOS-sized
cell grid, but NTCON teardown forcibly restored the caller's startup buffer,
viewport and cursor. A 30-row caller could therefore regain an empty 26th–30th
row and a cursor at row 30 after DOS had ended on row 25 or 28. Original
OpenNT's ordinary `nt_block_event_thread(0)` / `ResetConsoleState()` route does
not restore the initial geometry; its special stream-I/O branch is distinct.
This S changes project-owned NTCON teardown only. MVDM mirrors, guest media,
protocol and shared libraries are unchanged.

## Implementation and focused proof

`native_console_frontend.c` no longer saves or restores the startup geometry
or cursor position. Teardown still reselects the canonical buffer, releases
the temporary Window surface, and restores input mode and cursor shape. It
leaves the final DOS cell grid and cursor in place. Related comments were
corrected without changing execution behavior.

The x86 `/MT` graph was regenerated from the S30 object cache under
`build/M0-T423/S31/formal/`. The changed Run16 and NTCON targets and the
Console-channel lifetime fixture were compiled and linked; unchanged package
members retained their S30 hashes. Ninja's subprocess-output wait stalled on
this host, so the graph-emitted build commands were executed in order by the
tracked serial-build helper. The real private-Console fixture passed for
80x30 → 80x25 and 80x30 → 80x28, checking cells, final cursor, active
canonical buffer, input mode and cursor shape. The full private-desktop
channel-lifetime fixture and the existing frontend-scope lifetime fixture
passed. Logs are under `build/M0-T423/S31/`.

The transcript verifier now merges ordered intermediate line snapshots for
two DOS/native/DOS routes. Shrinking the final cell grid legitimately removes
older lines from the final screen; the verifier still requires the same
markers, exact marker counts and expected exit codes. Earlier attempts using
only the final screen failed for that reason and are not counted as product
failures or passes.

## Product comparison and publication

The isolated coherent candidate passed all 17 ordinary Console cases and all
17 private-desktop Window cases in `Verify-CommandExitStatus.ps1`, including
COMMAND, MEM, EDIT exit followed by MEM, nested COMMAND, native handoff,
actual guest text and exit status. Supplemental DOS→native→DOS,
native→DOS→native and repeated native-root routes passed; their logs are
`build/M0-T423/S31/logs/s31-*`. A visible-desktop read-only observer reached
the S30 frontiers separately: WINMINE's visible main window; SOL's original
memory-error dialog with resident WOW; and WRITE's original “Not enough
memory for Write” dialog with resident WOW. These remain frontiers, not full
SOL/WRITE acceptance. GUI observation timeouts while those windows remained
open are not process-success results.

The S30 published eight-file package was copied and SHA-256-verified in
`build/M0-T423/S31/published-backup/`. The tested S31 candidate was then
published as one eight-file set to `O:/winnt`; all eight published hashes match
the candidate. Published-package smoke tests passed for `direct-mem`, `edit`
(EDIT exit followed by MEM), and `native-cmd-dos`. One attempted
`native-root-frontend` published smoke did not run because that case requires
an isolated test-only frontend observer; `native-cmd-dos` was used for the
published native handoff check instead. The isolated root-frontend test had
passed earlier. No guest/configuration file was changed.

| Published file | SHA-256 |
| --- | --- |
| `ntmon.exe` | `D8C635C391FB88E68D36486B3AE2CCB69CF7A8F3A827EFAE6157D7DCD42B6958` |
| `run16.exe` | `8AE9C02C975CE9618B1A3E126F16225657ED1A0801CD9DB481DDB2E7D1F02BF2` |
| `ntsrv.exe` | `207365EB3F620DA3975B3D92D33CB2BF7F3FE1E54F6CE71406042ADF5D4006EF` |
| `ntvdm.exe` | `3EF7B9764BBC6BBCCED9D51AB4DBB92E5E67B69E07F5D294E4682D609962B9D0` |
| `ntw32.exe` | `226EA48B181018446F91E0481D86C08DD55CFBB76CC6D3C792960CAFEB360CFE` |
| `ntcon.exe` | `A4BE7B4F12025BD88E4DA73E53C416EAD306121821264A4197269CF53C00BDAC` |
| `WOW32.DLL` | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |
| `VDMREDIR.DLL` | `3F266DB9BC52F937F1786D89CFBE879E37E0B2077E67D731A05C555EC7CDAA5F` |

## Interpretation and follow-up

S31 removes a project-added teardown policy that contradicted the verified
ordinary OpenNT handoff. The automated Console API fixture proves the final
geometry/cursor invariant; the product suite proves no observed loss against
S30. It does not claim that the original outer CMD page or scrollback is
restored, nor does it prove every physical terminal's cooked-input behavior.
T423 remains open for owner side-test acceptance.
