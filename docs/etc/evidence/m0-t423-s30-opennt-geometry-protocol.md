# T423 S30 OpenNT DOS return geometry and RPC revision

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Source decision

Original `mvdm/softpc.new/host/src/nt_fulsc.c::calcScreenParams` uses the
visible Console window height and integer `MID_VAL` thresholds to choose
exactly one 80-column VGA text mode: 22, 25, 28, 43 or 50 rows. The cutoffs
are 23, 26, 35 and 46 inclusive. Thus a 30-row native viewport returns to
DOS as 80x28, not 80x25. S13's retained-last-DOS-mode fallback was a
project-added policy and is removed. The original MVDM video code remains
unchanged; NTCON's host-buffer handoff now makes the same selection. Existing
`opennt_console_resize_grid` keeps the original no-reflow, cursor-containing
row-copy behavior. This is visible-screen preservation, not archival
scrollback.

The shared `APP_PROTOCOL_VERSION` was already 25, but `service.idl` still
declared RPC interface version 23.0. S30 updates the IDL to 25.0 and its
three generated interface-handle references in the project-owned NTSRV and
BaseClient glue. There is no DTO or method layout change in this packet.

## Verification completed

- Fresh S30 Ninja graph generated from 130 selected CCPU sources. MSVC x86
  `/MT` commands for 118 dirty graph steps were executed in graph order from
  the validated S28 object cache; MIDL regenerated `service.h`, `service_c.c`
  and `service_s.c` with `vdm_service_v25_0`. The seven changed production
  EXE/DLL targets were relinked. Ninja's subprocess-output wait stalled in
  this host, so the exact graph-emitted commands were executed directly;
  an earlier false `CreateProcess` was due to selecting the host `Path` rather
  than VS `PATH`. No source-level compile/link error remains.
- Focused `console-frontend-test.exe` passed all original-mode threshold
  boundaries, no-reflow/cursor cases and a real Console API fixture. Its log
  is `build/M0-T423/S30/formal/console-frontend-test.stdout.log`.
- All 17 ordinary Console and all 17 private-desktop Window product routes
  passed against the isolated candidate, checking real guest/native text and
  exit codes. Logs are `build/M0-T423/S30/logs/m0-t423-s30-console17-*` and
  `m0-t423-s30-window17-*`. Both `native-zero` intermediate line snapshots
  show the actual 30-row native viewport returning to `buffer=80,28`.
- The generated v25 client passed real NTSRV registration, role/generation,
  stream receipt, revoke, disconnect and reconnect checks. The standalone
  client result is in `build/M0-T423/S30/basesrv-positive-client.stdout.log`.
- `Verify-ProductVersions.mjs` passed five real RPC negative variants for
  both run16 and NTVDM: old app version, wrong application protocol, false
  server app reply, false server protocol reply, and old MIDL interface.
  Each failed before task delivery with 1306. Logs are under
  `build/M0-T423/S30/version-negative/logs/`. The known long-path startup
  error 161 was removed from the test by the approved temporary `Z:` mapping;
  `Z:` was removed afterward.

## WOW frontier and publication

The private-desktop WOW observer could not start either S30 or unchanged S28
in this environment: both WINMINE launchers exited `0xffffffff` before window
sampling. Those reports are retained as an unusable test condition, not a WOW
result. With the owner's permission, the same existing observer was run on the
current desktop using its new read-only `-VisibleDesktop` option. It changes
only the observer's desktop choice, never product inputs, guest media or
SYSTEM.INI. The candidate reached all three separate S29 frontiers:

| Guest | S30 visible-desktop observation | Recorded evidence |
| --- | --- | --- |
| WINMINE | Visible Minesweeper main window and resident NTVDM | `build/M0-T423/S30/logs/m0-t423-s30-visible-final2-winmine-windows.txt` |
| SOL | Original visible memory-error dialog, `WOWExecClass` resident | `build/M0-T423/S30/logs/m0-t423-s30-visible-final3-sol-sol-windows.txt` |
| WRITE | Original visible “Not enough memory for Write” dialog, `WOWExecClass` resident | `build/M0-T423/S30/logs/m0-t423-s30-visible-final3-write-write-windows.txt` |

SOL and WRITE were also observed under the unchanged S28 package on the same
desktop with the same worker-window snapshot probe; both produced the same
dialog class, text and resident-worker state. This is noninteractive frontier
evidence, not full SOL/WRITE acceptance or a new gameplay claim. The observer
timeout is intentional while the GUI dialog remains; it is not interpreted as
a successful process exit. A first visible-script attempt rejected valid
worker-window samples because non-UI threads reported desktop-enumeration
errors; the final script keeps strict private-desktop checks and ignores those
non-UI thread errors only for visible-desktop sampling.

After the gates passed, all eight files were backed up to
`build/M0-T423/S30/published-backup/`, copied from the tested
`build/M0-T423/S30/runtime-candidate/` package to `O:/winnt`, and checked
byte-for-byte by SHA-256. `WOW32.DLL` is unchanged from S28; guest media and
configuration were not modified.

| Published file | SHA-256 |
| --- | --- |
| `ntmon.exe` | `D8C635C391FB88E68D36486B3AE2CCB69CF7A8F3A827EFAE6157D7DCD42B6958` |
| `run16.exe` | `32FFC061B7FB44F91EFD4B3F3C10832849882A046C46A27E1CA0D263DE2F7DE7` |
| `ntsrv.exe` | `207365EB3F620DA3975B3D92D33CB2BF7F3FE1E54F6CE71406042ADF5D4006EF` |
| `ntvdm.exe` | `3EF7B9764BBC6BBCCED9D51AB4DBB92E5E67B69E07F5D294E4682D609962B9D0` |
| `ntw32.exe` | `226EA48B181018446F91E0481D86C08DD55CFBB76CC6D3C792960CAFEB360CFE` |
| `ntcon.exe` | `DF1A3BEF1D9720CC5E3CB9802BD0274E87268D7AA93FB29453C893E72ACFB8FF` |
| `WOW32.DLL` | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |
| `VDMREDIR.DLL` | `3F266DB9BC52F937F1786D89CFBE879E37E0B2077E67D731A05C555EC7CDAA5F` |

The broader `Verify-BasesrvProduct.mjs` harness could not relink its stale
`base-client-rpc-first_test` fixture (seven pre-existing unresolved
worker-base symbols); its freshly built real BaseSrv client was run directly
and passed. This harness limitation is not recorded as a pass. T423 remains
open for owner acceptance; S30 has no remaining implementation gate.
