# T423 S27 worker control-plane audit

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Source and ownership decision

This review compares the selected x86 product graph, not merely similar
function names. Original OpenNT DOS/WOW policy remains in `src/opennt-host`
and `src/mvdm`; no original mirror source was edited.

| Mechanism | Source and current owner | S27 disposition |
| --- | --- | --- |
| BaseSrv connection and broker-death watch | Project-added `src/worker-base/connection.c`, already linked into both workers | Keep one implementation in `worker-base`; both production entries call it. |
| NTCON worker channel handle ownership | Project-added `src/worker-base/console_client.c`, already called by NTVDM and NTW32 | Keep one implementation; the workers retain their distinct presentation/state code. |
| Native `GetNextNativeCommand`, completion and returned-handle disposal | Project-added code formerly placed in `worker-base/next_command.c`; only NTW32 called it | Move the implementation and its fixture into `ntw32-exe`, link NTW32 directly and remove it from `worker-base.lib`. This is not shared NTVDM behavior. |
| DOS/WOW `GetNextVDMCommand`, task records, re-entry and completion | Original OpenNT/BaseSrv and MVDM call shape | Keep in its original owner; NTVDM must not call through NTW32's native command wrapper. |
| Worker admission, process identity, reservation and death watch | Project-added NTSRV carrier around original DOS/WOW policy in `base_service.c` | Already one reservation-bound connection/watch path with an explicit worker-kind distinction. Do not add a second NTW32 registry. |
| Direct native target and result | Project-added NTW32 launch and Run16 direct target wait | Keep native `CreateProcess`/process-handle semantics; the broker authenticates the request and records the direct task but does not invent its exit code. |
| Frontend route and worker frame contract | Shared protocol in `src/interface`, client handle binding in `worker-base`, presentation in each worker, service/rendering in NTCON | Keep one protocol; do not move NTCON server state into either worker. |
| NTVDM CCPU, DOS guest and WOW local state | Original MVDM execution plus worker-local standalone adaptation | Retain locally. Similar lifecycle names are not grounds to move original execution into `worker-base`. |
| Native hidden Console, target launch and presentation | Project-added NTW32 local implementation | Retain locally; the NTVDM guest frame path has different producers and teardown. |

The mechanical cleanup removes **one falsely shared module** (two source files)
from `worker-base.lib`, not a duplicated implementation: zero runtime algorithms
were copied, zero original-image lines were added, and there is no claim that
source-line count decreased. `worker-base` now contains only the two mechanisms
actually consumed by both workers. Both production links and all fixture links
were updated, and no reference to the old symbol names remains.

## Verified candidate and limits

- Formal MSVC x86 product graph: seven EXEs/DLLs rebuilt; unchanged WOW32.DLL
  copied from the accepted S25 package to form the eight-file candidate.
- Focused `ntw32-next-command-test` passed. `ntw32-execution-lifetime-test`
  reported 448 checks, zero failures, 12 completions, 16 cancellations,
  target survival and zero remaining handles.
- The composition check `verify-frontend-link-ownership.ps1` passed after its
  owner assertion was updated to require the NTW32-only wrapper in NTW32 and
  only true common clients in `worker-base.lib`.
- `frontend-scope-lifetime-test`, `basesrv-service-reservation-test` and
  `ntw32-close-test` all passed. These cover authenticated inner join/route
  rejection, raced native admission, original DOS/WOW record completion and
  native Console-session close without transferring that ownership to NTCON.
- The isolated candidate, with `Z:` mapped **directly to its package root**,
  passed all 17 ordinary Console and all 17 private-desktop Window output/exit
  cases. Each run removed the `Z:` mapping afterward. A mapping to the repo
  root reproduced the project's known long-path failure on both S25 and S27;
  those runs are not product failures or acceptance passes.
- With the freshly compiled observer, the official WOW gate passed on both
  unchanged S25 and S27: WINMINE reaches its localized main window; SOL and
  WRITE retain their original out-of-memory frontiers. This is not full SOL or
  WRITE acceptance.
- The NTW32 presentation input-return fixture passed 677 checks with zero
  failures on an isolated desktop. The shared Console client fixture completed
  its expected original-shape close callback with test-only exit 73.
- The older `base-client-rpc-first-test --ntw32-execution` fixture failed at
  direct-command submission on **both** S25 and S27. Failure-only diagnostics
  identify `ERROR_TIMEOUT (1460)` before any target handle is returned. The
  fixture registers a frontend root but never attaches a live NTCON
  presentation route before submission; current NTW32 `begin_io` correctly
  requires that route before native `CreateProcess`. The legacy positive
  fixture therefore no longer supplies its own preconditions. It is **not**
  counted as a pass, and the 17+17 real-product cases, which do attach NTCON,
  are the current running-path evidence. Restoring the fixture's authenticated
  presentation precondition remains test debt; weakening production admission
  to make this fixture pass is prohibited.

Log prefixes under `O:/winnt/Logs2`: `m0-t423-s27-move-console17-c`,
`m0-t423-s27-move-window17`, `m0-t423-s27-control-wow-fresh-observer`,
`m0-t423-s27-candidate-wow-fresh-observer`,
`m0-t423-s27-presentation-*`, and `m0-t423-s27-console-client-observer`.

The candidate's eight product files were copied individually into `O:/winnt`
after verifying no published-package process was live; each destination SHA-256
matched its candidate source. The post-publication `native-zero`, `direct-mem`
and `edit` smoke cases passed. Candidate package hashes:

| File | SHA-256 |
| --- | --- |
| `run16.exe` | `302EE7C4435AE40898E8CF5AE4AE8A53057C966030B726541BFBBE1BAAC354C4` |
| `ntsrv.exe` | `071A72BC7497F6D41E0104A21A0E87D49D0D6BCD9C92246BC76DF4E6BC09DD79` |
| `ntvdm.exe` | `50C95CDD36356C6223905401033F03D0BBA4C6BB03C12DDA67B14DB17362AB51` |
| `ntw32.exe` | `5C59F2EEFEBF60FC9BF31640DE974FFF1DB5F9A67988363E4AC06A24E702F3AA` |
| `ntcon.exe` | `6621F49619625812DCB83AA0665F8673ACBE570ED6B7FCE7CE7160B523A61224` |
| `ntmon.exe` | `9C482AF71C46005210B2B2BA0D93131F7ED542096A707525EFF8A3C4255A5612` |
| `VDMREDIR.dll` | `93B4CD74242027DA8D4AB5319C28C3B68D2ACAE7017FFC54FF2E4E3779319E1C` |
| `WOW32.dll` | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |

No concurrent NTCON source change had appeared in the shared worktree at this
delivery boundary. Any later side-session change must be reviewed, rebuilt and
tested as a separate reviewed P; this S27 package does not claim to contain it.
S28 owns the remaining management projection/divergence audit; S29 owns
whole-plane owner handoff. Neither is complete by this source-ownership move.
