# T225 S8 OpenNT NOSRVBLD Bootstrap Resolution 001

## Question

Does the pinned OpenNT 4.5 tools16 `NOSRVBLD.EXE`—distinct from the previously
rejected MS-DOS 4 tool—reproduce the NTIO message classes?

## Inputs and procedure

The tracked `tools/historical/opennt-4.5/nosrvbld.exe` is 16,299 bytes, SHA-256
`e49e569b7e0775bdd731bcf18cea1d85c29dabc1aa9a7285de51249387711665`.
It byte-matches the approved local OpenNT 4.5 tools16 copy.  Its companion
`buildidx.exe`, original `msbio.skl`, and original `usa-ms.msg` / `usa-ms.idx`
were copied to a disposable stage.

Under local DOSBox, the original `dosmsg.cmd` operation order was run:

```text
BUILDIDX USA-MS.MSG
NOSRVBLD MSBIO.SKL USA-MS.MSG
```

The emulator host process returns before its child has flushed generated
files; completion was therefore determined only after the generated files had
appeared and become readable.  No reference class file was supplied to either
tool.

## Observation

All four generated files exactly match the managed OpenNT source-input copies.

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| `MSBIO.CL3` | 1,339 | `279d5cf516a39248f910a17e02647f37737b2c4607a6cd119dc7236affed78e2` |
| `MSBIO.CL4` | 104 | `3c753ff790aad0f86a84130c8a40a23a420e8ebe842cb7dde442c55e9a525873` |
| `MSBIO.CL5` | 152 | `a929484571f630176541f39d8c411d6e924d8cd6358e6bc395f94b23295015d2` |
| `MSBIO.CL6` | 211 | `4a23d5d9e82762b4e0fde991c2c0347c1fe33e6097d2f1ed54da9dd6cfbd0440` |

## Interpretation

The prior audit remains valid for the MS-DOS 4 `NOSRVBLD.EXE`; it does not
apply to the distinct OpenNT 4.5 tools16 tool.  The NTIO message-bootstrap
gap is closed with the correct original tool.  The archived extractor remains
rejected and evidence-only.

## Follow-up

Create a narrow build runner that waits for an explicitly declared DOS batch
completion marker, then use the original tools16 bootstrap plus MASM/LINK16/
RELOC path to rebuild primary NTIO.  This resolution does not itself run an
NTIO assembly, link, fixture, guest trace, BOP, or runtime action.
