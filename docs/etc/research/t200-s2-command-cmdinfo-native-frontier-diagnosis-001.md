# T200 S2 — COMMAND CMDINFO native-frontier diagnosis

## Question

What is the complete original `54:01` input/result contract at the T200/S1
frontier, what does the current COMMAND package require before it accepts that
contract, and can source evidence alone identify the declined prerequisite?

## Inputs and method

This review uses only the pinned OpenNT tree and current source-built runtime
sources.  It does not import or link a historical object and it performs no
additional guest run.

| Input | Role |
| --- | --- |
| `src/opennt/base/mvdm/inc/cmdsvc.h` | packed 36-byte `CMDINFO` ABI and `54:01` identity |
| `src/opennt/base/mvdm/dos/v86/cmd/command/tcode.asm` | real-mode caller construction and CF/AX retry loop |
| `src/opennt/base/mvdm/dos/v86/cmd/command/tspc.asm` | exact record storage order and target buffers |
| `src/opennt/base/mvdm/dos/v86/cmd/command/comequ.asm` | `COMBUFLEN=128` and `EXECPATHLEN=273` |
| `src/opennt/base/mvdm/dos/command/cmdmisc.c` | original `cmdGetNextCmd` read/write order and host dependencies |
| `src/bx-vdm/bx_ntvdm_command_package_session_v1.c` | current package-session gate and mechanical reads/writes |
| `src/bx-vdm/bx_ntvdm_cmd_get_next_service.c` | current copied CMDINFO validation and transaction |
| `artifacts/build/t200-s1-native-cli-baseline-r1/run.log` | the sole current native observation |

## Original caller contract

Before `CMDSVC SVC_CMDGETNEXTCMD`, `tcode.asm` sets `DS` to its transient
group and `DX` to `ENV_PTR_SEG`, the beginning of the 36-byte record.  It
stores the actual DOS environment segment and byte count, current drive,
`UCOMBUF` address and capacity 128, `EXECPATH` address and capacity 273, and
the redirector-completion slot.  `cmdmisc.c` reads the record at `DS:DX`
before applying first-call/repeat-call policy.  It can return only the caller's
documented environment-capacity retry (`CF=1`, `AX=needed bytes`) before
command delivery.  Successful delivery writes the command line, executable
path size/type, admitted command/session outputs, and—only when `54:05` has
registered their locators—the two SCS/DOSDATA notification bytes.

The original host implementation is not independently composable in the CLI:
it relies on SAS/CCPU conversion, process state, console/redirection, and
other Win32-era host facilities.  The current source-derived provider is
therefore the correct owner for the contained contract; this finding neither
authorizes linking `cmdmisc.c` nor moving any COMMAND meaning into Bochs.

## Current package comparison

The current session accepts `54:01` only after all of these complete
COMMAND-package conditions hold:

1. a validated CLI launch plan and admitted drive snapshot exist;
2. `DS:DX` addresses one readable 36-byte CMDINFO range below the first MiB;
3. decoded `CmdLineSize` is 128 and `ExecPathSize` is 273;
4. command, executable, and (when supplied by `54:0F`) environment output
   ranges are individually within the same aperture;
5. the `54:0F` environment state yields either the original CF/AX capacity
   retry or a successful bounded delivery; and
6. the resulting complete multi-write transaction is accepted by the typed
   mantle mechanical-action seam.

`54:05` registration is deliberately not a prerequisite: its optional
post-delivery writes are attached only when valid.  This is consistent with
the original separate `cmdSetInfo` registration and with T199/S40.

The S1 log records only `next=54:01` and a fixture controlled stop.  It does
not record the event registers, CMDINFO bytes, bootstrap stage, launch-plan
state, or post-dispatch acceptance.  Therefore it cannot distinguish any of
the six conditions above.  In particular, source proof that the historical
caller normally uses 128/273 capacities does not prove that the source-built
guest at this exact frontier supplied those values or a valid `DS:DX` range.

## Decision: one bounded CMDINFO-contract observation

No repair is admitted from S1.  The next bounded work is a **test-only,
whole-CMDINFO observation surface**, not an adapter service implementation:

- before normal composition delegation, copy only the fixed event register
  state needed to identify `DS:DX` and real-mode geometry;
- use the existing typed mantle read action for exactly the source-defined
  36-byte CMDINFO range when that range is representable below 1 MiB;
- decode and report the named fixed-width CMDINFO fields, or report the
  geometry/read refusal without exposing unrelated guest memory;
- after normal delegation, report acceptance/outcome plus non-secret command
  session state (`54:0F` bootstrap stage, launch-plan presence, drive-snapshot
  presence, and delivery count);
- change neither event, CPU result, guest memory, provider selection, nor
  Bochs behaviour; compile it only into the research fixture, default-off from
  the product runtime.

This observes all current 54:01 gates as one original COMMAND ABI unit.  It
does not parse BOPs in Bochs, add a host capability, infer a result, or create
a trace-driven one-service repair.  Its one later native execution remains
separately admitted and will be integration evidence for the next complete
COMMAND repair decision.

## Confidence and follow-up

Confidence is high for the source contract and current guard set, because both
are direct source reads.  Confidence is intentionally absent for the live
reason the S1 event declined: the required input state was not recorded.

T200/S3 owns implementation and source-built compilation of this strictly
diagnostic fixture surface.  It may not execute a guest run.  Only after S3
acceptance may a subsequent packet admit the single bounded observation run.
