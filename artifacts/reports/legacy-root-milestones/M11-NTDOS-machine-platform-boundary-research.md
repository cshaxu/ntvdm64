# M11 NTDOS Machine And Platform Boundary Research

Status: research only, read-only against `ntvdm64`; no implementation proposal is approved by this report.

Scope: current `core`, current `vm`, and Microsoft NTDOS as a compatibility
sample.  This report deliberately does not choose how an owned DOS runtime is
implemented.  A future VDM may have several runtime profiles, including a
Microsoft NTDOS BYOB profile, another DOS profile, and a direct program
profile.

## Decision Summary

`core/machine` should not retreat.  Its present direction is sound: it owns
mutable, product-neutral guest mechanics and knows neither a host OS nor a
DOS runtime.  It should not absorb PC/AT boot policy, Microsoft BOP numbers,
NTDOS layouts, or Win32 filesystem semantics.

The important missing boundary is above `core/machine`, not inside the DOS
runtime.  It is a reusable **machine environment / bootstrap personality**:
an assembled x86 guest environment with reset convention, memory map,
BIOS/ROM service contract, selected virtual devices, and one or more ways to
enter guest execution.  The current VM PC/AT profile, firmware, and session
composition contain the first concrete instance of this role, but are not yet
an independently reusable contract.

The second missing boundary is a **guest-host transition adapter**.  It turns
a machine-visible transition (BOP, reserved ROM entry, interrupt hook, shared
memory mailbox, or other discovered mechanism) into calls on host capability
providers.  It belongs above the generic machine and below an NTDOS runtime
profile.  It must be protocol-specific: a generic dispatcher mechanism may
be shared, but Microsoft protocol numbers and structures must not leak into
`core`.

Therefore the right shape is not `owned DOS versus NTDOS`, but this:

```text
core/machine                 CPU, RAM, ports, IRQ, DMA, timing, execution
        |
machine environment          PC-like reset/BIOS/device/ROM/bootstrap contract
        |
guest-host transition        interception + ABI-specific marshaling
        |
runtime profile              NTDOS BYOB, another DOS runtime, direct loader
        |
product composition          selects policy, host resources, UI and lifetime
```

`nxvm.exe` uses a whole-machine boot entry through the machine environment.
An NTDOS profile can instead use a direct guest-runtime bootstrap while still
using the same environment.  A third profile can choose a smaller environment
or different entry route.  This is the desired source of variation; DOS
semantics need not be the primary architecture split.

## Evidence From The Current Repository

The current core already offers an explicit `core_machine` instance, CPU/FPU
profiles, RAM, port routing, PIC/PIT/DMA/KBC/video state, bounded execution,
stop/fault states, profile-time object borrowing, and a frozen execution
provider.  `core_machine_execution_provider` only provides reset, refresh,
and elapsed-time advance; it has no bootstrap model, guest transition hook,
or host service contract.

The current VM supplies the missing concrete whole-machine pieces in product
code: CMOS, FDC/HDC/FDD/HDD, PC/AT port and IRQ routing, ROM mapping, reset
vector, BIOS interrupt services, media wiring, execution-loop glue, and a
Win32/Linux display-input run loop.  `vm/profile/default_profile/pc_at_profile.h`
is a useful inventory of the environment facts, but it is a default-VM
descriptor, rather than a reusable environment contract.  The VM session owns
the concrete device instances and firmware integration.

`core/platform` is presently intentionally narrow: host-surface ownership,
display-frame data, sleep, and their shared implementations.  `vm/platform`
adds whole-machine presentation, host input translation, process/thread
lifecycle, and Console/window policy.  Neither layer presently represents a
filesystem, process-launch, named-pipe, CD-ROM, network, locale, or DOS drive
mapping service.  Thus it is inaccurate to call the current two platform
modules the complete NTDOS Host ABI; they are only an early subset of host
capability and full-machine policy.

The supplied NTDOS discovery record supports treating `NTIO.SYS`,
`NTDOS.SYS`, `COMMAND.COM`, HIMEM, DOSX, redirectors, MSCDEXNT, configuration,
and utilities as one coupled guest runtime.  It also identifies the key
machine-facing signals: BIOS/ROM behavior, memory layout, A20 and protected
mode behavior, devices, BOP-like calls, interrupts, and shared structures.
It does not yet establish exact entries, call numbers, layouts, or the minimum
working component set.  Those are hypotheses to test with owner-provided
BYOB material, not facts on which to freeze an interface.

## Where The Boundary Belongs

### 1. `core/machine`: keep it narrow; expose only generic mechanisms

`core/machine` must remain the owner of guest state and deterministic guest
mechanics.  It should expose more only where an upper layer cannot safely
express a machine event without private access.  Candidate mechanisms are:

* an explicit, profile-installed guest transition/intercept provider with
  ordered dispatch, a register/guest-memory view, result, and lifetime/freeze
  rules;
* explicit guest reset/entry-state configuration where the current reset-vector
  assumption is insufficient;
* an explicit ROM/memory mapping and firmware execution contract, if a
  profile cannot express it through existing public providers;
* versioned-at-build-time capability descriptors for CPU mode, A20, memory
  size, device presence, and interrupt/exception behavior.

These are mechanisms, not policies.  A core intercept must not identify a
Microsoft BOP, map `C:` to a host directory, parse CONFIG.NT, create a Win32
process, or choose a PC/AT ROM.  It must also not call `core/platform`:
guest-to-host translation is composition-owned, preserving the current DAG.

The present firmware registry is descriptor-only and cannot yet invoke a
provider.  That is a concrete gap for both conventional ROM/BIOS execution
and NTDOS-specific bootstrap research, but it is separate from DOS.  Its
future form should remain a generic service and ROM contract.

### 2. The missing middle: machine environment, not a larger core

The NTDOS sample makes a full-guest environment plausible because NTIO is not
merely a DOS API client: it initializes against a prepared virtual machine,
virtual BIOS/devices, and host-call conventions.  This does **not** prove it
requires disk boot or every PC/AT peripheral.  It proves that direct runtime
startup needs a specified substitute for the firmware/boot environment.

Define a machine environment as the following contract:

| Concern | Whole-machine VM entry | NTDOS direct-runtime entry |
| --- | --- | --- |
| Reset | architectural reset plus PC BIOS POST | prepared CPU/IVT/BDA/device state, exact values discovered per runtime |
| ROM/BIOS | ROM image and standard boot/interrupt services | compatible ROM entries or an equivalent discovered service surface |
| Devices | PC/AT devices plus media controllers | only the verified subset, but behaviorally compatible where NTIO touches it |
| Bootstrap | boot sector -> firmware -> OS loader | load/arrange NTIO/NTDOS/configuration/target under a verified protocol |
| Host resources | disk images and explicit user interaction | transition adapter calls to constrained host capability providers |

This layer is reusable by both products in concept, but the current source
topology has no home for a shared PC-specific implementation: `core/machine`
explicitly excludes PC/AT handler/ROM policy and `vm/*` cannot be included by
a future VDM.  Do not solve that by letting VDM depend on VM or by putting
PC/AT policy into core.  A later design decision must either introduce a
shared machine-environment package with a public provider contract, or split
the reusable part of the current VM profile/firmware into such a package.  Its
name and directory are secondary; the ownership rule is primary: code reused
by bootable VM and NTDOS composition cannot remain VM-private.

Until NTDOS evidence identifies the required subset, this should be a design
target, not a refactor request.  A direct-bootstrap environment may be much
smaller than the existing full PC/AT profile; the shared contract must permit
both without pretending they have identical boot semantics.

### 3. Guest-host transition adapter: the NTDOS adaptation boundary

This is the NTDOS-specific boundary, placed above machine mechanics.  It owns:

* recognizing the discovered transition source (for example BOP encoding,
  ROM/vector entry, I/O port, or shared mailbox);
* validating guest addresses, copying guest structures, translating register
  conventions, and returning flags/errors/callbacks;
* binding the Microsoft runtime version/profile to a finite ABI table;
* reporting unsupported services deterministically;
* turning host completion or input into a specified guest-visible event.

It does not own raw host APIs or guest DOS semantic state.  The former belongs
to platform capability providers and their product policy; the latter belongs
to the guest runtime.  This division permits a different DOS runtime to use
the same generic interception machinery with a different ABI adapter, or no
adapter at all.

### 4. `core/platform` and `vm/platform`: what each should mean

The host ABI is a vertical path, not a synonym for one directory.  A useful
separation is:

| Owner | Responsibilities | Must not own |
| --- | --- | --- |
| `core/platform` | policy-free, reusable host capability contracts and common host implementations: opaque files/directories, byte streams, clocks, locale data, optional process handles, device/media enumeration, host surface, scheduling | DOS drive letters, guest pointers/registers, BOP numbers, VM UI, containment choices |
| machine-environment / transition composition | maps a guest ABI request to a capability call; validates/copies data; converts results to guest form | direct OS-specific APIs, product-wide access policy |
| `vm/platform` | whole-machine interaction and media policy: Console/window/input, explicit image mounting, run-loop/thread policy, VM-only presentation | implicit host filesystem as DOS drives, Microsoft ABI, generic host services that future VDM also needs |
| future runtime product platform | per-run containment, path/drive namespace, cancellation, launch policy, user consent, presentation choices | machine dispatch mechanics and raw device emulation |

There is a deliberate tension here: an NTDOS runtime benefits from filesystem,
console, process, network, and CD-ROM capabilities, while a bootable VM must
not silently become a host-integrated DOS container.  That tension is evidence
that DOS namespace policy cannot live in `core/platform`, and NTDOS ABI cannot
live in `vm/platform`.  Core can offer capability contracts; each composition
decides which capabilities exist and under what containment rules.

## Consequences For The Six Questions

1. No meaningful retreat of `core/machine` is indicated.  It needs carefully
   selected generic exposure, chiefly a transition/intercept mechanism and a
   callable firmware/ROM provider model if research proves they are needed.
   The major new boundary is not "more core"; it is above core.

2. Yes: a PC-like machine environment is the candidate middle layer.  NTIO
   suggests NTDOS was built for an already-realized virtual PC environment.
   It must support both full BIOS disk boot and a direct prepared bootstrap;
   neither route should be encoded as the other's special case.

3. No conclusion in this report depends on an owned-DOS implementation.
   The proposed interfaces are driven by machine composition and guest-host
   transition needs, so they can be evaluated before any VDM DOS exists.

4. The current platform modules are only part of the eventual Host ABI.
   `core/platform` is the proper home for portable host capability contracts;
   `vm/platform` is the proper home for whole-machine policy.  The NTDOS
   protocol mapper belongs between them and the guest, in runtime composition.

5. The NTDOS adaptation layer is not one flat layer.  It has three useful
   levels: generic intercept mechanism; NTDOS ABI/profile adapter; and the
   selected capability/policy bindings.  This lets BYOB NTDOS, a different
   DOS adaptation, and a non-booting mode share only the level that is real.

6. BYOB is architecturally compatible with this design.  Treat each supplied
   Microsoft runtime as an external profile manifest containing logical
   component identity, role, architecture, version, hash, dependency class,
   discovered ABI profile, and verdict.  Manifests must never record a local
   path or copy a binary.  A release can contain the generic mechanisms and
   report an unavailable profile when the user's matching material is absent.

## Research Gates Before Any Interface Is Frozen

1. Establish one version-specific BYOB component graph and exact startup path:
   input files, load addresses, register/IVT/BDA state, ROM/device assumptions,
   CONFIG.NT/AUTOEXEC.NT handling, and target-program handoff.
2. Trace every guest-host transition by originating component (NTIO, NTDOS,
   HIMEM, DOSX, REDIR, MSCDEXNT, shell), including registers, guest buffers,
   side effects, synchronous/asynchronous behavior, and failure semantics.
3. Separate standard PC/AT observations from SoftPC/NTVDM-private behavior.
   Only the former belongs in a generic machine environment; the latter stays
   inside an NTDOS adapter profile until independently generalized.
4. Classify each requested host capability as reusable raw capability,
   product-policy mapping, or Microsoft-only ABI artifact.  This is the proof
   needed to place it in `core/platform`, VM/runtime platform, or the adapter.
5. Demonstrate a finite minimum profile with a test matrix.  Suggested rungs:
   bootstrap reaches NTIO; NTDOS initializes; COMMAND starts; a simple COM
   program runs; file I/O works; then XMS/DPMI, redirector, CD-ROM, and process
   integration are added separately.
6. Test profile isolation: a BYOB failure must leave ordinary NXVM boot,
   release construction, and a future non-NTDOS profile unaffected.

## Non-Decisions

This report does not assert that current NXVM already satisfies NTDOS,
that a complete PC/AT model is necessary, that Microsoft binaries can be
redistributed, that original NTVDM host DLLs can run on modern Windows, or
that any known BOP ABI can be implemented without reverse-engineering evidence.
It recommends no source changes and no new build target.

## Read-Only Sources Examined

* `D:\\home\\repos.hobby\\ntvdm64\\docs\\architecture\\overview.md`
* `D:\\home\\repos.hobby\\ntvdm64\\docs\\architecture\\module-layout.md`
* `D:\\home\\repos.hobby\\ntvdm64\\docs\\architecture\\contracts.md`
* `D:\\home\\repos.hobby\\ntvdm64\\docs\\architecture\\core-machine-instance-design.md`
* `D:\\home\\repos.hobby\\ntvdm64\\src\\core\\machine\\machine_interface.h`
* `D:\\home\\repos.hobby\\ntvdm64\\src\\core\\machine\\execution_provider.h`
* `D:\\home\\repos.hobby\\ntvdm64\\src\\core\\machine\\firmware_interface.h`
* `D:\\home\\repos.hobby\\ntvdm64\\src\\core\\platform\\host_surface_interface.h`
* `D:\\home\\repos.hobby\\ntvdm64\\src\\vm\\profile\\default_profile\\pc_at_profile.h`
* `D:\\home\\repos.hobby\\ntvdm64\\src\\vm\\composition\\session\\session.h`
* `D:\\home\\repos.hobby\\ntvdm64\\src\\vm\\platform\\platform.h`
* `D:\\home\\repos.hobby\\ntvdm64\\docs\\research\\microsoft-ntvdm\\README.md`
* owner-supplied `NTDOS-Discovery.md` (Draft v2), read as research input only
