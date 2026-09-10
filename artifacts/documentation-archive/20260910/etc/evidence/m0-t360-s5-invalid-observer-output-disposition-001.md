# M0 T360 S5 — invalid observer-output disposition

The product and fixed package inputs were valid, and the existing x86 observer
was rebuilt to report the selected stage root rather than the retired
`stage\\mvdm` spelling.  However, the requested report path was
`build/M0-T360/S5/original-systemroot-observation.txt` and its parent directory
had not been created.

`ObserveSoftpcStartup.mjs` launches the observer and then writes its mandatory
JSON envelope. The envelope write failed with `ENOENT`; the observer's own
report also had no writable parent. Thus no terminal, exception, SAS-store or
frame record exists. This attempt is invalid as runtime evidence and cannot
characterize app, SoftPC, COMMAND, DEM or CPU behavior.

T360 S6 is the sole replacement. It retains the same product, stage and
workload and changes only the external observer-output precondition by creating
the report directory before launch.
