# T228 S1 P9 - bounded guest continuity bracket result

## Scope

This result uses only the already registered, default-off terminal history and
provenance diagnostics in the existing P6 executable. It is an owner-domain
integration observation, not a trace-selected BOP implementation exercise.

## Inputs and procedure

The immutable executable was
build/M0-T228-S1/far-call-bp-provenance-20260818a/bin/ntdos64-native.exe
(SHA-256 301004c82ae6e3c7412edfb3b2352ab60b63605c56054293555eb011521afb48).
The hash-locked T228 four-image input manifest was
c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a.

The existing T228 integration runner executed paired Direct and Readonly runs
at 10,000, 100,000, 300,000, and 1,000,000 instruction ticks. Every run
enabled only the existing guest-exec ledger, terminal scalar history, terminal
position, and registered provenance output. No source, guest image, input
profile, BOP/provider, CPU behavior, or host capability changed.

## Results

| Budget | Direct/Readonly parity | Terminal location | Lifecycle interpretation |
| ---: | --- | --- | --- |
| 10,000 | byte-identical stdout and stderr | 010D:0546 | Early NTDOS startup; no guest-exec ledger entry. |
| 100,000 | byte-identical stdout and stderr | 0041:53E2 | Entered resident domain, still nonzero code. |
| 300,000 | byte-identical stdout and stderr | 0C41:8097 | COMMAND resident domain after the one recorded 50:36 startup transaction. |
| 1,000,000 | byte-identical stdout and stderr | 2073:E790 | Zero-filled instruction window; one recorded 50:36 transaction remains the only ledger entry. |

The 300,000-tick terminal provenance begins
47 47 EB E9 5F 07 C3 8F 06 AA 91 33 DB 93 92, whereas the 1,000,000-tick
provenance window is fifteen zero bytes. At 1,000,000 ticks the latest
captured CS transition was 074A:068B to 2073:02B4 and its successor window
was also zero-filled. The established later 5A5F terminal is therefore not
the earliest observed loss of executable continuity.

## Interpretation

The first observed departure from an explainable resident COMMAND domain lies
strictly after 300,000 and no later than 1,000,000 ticks in this immutable
composition. The fault is independent of Direct versus Readonly mutation
policy. It predates the later transient 5A5F transfer and cannot be repaired
by a 50:xx BOP provider, a COMMAND file reload shim, or an address-specific
RAM seed.

The complete next owner domain remains guest COMMAND/NTDOS allocation and
control-transfer continuity with bx-core/bx-mantle mechanical execution only
for a demonstrated neutral error. Existing evidence does not yet identify
the first invalid transfer, nor prove whether EndInit's initial copy,
LodCom relocation, or another guest allocation/control path is responsible.

## Follow-up

Do not extend budget sampling into a trace-driven backlog. Define one
selector-blind, default-off lifecycle diagnostic only if source/ABI/failure
mapping shows it can distinguish the first guest allocation/control transfer
without interpreting BOP identities or modifying guest state.