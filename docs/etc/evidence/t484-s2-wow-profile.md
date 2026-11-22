# T484 S2 P4 independent WOW source profiles

This batch resolves the five global authored-profile failures discovered in
P3. wow_control_brush.c, wow_control_state.c, wow_control_fnid.c,
wow_default_window_bits.c and wow_class_client.c were inheriting the full
SoftPC original-thunk compile profile. They now use the generator's existing
adapterCflags. Their existing include roots, forced declaration headers,
stdcall ABI and control-specific selectors remain unchanged. No new profile,
test exemption, product source or mirror change was added.

## Actual verification

- The exact five changed compilation rules rebuilt successfully (PID10296).
  The default full macro check then passed545 authored/generated commands.
- Final cached full11-component build PID26724 passed,2026-11-22T16:56:46Z
  to16:57:27Z. All native graphs were unchanged/no-op; the earlier x86 pass
  compiled and linked the final five-unit change.
- Raw comparison with P3:8 images identical; NTVDM and Hook32 differ only by4
  timestamp bytes each, WOW32 only by6 timestamp bytes. In particular the
  removed macro inheritance produced no WOW32 code-byte change.
- Publication PID30016 passed to assets/release and O:/winnt/system32:
 11 images/22 hashes, package2588d3a4-f526-43d0-99be-2c47727b9612.
- Post-publication PID28456 passed the actual linked C-VID contract,
 9-graph/434-object A/B boundaries, full545-command macro check and22 hashes.
 Existing runtime Full evidence is reused under the owner's proven
 byte-identity/timestamp-only rule; no fresh Full runtime pass is claimed.
- Documentation, relative-link and diff checks passed in PID18004. The
 containing S2 P4 commit identifies this delivered correction.

[Compact results](../../../artifacts/build-migration/t484/s2-wow-profile/targeted-result.json)
retain the final runs and binary comparisons. The first mechanical edit
asserted on the fifth rule before writing any source; its unnecessary build
was stopped, then the exact five-rule edit was applied and verified above.
All original guests and assets/winnt.zip are unchanged. Non-error mirror
delta is0points/0lines. The P3 full-profile failure is resolved, not waived.

This fixes a prerequisite of the remaining WOW graph migration; it does not
remove that PowerShell generator or close S2. Worker, native-family and WOW
graph migration remain, followed by S3-S5. Three delivered batches' old
product rollback copies were removed through Clear-BuildOutput with reviewed
membership, releasing30,637,542 bytes without touching stable caches.
