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

## Correct short-root container

The user-designated `O:\ntvdm64` already holds a format-3 runtime container.
Its sixteen fixed media assets all match `runtime-manifest.json` exactly. Its
complete generated shell value is:

```text
O:\ntvdm64\mvdm\system32\COMMAND.COM
```

This is 36 characters. Format 3 explicitly makes only
`original-softpc-process.exe` replaceable for a formal-product observation;
the media assets remain immutable and hash-checked.

## Disposition

S5 is capacity-invalid and yields no COMMAND/NTDOS/CPU inference. S6 is the
one permitted replacement observation: it may replace only the product EXE in
the already verified `O:\ntvdm64` fixed-media container and must retain the
same product SHA-256 and `TrnSeg` two-byte observer configuration. It may not
change, re-stage, delete or reinterpret any media input.
