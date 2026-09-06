# M0 T391 S7 — tight `COMMAND /C` app-entry compatibility

## Defect

The child-product app-entry boundary correctly resolves `command /c ver`, but
the DOS spelling `command/c ver` formerly supplied `command/c` as one Windows
image token.  Because `/` is a Windows path separator, product-directory
resolution was skipped, the token remained unresolved, and the established
unresolved disposition delegated the whole text to host `COMSPEC /c`.
On x64 Windows that host shell could then attempt to start the DOS
`COMMAND.COM` directly.

The fault is before VDM startup and is not a `54:08` image decision: `54:08`
still starts the child product with its tail unchanged.  The child app entry
remains the only image resolver/classifier.

## Repair

`mvdm_image_resolve_command_line` now recognizes only a first token
equal to `command/c` or `command.com/c`, case-insensitively.  It splits that
exact terminal `/c` into the DOS image name and an ordinary `/c` argument,
then follows the unchanged product-directory, current-directory/PATH and
image-type flow.  It does not reinterpret slash-containing paths, other DOS
programs, or arbitrary application switches.

## Published x86 verification

The complete formal x86 product was rebuilt and published as
`O:\\ntvdm64\\ntvdm32.exe` and `build/output/ntvdm32.exe`.

SHA-256: `053da3d0b91b8e1f50393b90fed3883ecc199ead64b21f29b3b9e8aeb4969890`

The no-diagnostic clean-Console integration fixture returned zero for all
rows, each `VER` row requiring `MS-DOS Version 5.00.500` in the Console
buffer:

```text
case=command/c ver
result=0
case=command.com /c ver
result=0
case=command.com /c command.com /c ver
result=0
case=command.com [Console exit]
result=0
```

The fourth row retains the existing real-Console `exit` check for an ordinary
second `COMMAND.COM` prompt.
