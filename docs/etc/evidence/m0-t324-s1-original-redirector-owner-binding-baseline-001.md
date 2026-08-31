# M0 T324 S1 — Original Redirector owner/binding baseline

## Question

What is the complete original VDMREDIR package selected by the MVDM build, and
which of its local file, pipe, network, device, VDD and status boundaries can
be recovered without replacing the package with a second redirector?

## Inputs

- `src/mvdm-host/vdmredir/sources` and all 16 listed source units.
- Original caller surfaces `src/mvdm-host/dos/command/cmdredir.c`,
  `cmdexec.c`, `cmdmisc.c`, and `src/mvdm-host/inc/vdmredir.h`.
- Existing bounded external interface family:
  `src/adapter-mvdm-host-out/redir`.
- Selected external original candidate: identical `ntstatus.c` copies at
  `OpenNT/ds/netapi/netlib`, `opennt-4.5/nt/private/net/netlib`, and
  `opennt-src-2/nt/private/net/netlib`.

## Package composition

| Original unit | Primary owner role | Current recovery disposition |
| --- | --- | --- |
| `vrdisp.c`, `vrinit.c`, `vrdll.c` | Dispatcher, DLL lifecycle and VDD hook installation | Original control skeleton; VDD/module lifecycle needs a later bounded adapter disposition. |
| `vrnmpipe.c`, `vrmslot.c` | Named-pipe and mailslot protocol | Public Win32 local transport candidate; async completion/frame delivery needs owner review. |
| `vrmisc.c`, `vrputil.c` | Local utilities and DOS/network pathname conversion | Direct original helper candidate once bounded input spans are identified. |
| `vrnetapi.c`, `vrremote.c` | NetAPI, downlevel/RAP and remote network services | Preserve source and original failures; modern NetAPI can be considered only per reached local contract. RAP transport remains a hard boundary. |
| `vrnetb.c` | NetBIOS service | Explicit device/network capability boundary; no synthetic adapter success. |
| `vrdlc5c.c`, `vrdlcpst.c`, `vrdlcbuf.c`, `vrdlcdbg.c` | DLC controller, post/completion and diagnostic paths | Device/VDD asynchronous boundary; retained source but not a local-file recovery candidate. |
| `vrdebug.c` | Optional Redirector diagnostics | Original optional diagnostics; excluded from product behavior unless an original activation path is reached. |

## Reached external contracts

1. **COMMAND completion record.** `cmdredir.c` is original COMMAND owner
   code, not VDMREDIR itself. Its `REDIRCOMPLETE_INFO` and standard handles
   cross a 32-bit MVDM register shape. Existing `ADAPTER-REDIR-001` and
   `MVDM-HOST-DIV-020` correctly publish only per-session opaque identities.
2. **Local files and pipes.** `cmdredir.c` uses public file, event, thread,
   wait, temporary-file and pipe APIs. These are candidates for original
   source recovery with a session-owned handle/worker lifetime, never global
   parent standard-handle replacement.
3. **VDM guest addresses.** `vdmredir.h` retains `GetVDMAddr`,
   `Sim32GetVDMPointer` and address-conversion forms. They use the existing
   bounded SoftPC/session guest-memory rule; S1 creates no Redirector mapper.
4. **VDD lifecycle.** `vrinit.c` calls `VDDInstallUserHook`. This is a VDD
   owner transfer; it cannot be reported as installed by a local pipe shim.
5. **NetAPI/RAP.** `vrnetapi.c` uses `NetpNtStatusToApiStatus`, `XsNet*`,
   NetAPI helpers and LM session operations. `XsNet*`/RAP is a hard downlevel
   transport boundary; it retains original unavailable behavior.
6. **DLC/NetBIOS.** Device completion logic remains VDD/device-owned source
   evidence and an explicit unavailable transfer in the local CLI profile.

## Selected external mirror identity

The three original `ntstatus.c` candidates are byte-identical: 12,382 bytes,
SHA-256 `3db640c6cdc48e0835cc6a54c69bd12196245fba3feeb486a79e45a7c9abbf86`.
The source policy already selects this exact `netlib` slice. It is missing from
`src/opennt-host`; importing it byte-for-byte is required mirror completeness,
not permission to import NetAPI, NetRAP or CSRSS product shells.

## Interpretation and follow-up

The package has a local host cohort (dispatcher/utilities, file, standard
stream, named-pipe/mailslot and status conversion) and a non-local cohort
(VDD/DLC/NetBIOS plus remote/RAP). Import `ntstatus.c` under
`src/opennt-host/netapi/netlib/` with provenance/README registration, then
recover one local source-shaped cohort. No per-BOP provider is admitted.
