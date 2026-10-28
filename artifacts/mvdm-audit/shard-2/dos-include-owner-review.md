# DOS shared guest includes and source roles

This shard-2 group covers eight pending guest-source paths. The [observer](dos-include-owner-observer.py)
and [receipt](dos-include-owner-observations.json) verify each current file
against both pinned original editions and the DOS16/WOW32 frozen catalog
identities. All retain 100% original bytes. No guest behavior or runtime
restoration percentage follows from source identity or a textual include.

| Path | Proposed role | Evidence and remaining boundary |
| --- | --- | --- |
| `dos/v86/inc/oemnum.inc` | excluded candidate: documentation-only file role | Its 17 lines are blank or MASM comments, explicitly identifying an OEM assignment list rather than assembler definitions. This does not exclude DOS GetVersion, OEM identity or compatibility semantics. |
| `dos/v86/h/bios.inc` | pending | Contains BIOS interrupt macros. The actual original assembler consumer and search-root/conditional path remain unproved; lack of a literal hit does not exclude it. |
| `dos/v86/h/process.h` | pending | Original `buildmsg.c` includes `"process.h"`, and command's recipe invokes `buildmsg.exe`; the tool's actual producer and header-search resolution remain unproved. Same-name host SDK headers are different objects. |
| `dos/v86/h/stddef.h` | pending | Generic-named declarations have possible SDK collisions; identify the actual original guest compiler consumer and search order before disposition. |
| `dos/v86/inc/msdata2.asm` | pending | Includes `msdata.asm`/`msinit.asm`, while the original DOS kernel recipe names `msdata.asm` directly. Establish whether the alternate wrapper is used by another profile before deciding its role. |
| `dos/v86/inc/pathmac.inc` | pending | Defines a MASM path-label macro. Identify generated, conditional or external original consumers before deciding whether the file is an unused historical role. |
| `dos/v86/inc/pcgequ.inc` | pending | Contains graphics device equates; find actual original consumers and profile, without excluding graphics capability. |
| `dos/v86/inc/pcinput.inc` | pending | Contains real input routines rather than only declarations. Establish caller, hardware state ownership and target; absence of a literal include is insufficient. |

The earlier [shared include follow-up](shared-guest-include-followup.md)
records related search and provenance limitations. This pass adds both
original-edition identity checks and a reproducible comment-only check, not
execution evidence. The coordinator must replay and review before applying the
one proposed file-role exclusion. No build, guest, product, shared ledger,
running process or Ninja tree was modified.
