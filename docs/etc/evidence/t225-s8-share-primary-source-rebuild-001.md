# T225 S8 SHARE Primary Source Rebuild 001

## Question

Can the missing primary `SHARE.EXE` input be reproduced from the pinned OpenNT
source and the locked historical tools, without consuming a reference binary?

## Inputs

- OpenNT source: `O:\repos.external\OpenNT`, revision
  `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`;
  `base/mvdm/dos/v86/cmd/share/share.asm` and original `share.lnk`.
- Historical assembler: `tools/historical/opennt-4.5/masm.exe`, SHA-256
  `e54afaa9565e046cd862c29a015d685de43311acccf796469030e162ced6d00d`.
- Historical linker: `tools/historical/opennt-4.5/link16.exe`, SHA-256
  `910ed26e6c8d72d64faa82a7eeabb6d5c002b347024c8f1d778fddbbb8ac1b00`.

The source and response file were copied into a newly created disposable
`build/M0-T225-S8/share-source-rebuild-r2/` directory.  No `.EXE` from
`refs/opennt`, the external OpenNT tree, or the approved reference sequence
was copied or supplied to either tool.

## Procedure

The original makefile's assembler flags were used with MASM's non-interactive
file form:

```text
MASM.EXE -Mx -t -W1 share.asm,share.obj;
LINK16.EXE /E @share.lnk
```

The first disposable invocation omitted the explicit object filename and
MASM therefore requested it interactively.  It produced no output artifact.
The second, separate disposable invocation above completed normally.

## Observation

`LINK16` exited successfully and produced `share.EXE` with:

| Property | Observed value |
| --- | --- |
| Bytes | 882 |
| SHA-256 | `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc` |

This is the locked identity required for the primary SHARE role and exactly
matches the prior source-build evidence.  Equality is an identity check only;
this reconstruction remains provenance-distinct from the approved reference
binary sequence.

## Interpretation and follow-up

The SHARE portion of the previously absent primary sequence is reproducible
from source under the approved historical toolchain.  This is not yet an S8
fixture input: a fresh, manifest-bearing primary stage must be created after
the remaining NTIO, NTDOS, and COMMAND source rebuilds use their respective
original closure recipes.  No fixture, guest trace, BOP, machine, or profile
behavior was run or changed.
