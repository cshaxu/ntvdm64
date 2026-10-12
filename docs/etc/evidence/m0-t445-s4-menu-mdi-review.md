# T445 S4 standalone menu and MDI review

## Question and boundary

Does the standalone component implement the admitted menu/MDI presentation
while preserving native management? S3 is owner-closed; S4 remains active.
This is review evidence, not owner acceptance. The
[working plan](../operations/t445-win31-window-style-plan.md) retains original
source comparisons, failed attempts and run chronology. No product component,
guest/mirror, global setting or production release changes. Native animated-menu
blankness remains the independent S7 question, not a repair claimed here.

## Candidate and procedure

Background tests/observation/verify-win16-style.ps1 run at
build/M0-T445/S4/r052: MSVC x86/x64 /MT /O2 /W4 /WX, Ninja, standalone library
and authored fixtures only. Runner PID35852 started 2026-10-12 13:38:59,
180-second deadline, exit0. Both widths pass 27,261 retained checks plus
menu/MDI and lifetime assertions. All 24 recorded input hashes were rechecked
after execution: zero mismatches.

| Executable under r052 | SHA256 |
| --- | --- |
| x64/win16-style-demo.exe | C76919BBB7071A80633BF946E04B7E9A0A85F5B45F9BE17EC05FD7E91F97E096 |
| x86/win16-style-demo.exe | 35C62AB90A50CF2633EF5F47FA2C5662070D7B64F7986CBCD90E11D84C3E2A9F |

r052-glyphs/resources.json verifies CHECK14x14 and MNARROW7x11 against pinned
VGA.DRV resources. No original executable code is imported.

## Requirement coverage

| Requirement | Evidence and result |
| --- | --- |
| System/ordinary menu painting | menu_items/painter/scope implement string rows, disabled stipple, selection, separators, shortcuts, original check/arrow and border; popup captures reviewed |
| Menu input | Queued native loops pass Alt/F10, arrows, Escape, mnemonics, hover switching and disabled/direct-item navigation; not physical input acceptance |
| Menu ownership | Temporary item data/type restored; application owner-draw retained; nested-loop and callback removal/destruction cases pass |
| MDI | Child geometry, activation, maximize/restore, merged system/restore controls, cross-owner WM_NEXTMENU and child-only close pass both widths; maximized capture reviewed |
| S3 preservation | 27,261 checks per width retain frame/client geometry, titles, palettes, native-button suppression and GDI lifetime; recorded GDI cycles show no net growth |
| Source boundaries | Standalone graph only; no production integration or mirror change |
| Visual/physical acceptance | Owner trial of gallery12/15/18 remains pending; prints and queued messages do not prove DWM/RDP or actual pointer behavior |
| Delivery | Governance/link and diff checks pass; implementation committed and pushed as de5526a16; unrelated queue/proposal work excluded |

## Separate review finding

Reviewed item restoration, nested hook scopes, cross-owner state pins and
reentrant callback exits. STYLE_NCUAHDRAWCAPTION/FRAME incorrectly shared the
mouse-move branch, allowing non-mouse payloads into menu hover selection.
r052 separates these drawing notifications. A focused test sends payloads
inside a real menu cell and verifies no selection change, on both widths.
This local dispatch repair does not diagnose native animated-menu blankness.

## Limits and follow-up

Implementation delivery de5526a16 is on origin/main. This is the standalone
trial delivery, not S4 owner acceptance. All remaining dirty paths belong to
the pre-existing WOW queue/proposal work, not the delivered component.

Arbitrary owner-drawn/bitmap bars, scrollbars, tool/layered/custom-client-edge
forms remain unsupported. Minimized presentation remains native as admitted
in S3. Arbitrary cross-thread/custom WM_NEXTMENU targets and external consumers
of native menu-bar geometry are not general compatibility claims. Final owner
acceptance remains open; keep these boundaries explicit during S5/S6 integration.
