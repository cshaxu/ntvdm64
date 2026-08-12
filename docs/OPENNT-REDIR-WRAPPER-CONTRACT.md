# OpenNT REDIR Wrapper Contract

## Status

This is a research contract for a future independent `ntvdm.exe` wrapper. It
is derived from the checked OpenNT guest `redir.exe` and host `vdmredir`
sources. It is not implemented by the current research executable and does
not make the historical host DLL a dependency.

## Wire Entry

The guest macro expands each redirector request to:

```text
C4 C4 57 <selector>
```

`0x57` is `BOP_REDIR`; the following byte is a contiguous service selector.
The wrapper must recognize the full four-byte form only after normal guest
instruction fetch/translation has succeeded. It must not reserve or broadly
override unrelated `C4` instructions.

## Dispatch Rules

1. Validate that the selector is in the selected provider profile before any
   register or guest-memory change.
2. Use checked guest-memory spans for every far pointer, structure, string,
   and output buffer supplied by the guest. Validate all output spans before
   an operation begins; do not leave partial guest writes after a validation or
   host I/O failure.
3. Treat the BOP handler as a constrained handled-resume event. It may return
   an approved register/flags result or a controlled fault/stop result, but it
   may not directly perform an arbitrary CPU mode transition.
4. Unknown, disabled, or unsupported selectors must report an explicit DOS or
   network failure contract. They must never fall through to a host API or
   silently succeed.
5. Bind all host paths, pipe names, identities, shares, and network handles to
   a wrapper-owned capability provider. A guest pointer is never a host pointer
   and a guest path is never an authority token.

## Service Tiers

OpenNT `rdrsvc.h` and `vrdisp.c` show a contiguous `0x00..0x31` table. The
built guest uses these tiers:

| Tier | Selectors | Required wrapper capability | Initial policy |
| --- | --- | --- | --- |
| Lifecycle | `0x00`, `0x01`, `0x0f` | provider session create/destroy and DOS process reset notification | disabled unless REDIR profile is selected |
| Named pipes | `0x02..0x08`, `0x20`, `0x21`, `0x23`, `0x24` | capability-scoped pipe handles, query, state, transaction, cancellation, and completion queue | unsupported until a dedicated pipe provider exists |
| Mailslots | `0x09..0x0e` | capability-scoped datagram endpoint and buffers | unsupported until a dedicated mailslot provider exists |
| Net API and mappings | `0x10..0x1a`, `0x2b..0x2e` | explicit connection/mapping model, server/service queries, and host identity policy | unsupported by default |
| Identity queries | `0x1b..0x1f`, `0x22` | wrapper-selected computer, user, domain, logon-server, and DC identity provider | may return profile data only; never ambient host credentials by default |
| NetBIOS/DLC | `0x25..0x27`, `0x31` | NCB/DLC provider plus protected-mode awareness | unsupported by default |
| Interrupt acknowledgement | `0x2f`, `0x30` | queued virtual interrupt acknowledgement and EOI sequencing | enabled only with a matching async provider |

Selectors `0x28..0x2a` describe the historical shared window and assign-mode
control. Their guest initialization calls are commented out in the checked
REDIR source, but a complete compatible provider still needs a profile decision
for them rather than accidental success.

## Asynchronous Completion Is a Separate Contract

`int5c.asm` uses virtual network interrupt `0x76`; selector `0x26` chooses a
completion disposition and selector `0x30` acknowledges it. OpenNT host code
maintains an async event queue and injects virtual hardware interrupts. The
guest's `VDM_REDIR_DOS_WINDOW` contains a post-routine address, DLC timer tick,
and status buffers.

Therefore an async-capable provider must supply all of the following as one
atomic model:

- a per-machine completion queue with no cross-session sharing;
- prevalidated, ownership-tracked guest spans for pending requests;
- a safe point for virtual IRQ `0x76` injection;
- register/flags preservation through the guest post routine;
- acknowledged removal only after the guest's `0x2f`/`0x30` completion path;
- cancellation and reset that drain pending operations without later writing
  into recycled guest memory.

Providing a synchronous host call alone is insufficient for these selectors.

## Product Boundary

The generic engine needs only instruction-trap delivery, checked guest memory,
register/flags patch validation, and scheduled virtual IRQ injection. The
wrapper owns the BOP `0x57` selector table, profile policy, host capabilities,
and all REDIR state. `nxvm` and a basic autonomous DOS product need not link or
enable this provider.

## Verification Gates

Before any selector tier is claimed supported, evidence must include:

1. a selector-specific register/flags and guest-memory marshalling test;
2. rejected-pointer, overflow, ROM/device-mapping, and stale-completion tests;
3. explicit unsupported-selector test;
4. per-session isolation test;
5. for async tiers, deterministic enqueue, IRQ injection, acknowledgement,
   cancellation, and reset tests.

The `redir.exe` link result proves none of these runtime gates. It supplies the
guest-side input required to test them later.
