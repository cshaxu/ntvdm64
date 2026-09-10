# T225 S55 COMMAND initialization through EXEC stage ledger

Date: 2026-08-17
Status: source/current-route reconciliation; no repair admission.

## Question

After the bootstrap-provider and declared-image checks, does the complete
original COMMAND initialization-to-EXEC route identify a package-owned
host, adapter, or machine gap before the existing paired native terminal?

## Source-defined stages

| Stage | Original owner and contract | Current evidence | Disposition |
| --- | --- | --- | --- |
| Initial COM load | NTDOS `$Exec` allocates the PSP block, reads the COM image at `Exec_DMA:0000`, sets `Exec_Init_CS=Exec_DMA-10h`, `IP=100h`, initializes the stack return word, and transfers with `retf` (`msproc.asm:981-1087,1332-1389`). | S43/S51 record the source-shaped `50:36` handoff with `DS:SI=0C41:0100`, `AX:DI=0C41:FFFE`, `DX=0C41`; S49/S50 prove the generic real-mode handoff mechanics. | Guest DOS plus bx-core mechanics; no adapter loader, PSP, arena, or EXEC replacement is admitted. |
| Complete image bytes | NTDOS reads the declared COM image through normal DOS file operations before the transfer. | S55 image-materialization fixture proves normal `50:12/16/02` open/read/write/close preserves all 50,384 bytes of the source-built COMMAND.COM in Direct and Readonly. | Current DEM namespace/provider path is closed for this declared artifact; native execution is intentionally a separate question. |
| COMMAND bootstrap services | The initial COMMAND image requests COMSPEC, AUTOEXEC and its initial environment through original COMMAND selectors. | S22/S35 observe `54:02`, `54:0D`, and two `54:0F` calls; S20 family evidence verifies the associated Direct/Readonly contracts, including the source-defined paragraph retry. | Current COMMAND host-service package is not selected as a leaf repair. |
| Initialization image layout | `init.asm` defines an `INIT` segment which is explicitly deallocated after initialization. `uinit.asm` includes `comimsg.inc` into that INIT image. | The verified COM mapping places the copied first-fault EIP `0C41:2357` at the bytes of `Help6Msg`, an INIT message datum. | The byte identity is useful provenance only. An EIP copied by the current first-fault record is not an instruction-boundary witness, so this is not a code-location attribution or an image-corruption finding. |
| Help branch | `CheckHelp` reads the PSP command tail, calls `Init_Parse`, and reaches `HelpMsgs` only if it sees `/?`; it then calls `RPrint` for the zero-terminated message list and terminates (`init.asm:1897-1950`). | No retained native record supplies the command tail, parser result, instruction window, or a pointer value for this branch. | Neither a missing `/?` provider nor an unterminated-help-list repair is proven. |
| Startup-machine interrupt | The observed `C4 C4 06` is the original SoftPC `illegal_op_int` / guest-IRET path, owned by the startup-machine package rather than BOP host services. | T225 S23/S25 complete the source-derived PIC/BDA transcript, native machine transaction, and paired Direct/Readonly package validation. | Retain the selector-blind machine implementation. The order `06h` then fault does not prove causality or authorize a new machine change. |
| First bounded terminal | Bochs enforces a real-mode segment-limit word read at `DS=0C41`, offset `FFFF`; the original COMMAND image contains initialization data at the copied EIP. | S30/S35 and S55 paired runs are byte-identical in Direct and Readonly. The record has neither preceding instruction bytes nor a run-equivalent COMMAND map/listing. | The terminal is an unclassified guest/core-continuity observation, not a host-mutation difference and not a named BOP/provider failure. |
| Child and parent return | A later guest child must terminate through NTDOS cleanup/`50:3C`, then the original COMMAND `54:0B` return protocol. | No native evidence has reached child entry, `50:3C`, or `54:0B`; existing focused COMMAND lifecycle tests do not substitute for that reach. | Still unproven; no synthetic child, host child process, or adapter parent-return shortcut is permitted. |

## Consequences

The source map closes the presently testable provider prefix and separates it
from the unobserved guest continuation. It finds no package-owned host
capability omission: the Direct/Readonly distinction is excluded, the COMMAND
image is intact at its declared provider boundary, and the recognized machine
interrupt already has its original-shaped owner.

The map does **not** close multi-program execution. The remaining evidence
gap is a source-equivalent explanation of the guest/core continuation between
the successful initial COMMAND service sequence and the bounded segment-limit
terminal. It cannot be filled by treating a single BOP, the HELP text, or
selector `06h` as an implementation task.

## Explicitly rejected responses

- Do not add an adapter EXEC, PSP, MCB, DOS filesystem, COMMAND image patch,
  synthetic child, or `54:0B` shortcut.
- Do not change Bochs segment wrap/limit behavior, prewrite a return frame, or
  introduce a special case for `0C41:2357`.
- Do not create a trace-selected BOP task. Native tracing remains only a
  bounded package-validation tool after a separately admitted whole change.

## Next T225 decision boundary

The active packet has now completed its source/current-route reconciliation.
Before any implementation can be admitted, a whole-package source/ABI map
must identify a missing mechanical prerequisite that is independent of a
single trace address, or demonstrate an existing prerequisite is already
closed. Until then, the correct state is evidence-complete but not execution
complete; no leaf repair follows from this ledger.