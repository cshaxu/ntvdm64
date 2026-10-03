# T423 S28 management projection and divergence audit

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Source/owner ledger

This is an audit of the selected x86 production graph, including project-added
code inside `ntsrv-exe/opennt`. No original OpenNT/MVDM mirror or guest binary
was changed. S26 rejected Job-descendant authority; this packet does not
quietly restore it.

| Mechanism | Source and disposition |
| --- | --- |
| DOS task stack, completion and re-entry | Original BaseSrv DOS records and NTVDM caller shape. Retained in place; NTMON only receives a copied projection. |
| Win16 task stack and completion | Original WOW records. Corrected the project-added snapshot projection to put their count in `stack_depth`, the same field already used by DOS and native workers. Original WOW policy is unchanged. |
| Native direct task | Project-added NTSRV `Win32Record` created only by an admitted Run16 request and bound to NTW32's actual target PID. Retained as one Direct record list; it is not a second execution scheduler. |
| Native descendants | Windows/target own their execution, parent wait and exit code. The rejected Job observer supplies no production event; no Observed record, parent PID, inferred task or fabricated completion is exposed. |
| Management DTO | Project-added `service.idl` and `OPENNT_BASE_WORKER_INFO` formerly carried both `reserved` and `stack_depth` for the same count. Removed `reserved`; one versioned `PID/KIND/STATE/START/TASK/STACK/IMAGE` record remains. Protocol 25 prevents mixing layouts. |
| NTMON | Already consumes only NTSRV's snapshot and uses kind 0 DOS, 1 Win16, 2 Win32. Its displayed task depth comes from the one DTO; no `MEMBERS=` rendering or process enumeration was found in its production path. Retained unchanged. |
| Run16 worker selection and direct waits | Already one authenticated NTSRV admission route with a native-kind branch; Win32 process exit code remains from the real target HANDLE. No duplicate policy was extracted. |
| NTCON frontend | One frontend route and frame/event contract for both worker kinds; it does not create task records or decide worker READY/BUSY. Presentation owner remains separate from NTSRV. |
| Worker-base | Only the shared connection/death-watch and ordered frontend client remain, as proven in S27. NTW32-only native get-next stays local; original NTVDM `GetNextVDMCommand` remains untouched. |
| NTW32 execution and hidden Console | Worker-local Windows process/Console mechanics are not DOS/WOW semantics and are not moved into a false common scheduler. Its one admitted Direct completion is reported to NTSRV; descendant Windows behavior remains native. |

The concrete cleanup removes one duplicate 32-bit management field from both
local and RPC layouts, two dead native-record fields (`observed` and
`parent_process_id`), the associated unreachable branches, and obsolete Job
comments. `stack_depth` is the sole depth source. On a resident worker with no
admitted task, the snapshot reports `STACK=0`, `TASK=0`, `IMAGE=<EMPTY>`.
Native CMD descendants are deliberately **not** represented as a full stack;
that would require the separate source-of-truth decision rejected in S26.

## Verification and publication

- x86 MSVC production sources and graph-selected links produced the seven
  executable/DLL targets; the VDM-TIB ownership audit passed. The unchanged
  accepted `WOW32.DLL` formed the coherent eight-file candidate. The extra
  clean-root Ninja retry failed in its launcher environment before locating
  `cl.exe`; it is not counted as a build pass. The selected x86 compile/link
  commands and the earlier S28 incremental product build supplied the actual
  tested binaries.
- BaseSrv default lifecycle fixture passed, including a new assertion that a
  live original WOW record projects `kind=1, stack_depth=1, task=iTask`.
  The native-worker fixture passed after its simulated same-Console identity
  was explicitly reported through the authenticated test connection. Its
  earlier no-membership failure also occurred on unchanged S27 and was not a
  production regression. `monitor-rpc-test` and the x86 NTMON layout fixture
  passed.
- The isolated candidate passed all 17 Console and all 17 Window cases of
  `Verify-CommandExitStatus.ps1`, including direct/interactive/nested
  COMMAND, MEM, EDIT, native built-ins/streams, real guest text and expected
  exit codes 0, 1 and 7. Each case checks captured guest/native text, not just
  the observer's exit. Logs: `O:/winnt/Logs2/m0-t423-s28-candidate-console17-*`
  and `m0-t423-s28-candidate-window17-*`.
- The headless network-profile WOW gate preserved the existing frontiers:
  WINMINE reached its localized main window; SOL and WRITE reached their
  original out-of-memory dialogs. This is **not** full SOL or WRITE acceptance.
  Logs: `O:/winnt/Logs2/m0-t423-s28-candidate-wow-*`.
- A standalone inherited-Console MEM probe returned 87 for both unchanged
  S27 and S28 in the current crowded shell. The same two packages returned 0
  under the isolated desktop, and the full product matrices passed there.
  The inherited-shell result is retained as an environmental observation,
  not silently converted into an S28 code regression or a pass.
- Only `Z:` was temporarily mapped to each candidate package root for these
  tests and was removed after each run. No other drive was substituted.
  `O:/winnt` received the verified eight-file package; exact SHA-256 hashes
  matched, and published MEM and `cmd.exe /d /c ver` smoke checks both exited
  0 with the expected screen text. No guest media or configuration changed.

| Published file | SHA-256 |
| --- | --- |
| `run16.exe` | `6FC19E54840EF9220420C24EE380FE4F481EF76E111018D7DBAFFF8C81EF30B2` |
| `ntsrv.exe` | `21F158E5F6359EC70C7AB9DC8DC55836DA78B0F4F4170B2220E9D1E9F49415DC` |
| `ntvdm.exe` | `0CEF521662A57204573383C3725EADF781AC00A572983A8538FE8CC4E3849410` |
| `ntw32.exe` | `1838DE13DBBCEE9CA5A2E91BDDFB94D35C530DF613FFFA51DAF16A74E06B112D` |
| `ntcon.exe` | `D1B8487317678F58981185063D4A6A65BE4A5DEE105C309C946A836E95E9A2FE` |
| `ntmon.exe` | `8C62D13121CE6283ED0CE62A565D63D8616951DC940B520EDBA230DA1ED80E02` |
| `VDMREDIR.dll` | `02AD32A276D346D5C58025A74F241D9CDF2506B8BAF0802A563853CD5E7144C6` |
| `WOW32.DLL` | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |

S29 retains whole-plane fault/reuse/management acceptance, the previously
recorded supplemental final-banner limitation, and any independently reviewed
NTCON side-session change. S28 does not claim T423 closure.
