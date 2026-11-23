# T484 S3 P23: consolidate COMMAND child production

New-T317CommandChildNinja.ps1 had no current caller and linked only three
sources from before the request-object boundary. Its fixture expected an
ambient COMSPEC child and could not link the current executor/request API.
It is retired together with its duplicate fixture, not counted as a test pass.

The existing native ntvdm-command-launch-test exercises the real current
preparation and start implementations with bounded product-path/hook seams.
It checks separate environments and working directories, per-child output,
exit codes 65/66, handle ownership and rollback, and single-use requests.
Three parent standard-handle invariance assertions preserve the old fixture's
remaining useful contract. Native path selection and hook injection are seams;
this is not an end-to-end Run16 or guest execution test.

The explicit CMD entry is tools/build-fixtures.cmd x86 command-launch.
Final test PID33008 returned zero with 74 assertions. The
[receipt](../../../artifacts/build-migration/t484/s3-command-producer/result.json)
records executable identity. Production and original guest sources are unchanged.

Cached full compilation and separate 11-raw/22-publication identity checks
complete this bounded delivery; unchanged images require no recopy. The
containing commit is P23. Removing the last literal Ninja generator does not
complete S3: interpreted inline and diagnostic producers still require review,
and S4 publication plus S5 final clean acceptance remain pending.
