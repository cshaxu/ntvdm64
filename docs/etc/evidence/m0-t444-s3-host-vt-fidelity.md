# T444 S2 disposition and S3 host VT fidelity

## S2 closure

Owner approved audit-only closure and S3 execution. No production changes,
environment filtering, private capability marker or query responder were added.
Existing Console palette access is not universal VT capability negotiation.

Read-only evidence: run16/main.c launch_native copies the caller environment;
native_launch.c environment_copy changes product connection entries only;
nthook32-dll/create_process.cpp preserves native creation environments.
Sibling MySMB src/platform/win32/text_console.c chooses vt_output after VT
enablement and buffer allocation, then sends SGR 38;2/48;2. TERM/COLORTERM do
not select its indexed fallback. No sibling source changed. Installed Edit
version was 1.2.1; absent OSC replies are historical S22 evidence, not a fresh
S2 runtime result. The owner declined query-response expansion. No automatic
application adaptation is claimed.

## S3 question and procedure

Baseline: accepted fb7650ce0, release 0.0.444, protocol 45.
Measure Windows Console conversion rather than assume its quantization rule.
Exercise production frame/presentation owners with build-owned probes. Builds
and tests are background-owned, without computer use, subst or guest changes.
Results and limitations follow below.

## Reproducible test and observations

Entrypoint: tests/observation/verify-native-vt-fidelity.ps1, with BuildRoot
build/M0-T444/S3/r003, FrontendBuild build/M0-T444/S1/r115/Frontend, and Observer
build/M0-T444/S1/r050/console-startup-observer.exe. Launch the script as an owned
background PowerShell process; it launches hidden, bounded compiler/test children
and writes their command, PID, start, deadline, elapsed time and exit status.
MSVC /MT /O2 /std:c11 builds an AMD64 verifier and I386/AMD64 producers. The
accepted frontend objects/library are reused only with unchanged source versus
fb7650ce0; dependencies.json records their hashes. sources.json and artifacts.json
record new inputs and outputs. No product image is compiled or replaced.

The verifier creates an ordinary Console buffer on a private test desktop.
Production run16_native_launch_pack/start (also used by the native worker)
launches each producer with explicit output handles. An independent
ReadConsoleOutputW/CSBIEX read is compared with production ntvwm_capture_* and
ntvwm_text_frame_pack. The actual frontend receiver and session project the
frame to another Console buffer; frontend_window_decode_frame and
kvm_window_render_frame render its pixels. The test font deliberately lights
one pixel and leaves the next dark, independently checking foreground and
background RGB against the oracle for every cell. Session unbind acknowledgement
precedes freeing retained video storage. Teardown checks original palette recovery.

Each producer width passes 23 frame cases:

- Four VT sequences (standard/bright, 256 colour, arbitrary RGB, default/reset),
  each sent whole and one byte per write, with default and custom palettes.
- VT-disabled literal-output cases, both write shapes and both palettes.
- Indexed API output exercising all 256 foreground/background pairs with
  default/custom palettes, plus a palette-only change of retained cells.
- Separate file and anonymous-pipe tests preserve the exact RGB escape bytes
  through the production native creation/handle-materialization primitive.

Results: x64 230,987 checks / 0 failures; x86 230,987 / 0. The large count is
per-cell/pixel assertions, not that many independent scenarios. Each verifier
returns zero and its observer confirms normal process exit. Whole/fragmented
sequence sample attributes agree. Standard red-on-blue is 0x14, bright
green-on-magenta 0xda; SGR reset returns 0x07. Disabled VT leaves literal sequence
text instead of applying colour. All 16 palette entries and projected cell
attributes match; Window foreground/background pixels match that same palette.

Selected measured default-palette examples (hex RGB, not a universal mapping):

| Request | API-observed result | Frontend result |
| --- | --- | --- |
| 256 FG 196 / BG 21 | attr 0x9c, FG e74856 / BG 3b78ff | Both routes equal oracle |
| RGB FG 7b2d43 / BG 1791d2 | attr 0x35, FG 881798 / BG 3a96dd | Both routes equal oracle |
| RGB FG 0ac85a / BG be1ea0 | attr 0xda, FG 16c60c / BG b4009e | Both routes equal oracle |

For the custom table, the first RGB example again exposes index pair 5/3 but
its displayed colours become 462e3a/2c2024. This is measured behaviour only;
the product adds no nearest-colour algorithm. The API exposes indexed fallback
information, not the original requested RGB. These observations do not prove
Windows discarded its internal RGB or how a directly displayed host buffer
would render it. They prove what our capture boundary can actually read.

### Runs and review

- r001: both compiler builds succeeded; sandbox denied the observer's private
  desktop before assertions (observer exit 69). Not a product failure or pass.
- r002: initial x86/x64 matrices passed, but review found that test-owned frame
  storage could be freed before the frontend released its pointer. Its results
  are preliminary, superseded by r003, not used as final acceptance.
- r003: explicit acknowledged unbind fixes fixture lifetime; producers now use
  the real native launch primitive, and palette-only/literal-output checks are
  added. Owned runner PID 3884 started 2026-10-12T04:26:23.0432903Z, exited zero.
  Build x64 11,696 ms, x86 9,644 ms; tests x64 4,886 ms, x86 3,520 ms.
  Verifier SHA256 BACBA04494F774B22FC4B6F9E6A18774AA287820231AAF615FB28411FCE7F293;
  producer x86 40B963D3FADCDDCC4B613EBE1B559E5370EE2ABF9A68A02DC8EB0BAFC48EE5A2;
  producer x64 1699EA876EB35ABF848552AA893EF742AD058AB31B680D7BCCAF0567C3151BF4.

Admission governance was rerun successfully using the S2 r001 documentation
runner; those disposable gate logs were refreshed, not any sealed S1 product
artifact. Final S3 governance uses a separate run below.

## Conclusion and coverage boundary

No additional production repair is justified by these cases. The current
indexed-16 contract preserves host-observable cells and custom palette through
both frontend routes. RGB/256 requests are accepted by host VT but their original
colours cannot be reconstructed from the indexed API snapshot. S2 does not
promise that arbitrary applications negotiate this limitation automatically.

This is production-linked component integration with real Windows Console and
both native writer widths, not a new full-package RPC launch/pipe test or a
physical Windows Terminal screenshot. Window evidence verifies production
rendered pixels, not desktop scaling/compositor colour management. Redirected
bytes are verified through the same creation primitive, not a fresh broker
round trip. It is neither an exhaustive set of all RGB values nor a guarantee
of identical host conversion on other Windows versions.

No src/ or assets/ change: reuse S1 r115 real-pipe/fault/handoff/palette tests,
r116 Console17/Window17 and independent WOW/control/service gates, and r119
deployed COMMAND/CMD smoke by unchanged production input/release identity.
No guest/WOW capability is newly claimed. Edit OSC replies, MySMB's choice of
RGB output and unrestricted RGB cells remain excluded by owner direction.
Full ten-component rebuild/republication is unnecessary for this test/evidence
delivery; final checks must verify both existing published sets remain equal
to the accepted S1 manifest. T444 remains open for owner closure decision.

## Final review

r004 exits zero: it verifies the exact 23-case and zero-failure receipts at
both widths, tested source/dependency hashes, no production/release diff from
fb7650ce0, and 30 matching hashes across the ten accepted package, assets and
O:/winnt/system32 images. It reruns the production-linked S1 palette/handoff
fixture on a private desktop: 49 checks, zero failures, normal observer exit.
Documentation governance, relative links and diff checks pass. The ordinary
host conhost.exe version read is 10.0.26100.1; no universal Windows-version
quantization claim follows from these observations.

Review separates newly compiled test artifacts from unchanged released images.
Only test sources and indexed governance/evidence are submitted. No mirror,
guest, runtime environment or live owner session is modified. A final doc-only
gate after recording these results is retained in S3 r005.
