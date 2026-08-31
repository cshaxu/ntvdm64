# M0 T333 S3 P3 — Original VDMREDIR DLL dependency boundary

## Question

Can the complete original `VDMREDIR.DLL` be independently linked before the
remaining Redirector owner cohorts are recovered?

## Procedure

Using the 16 objects that passed the paired P2 source builds, invoke the
modern x86 linker without `/FORCE`, with the original `vdmredir.def`, original
`VrDllInitialize` entry name and public system import libraries. No unrelated
SoftPC libraries or substitute provider were added.

## Observations

- The linker correctly reads the original export list but reports that the
  historical cdecl `VrDllInitialize` is not a public PE `DllMain` stdcall
  entry. A final sidecar DLL therefore needs a narrowly owned loader-ABI
  bridge which invokes the original entry; it is not a Redirector policy
  replacement.
- The probe reports 65 unresolved symbols, which partition into source-owner
  cohorts rather than one missing Redirector function:
  - CPU40/Sim32/register/ICA/`SoftPcEoi` mechanics;
  - existing session-backed mailslot, guest-copy and asynchronous-lifetime
    adapter forms;
  - original `VDDInstallUserHook` host binding;
  - NetAPI/NetLib conversion and public local-service helpers;
  - RAP worker/crypto/session-key functions;
  - NetBIOS queue and completion forms; and
  - DLC driver/completion forms.
- This is expected from original `vrdll.c`: process attach calls
  `VrInitialize`, which in turn initializes VDD hook state, NetBIOS and DLC.
  A named-pipe-only DLL cannot be made runnable by omitting those source
  calls without changing original lifecycle semantics.
- Existing unmodified `MS_bop_7` already provides the current exact failure
  disposition: a failed `LoadVdmRedir` attempt changes its per-session state
  to failed and returns `ERROR_INVALID_FUNCTION`, while DOS continues.

## Interpretation

S3 closes with complete source-package compile evidence and a classified
dynamic-module boundary. It does not claim a runnable DLL or synchronous
named-pipe runtime. S4 and S5 own their respective source cohorts; only S6
may link/stage the final sidecar after those cohorts have a result. No
Redirector-local replacement loader, VDD, NetBIOS, DLC or RAP provider is
admitted by this probe.
