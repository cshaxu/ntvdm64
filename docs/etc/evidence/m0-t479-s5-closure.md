# T479 S5 command continuation closure

S5 is closed on its bounded Win3.1 command continuation contract. This does
not close full I/O, nested payload or Win95 acceptance. The owner-authorized
sequence continues automatically with S6.

## Requirement and actual proof

| Contract | Evidence |
| --- | --- |
| Both 54:08 forms and 54:0A use actual saved-VM inputs | Production entry/capture/executor witnesses159,165,167,192,209,210,214--216; exact client-state assertions, no CPU impersonation. |
| Independent requests and both completion orders | Native-order165/167 and two-VM continuation records; 320 callback identity/stack/register/flags checks. |
| VM retirement, handle reuse and stale completion | Real retirement161 and reuse163--167 plus focused continuation lifetime tests; forced retirement overlap has host-unit evidence, not exhaustive VMM allocation-failure proof. |
| Shared original behavior | P3 shares host COMSPEC selection; P4 shares original command/environment conversion and preserves ordinary create/resume/wait/reentry ordering. |
| Correct original reentry owner | Diagnostic215 records two downstream DOS workers executing original cmdExec32 with CmdSize=3 and cmdReturnExitCode with CmdSize=0. Actual MEM output returns to each VM. Normal diagnostic214 and216 also pass. |
| Failure and regression | Prior command suites and Full212 selected16/16 gates; failed199/203/205/208 controls retained and startup routing defect corrected. External DOS4GW/Nesticle fixtures remain explicitly skipped. |
| Delivery | Complete cached x86/x64 build211; package6c79f45b-88bd-4359-a9c8-15e3a753457e published and hash-verified at both targets in507619ea8. No production change after that delivery; final probe compiles its changed test only. |

[Detailed implementation and ownership audit](../operations/m0-t479-s5-cmdexec-reuse-audit.md),
[original continuation evidence](../operations/m0-t479-s5-command-continuation.md),
[P4 delivery](m0-t479-s5-text-preparation.md) and
[closure hashes](m0-t479-s5-closure.json) retain the distinct evidence scopes.

## Reentry interpretation correction

Original CF=1 means GetNextVDMCommand supplied a command to that DOS worker's
execution queue. It does not mean any frontend hosted by guest Windows must
receive CF=1. The approved VM frontend obtains a new authenticated downstream
execution context, without aliasing the enclosing payload's queue. In the
real `command /c cmd /c mem` witness the downstream DOS worker owns and
executes the original reentry cycle. The VMM caller waits for its chain and
receives its terminal completion. Inventing a second pending guest command
or returning CF=1 without one would be incorrect.

This supersedes the earlier audit inference that S5 required a CF=1 producer
in the VMM completion executor. It preserves actual reentry at its queue owner;
it does not authorize moving a queued command between workers or implementing
arbitrary host-to-guest launch routing. Guest binaries remain immutable.

## Mandatory S6 and later work

S6 retains all47 live I/O operations: actual keyboard/mouse delivery and focus,
fonts/geometry/palette, graphics, titles and control events, final-output serial,
cancellation and nested chains/payloads. The passed tests use provider input;
they do not prove physical guest input. Unsupported operations observed in208
remain S6 gaps, not accepted success stubs. S4 Win95 continuation follows the
Win3.1 S6 work. S7 retains comprehensive lifecycle/product acceptance and
unverified full ICW1/VMM allocation-failure combinations.

The published addon remains a candidate pending full I/O acceptance. Concurrent
queue/proposal edits are preserved outside this closure commit.
