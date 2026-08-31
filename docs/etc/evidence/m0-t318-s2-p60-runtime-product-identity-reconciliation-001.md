# M0 T318 S2 P60 — fixed-container product-identity reconciliation

## Question

Does the retained `0xC0000005` result in P57 describe the current formal
CPU40 x86 product, or a distinct product copy within the same immutable media
container?

## Inputs

- The unchanged fixed container
  `build/M0-T318/S2/runtime-r33-config-system-media` and its manifest.
- Its current staged product, `original-softpc-process.exe`, and retained
  comparison copy, `original-softpc-process-p60.exe`.
- The container's normal startup record, `startup-observation.txt` and its
  JSON companion.
- The retained P60 result in
  `artifacts/research/m0-t318-s2-p60-fixed-r33-command-current-directory.json`.

## Procedure and observations

Both executable files have the same image size (3,082,752 bytes), but they
are not the same product identity.  Their SHA-256 values are respectively:

- `original-softpc-process-p60.exe`:
  `c4b32b31c21ea67742d54ebd1401f42eea1bb2cafcf1588885cebeae4f43d7b0`;
- current `original-softpc-process.exe`:
  `d7628154757c8d27320d48d29b9c1b6f85ded1a39daca16bb752cd87000e58d8`.

Their `.text` section hashes also differ.  The retained P60 launcher JSON
names the former copy explicitly and records `exited`, `0xc0000005`.  In
contrast, the current container manifest names the latter as its staged
product, and the normal record for that exact stage records `timeout` with the
observer sentinel `0x53504354` after 8000 ms.

The media/firmware manifest hash is the same in both records
(`e40dd218968ddfa009a9751cd534b3e1a5a243b20c61b5d43fe2eeaa94bcd11b`).
Thus this is a product-identity difference, not a reason to alter the fixed
observer, command line, media, firmware or console ownership.

## Result and consequence

P57's source-contract conclusion about the COMMAND `MAX_PATH + 1` receiving
buffer remains valid, but its P60 runtime result is **historical evidence for
the retained `-p60` executable only**.  It must not be used as the current
CPU40 product baseline or as an attributed original exception.  The current
fixed-container baseline is the staged formal product that remains alive to
the eight-second bound; that is liveness only and does not establish NTIO,
NTDOS, `EXEC`, or parent return.

This correction does not authorize a repeat run of unchanged code.  The next
runtime observation remains contingent on a source-shaped product repair;
the current work continues with a static original CPU40 owner-contract audit.
