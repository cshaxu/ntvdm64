# M0 T412 S6 removal ledger

## Scope and method

This ledger covers the S6-owned retirement of the pre-broker, autonomous
single-process BaseVDM implementation and its unreachable product wiring.  It
does not include the owner's concurrent `QUEUE.md`, physical-mapping, VS Code
or CCPU proposal changes.

The source comparison is `git diff --numstat` over the removed app, BaseVDM,
fixture and generator paths.  The generated x86 graph is
`build/M0-T412/S6/formal-clean/build.ninja`.

## Removed material

| Group | Files | Added | Deleted | Disposition |
| --- | ---: | ---: | ---: | --- |
| Local BaseVDM queue/client/broker | 6 | 9 | 1,097 | Removed.  This was the old local record/retry/policy provider, superseded by original `srvvdm.c` behind the authenticated standalone BaseSrv transport. |
| Old app entry and launch declaration | 4 | 22 | 862 | Removed.  The product now selects `run16_entry.c`, `basesrv_entry.c` and `worker_entry.c`; the deleted process was not a fourth product. |
| Obsolete fixtures and observation preparation | 5 | 0 | 944 | Removed with their retired provider. |
| Retired generator/audit wiring | 7 | 18 | 368 | Removed.  The formal graph no longer builds a local BaseVDM archive or a second `original-softpc-process.exe`. |
| Parent composition README | 1 | 8 | 11 | Updated to remove the retired provider from the selected composition description. |
| **Total, retired implementation and adjacent composition material** | **23** | **57** | **3,282** | **Net −3,225 lines** |

The 22-file retired implementation/wiring subset (before the one parent README
update) is 49 added and 3,271 deleted lines, or net -3,222.  The deleted source
set includes the former `base_vdm_local.c`,
`base_vdm_client.c`, `base_vdm_broker.c`, app `entry.c`, and
`launch_declaration.c`.  Their old unit/fixture/generator companions are
deleted in the same change, so no test silently exercises a provider that the
product cannot select.

## Retained product ownership

The following are deliberately retained rather than counted as autonomous
duplicates:

| Surface | Current owner | Reason |
| --- | --- | --- |
| DOS/WOW command records and wait/reentry semantics | original `opennt-host/base/win32/server/srvvdm.c` | The recovered original BaseSrv owner executes through the standalone service adapter. |
| `GetNextVDMCommand`, `ExitVDM` and related client bodies | original `opennt-host/base/win32/client/vdm.c` | The recovered original BaseClient calls the finite authenticated transport. |
| `run16.exe` | `run16_entry.c` plus original BaseClient classification/config calls | This is the standalone representation of the CreateProcess parent role. |
| `basesrv.exe` | `basesrv_entry.c` plus original BaseSrv service dispatch | This is the one broker process; it has no retired local queue provider. |
| `ntvdm.exe` | original `softpc.new/obj.vdm/ntvdm.c::main` | The original worker C entry is the PE entry.  A finite standalone binding is called by that entry solely to supply process-private BaseClient capture storage, broker connection, selected session, guest-memory lease and termination escape; it contains no classification, record queue, argv rewriting or worker scheduling policy. |

The original worker entry is restored without a CRT hook or renamed `main`.
The source keeps its original `TimerInit → CpuEnvInit → nls_init → host_main`
sequence.  The only surrounding calls are explicit, process-local setup before
that sequence and matching cleanup after it.  This is the smallest boundary
that replaces NT4 process/CSRSS initialization which cannot be held by either
`run16.exe` or `basesrv.exe` in the worker address space.

## Formal graph check

The regenerated S6 graph exposes exactly these product roots:

```text
run16.exe: run16_link
basesrv.exe: basesrv_link
ntvdm.exe: worker_link
VDMREDIR.dll: redir_dll_link
```

Searching that graph for `base_vdm_local`, `base_vdm_client`,
`base_vdm_broker`, `basesrv-bindings`, `original-softpc-process`,
`app-machine-shell`, `launch_declaration`, `worker_entry` and
`mvdm_softpc_original_entry` has no hits.  `ntvdm.lib` is the sole worker
parent import library used by `VDMREDIR.dll`; no second SoftPC process image
or second worker entry remains.

## Verification status

Generation and graph-reachability checks pass.  Direct x86 compilations of the
new standalone binding and the original `ntvdm.c` entry succeed with the
generated flags (the binding retains only the pre-existing `fopen` warning).
A fresh, one-job graph at `build/M0-T412/S6/direct-entry-clean2` then compiled
and linked all 465 `ntvdm.exe` prerequisites, followed by `run16.exe` and
`basesrv.exe`.  It used the visible generated `run-ninja-parallel.cmd` path;
there was no hidden process or Defender exclusion.

The three linked products were published to the declared runtime root:

| Product | Bytes | SHA-256 |
| --- | ---: | --- |
| `O:\winnt\run16.exe` | 244,224 | `F54C36F18DD95E5A6870F77B39161A106AEB1BDCD7493F642026032AA442B10E` |
| `O:\winnt\basesrv.exe` | 259,072 | `A943C67F7B46E28445B35EA973EAC3B41ACBC4752DCD18E2A347ED5F6B6A6F1A` |
| `O:\winnt\ntvdm.exe` | 3,245,056 | `40F61F4065BC75F7F13871CAFD9D87B3F8D5E725B88441B8BCB520A28E901F0E` |

The S2--S5 same-build runtime matrix remains the behavioral gate; this ledger
claims compilation, link and exact publication only, not guest acceptance.
