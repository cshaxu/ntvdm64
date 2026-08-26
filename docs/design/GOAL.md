# Project Goal

> **Current rebootstrap goal.** This section supersedes the transition-era
> component list below. The current source model has twelve components:
> `bochs-core`, `opennt-mvdm-host`, `opennt-platform-abi`,
> `opennt-guest-dos`, `opennt-guest-wow16`, `adapter-bochs`, `adapter-bop`,
> `adapter-softpc`, `adapter-win32`, `adapter-vdm-monitor`, `session`, and
> `app`.

The product is a self-contained, non-invasive `ntvdm.exe` CLI. It can use
public Win32 APIs and ordinary host resources but never needs replaced Windows
files, a rebuilt kernel/private subsystem, or installation-time mutation.
OpenNT recovery is source-first: import the canonical package union, preserve
the original interface shape through the smallest adapter, use a registered
mirror overlay only when necessary, and author new behavior last.

`opennt-mvdm-host` is the one canonical non-guest/non-tool MVDM host mirror;
it replaces separate BOP, host, SoftPC and utility source components.
`opennt-platform-abi` supplies exact external OpenNT declarations. DOS/V86 and
WOW16 are complete but load-only guest mirrors, never host link inputs.
`adapter-vdm-monitor` is a formal same-shaped user-mode monitor boundary from
the start; it records deterministic unavailable outcomes instead of claiming
to recreate NT4 kernel VDM or CSRSS.

## Superseded transition-era goal record

Deliver an independently buildable, non-invasive command-line NT 4-era DOS environment. OpenNT NT4 remains the normative source for the guest operating environment: NTDOS/DOS utilities, WOW16, and the historically observed NTVDM service contracts. Bochs 2.6 is the selected guest-machine backend: it owns x86 execution, PC memory, firmware and emulated-device mechanics.

The stable user experience is a CLI `ntvdm.exe` that directly accepts a DOS
program or command target, including `.com`, `.exe`, `.bat` and admitted `.pif`
launches.  `app` owns argument admission, image selection and session startup;
the original DOS/COMMAND/WOW16 and OpenNT host packages retain execution,
environment, process-return and provider semantics.  A source-layout restart
may not replace this contract with a bespoke launcher or a different runtime
interaction model.

The target product separation is: `bochs-core` (adopted machine), `adapter-bochs`
(Bochs-only assembly), `opennt-abi` (shared original VDM declarations),
`opennt-guest-dos` (complete canonical DOS guest source/artifact mirror),
`opennt-guest-wow16` (complete canonical WOW16 guest source/artifact mirror),
`opennt-host` (host capabilities), `opennt-bop` (BOP source mirrors),
`opennt-softpc` (original SoftPC firmware/ROM and machine-contract inputs),
`opennt-utils` (selected original utility packages), `adapter-softpc`
(Bochs-backed SoftPC/CCPU interface recovery), `adapter-win32` (unavailable
Win32 compatibility), `adapter-bop` (selector-blind copied-frame BOP
ingress/completion), and
`app` (CLI and final composition), and `session` (the dependency-free
per-VDM lifecycle, neutral resource and event foundation). `app` owns
composition and creates the session; `session` is not another composition
layer. This separation exists to make maximal
original-source reuse and minimal source intrusion mechanically enforceable.
The two guest mirrors are never linked into the host process: `app` selects an
original guest binary through a manifest and loads it into the Bochs machine.
All OpenNT mirrors are one canonical path-wise union selected from the pinned
OpenNT and OpenNT-4.5 MVDM baselines; conflicting paths are resolved once at
complete-package scope rather than retained as parallel product editions.

The recovery objective is to restore the complete original NTVDM behavior as
quickly as evidence permits with the smallest possible OpenNT source diff.
Original OpenNT translation units and complete source packages are the default
implementation.  Where the original SoftPC/CCPU or historical Win32/NTDLL
implementation is unavailable, the replacement first preserves the original
function name, parameters, calling convention, returned layout, ordering and
failure behavior; `adapter-softpc` backs that interface with Bochs mechanics,
and `adapter-win32` backs it with modern public Win32 APIs.  A newly shaped
interface or free-standing replacement is a last-resort registered exception,
not a normal recovery technique.

The immediate goal is not to recover the unavailable NT4 x86 V86/CCPU product composition. It is to establish a small, auditable Bochs-to-OpenNT boundary and use it to run the first contained NT4 EN-US DOS profile. No claim about WOW16, broad device compatibility, or a retail-compatible `ntvdm.exe` follows from that first slice.

The modern runtime target is one MSVC x64 `/MT` host process. This constrains
the command-line invocation shell, `adapter-softpc`, `adapter-bochs`, and adopted Bochs
core to one C/C++ ABI and CRT. It does not alter the emulated CPU: the first guest profile remains
CPU5/Pentium-MMX. Historical OpenNT source remains on its evidenced historical
toolchain path and is not made a modern runtime-link input merely to achieve
toolchain uniformity.

The runtime boundary is deliberately hard: Bochs never implements DOS, DEM,
COMMAND, WOW, or host-service BOP semantics; OpenNT never supplies CPU
interpretation, PC firmware, or emulated-device semantics. A separately
bounded machine-composition component may route an individually admitted
historical **machine** BOP to Bochs-owned mechanics, but it is neither a
SoftPC backend nor a general BOP implementation. The cross-boundary contracts
are versioned and documented in `etc/research/bochs-26-backend-adoption.md`.

## Bochs Rewrite Stop Rule

The selected backend is an adoption, not a rewrite project. Narrow,
evidence-backed lifecycle and controlled-execution shims are allowed only to
make the declared M0 contract callable without the historical Bochs product
shell. Work must stop for user direction before changing or replacing the
Bochs CPU decoder, instruction-handler semantics, virtual/physical memory
subsystem, generic device framework, or firmware behavior in order to make
the design work. Accumulating such changes is treated as a Bochs rewrite even
if each change is small in isolation.

Bochs is admitted deny-by-default. A feature may enter only when a declared
first-profile OpenNT caller requires it, its owner and boundary effect are
recorded, and a focused negative test proves that its absence remains the
default. Convenience, feature parity, or upstream availability is not an
admission reason. The objective is to preserve OpenNT's original service
ownership, layouts, order and failure behavior—not to replace it with Bochs
or adapter policy.
