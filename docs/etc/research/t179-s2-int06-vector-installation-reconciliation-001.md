# T179 S2 INT 06 installation reconciliation 001

## Source closure

The retained source identifies the writer of the observed vector state.
`nt_bop.c` `MS_bop_F` calls `kb_setup_vectors()` after spckbd is loaded and
the IVT pseudo-ROM hooks are present. `spckbd.asm`'s vector grabber then writes
the INT 06 IVT offset and segment to `int06h_vector`. That routine is exactly:

```text
BOP 06h
jmp iret_com
```

Thus the normal-return trace target `073B:0740` with bytes `C4 C4 06` is the
source-defined SoftPC pseudo-ROM handler, not an unexplained guest vector
corruption or an adapter-installed BOP.

## Runtime reconciliation

The S19 trace shows Bochs delivering #UD through precisely that IVT target,
while the then-current adapter passed selector `06h` through. Its repeated
six-byte fault-frame pushes and final stack-limit triple exception are the
expected consequence when the vector routine re-enters the same #UD without
the original SoftPC `illegal_op_int` consumption path.

`illegalp.c` is the original owner: it reads the CPU-created frame at SS:ESP,
reports, advances saved CS:IP by one byte, then calls `unexpected_int` before
guest IRET. T176 already links this unchanged handler inside the separately
owned machine-composition component; it is not an adapter host service and it
does not make DOS/CLI results.

## Classification

| Surface | Owner/disposition |
| --- | --- |
| INT 06 IVT installation | Original NTDOS/SoftPC spckbd initialization; source-proven. |
| #UD/real-mode frame delivery and stack enforcement | Bochs native CPU mechanics; source/trace-proven. |
| Selector `06h` semantic handling | Original SoftPC `illegal_op_int` machine component; linked but not observed on the normal-return target path. |
| Adapter host-service plane | Explicitly uninvolved; pass-through is correct. |
| CLI result | Explicitly uninvolved. |

## S3 decision input

The unknown writer is resolved. A single, separately bounded observation of
the existing native-POST/machine-composition binary with the retained
source-built SHARE input would test the already-linked original handler in its
correct source-defined context. It requires no source change, new device,
vector synthesis or BOP provider. S3 must decide whether that one observation
has sufficient value; otherwise this package closes with the static closure.
