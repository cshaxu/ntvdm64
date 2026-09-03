# M0 T387 closure — Positional COMMAND CLI and package publication

## Closed scope

T387 made the normal product spelling a declared DOS command:

```text
ntvdm.exe dir
ntvdm.exe program.com argument
```

App serializes that declaration through the existing original-shaped Base VDM
record; it does not parse DOS syntax in `COMMAND.COM`, load a guest program or
replace SoftPC's parser. It forwards original SoftPC options and supplies the
original `-f` foreground launch precondition exactly once.

The selected CPU40/x86 product is published as `ntvdm64-0239.exe` in both
`build/output` and `O:\ntvdm64`, with the same bytes selected as
`O:\ntvdm64\ntvdm.exe`. The fixed installed runtime verified a positional
`echo` redirection with original COMMAND and exited successfully.

## No-argument disposition

No argument is explicitly user-visible unsupported for this product revision:
app shows a message explaining that it currently starts one DOS command per
invocation. It does not silently exit or pretend this is an interactive
COMMAND session. The queued interactive-console package owns the original
Base VDM command producer, Console input and SoftPC keyboard/IRQ recovery.

## Evidence

- [S1 positional declaration](../etc/evidence/m0-t387-s1-positional-command-declaration-001.md)
- [S2 installed package publication](../etc/evidence/m0-t387-s2-runnable-package-publication-001.md)
- [T387 operational plan](../etc/operations/m0-t387-positional-command-cli-001.md)
