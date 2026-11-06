# T479 S3 independent host command preparation

## Bounded source recovery

The approved [stage plan](m0-t479-guest-bridge-plan.md) separates host preparation
from the later guest caller. Original `dos/command/cmdexec.c` remains the owner
of ordinary COMMAND conversion, native launch, reentry and return ordering.
Its `SetStdHandle` spells a TLS adapter, not a process-global mutation. Keep
that path; do not replace it with a second command interpreter.

The missing boundary is that its captured input and completion globals describe
one original execution at a time. Future VMM requests cannot retain guest
pointers or borrow those globals while another VM runs. Directly running a
second `cmdExec32` worker does not supply independent reentry/return semantics.
Those guest transitions remain S5 obligations, not part of this host stage.

## Implementation and ownership

1. `mvdm_command_capture_create` reuses the ordinary capture's bounded mapping
   leases and copying functions. Each object copies raw command, environment,
   standard-handle tokens, original service choice and default drive. It does
   not enter the session's ordinary singleton. CR-to-NUL guest writes retain
   the existing original command contract. All leases end before return.
2. A capture view borrows only its object's host buffers. Token-to-handle
   resolution, original command/environment encoding and selecting the guest's
   directory remain the caller's synchronous preparation duties. In particular
   the raw guest standard-handle numbers are not an asynchronous native handle
   attachment. The view records that distinction explicitly.
3. `ntvdm_command_launch_prepare` takes converted command/environment, an
   absolute directory, resolved streams and the authenticated pair produced by
   S2's `ntvdm_vm_host_launch_context`. It copies data and duplicates resources
   without inheritance. Endpoint direction is classified while the originating
   session is bound, before asynchronous launch. No guest pointer is retained.
4. Launch reuses the existing Run16 command construction, child-only internal
   environment, endpoint classification, suspended creation and Hook installer.
   Explicit requests supply their own capability pair and captured endpoint
   mask. They never query or rebind the worker's upstream frontend. Ordinary
   calls retain the existing ambient adapter and TLS stream behavior.
5. Each explicit child receives only its two capabilities and three streams
   through the shared process-launch attribute helper. File/pipe handles retain
   their existing object and access; Console endpoints remain tagged identities
   for NTVWM substitution, not raw host CMD Console handles. Temporary inheritable
   duplicates and attribute storage are released after CreateProcess.
6. A request is single-use even on launch failure. The caller owns successful
   PROCESS_INFORMATION and its eventual result; errors return directly to that
   caller. No shared exit-code slot, waiting thread, retry, timer, or lifecycle
   policy is introduced. Destroy cannot race start. Failed installation aborts
   the uncommitted suspended child using the existing installer contract.

## Verification obligations

The checked-in `test/ntvdm-exe/command/command_capture.c` exercises the production
capture against bounded synthetic guest memory, independent snapshots, unchanged
ordinary state, lease failures and boundary rejection. This is a unit boundary,
not evidence of guest VMM execution or correct VM address mapping.

`command_launch.c` launches real children through the production adapter with
test-owned path and installer boundaries. Independent requests vary directory,
environment, pipe and exit code, discard original inputs, launch concurrently,
and reject unrelated inheritable handles. Failure cases cover malformed blocks,
stream directions, duplication, path lookup, installer rollback and repeated
start; resource counts must balance. These tests do not prove real Hook/RPC
integration, which still requires the ordinary product gates and S5's actual VM
caller. This design record does not claim guest runtime capability or publication.

The first cached build exposed a missing process-launch link dependency, now
added to the existing softpc-win32-binding consumer dependency rule. Preserve
that failed log; only a successful rebuilt candidate qualifies for tests.

Focused verification now passes 31 capture and 59 launch assertions. Initial
resource-count diagnostics separated two OS initialization effects: raw
CreateProcess alone added six process-lifetime handles, and raw CreatePipe
added the retained NamedPipe root handle. The final fixture initializes those
facilities before measuring requests and requires exact handle equality after
owned sources are closed (134 before, 132 after, two source capabilities
intentionally removed). Temporary native-object diagnostic code is removed;
its output remains in task evidence. Child inheritance checks compare object
identity because startup can reuse an unrelated numeric handle value. The
installer rollback, interactive endpoint-mask and suspended-start cases pass.
The complete package regression remains a separate gate.

## Remaining stage boundaries

No guest entry is enabled in S3. Guest callback timing, mapping selection,
encoding/service preparation, CF/AL and DOS reentry are verified at their actual
caller in S4/S5; complete per-VM I/O is S6. This stage must not represent these
pending obligations as runtime support. Mirror and guest changes remain zero.
