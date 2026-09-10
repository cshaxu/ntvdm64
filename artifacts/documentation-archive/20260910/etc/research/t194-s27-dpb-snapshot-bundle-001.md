# M0 T194 S27: DPB Snapshot Bundle

Packet: M0 T194 S27  
Disposition: immutable bundle assembled and hash-verified; no process started.

## Procedure

`tools/build/New-T190S5CommandBootstrapBundle.ps1` copied the S26 engine and the
retained S22 ROM root into
`artifacts/build/current/t194-s27-dpb-snapshot-bundle-r1`. The tool only copies
and hashes the declared members; it does not run the runner, shim, engine,
Bochs CPU loop, or guest.

## Manifest Identity

| Member | SHA-256 |
| --- | --- |
| Engine | `4A562EBDCD50E3FD16062013B1A9891FBEE84AB602B120925D46C756D7B01531` |
| BIOS | `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6` |
| VGA BIOS | `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6` |
| Option ROM | `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB` |

The engine identity is exactly S26. The three ROM identities are exactly S22.
No source, Bochs object, configuration, device, profile, root, or runtime
behavior changed.

## Follow-up

A later packet may admit one bounded, watchdog-controlled observation using
this exact bundle and the previously pinned CLI pair/root. Its result must be
classified from retained trace evidence; bundle assembly alone establishes no
guest execution or continuation result.
