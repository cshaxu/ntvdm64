# M0 T484 delivery reconciliation

T484 migrated the supported build, generator and release entry points to CMD.
CMD invokes the native toolchain and existing project tools. Original guest
media and the package structure were preserved. The final S7 delivery records
verification of the additional DOS error-dialog mouse item.

| Stage | Delivered scope | Evidence |
| --- | --- | --- |
| S1 | Ordered inventory and migration design | [Inventory](t484-s1-build-inventory.md) |
| S2 | Eleven-image host build composition | [Worker build](t484-s2-worker-cmake.md) |
| S3 | Addon and test producer dispositions | [S3 closure](t484-s3-closure.md) |
| S4 | Package preparation and test input chains | [S4 matrices](t484-s4-matrix-preparation.md) |
| S5 | Shared PIF and INI producers | [PIF layouts](t484-s5-pif-layouts.md), [INI editor](t484-s5-shared-ini.md) |
| S6 | Clean and cached builds, negative cases, release identity and live launcher matrices | [CMD acceptance](t484-s6-clean-cmd-acceptance.md) |
| S7 | Release of mouse capture and confinement at the DOS modal dialog boundary | [S7 verification](t484-s7-dos-error-dialog-mouse.md) |

S6 verified the clean and cached build graph, 14 Ninja graphs with 2,199
expanded command rows, 15 released addon hashes, 14 Win3.1 cases, and four
Win95 startup/exit cases. The Win95 return-code-1 boundary and the initial
exit timeout followed by a passing retry remain in the S6 evidence.
The Windows live launcher matrices were recorded against the S6 package.

The final S7 package is `0b82f7de-640f-43e1-962e-73f904c0bf60`.
Its cached full build passed, Full passed 20 of 20 gates, the native capture
contract passed, and five isolated real error-panel trials passed. All eleven
images were published to both destinations and the 22 installed/repository
copies were verified against the final manifest. The Full test launcher
still runs existing test harnesses in their original languages. The CMD-only
build guarantee applies to production, code generation, fixture production
and publication entry points.
