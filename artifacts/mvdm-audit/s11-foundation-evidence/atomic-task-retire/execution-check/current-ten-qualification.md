# S11 current ten-input candidate qualification

Candidate: `build/t473-s11/current-ten-image-package`; input manifest SHA-256
`37823EA1869847EDE3E2A89EF7FC8AD120713A315F268A6A343D7916E438F8A8`.
This candidate is unpublished; S11 and T acceptance remain open.

| Requirement | Evidence | Boundaries |
| --- | --- | --- |
| Ten current inputs | `current-ten-manifest.json`; post-test input hash comparison | All ten matched at last check; not functional proof. |
| Guest media | `verify-guest-media.py` on this package | 59 immutable files passed; configuration contents excluded. |
| Cached builds | [Build status](../../../s11-background-build-status.md) | Selected x86/x64 graphs passed incrementally; not clean build or publication. |
| Console and Window commands | [Matrix](current-ten-matrix/) | 17 + 17 expected results passed. |
| RPC and lifecycle Control | [Control](current-ten-control/) | All twelve gates passed; preceding parameter failures retained. |
| Original memory export | [VDD witness](../../export-template-candidate/current-ten-live/) | Actual worker path passed query, allocation, reallocation, free and one exhaustion case. |
| Shared WOW tasks and dialog | [Witness](current-ten-idle-dialog/) | Two tasks and 17 dialogs passed; not all callback interleavings. |
| SOL Options and gameplay | [Options](current-ten-sol/), [gameplay](current-ten-sol-gameplay/) | Focused interactions passed; Options remains seconds slow. |
| Notepad, Write and Program Manager | [Find](current-ten-notepad-write/), [New](current-ten-progman/) | One normal dialog interaction each; no claim for all app features. |
| Required Full product suite | No current-candidate passing receipt | Incomplete. Automatic review rejected Winmine after a previous probe modified the user's VirtualStore profile; do not run indirectly or claim a historical Full pass applies to this package. |
| Adoption / retirement interleaving | `lock-contention-r2/`, `native-adoption/` | Lock exclusion and separate paths have evidence; exact combined interleaving is not exercised. |
| Remaining S11 and T-wide boundaries | [Recovery worklist](../../../recovery-worklist.md), approved stage plan | Open. Source classification, similarity and isolated witnesses do not prove all capabilities. |

A candidate and focused tests do not authorize partial publication or prove T completion.