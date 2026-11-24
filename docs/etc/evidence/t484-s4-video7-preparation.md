# T484 S4 Video7 preparation

## Simplification and retained contract

P25 removes the external InputPif, CommandCom and ProbeDirectory parameters.
No current script caller was found. The unchanged independent probe invokes
the actual Video7 INT10 BIOS entrypoint; it does not read a PIF or require a
particular external installation directory. PIF copying/editing was launch
scaffolding, not an asserted PIF contract. This change does not claim arbitrary
user PIF compatibility coverage.

Use the existing CMD probe producer, input alias copier and physical staging
owner. Run the isolated package's own COMMAND /c with explicit Run16 --wait.
Keep default/all mode selection, preservation/clear checks, two illegal-mode
checks, private desktop, deadline and owned-process cleanup. Additionally
require observer success and guest/COMMAND zero exit. Retain the verified
manifest and probe/image hashes. No new preparation tool is introduced and
the test no longer writes into a caller-supplied probe directory.

## Verification

Owned Userenv process23932,2026-11-24T19:13:31Z to19:15:16Z, exit0, runs
build/t484-s4-video7/run.ps1. Cached full build passes. Actual default and
all-mode guest runs produce their exact markers: modes=2 and modes=32,
preserve=1,clear=1,invalid=2, with no failure marker and normal completion.
The all-mode probe SHA256 is
D434C927A408CC393FD03F967CFF813767F6445DA8F3945551EAC89A817A5DD3.
There is no injected input, Computer Use, original guest modification or
new Full acceptance claim.

All11 raw product images and22 dual-published hashes remain identical;
publication is verified identity reuse, not an unnecessary recopy. The
existing cleanup owner removes the two isolated runtimes after success.
Parser, documentation/tracker and diff checks gate P25 delivery.

## Progress

Frozen census remains343/355 disposed,12 pending. Supplemental preparation
chains decrease8 to7. S4 remains active and S5 remains outstanding.
