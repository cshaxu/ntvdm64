# T465 S2 — classic DDE conversation acceptance

## Decision

S2 closes the selected classic `WM_DDE_*` conversation/data cohort without a
runtime-source change.  The selected original owners were already composed;
this delivery extends the real guest/native witness so that its admission
claim matches the complete finite cohort rather than treating `EXECUTE/ACK`
as a proxy for `INITIATE`, `DATA`, `POKE` and `ADVISE`.

`wddeml32.c` remains excluded.  No DDEML instance manager, scheduler,
conversation table, guest edit or synthetic provider is introduced.

## Owner and authority ledger

| Family | Original owner and authority | S2 witness | Failure/lifetime boundary |
| --- | --- | --- | --- |
| INITIATE / atom ACK | `wmdisp32.c::WM32DDEInitiate`, `wmsg16.c::ThunkWMMsg16`, `wdde.c::DDEInitiateList` | Native peer posts `WM_DDE_INITIATE` with two global atoms; Win16 replies with a direct-atom ACK; native peer verifies both atoms and completion. | The per-invocation initiator node is published only for conversion and is retired by the matching original unthunk. |
| EXECUTE / packed ACK | `WM32DDEExecute`, `WM32DDEAck`, `phDDEFirst` | Both native-to-Win16 and Win16-to-native directions validate payload and ACK. | ACK releases only the original pair/alias selected by the endpoints. |
| DATA / POKE / ADVISE | `WM32DDEData`, `WM32DDEPoke`, `WM32DDEAdvise`, `ThunkWMMsg16`, `phDDEFirst` | Win16 sends each CF_TEXT packet to the native peer; the peer validates the original payload and each sender-owned handle retires once. | A rejected test-only `PostMessage` observes `FreeDDElParam` and exactly one original `GlobalFree`, while retaining the source guest handle until its own cleanup. |

The native peer is an isolated test endpoint.  It binds neither a WOW task nor
a DDE provider; it only observes the selected original transport.

## Exact execution evidence

The checked-in command was run from the configured modern PowerShell runner
against the sealed T464 package:

```powershell
pwsh -NoProfile -File tests/observation/verify-win16-message-probe.ps1 `
  -PackageRoot build/t464-s2/package-r5 `
  -Observer build/.ninja/msvc-x64-release/message-probes/console-startup-observer.exe `
  -CompilerDriver build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd `
  -BuildRoot build/t465-s2-classic-r3 `
  -StructuredPayloads -ThreadClientView -ClassicDde
```

It passed with unchanged guest input.  The retained result records:

- two actual Win16 tasks, normal message/reply/nesting/filter retirement;
- native `INITIATE` → original registration → Win16 direct atom ACK →
  original registration retirement (`ddeInitiateAck=1` on both sides);
- both `EXECUTE/ACK` directions, normal alias release and retirement;
- `DATA`, `POKE` and `ADVISE` CF_TEXT packets; and
- the isolated failed-DATA-post rollback.

The related source/body fixture also passed `101` alias/free cases with zero
errors.  Its scope is exact source-shaped alias mechanics only; it is not used
as the guest/native acceptance proof.

## Changed test contract

`win16_message_probe.asm`, `wow_message_peer.c` and
`verify-win16-message-probe.ps1` now carry one additional fixed-width result
field for INITIATE acknowledgement.  The guest treats the two ACK words as
atoms directly, while ordinary data ACK remains packed.  This prevents a
regression where the selected original `DDEInitiateList` discriminator could
be bypassed by a test that only exercises packed ACK.

No product binary, asset, installed component or guest image changed, so a
runtime publication is neither required nor claimed for this documentation/
test-only P.

## S3 handoff

S3 owns only cancellation and loss: peer disappearance, task/module loss,
pending conversation cleanup and no-surviving alias/callback/payload proof.
It must use the same selected owners and real guest/native path.  DDEML,
cross-worker scheduling, new providers and guessed private USER state remain
outside this T.
