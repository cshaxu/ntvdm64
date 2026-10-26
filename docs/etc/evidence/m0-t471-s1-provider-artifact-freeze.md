# M0 T471 S1 — whole-provider prerequisite and artifact freeze

## Question

Can one ordinary, reproducible ten-image runtime package enter the final
WRITE/WINMINE/SOL acceptance without treating an earlier provider limitation,
test-only fixture, or unfinished implementation as a pass?

## Inputs and procedure

The selected artifact is the unchanged T467 S3 package:
`build/t467-s3/package-r2/SYSTEM32`, with the release identity in
`assets/release/release-manifest.json`.  On 2026-10-26, each manifest row was
SHA-256 compared with its release, staged-package and installed
`O:\winnt\system32` counterpart.  All ten rows matched the manifest:

| Image | SHA-256 | Result |
| --- | --- | --- |
| `run16.exe` | `91811CEF9FC34564827AE530D700F46CF06BA7763035FC3DF206D1674AB2A30D` | three-way match |
| `ntsrv.exe` | `6BB3A000B379C669B3EE4BDA5C8AB19FCDA05A5A9CBD236A9AEBA27CCF5D4058` | three-way match |
| `ntcon.exe` | `9A192E04B56FBE8AD190FC207CF0D6755FF2D32DAAC0821399A6DAAAC46F5A8E` | three-way match |
| `ntvdm.exe` | `7F2312AF2CC8480264B4935E2F5C9910D1A99D38C78C8F8DB57156FF1168611B` | three-way match |
| `ntvwm.exe` | `A55E480E1CFC76508757501A20E62867A660CB95FED458040E4B2B3829410B46` | three-way match |
| `ntmon.exe` | `08BE0C0125C1B97C2C0E27FB5CE6A124594064A846A8810CA0AC572F91BC5CAC` | three-way match |
| `WOW32.DLL` | `506586EFB2B4C953C7D10468A592570E470209A6224A3B3D244880B03F7FFC23` | three-way match |
| `VDMREDIR.DLL` | `BC103561A4ED4656A08E657F4960D47630BB39BC81F11E54B59436F41777BE96` | three-way match |
| `nthook32.dll` | `8BF9C75AEDF32D7CD8BFF36D5727252834C487E8EFB195FC7F213BC78D5C427E` | three-way match |
| `nthook64.dll` | `D93FE883E7D5910568BD9C184A5866E84AF3EE9F50EFC7686974385F3D73ACB2` | three-way match |

T470 and this S1 change only evidence/documentation; they do not alter that
package or any guest media.

## Provider and edge reconciliation

The program transfer table assigns every provider to one closure.  The rows
below reconcile the prior owner record, selected normal/failure/retirement
evidence, and its accepted limit.  `Explicit limit` is not a functional pass
and cannot be replaced by a temporary W20 provider.

| Workstream | Owner closure | Disposition for T471 |
| --- | --- | --- |
| W01 messages/callbacks/execution | [T449](../../history/m0-t449-closure.md) | Closed original execution, callback and targeted-retirement owners; no second USER transport/scheduler. |
| W02 USER objects/shared view | [T451](../../history/m0-t451-closure.md) | Closed publication, mutation, destruction and direct-root CUI ownership. |
| W03 modules/memory/aliases | [T452](../../history/m0-t452-closure.md) | Closed loader/alias lifetime; retained WinOldAp evidence is bounded. |
| W04 files/environment/OEM | [T453](../../history/m0-t453-closure.md) | Closed original OEM and rollback provider scope. |
| W05 resources | [T454](../../history/m0-t454-closure.md) | Closed lookup/load/lock/free ownership. |
| W06 dialogs/input/hooks/timers | [T457](../../history/m0-t457-closure.md) | Closed selected control/callback/input/retirement scope. |
| W07 GDI/DC/drawing | [T455](../../history/m0-t455-closure.md) | Closed selected DC/DIB/palette lifecycle scope. |
| W08 fonts/text/metafiles | [T456](../../history/m0-t456-closure.md) | Closed selected text/font/metafile conversion and release scope. |
| W09 menus/resources/accelerators | [T458](../../history/m0-t458-closure.md) | Closed selected conversion; private desktop refusal remains an environmental non-pass. |
| W10 Clipboard | [T460](../../history/m0-t460-closure.md) | Closed selected CF_TEXT/CF_DIB, deferred render and task-loss routes; fresh peer recompilation is explicitly test-host-sensitive. |
| W11 DDE | [T465](../../history/m0-t465-closure.md) | Closed selected classic transport; DDEML, cross-worker and arbitrary peer-loss routes remain outside scope. |
| W12 Shell/Registry | [T461](../../history/m0-t461-shell-registry-closure.md) | Closed selected worker-local façade and parent-owned durable overlay. |
| W13 Winsock | [T467](../../history/m0-t467-closure.md) | Closed selected loopback/synchronous/async route; arbitrary external-network and forced-race cases remain limits. |
| W14 COMM | [T469](../../history/m0-t469-closure.md) | Original route fixture passes; real external peer, one-shot COM3 profile and inherited first-port task-loss discrepancy remain explicit non-passes. |
| W15 printing/spool | [T468](../../history/m0-t468-closure.md) | Selected HDC/common-dialog route passes; legacy raw-profile/device-capabilities endpoints and abrupt raw job remain non-passes. |
| W16 multimedia | [T466](../../history/m0-t466-closure.md) | Selected host binding is closed; no physical wave/MIDI/MCI completion claim. |
| W17 ToolHelp/debugger | [T470](../../history/m0-t470-closure.md) | Original debugger ownership is closed; ToolHelp query/class enumeration are immutable guest/provider non-passes. |
| W18 common dialogs/OLE | [T459](../../history/m0-t459-closure.md) | Selected modeless dialog/OLE alias path closed; guest OLE client/server remains an immutable boundary. |
| W19 hard errors | [T464](../../history/m0-t464-closure.md) | Public response/cancel/fatal-tail binding closed; private NT4 hard-error UI is unavailable. |

The bounded T462 residue attribution is also retained at
[T462](../../history/m0-t462-wow-window-residue-closure.md): no reproducible
normal-path painter owner was found, so it grants neither a masking repair nor
a visual pass.

### C11 and C12 sweep

`C11` requires every selected source/consumer edge to have one original owner,
one build/import/call path and no placeholder.  The table above resolves those
edges only through their named closure; T471 introduces no registration,
dispatch or direct-data bridge.  The sensitive shared boundaries remain:
modern USER transport (W01), public HWND/shared view (W02), alias mapping
(W03), callback/retirement (W01/W06), and original provider-specific direct
data (W05/W07/W10/W14/W17).  None is reimplemented by this acceptance task.

`C12` requires a same-artifact delivery.  This freeze supplies that identity;
S2--S4 must run against only this manifest-equivalent ten-image set.  A source
or package change invalidates this freeze and requires a new three-way hash
record before any result is aggregated.

## Admitted S2 matrix

S2 must use the frozen artifact and actual immutable applications, without a
test-only substitute:

| Application | Required ordinary interaction and lifecycle |
| --- | --- |
| WRITE | edit/select; menu and dialog; save/reopen; failure/cancel; resize; normal close/restart. |
| WINMINE | reveal/flag/timer; menu/difficulty/new game; normal close/restart. |
| SOL | deal/legal move/draw; options; repaint; normal close/restart. |

Every result must record the exact package identity.  A provider regression is
returned to the owning workstream above rather than repaired in S2.

## Conclusion

S1 passes its finite admission gate: there is one hash-matched ordinary
artifact, every W01--W19 prerequisite has a named closure or an explicit
non-pass, and no unassigned provider work has been admitted.  This is not a
whole-provider runtime acceptance; S2--S4 remain required.
