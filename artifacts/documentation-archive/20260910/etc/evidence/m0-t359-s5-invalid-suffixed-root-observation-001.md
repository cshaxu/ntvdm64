# M0 T359 S5 P1 — invalid suffixed-root observation disposition

## Attempt record

S5 used the new stage
`O:\ntvdm64\m0-t359-s5-command-transient`. Its `mvdm` directory itself was
44 characters, but the original NTDOS configuration contract consumes the
complete generated value:

```text
O:\ntvdm64\m0-t359-s5-command-transient\mvdm\system32\COMMAND.COM
```

That value is 65 characters, exceeding the original 63-byte `commnd` input
limit. The console-owned process timed out while its primary thread was in
`app_report_media_root_rejected`; the mapped frames identify an app-owned
capacity rejection, not SoftPC or COMMAND execution. No BOP, SAS-store or
exception report was produced.

## Superseded short-root calculation

The user-designated `O:\ntvdm64` already holds a format-3 runtime container.
Its sixteen fixed media assets all match `runtime-manifest.json` exactly. Its
complete generated shell value is:

```text
O:\ntvdm64\mvdm\system32\COMMAND.COM
```

This is 36 characters only for the command-image pathname.  That was not the
complete original `shell=` value: unchanged `cmdconf.c` also appends
` /p <SystemRoot>\\system32`, repeating SystemRoot.  With
`O:\\ntvdm64\\mvdm` the actual generated value is 65 visible bytes, not 36,
and exceeds the original 63-visible-byte `commnd` input limit.  This correction
supersedes the earlier short-root calculation.  Format 3 explicitly makes only
`original-softpc-process.exe` replaceable for a formal-product observation;
the media assets remain immutable and hash-checked.

## Disposition

S5 is capacity-invalid and yields no COMMAND/NTDOS/CPU inference.  The later
S6 observation confirms app-owned rejection at the verified root.  Recovery
belongs to original-SystemRoot package-layout binding, not a COMMAND, DEM, CPU
or BOP repair.
