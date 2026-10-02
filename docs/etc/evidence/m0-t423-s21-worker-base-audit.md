# M0 T423 S21 worker-base audit

## Question

Which current NTVDM/NTW32 mechanisms are project-added, semantically identical,
and safe to share without moving original OpenNT/MVDM execution ownership?

## Inputs and method

- `src/ntvdm-exe/softpc/mvdm_standalone_worker.c` and
  `src/ntvdm-exe/win32/console_client.c`;
- all `src/ntw32-exe/*.c` implementation units;
- `src/worker-base/{connection.c,console_client.c}`, its generated formal
  build graph, and the NTSRV client declarations used by both workers.

The review searched by ownership and behaviour rather than function spelling:
connection/registration, channel transport, copied-record validation,
cancellation, text/video transfer, input conversion, handoff barriers,
completion, cleanup and failure rollback. Each candidate was compared for
caller, resource ownership, lock scope, normal completion, peer loss and
explicit shutdown.

## Disposition ledger

| Mechanism | Current source/provenance | Disposition | Reason |
| --- | --- | --- | --- |
| Connect/current client, broker watch, rollback, disconnect | Project-added; `worker-base/connection.c` | **Shared and production-linked** | Same transition sequence; neither owns a frontend nor changes task policy. |
| Ordered frontend pipe transfer, cancellation, wire validation | Project-added; `worker-base/console_client.c` | **Shared and production-linked** | Both worker client owners use the same instance client and borrowed pipe/peer/cancel contract. |
| Activation, key prepend, bounded video, ordinary input decode | Project-added; `worker-base/console_client.c` | **Shared and production-linked** | Same protocol operations and validation; DOS relative-mouse conversion remains caller-local. |
| DOS frontend acquisition, command binding, active retry, original input watcher | Mixed project binding around original MVDM; `ntvdm-exe/win32/console_client.c` | **Keep in NTVDM** | Binds original event thread, guest mouse IRQ path, command re-entry and DOS task context. |
| NTVDM bootstrap, session/guest-memory/CCPU/WOW binding and unwind | Original-MVDM caller plus standalone binding | **Keep in NTVDM** | Its order is the DOS/WOW execution boundary; a common framework would move original ownership. |
| Native registration, channel receipt, target creation/receipt/completion | Project-added NTW32 execution owner; `ntw32-exe/main.c`, `execution.c` | **Keep in NTW32** | Native target completion and surviving Console members differ from DOS record semantics. |
| Console capture/seed, text frames, pointer composition and final barrier | Project-added NTW32 presentation owner | **Keep in NTW32** | Owns native Console and presentation lock; NTVDM owns neither Console storage nor membership. |
| Native execution-pipe transfer | Project-added; `ntw32-exe/channel_io.c` | **Keep in NTW32** | Its peer-death contract maps to `ERROR_PROCESS_ABORTED`; frontend transport has distinct cancellation/failure-latch semantics. |
| `CsrPortHeap` allocation/destruction | Per-process bootstrap in both EXEs | **Keep local** | Global symbol and each worker's initialization/unwind order remain process-local. |

## Production wiring proof

The build generator makes `worker-base.lib` from `connection.c` and
`console_client.c`, and links it into both `ntvdm.exe` and `ntw32.exe`.
NTVDM calls the common connection lifecycle in its standalone bootstrap and
uses the shared protocol client from its Console binding. NTW32 does the same
in `wmain` and its presentation client. The shared rows are therefore actual
production code, not wrappers beside retained duplicate implementations.

## Verification

`git diff --name-only` after the audit contains only this record, its index,
and the `worker-base` ownership note: no production C, header, IDL or build
manifest changed. The existing protocol-18 formal x86 package therefore
remains the exact production-code baseline rather than a stale different
binary. Its focused worker-boundary fixtures were rerun:

- `frontend-scope-lifetime-test.exe` passed root/private capability, inner
  join, disconnected capability, state-event retry and final-channel cases;
- `basesrv-service-reservation-test.exe --native-backend` passed authenticated
  native registration, live identity, member report and rundown cases;
- `monitor-rpc-test.exe --empty` passed absent-worker and version-rejection
  cases.

The unchanged, hash-matched `O:/winnt` protocol-18 package was also exercised
again with the ordinary frontend.  Fresh logs under `O:/winnt/Logs2` record
successful `empty`, native `ver`, interactive `MEM`, nested `COMMAND`/`MEM`,
direct `MEM`, `COMMAND.COM /c ver`, and EDIT-return routes.  The EDIT route
was rerun alone with a fresh prefix after the batch completed its preceding
routes, and passed.  A fresh real two-session NTW32 management run then closed
only the selected NTW32/CMD pair; the independent session received
`ISOLATED-SESSION-OK` and returned 23.  These routes cover normal completion,
DOS/native handoff, nested input ownership, explicit close and isolation.

The S20 formal graph itself remains the 590-node x86 proof recorded in its
predecessor evidence. A fresh S21 graph was generated under
`build/M0-T423/S21/formal`; its first build attempt exposed only a local
tool-environment PATH collision before any source diagnostic. It is not
counted as a build result and does not alter the unchanged production baseline.

## Conclusion

No additional project-added mechanism is both semantically identical and safe
to extract. Remaining similar code has distinct original DOS/WOW or native
Console/target ownership. Moving it would violate the S21 non-goal rather than
reduce duplication. The focused and real-package checks above satisfy S21's
applicable runtime verification without claiming a new binary publication.
