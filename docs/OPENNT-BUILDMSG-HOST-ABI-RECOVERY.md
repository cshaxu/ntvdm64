# OpenNT BUILDMSG Host ABI Recovery Record

Status: Gate 0 host-tool compatibility evidence, 2026-08-07. This describes
only the disposable modern-host builder used to generate COMMAND message
includes. It is not guest code, a DOS runtime service, or a retail BUILDMSG
replacement.

## Failure

The managed `COMMAND` source stage initially generated a malformed
`COMMAND.CLF`: from message 1024 onward, distinct `:DEF` records were emitted
as the body of one message. The resulting byte-length expression exceeded the
one-byte message length field, so both JWasm and the OpenNT-source-built
MASM386 rejected `tdata.asm`.

The `command.skl`, `usa-ms.msg`, and `usa-ms.idx` inputs were hash-identical
to the earlier structural stage. BUILDMSG debug output showed that it had read
the individual records correctly. In `DefRecord`, however, the historical
source parsed a status token through `%s` into this local:

```c
char MsgStatus;
```

On the original 16-bit build this stores `"U\0"` into a layout that happens to
leave `MsgNumber` usable. On the modern 32-bit host it overwrote the adjacent
integer, making all later comparisons equal the first matching message
number, 1024.

## Host-Only Correction

`toolchain-probe/stage-opennt-buildmsg.ps1` changes only the generated host
copy to `char MsgStatus[2];`. It retains the original parser, `%s` conversion,
record ordering, and emitted guest bytes. This is alongside the already
recorded host-only corrections for 16-bit message-number wrapping, mutable
literal handling, bounded directive parsing, and temporary scratch capacity.
The imported OpenNT source is unchanged.

## Verification

The fresh `buildmsg-stage-v13` output has:

| file | SHA-256 |
| --- | --- |
| `COMMAND.CLF` | `BF0BC8A8548C06987EF3EA9157962B3D183FE5FE8C2627C8AABEDF470588D8EE` |

That hash equals the previously known-good v18 generated input. Its message
1024 block ends before the separate message 1025 label, restoring the required
one-byte length expressions. `command-stage-v20`, created from manifest-
verified `src/opennt`, uses this builder and successfully assembles `tdata`.

This proves a narrow host ABI correction for this generator path. It does not
prove historical BUILDMSG executable identity, any guest runtime behavior, or
retail byte equivalence.
