# T484 S4 P40 Win31 identity preparation

The Win31 editor and identity runners share the matrix CMD launch operation.
It owns the CONFIG.NT DOSONLY selection, enhanced WIN.PIF generation, WIN.INI
load selection and optional SYSTEM.INI VMD entry. It reuses guest-ini and
PIF.EXE. The launch writes are restricted to the owned test installation.
The identity runner checks for an active VMD entry before requesting insertion.

The existing prepare-test-inputs owner can also install optional witnesses
under PATCH of an owned copy. The identity runner uses it for unread input,
ordered command, nested MEM/COMMAND and reuse inputs. It keeps the immutable
MEM and COMMAND hash checks. The runner selects scenarios and supplies
arguments to the existing native PIF writer and authored witness binder.
Runtime assertions and copying collected evidence remain in the test runner.

The evidence root is build/t484-s4-identity-preparation. PID2368 passed the
ordinary editor preparation comparison followed by a real Win3.1 DOSONLY=0
identity case using the P39 diagnostic package. Two DOS VMs reported distinct
identities and generations. Negative API behavior and 320 callback identity,
stack, register and flags assertions passed. Windows exited normally and
the source installation hashes were unchanged.

PID8020 completed the cached full build and the final preparation contracts.
The real copy checks include the previous editor INI transformation and PIF
targets, a wrong-owner refusal, byte-identical MEM input, DOSONLY selection
and exact configuration restoration. A second copy verified empty WIN.INI
load and DOSONLY=1. All 22 hashes in the two published host locations match.
The test-only changes reuse those release artifacts. All owned O:/tmp
copies were removed.

The Win31 identity preparation caller is migrated. The specialized identity
variants were not all rerun in this P. The census is 351/355 disposed, 4
pending, plus 5 overlapping supplemental chains.
