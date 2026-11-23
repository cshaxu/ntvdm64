# T484 S3 P8: EMS/XMS profile production

Two interpreted producers are replaced by tools/build-memory-probes.cmd,
native NASM profile targets and the three unchanged C PIF/config generators.
EMS retains its separate probe output and runtime configuration destinations.
XMS preserves all three effective INT15/low-DOS modes and both startup-fault
states; ReserveInt15 continues to override LowDos as in the old entry.
Existing native-generator optional modes remain available in their binaries.

PID23096 compares the old and new EMS outputs plus all eight original XMS
switch combinations:9 EMS and56 XMS files are byte-identical. The three PE
tools differ only in verified COFF/debug timestamps. Rejections cover invalid
profile, existing output and an output outside repository build. Final checks
also cover invocation outside the repository, a spaced output directory and
unchanged helper hashes/mtimes on the repeated cached build.

Two failed setup attempts are retained. The first used a test runtime path
too long for the original PIF64-byte field and failed inside the unmodified
old native builder. The second exposed the new CMD entry's unnecessary
normalization of runtime path spelling, which changed embedded PIF bytes.
The entry now passes that text unchanged. Final comparisons use the original
supported short-path domain; no native generator, product or guest fix was
silently added. Original long-path limitations remain explicit in tools docs.

Cached full host build PID15816 passed. All11 raw host images and22 published
hashes match the current manifest; no product images require republication.
No guest runtime test is claimed: unchanged probe/config bytes reuse existing
behavior evidence under the owner's no-repeat authorization. Original guest,
product and mirror sources are untouched. No executable consumers of the two
retired producers remain.

[Matrix receipt](../../../artifacts/build-migration/t484/s3-memory-profiles/matrix.json)
and adjacent attempt/comparison/build/final receipts record this delivery.
The containing commit delivers P8. S3 remains active; other test producers,
S4 publication and S5 clean end-to-end acceptance remain incomplete.
