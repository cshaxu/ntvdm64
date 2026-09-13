# Console input, display and resource restoration

## Status and objective

Unadmitted candidate requested by the owner on 2026-09-12, immediately after
the [DOS file-service package](proposal-dos-file-service-restoration-001.md)
in [Queue](../states/QUEUE.md). No numeric T or active S is allocated here.
Restore original-owner policy and minimize unnecessary diff/overlay code while
preserving native Console COMMAND/EDIT keyboard and mouse usability. This is
an audit-and-repair package, not a report-only exercise.

The [source audit](../etc/evidence/opennt-replacement-audit-20260912.md#consolidated-discussion-index-confirmed-owner-overlap)
supplies D09, D23-D25, D28-D30 and D34. Related unresolved keyboard and Console
capability questions are dependencies where reached, not permission to reopen
an unrelated repository-wide audit.

## Owner requirements

- Support native Console only. First retire the independent project-owned
  presentation-window route and input synthesis (D23), after identifying
  exclusive code, consumers and build selection. Remove exclusive code and
  stale wiring/tests; preserve shared Console functionality, original guest
  rendering and unrelated app error dialogs. Do not delete historical OpenNT
  mirror bodies merely because inactive.
- Preserve host-arrow absolute-coordinate mouse interaction, correct clicks,
  releases and drags. Do not introduce forced hiding, confinement or recentering
  merely to imitate old Console facilities.
- Preserve embedded resources, selected ROM/media and package-relative
  configuration loading. D25 reduces diff and restores original source shape
  where composable; it must not remove embedding or reinstate Windows system
  directory assumptions. No host boot-file mutation.
- Verify conhost and Windows Terminal separately. No private Console Server
  reconstruction, injection, host modification or fake-success capability
  stubs. A similarly named public API is not proof of equivalent behavior.

## Included audit points

| Point | Restoration and user-visible acceptance |
| --- | --- |
| D09 | Separate host mouse-pointer visibility/count from text-cursor state. Audit startup, focus and exit callers. Use a proven binding or explicit limitation with safe caller handling; no text-cursor corruption, stranded capture or infinite visibility loops. Preserve current mouse UX. |
| D23 | Remove the exclusive independent-window presentation/input implementation, not replace it with another GUI backend. COMMAND/EDIT remain native Console workloads. |
| D24 | Recover character-to-key policy against original owners, including partial DoStringPaste overlap, without conflating physical keys, character packets and paste. Verify characters, modifiers, repeats and down/up pairing. |
| D25 | Preserve embedded and package-relative input mechanics; recover original loading/control/error structure. ROM/configuration identity and loading must survive a different working directory; missing/corrupt input must not silently select unrelated host files. |
| D28 | Restore prepend-on-return ordering rather than WriteConsoleInputW append semantics. Returned events must precede subsequent arrivals in the actual consumed queue. |
| D29 | Restore NOWAIT/NOREMOVE atomicity and reserved-key filtering, including Alt+Enter pairing and Ctrl exclusion. No unintended blocking, consumption, event loss or phantom modifiers. |
| D30 | Resolve Console keyboard-layout ownership rather than assuming the calling thread's layout matches it. Verify supported layout changes and layout-sensitive translation. |
| D34 | Recover useful original per-buffer palette/state policy through demonstrated Console presentation capabilities. No implicit independent-window fallback; test supported palette/buffer changes and explicitly record unsupported graphical behavior. Text EDIT success is not graphics acceptance. |

## RDP and character-input distinction

Current `nt_event.c` DIV-211 explicitly describes Console/RDP records without
a PC scan code and implements `nt_rdp_*` normalization, virtual-key mapping
and character-derived transitions. D24 therefore touches this RDP-motivated
integration, but is not an RDP protocol implementation and does not establish
that all remote input is character input. Original DoStringPaste overlaps only
part of the logic; do not replace the whole keyboard adapter with a paste path.

Separate tests for scan-bearing events, virtual-key-only events, character or
VK_PACKET input, modifiers/releases, and unrepresentable/surrogate characters.
Actual RDP evidence is required for remote-session acceptance; synthetic records
prove only the normalization contract. Keep original downstream SoftPC keyboard
and guest-controller ownership intact.

## Ownership and proposed stages

For each row record: current selected implementation; original OpenNT owner;
evidenced divergence reason; exact restoration/removal and outgoing bindings;
positive, negative and runtime acceptance. MVDM algorithms remain in
`mvdm-host`, admitted non-MVDM originals in `opennt-host`, and only finite host
mechanics in adapters. Do not import Console Server wholesale. Two unsynchronized
input queues do not restore the original ordering contract.

Proposed stages, not admitted S packets:

1. Freeze native Console regression evidence and remove exclusive D23 code,
   preserving shared resources, native rendering and keyboard/mouse behavior.
2. Restore D28/D29/D24/D30 as a coherent input boundary; resolve D09 without
   changing the accepted absolute-pointer interaction model.
3. Recover D25 resource structure and D34 bounded Console presentation as
   separate owner slices, with no loss of embedded ROM/configuration loading.
4. Reconcile all eight audit rows, residual diffs and the selected build graph;
   verify integrated COMMAND/EDIT and applicable display/resource workloads.

## Admission and exit evidence

### U05 and U08-C explicit closure

U05 is owned here in full for the selected native Console input contract:
inventory normalization consumers, then retain the scan/VK/character/modifier
and surrogate tests above with actual down/up ordering, repeats and layout
changes. Synthetic records do not establish RDP acceptance.

U08-C owns selected Console capability gaps, including read/peek/nonblocking
behavior, layout, pointer visibility and buffer/palette operations. For each,
name the original caller, available host binding or exact unavailable server
mechanic, and test the caller's failure behavior without fake success. Record
conhost and Terminal differences explicitly. Shared worker-native wait/alert
mechanics belong to the CCPU40 event proposal; Console queue semantics remain
here. Missing mandatory capability needs owner disposition, not omission.

Record source/toolchain/runtime identities and the sole x86 CCPU40 profile.
Verify the independent-window route is neither selected nor publicly reachable,
and removed exclusive code has no remaining consumers. Test event ordering
under concurrent arrivals, peek/nonblocking reads, shortcuts, layouts, mouse
movement/click/drag and focus/exit restoration. Test package-relative and
embedded input identity, configuration overrides, different working directories,
and missing/corrupt-resource failures without mutating real host system files.

Record conhost, Terminal and RDP evidence separately; unavailable environments
are limitations, not passes. Builds stay in `build/`; admitted current-product
deployment uses `O:\ntvdm64\ntvdm32.exe`, with observations under
`O:\ntvdm64\logs\`. Run documentation governance and diff checks. Each row
requires verified restoration/removal or an explicit owner-approved limitation
before closure. This proposal does not admit implementation now, change the
product entry, or authorize broker or internal WOW/DPMI repairs.
