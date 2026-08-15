# Proposal: OpenNT Redirector Network and IPC Capability Profile

## Decision

Recover the `57:00..31` OpenNT Redirector family as one optional CLI
network/IPC capability profile.  It is neither a substitute for DEM local
host-drive services nor a collection of independently implemented BOPs.

The profile may call ordinary user-mode Win32 network, named-pipe and related
APIs under explicit CLI policy and normal permissions.  Non-invasive means
that the product remains unpack-and-run: it does not restore or install
`VDMREDIR.DLL`, drivers, VDDs, services, kernel APIs, or Windows system files.

## Boundary

```text
guest BOP 57:service
  -> bx-vdm Redirector ingress and fixed-width request/result ABI
  -> optional CLI network/IPC capability provider
  -> normal user-mode Win32 network / named-pipe APIs
```

No host `HANDLE`, `OVERLAPPED`, callback pointer, Win32 structure, or host
thread identity may cross into guest RAM.  `bx-vdm` owns copied DOS/Redirector
layouts, opaque capability tokens, DOS failure translation and copied
completion records.  bx-core/bx-mantle remain owner of CPU, memory, PIC,
interrupt and device mechanics.

## Scope

The original `rdrsvc.h` table remains the authoritative 50-service identity
set.  It is recovered by coherent subfamilies:

1. lifecycle and unavailable/default profile behavior (`00,01,0F`);
2. synchronous named pipes (`02..08`);
3. mailslots (`09..0E`);
4. remote Net API, server/use/workstation/message and identity (`10..22`,
   `2B..2E`);
5. asynchronous named-pipe completion (`23,24`);
6. NetBIOS/DLC and interrupt/callback machinery (`25..27,2F..31`);
7. redirector memory-window and mode/scheduler state (`28..2A`).

Each subfamily is admitted only after its original source/layout/failure map
and full-family positive/negative regression exist.  A trace hit may validate
a completed subfamily; it never chooses the next handler.

## Profiles and failure behavior

The default local-host profile has no Redirector network capability.  In that
profile every `57` request follows the current source-derived unavailable
continuation (`RIP + 4`, `CF = 1`, `AX = 1`) rather than raw `#UD` or fake
network success.  A named `--network-profile` selection is required before a
provider can be enabled.

Permission failures, absent network components, disconnected pipes, callback
overflow and unsupported legacy API data must be translated through each
original subfamily's documented failure contract.  They must not fall back to
direct local host-drive behavior.

## Dependencies and queue placement

This package depends on:

- Host-integrated OpenNT BOP capability recovery, including the shared
  drive/handle/provider ABI;
- modern CLI lifecycle governance and audit/cancellation contract;
- DOS standard-handle and redirection capability for compatible handle and
  disconnect semantics; and
- opt-in virtual-device capability profiles before NetBIOS/DLC or guest
  interrupt completion can be admitted.

It is therefore ordered after those packages and before DPMI/WOW closure.  A
network profile does not block direct local DOS drive recovery, DPMI, or WOW;
unadmitted subfamilies remain explicitly unavailable.

## Acceptance evidence

- complete selector/service/source/ABI/failure ledger for all 50 services;
- provider selection and explicit-unavailable family regression;
- copied request/result/completion ABI tests proving no raw host object
  crosses into guest state;
- synchronous pipe success, unavailable, access-denied and disconnect tests;
- where admitted, asynchronous completion and NetBIOS/DLC device/interrupt
  tests owned by bx-core/bx-mantle; and
- one bounded native integration trace only after the corresponding complete
  subfamily has passed its regression suite.
