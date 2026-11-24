# T484 S3 P130 dialog lifecycle producers

OriginalModalOnly, InitialPublicationOnly and dialog-lifecycle now select
shared native CMD producers. The first uses ordinary actual CCPU storage;
initial publication reuses the registration-notification namespace and original
TD predicate already owned by the startup fixture. Dialog lifecycle selects
the existing page-publication/callback and dialog-capture fault translation
units with the exact corresponding production compiler profiles. No product
hook, copied dialog implementation or duplicate-provider linker override.

PID14700 passes the first two but fails compiling the dialog-scope injection:
its stale explicit nt.h inclusion expects an include root no longer supplied
by the B owner. Remove that unused include; the existing context interface
already supplies all types/functions required by the injection. PID29500 then
passes all three, cached full host build and11 raw/22 published image identity
checks. Runtime assertions are unchanged. Publication is verified unchanged,
not repeated. Existing imported header/link warnings remain visible.

The actual assertions cover nested A/W modal calls and veto, first task/desktop
publication, capture exception and callback rejection restoration, four physical
allocation/publication rollback cases, resource ownership and nested dialog
termination. These are bounded native/CCPU contracts, not guest execution.
The independent native-slot group includes dialog-lifecycle; neither that group
nor these standalone switches is called by Invoke-ProductVerification Full.
Do not describe this delivery as a new Full run or these tools as Full gates.

Remove four obsolete InitialPublicationOnly build branches (35 net lines).
Original-modal and real-dialog preparation still serves the remaining terminal,
callback and hook overlap modes, so that shared old producer is not deleted yet.
The final runner repeats all three after cleanup; its receipt controls delivery.

[Receipt](../../../artifacts/build-migration/t484/s3-lifecycle-dialog/result.json)
and build/t484-s3-lifecycle-dialog retain the failed compile and successful runs.
No production/mirror/original guest changes. All original30 public normal
entries now have native producers or the one explicit page alias. Six of nine
public fault switches are native; TerminalSchedule, CallbackGraphOverlap and
HookGraphOverlap still require disposition. B0195 is therefore still incomplete;
file completion remains225/355,130 open. S3/S4/S5 remain open.
