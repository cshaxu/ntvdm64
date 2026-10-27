# Console code-page state: original contract and handoff candidate

## Evidence and contract

Original `src/mvdm/vdmutils/graftabl/graftabl.c` calls the Windows Console
`GetConsoleOutputCP` and `SetConsoleOutputCP` APIs; its `/STATUS` reads the
same state and its setter changes output, not input. Original
`src/mvdm/dos/command/cmdexec.c::cmdCreateProcess` passes inherited standard
handles and launches with `CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE` and
without `CREATE_NEW_CONSOLE` for the ordinary CUI launch. Thus the original
ordinary launch uses one OS Console's shared mutable state; do not infer this
contract for redirected file/pipe bytes or every exceptional launch path.
Original `softpc.new/host/src/nt_fulsc.c::loadNativeBIOSfont` consults the
output CP for font selection when registered, with CP437/failure fallback.
This is original algorithmic evidence, not proof that the same code path ran
in the witness below. Build `build/.ninja/msvc-x86-release/worker/build.ninja`
selects the file; conditional compilation and actual caller need their own
separate verification.

The isolated S9-package case and precise limitations are documented in
`standalone-tool-chain-audit.md`: `SetConsoleOutputCP(850)` succeeded in a
native child; subsequent native carrier reports 850 while NTCON reports 437;
DOS fixture byte 9B was displayed as U+00A2, CP437's interpretation, not
U+00F8 from CP850. Original GRAFTABL and an original NT4 runtime were not run.
`src/ntcon-exe/frontend_session.c::prepare_text_frame` explains a selected
path that decodes every text-frame byte through NTCON's current output CP.
The test proves a specific DOS output consequence, not that the DOS internal
INT 21h 6601 country-code state itself should change.

## Candidate: one handoff authority, not a second policy broker

The existing NTSRV route controls which worker owns the frontend I/O; NTCON
owns the actual externally attached Console, while NTVWM's hidden Console is
a temporary projection for its currently executing native CUI targets. Do not
add a cache in Run16, duplicate global code-page facts in NTSRV, or make
worker-base select application types. Use existing Console I/O transport for
actual Console state and existing NTSRV RPC for admission/completion only.

1. Once NTSRV grants NTVWM I/O and before allowing a target to run, query
   NTCON's input/output CP using existing `CONSOLE_IO_CODE_PAGE`; apply each
   value to NTVWM's own Console, verifying `SetConsoleCP` and
   `SetConsoleOutputCP`. This extends the existing
   `ntvwm_presentation_begin` / `ntvwm_presentation_seed` handoff, not every
   frame. Input and output are independent; copy them separately. Any failure
   must abort admission and follow the existing NTSRV failure path.
2. Before a native I/O lease is released, after the last target has ceased
   writing and while the transport remains owned, obtain the hidden Console's
   actual CP values. Add an explicitly named setter to the existing Console
   I/O operation family, implemented in NTCON by the corresponding Win32
   setter, with acknowledged per-field results. Apply before broker release
   and before DOS can resume; ensure the final logical/render barrier still
   applies to the correct video serial. Since a partial pair of OS setters
   can fail, define and test how the old values are restored or surface a
   non-successful handoff; never report success merely because a request was
   sent. Do not change guest BOP, guest code, or command-name dispatch.
3. When `WORKER_IO_KEEP` means native carrier remains owner, do not overwrite
   the live worker from a stale NTCON value. Normal subsequent native targets
   use the still-current native Console. Audit both explicit checkpoint end
   and the service-requested `io_release` path, including nested CUI, failures,
   closure and outstanding frames; a change in only one path is incomplete.
4. NTCON/NTVWM states are separately observable while the native lease is
   active. The handoff candidate makes them agree at transitions; it does not
   claim synchronous visibility to other processes already attached to the
   NTCON Console during native execution. Such processes can also mutate its
   CP concurrently, which source ownership alone cannot serialize. Determine
   whether original concurrent observers are in the supported contract before
   choosing a collision policy; do not silently claim or authorize a semantic
   retreat. This unresolved concurrency is a design boundary, not a test pass.

This is a repair candidate, not an implemented change or approval to alter
observable semantics. Confirm channel order, route generation and frame
ownership at exact integration points before coding. Raw output CP needs no
synchronization when no Console frontend exists and redirection is a file or
pipe; it must never rewrite redirected data or alter DOS's own code-page state.

## Acceptance matrix

- Reproduce the saved 437 -> native 850 -> DOS byte 9B case; verify NTCON
  Console becomes 850 before DOS publication and observed U+00F8, with 437
  control, no-change control, both existing-parent and fresh root Console.
- Native child reports the pre-existing frontend CP at first admission;
  native set -> second native child reads it; alternating native/DOS workers,
  repeated switches, nested requests and `WORKER_IO_KEEP` preserve it.
- Input and output CP differ; set each independently and verify the other
  remains unchanged. Invalid CP or injected API/transport failure reports
  failure, preserves the defined state, does not send fake completion and
  does not leak I/O ownership or deadlock on NTSRV/NTCON/worker death.
- Redirected files, pipes, `>`, `>>`, `|`, normal and failed native exit,
  Console-close/worker-close and final frame barrier remain correct; no extra
  polling, sleeps or per-frame synchronization.
- Distinguish semantic evidence from actual original GRAFTABL availability,
  original NT4 comparison and third-party live mutation, which remain
  separately unverified. Ensure no guest modification or build.

## Pending GRAFTABL build-carrier inventory

`vdmutils/graftabl/obj/_objects.mac` lists original Alpha/MIPS/i386/PPC `.obj`
and `.res` as generated build objects; `sources` names `graftabl.c`, resource,
console UMAPPL and `TARGETPATH=obj`. The current retained
`obj/i386/graftabl.obj` is PE-i386 COFF with `_main`, imported
`GetConsoleOutputCP` and `SetConsoleOutputCP`, source debug name
`w:\\nt\\private\\mvdm\\vdmutils\\graftabl\\graftabl.c`; it is an object,
not evidence of an installed/runnable executable. The `.pdb` is a retained
build debug artifact by path and format claim, not a proved source or runtime
provider. All three remain pending pending provenance, build/deployment and
artifact retention disposition; absence from worker link is not functional
exclusion of the standalone utility. No retained binary was executed.
