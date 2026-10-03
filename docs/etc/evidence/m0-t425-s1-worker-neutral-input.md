# T425 S1 worker-neutral input

## Admission and provenance

Baseline is T424 S13 ff50ff588, production S12 46abdb554; admission f349228f6.
The owner authorizes one frontend event contract and worker-local adaptation,
not moving original device algorithms, changing imported libraries or claiming
an RDP repair from simulated input. New results are build/M0-T425/S1/r001;
the dependency-checked, mutable S2/r001 cache is retained. Sealed prior runtime
packages and reports are not overwritten.

| Logic / current production caller | Provenance and disposition | Reason retained locally |
| --- | --- | --- |
| Window callback/FIFO, window_input_queue.c -> native_console_frontend.c | Project-owned copied input; remove host pointer/clip sampling used only by the native branch. | Frontend owns host events, not guest/device position. |
| window_records/window_input -> window_mouse.c | Project-owned normalization; one FRAME_MOUSE record with signed relative pixels, source extent, modifiers and buttons for either worker. Remove native-only absolute conversion and old pointer tag. | No worker kind or hidden Console knowledge needed. |
| collect_dos_console/console_input | Project-owned Console source mapping; already common absolute cells and unchanged. | Current logical viewport and physical source select conversion, not worker type. |
| window_keyboard.c -> common Console record queue | Project-owned Windows layout, dead-key and delivered-key ledger; remove native dispatch parameter and text branch. | Windows character translation remains frontend-owned; original DOS normalization remains worker-owned. |
| common/console/client.c -> both actual worker clients | Existing shared copied-wire decoder; add one bounded FRAME_MOUSE decoder used by both clients. | Common cannot depend on EXE-private code or worker-base. No new duplicate client or transport. |
| NTVDM win32/console_client.c decode_input | Project-owned device seam; use common decoder, copy to the existing local relative record. | Original nt_event.c ingress and MVDM mouse IRQ/position/bounds/reset/painting stay unchanged. |
| NTVDM softpc mouse bridge/guest adapters and original mouse_io.c | Existing device-specific adaptation and original device algorithms, not frontend normalization. No edits. | VirtualX/VirtualY, ICA/IRQ, INT33 state and guest drawing are not native Console semantics. |
| NTVWM presentation.c -> text_frame.c | Project-owned native device interpretation; consume the same validated frame sample, accumulate/clamp locally and emit real Windows mouse records. Remove absolute-position wire path. | Native viewport, pixel-to-cell conversion and display-copy cursor are worker-owned. |
| common protocol identity / NTSRV MIDL consumers | Existing version boundary; APP 0.0.425, protocol/RPC35, I/O23. Regenerate MIDL and replace reached v34 symbols; stale identity fixture v29 also corrected. | Explicit mismatch rejection prevents mixed I/O interpretations. |

Original OpenNT has no independent dual-worker Window frontend. The earlier
project relative converter and native accumulator are reused; only their
copied contract is unified. FRAME_MOUSE fits the existing 16-byte private
INPUT_RECORD payload. Its wire menu/focus fields are explicitly source width/
height; normal Windows event tags keep their own meanings. The decoder checks
extent, modifiers, action, buttons and unused fields before narrowing. The
old 0x8002 tag is rejected. NTVDM's 0x8001 remains a local original-loop ingress,
not a second frontend protocol. No helper, scheduler, target ownership or
new lifecycle policy is introduced; worker-base connection ownership remains.

## Verification checklist

- [x] Remove NTCON input worker-kind decisions, native absolute conversion and host pointer sampling.
- [x] Both production clients use the same decoder and private sample; original DOS ingress retained.
- [x] Atomic ENTER/MOVE, reset/release, failed sink, stale source, full signed deltas and geometry validation.
- [x] Keys, side modifiers, text, dead keys, layout changes, returned keys, retirement and cooked input.
- [x] Native pixel accumulation/clamp, buttons, display-copy cursor and independent device state.
- [x] x86 /MT CCPU40 affected closure and regenerated RPC35 MIDL.
- [x] Seven focused fixtures, real cross-process pipe input return and retained channel lifetime fixture.
- [x] Console17 and Window17 product matrices.
- [x] Retained EDIT/relaunch/isolation/retirement, RPC/version/WOW frontiers and coherent publication.
- [x] Reviewed S1 P1 delivery; S2/S3 ownership/publication work remains open.

Reproducible focused runner:

```powershell
tests/observation/verify-worker-neutral-input.ps1 `
  -BuildRoot build/M0-T424/S2/r001 `
  -Observer build/M0-T424/S2/r001/observer.exe `
  -LogRoot build/M0-T425/S1/r001/focused-r003
```

Use a fresh build log directory. The runner uses a private desktop, checks
real exit status and retains reports; it does not manipulate the user desktop.
Ninja's generator now selects frontend-window-mouse-test.exe alongside the
other production-linked frontend tests. Existing assertion conditions remain;
obsolete absolute-host-position cases are replaced by relative device cases.

## Actual attempts and results

build.cmd, build-products.txt, mouse-build.txt, identity-build.txt and
wow-build.txt retain tool invocations. MSVC x86 /MT and C11 frontend flags match
the retained graph. Restricted compiler subprocess startup stalled; the exact
owned build processes were stopped and the installed toolchain rerun outside
that restriction. Initial compilation caught v34 MIDL consumer names after
RPC35 generation; they were synchronized and the product closure rebuilt.
WOW32 inputs were unchanged and its audited Ninja dependency graph reported
no work. No mirror/library/guest diff was introduced.

The first extra full Console API fixture exposed an old, incorrect test
expectation: console_grid.c wraps an out-of-range cursor X to zero, while the
fixture expected 79. Only that exact expectation was corrected to X=0 with
Y=42 retained; the production resize primitive and remaining cell/threshold
assertions are unchanged. A converted native mouse fixture initially omitted
extent on its post-LEAVE re-entry; required source geometry was supplied.
Both failed attempts remain in r001/focused-r002, not counted as passes.

focused-r003: seven fixtures pass. Native text/device checks=133 failures=0;
presentation=426/0; real input-return=688/0. The latter includes common decoder
malformed extent/modifier/unused-field/ENTER/LEAVE and obsolete-tag negatives.
Keyboard dispatch additionally proves supplementary UTF-16 delivery and stale
source/scalar rejection. NTVDM's unchanged normalize-key boundary still rejects
unrepresentable surrogate characters; no Unicode guest keyboard is invented.
channel-r001.txt passes real pipe EOF, staged publication, logical geometry,
all ten original VGA thresholds, current cursor and private handoff tests.
The link-ownership audit passes with deliberate leakage negatives preserved.
Console17 and Window17 both pass all 17 unchanged product assertions.

run-retained.ps1 passes modern EDIT Ctrl+Q, same-CMD cooked/DOS return,
worker/frontend loss, independent-session survival and receipt checks.
run-service-gates.ps1 passes eleven RPC/lifecycle fixtures and five GUI/text
routing cases. run-versions-wow.ps1 passes five app/protocol/interface mismatch
negatives. The separate real-service identity fixture rejects the old app and
wrong protocol and verifies generated RPC major 35. WOW observation retains
WINMINE's visible window and SOL/WRITE's existing memory-error frontiers;
these are not gameplay or SOL/WRITE functionality acceptance.
run-strict-dir.ps1 reports entered-dos=1, dirty-prompt=0, final=0.
run-private-focused.ps1 additionally passes 32 text-handoff checks and the
full Console channel fixture. All these scripts/results are under the new
build root; prior sealed reports remain intact.

freeze-publish.ps1 validates 256 source inputs, PE x86 identity and all eight
cache/staged hashes. Publication replaces only the eight products at O:/winnt,
with the exact accepted T424 package retained in accepted-t424-recovery.
Guest/configuration files are unchanged. release-source-manifest.json,
candidate-manifest.json, recovery-manifest.json and published-manifest.json
retain full identities. Published SHA256 values:

| Product | SHA256 |
| --- | --- |
| run16.exe | 4C82B425AFF01C9A58ED094D1E8DCD615845368E69A3A2A5B9A38416C734EE67 |
| ntsrv.exe | F0896DD4D5BDA59C2146D0FA165E79714D9A78F5C711B64A3FF64A2C753092ED |
| ntvdm.exe | 9700E4E8DB79BE74DF9B81268753F30346ABF6EC0A7CB2E1594B2A6EF50C7109 |
| ntvwm.exe | DB5E38709D8C692B492B9895B77E70FC3548AF7DB54E86753719A109DB833392 |
| ntcon.exe | C84E184C80E078DA3CA59D40B5A1A077C8D9362EE230261E44BD58220B95BFFF |
| ntmon.exe | 2FD9C5CF4265455E32DBC3C681DA19E49E12BBD645B0AD022F18F93164285B83 |
| WOW32.DLL | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.dll | 910DAFBBA435D19919D9DFC1B519FEA8034D924119A08018DECF07D740622B08 |

postpublication-smoke.ps1 passes twelve immediate native/DOS relaunch pairs,
twelve interactive CMD relaunches, same-outer-CMD DOS/native/cooked return,
GUI startup exit 0 and explicit-wait exit 37. All eight published hashes
remain identical after these tests. Documentation governance, relative links,
diff whitespace and no original-mirror/imported-library delta checks pass;
Z: is absent after the tests. S1 P1 is this evidence's containing delivery
commit, pushed to main. Source review confirms that the remaining owner slots
are not mistaken for input normalization and no second old wire decoder or
absolute native-pointer branch remains. T425 is not closed by this delivery.

## Explicit remaining boundaries

Physical RDP capture/clip behavior is not verified or declared repaired. The
unchanged library checks capture ownership before backend conversion. Native
worker font/extent differences remain excluded. The old frontend DOS/native
owner, pending and publication slots remain honestly for S2/S3, not S1 closure.
No guarantee of host scrollback is added. Existing 30ms capture polling remains
intentionally unchanged. Source/API/fixture evidence is not physical acceptance.
