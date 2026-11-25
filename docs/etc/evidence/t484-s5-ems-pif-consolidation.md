# T484 S5 P1 EMS PIF consolidation

The existing PIF editor now writes the Windows 386 minimum XMS and minimum
and maximum EMS fields alongside its existing maximum XMS option. It
rejects out-of-range or missing arguments before writing the file. The
producer keeps the old PIF layout, priorities and zero-checksum convention
by starting from one authored format template whose title, program and
configuration fields are blank. The template is derived from the project
test writer, with no imported guest binary.

The EMS helper uses the existing PIF editor for that template and writes
the small fixed CONFIG.NT and AUTOEXEC.NT files through CMD. Four live
integration callers and the two CMD callers now share it. This removes
one native producer executable, its one build target and 69 producer
source lines. The editor adds three field options through its existing
parser. Pure DOS and XMS writers remain for the next migration batch.

## Evidence

The old writers were built and retained locally by PID 15320. PID 17036
built the candidate editor and matched complete PIF bytes for normal and
close-on-exit EMS, pure DOS and XMS. A missing argument and a value outside
the 16-bit field range both failed without changing the test file. The
actual CMD EMS route matched all six PIF and NT configuration files from
the old writer. A separate relative-path comparison preserved the old
caller-spelled path. Evidence is under build/t484-s5-memory-profiles.

PID 13796 passed real guest Mapping and CallReturn using the candidate
PIF editor. The produced COM test programs exercised original INT67
services and both observers reported normal launcher exit. PID 10020
completed the cached full host build and matched eleven raw and twenty-two
published host hashes. The coherent CMD publisher verified all fourteen
addons. Only PIF.EXE changed among the released addon binaries. The
published editor then passed the actual ems-call case through the
isolated preparation entry, including rejection of a non-stage path and
of profile replacement. Original guest binaries and winnt.zip were
untouched.

The published PIF.EXE SHA-256 is:
08B4F2CA568DF5DD0E488E0A35BE9B99D3E5DE06FD8095453828AD51DDD411F6

This delivery covers EMS and the tested PIF field behavior. It does not
close S5 or claim fresh Full acceptance of the product.
