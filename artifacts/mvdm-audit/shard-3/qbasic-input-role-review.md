# Historical QBasic binaries and sample — shard 3

Scope: `dos/v86/cmd/qbasic/{money.bas,msherc.com,qbasic.exe}`, selected from current shard 3 pending rows. `qbasic-input-observer.py` and captured JSON compare source identity with both original editions, archive members, installed system32 paths and installed root QBasic. This is read-only: no guest program, compiler, build or media modification was performed.

Historical `qbasic/makefile:14-24` binplaces prebuilt qbasic.exe and msherc.com, money.bas, help files and other examples; it does not build QBasic from source. MONEY.BAS describes itself as a QBasic example. The source tree binaries and examples alone cannot establish functionality or that they are the binaries installed by the present package.

| file | disposition | exact identity and boundary |
|---|---|---|
| `money.bas` | pending | Matches both reference editions; historical sample is not found by exact name in the package or checked system32 root. Another installation/media path was not exhaustively searched. Runtime sample behavior and output data lifecycle untested. |
| `msherc.com` | pending | Matches both reference editions; historical packaging recipe includes it, but no exact named member is in the package and no checked system32 copy exists. The historical helper's actual usage and device behavior remain unproven. |
| `qbasic.exe` | pending | The mirror matches OpenNT 4.5 (257,824 bytes, SHA-256 `e8341b546cc12cfd4d59a7eb24e116f2cfbcae71779d9a7f712902f65e5deb6e`); OpenNT reference path is absent. Archive `WINNT/QBASIC.EXE` is a different 254,799-byte image, SHA-256 `ecca463f1820749a1408fc8ef3799d23fb232c188669ca9d5db3769ce628746b`, matching installed `O:/winnt/QBASIC.EXE`. This is an identity difference, not a demonstrated error or claim of upstream provenance for the packaged binary. |

No exact named package member or system32 file for the first two means only those specific locations were absent at observation time. The root QBasic match establishes bytes only, not execution, BASIC compatibility, any sample functionality or a restoration percentage.
