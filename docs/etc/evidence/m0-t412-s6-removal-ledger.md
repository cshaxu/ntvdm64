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
| `ntvdm.exe` | current `worker_entry.c` plus original `ntvdm.c` | The wrapper remains a recorded reduction target, not a second command-policy provider: it still binds process-private capture heap, broker connection, guest-memory/session lifetime and package media roots before calling the renamed original entry. |

The final row is intentionally **not** declared restored.  Original
`obj.vdm/ntvdm.c` has its own `main`, but a raw link currently lacks those
process-private standalone bindings.  S6 must not hide them in a CRT hook or
claim that merely renaming `worker_entry.c` makes original entry semantics
true.  The required follow-up is to move each proven binding to its actual
host/API boundary, then remove `/Dmain=mvdm_softpc_original_entry` and prove
the linked `main` comes from original `ntvdm.c`.

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
`app-machine-shell` and `launch_declaration` has no hits.  `ntvdm.lib` is the
sole worker parent import library used by `VDMREDIR.dll`; no second SoftPC
process image remains.

## Verification status

Generation and graph-reachability checks pass.  A direct x86 compilation of a
clean original CCPU object succeeds with the generated flags.  The complete
formal rebuild is not claimed: Ninja's verbose MSVC include-output path stalled
in this environment, and the owner reported Defender detection during the
subsequent build attempt.  The exact Defender detection record is unavailable
to the current process.  No new formal-clean binary was staged to `O:\winnt`.

S6 closure therefore still requires a security-approved, fully completed clean
x86 rebuild followed by the S2--S5 same-build matrix and package hashes.
