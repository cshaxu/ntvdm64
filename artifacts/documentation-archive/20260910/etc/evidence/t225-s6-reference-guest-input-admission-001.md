# T225 S6 Approved Reference Guest Input Admission

## Question

May the T225 S6 four-image installation fixture use locally available OpenNT
binary files whose identities exactly match the already locked guest-artifact
ledger, while the local historical toolchain remains unavailable?

## Approval and inputs

The owner explicitly approved this controlled staging on 2026-08-16 ("允许").
The source tree is `O:\repos.external\OpenNT` at revision
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`.

| Role | Source path | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| NTIO | `base\mvdm\dos\v86\doskrnl\bios\NTIO.SYS` | 33,792 | `cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937` |
| NTDOS | `base\mvdm\dos\v86\doskrnl\dos\NTDOS.SYS` | 27,858 | `957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84` |
| COMMAND | `base\mvdm\dos\v86\cmd\command\COMMAND.COM` | 50,384 | `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43` |
| SHARE target | `base\mvdm\dos\v86\cmd\share\share.EXE` | 882 | `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc` |

These values match the identities recorded in the locked source-built ledger,
but they remain a **separate guest-image sequence**: the files above were
obtained from the local OpenNT tree, whereas the primary sequence is produced
by the project's original-toolchain source-build closure. The equality of a
file hash proves a content correspondence between the two sequences; it does
not collapse their provenance, turn this staging pass into a source build, or
change any source-built classification. When both sequences are physically
available, they are deliberately useful as cross-validation inputs.

## Procedure and result

`tools/build/Stage-T225S6ReferenceInputs.ps1` verifies the source-tree
revision, byte count and SHA-256 for every input before copying it to
`artifacts/research/t225-s6-reference-inputs-r4`. It emits a BYOB fixture
profile and a machine-readable source/role/hash/size manifest in the same
directory. The script refuses to overwrite an existing staged root.

## Boundary

The staged set is classified as an **approved-reference-binary-research-input**.
It is allowed only as the secondary sequence for T225 S6 fixture installation
verification and cross-validation. It is not a default build input or runtime
discovery source, release material, or a basis for a source-build claim. It
does not admit a guest trace. A subsequent fixture result may prove only
exact-image profile installation; it cannot prove NTDOS `$Exec`, COMMAND
lifecycle, or child return.

## Follow-up

Run the same four-image fixture independently against (1) the primary
original-toolchain source-built input root, once restored, and (2) this
explicit secondary reference input root in fresh `build/t225-s6/<run>/`
directories. Record Direct and Readonly installation outcomes separately for
each sequence and compare them; Overlay and retired Virtual must remain
rejected. A result from one sequence never substitutes for a missing result
from the other.