# T484 S4 DLC and generic-call preparation

P16 completes two supplemental preparation chains without a new tool.
DLC stages the canonical VDMDLC.COM cache output through TestFiles, removing
redundant copies. Its evidence now names the actual vdmredir_dlc_probe.asm
source instead of an undefined variable. Generic-call uses the existing CMD
input owner for GENPROBE.EXE and GENHOST.DLL aliases and stages both through
TestFiles. Runtime assertions and result collection are unchanged.

## Verification

build/t484-s4-dlc-generic records owned PID23316, modern pwsh with current-token
Userenv, 2026-11-24T17:55:51Z to17:57:53Z, exit0. Cached full build passes.
Real DOS DLC returns the asserted original unavailable-provider status07;
this is not evidence that a DLC adapter is available. Generic normal and
mismatch each pass three guest phases, zero errors and three releases, with
the existing report length, signature and nested callback assertions intact.

All11 raw product inputs and22 dual-published hashes remain unchanged.
No new product image, Full run or original guest rebuild is claimed. Shared
CMD input rejection and physical-stage contracts retain their existing P14
and staging evidence because those implementations are unchanged.

## Accounting

Frozen census remains343/355 disposed,12 pending. Supplemental preparation
chains decrease21 to19. S4 and final S5 acceptance remain open. The containing
P16 commit supplies the delivery identity after governance checks and push.
