# M0 T423 S20 Event-Driven Frontend Retirement

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Question

Can the project-owned NTCON creator and retirement path stop using its 100 ms
timeout without reusing the request-ready capability or weakening the S17
`retire -> restored` acknowledgement?

## Inputs

- S19 P2 baseline `0683ff05a` (protocol 17).
- The admitted S20 scope in the T423 presentation proposal and active packet.
- The project-owned NTSRV frontend-root service, Base RPC client and NTCON
  session service.  Original OpenNT DOS/WOW scheduling and guest media are
  outside this change.

## Design And Ownership

Protocol 18 adds a root-private, authenticated `FrontendStateChanged` RPC.
NTSRV owns the auto-reset event and duplicates only a wait handle to the
authenticated root.  It is deliberately separate from the request-ready
capability: request arrival does not establish that retirement is now legal.

NTCON places both its creator process and this state event directly in its
wait-set.  A creator signal is remembered and removed from the wait-set, so it
cannot produce a permanent signalled-handle spin.  A failed
`RetireFrontend(ERROR_BUSY)` retries only after a broker state event; it has no
timer or sleep fallback.  The existing S17 restoration acknowledgement remains
a separate event and ordering boundary.

## Mutation Coverage

NTSRV signals the state event after mutations that can alter frontend usage or
retirement eligibility: root attach/clear, worker termination, native backend
registration, native report and inflight completion, worker-channel submit and
take, and successful frontend retirement.  Disconnect and service stop close
the root-owned event with the connection while holding the service lifecycle
boundary; no caller receives a durable local handle from that cleanup path.

## Verification

`frontend-scope-lifetime-test` models a first `ERROR_BUSY` return followed by
the service state event.  It asserts that the retry occurs only after that
event, while preserving the existing restoration acknowledgement contract.
`base-service-reservation-test --native-backend` verifies authenticated root
access to the event, denial for a non-root worker, its initial nonsignalled
state, and signal after native-backend registration.

Both focused fixtures passed from `build/M0-T423/S20/formal`, as did the empty
PID-only monitor RPC fixture.  The protocol header invalidated the formal
graph intentionally; its serial x86 rebuild completed all 590 nodes, with only
the pre-existing VDMREDIR `.def` DESCRIPTION warning.  All seven published
images report PE machine `0x014c`.

The coherent protocol-18 package was then published to `O:/winnt`.  Its key
runtime checks passed with actual observer output under `O:/winnt/Logs2`:

- `empty`, `native-zero`, `native-interactive-return`, `mem`, `nested-mem`,
  `direct-mem`, `command-c`, and `edit` passed their existing output and exit
  assertions.  The first combined runner stopped after a shell exit-code
  propagation detail, so `nested-mem` and the direct/EDIT group were rerun
  with fresh prefixes and passed independently; this is retained procedure
  detail, not a product failure.
- Real NTW32 management close passed: the selected NTW32 and attached CMD
  ended, while an independently registered Console session stayed live,
  accepted `ISOLATED-SESSION-OK`, and returned 23.
- An isolated broker-loss run held observer input, terminated only its exact
  `O:/winnt/ntsrv.exe`, then released the gate.  The direct COMMAND observer
  completed with `0x000006ba` (1722), proving a bounded broker-loss result
  without a timer retry.

Published SHA-256 values are:

| Image | SHA-256 |
| --- | --- |
| `run16.exe` | `83D888C6EACA1CD448C7DFF0227B0540F7C81ABE9A6058FD53D958F9FDEF0142` |
| `ntsrv.exe` | `2F3C9FBBC9028799572FACDF6E9C7FC6A4DC494ACAF2BBE5D0DE1C144045D491` |
| `ntvdm.exe` | `E64C4A5054E989E0F477101B0B985BF2CF713E1897DFE4C2C5203B982B892BF6` |
| `ntcon.exe` | `ECD071A68BE9235B6BF8DD08F5063707F7056EF63B81602FD21C61F9C6700CE4` |
| `ntw32.exe` | `EE41D67E0914BA007780D8A865612307C00503500BC4D827CB1DBE5810446675` |
| `ntmon.exe` | `6D605BA12AC16199A10FABA6AE90865CEEE43F530339997A3203A5995B84AFBA` |
| `VDMREDIR.dll` | `74BF30218988B8E56614628938AE1A59825658ED1FE2C9B4853BE87D41EB6307` |

## Interpretation

The event is a product-owned lifecycle correction, not a change to original
OpenNT execution semantics.  Focused lost-wake and root-authorisation tests,
normal/nested DOS/native output regression and independent NTW32 lifecycle
coverage support the claim that the timer was removed without weakening the
existing completion/restore boundaries.

## Follow-up

S21 audits only project-added, semantically identical NTVDM/NTW32 lifecycle
mechanisms for `worker-base` extraction.  It must not move original OpenNT
execution or DOS/WOW task ownership out of their source owners.
