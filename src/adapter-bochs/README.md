# adapter-bochs

The sole production caller of `bochs-core`, owning only Bochs construction and
opaque mechanical lifecycle. Existing project code is recovery evidence until
audited in the Bochs foundation task.

## M0 T273 S1 recovery register

The quarantined 11-file adapter candidate set is project-authored Bochs-only
assembly.  It has no imported Bochs body after the historical port-space
extraction, but must still be reviewed per file before any copy.  The audit
records that each future adapter input may depend only on `bochs-core` and
must not introduce OpenNT, DOS, VDM, WOW, BOP, SoftPC/CCPU or Win32 service
semantics.
