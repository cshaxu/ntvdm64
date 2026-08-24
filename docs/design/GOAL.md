# Project Goal

Deliver an independently buildable, non-invasive command-line NT 4-era DOS environment. OpenNT NT4 remains the normative source for the guest operating environment: NTDOS/DOS utilities, WOW16, and the historically observed NTVDM service contracts. Bochs 2.6 is the selected guest-machine backend: it owns x86 execution, PC memory, firmware and emulated-device mechanics.

The target product separation is: `bochs-core` (adopted machine), `adapter-bochs`
(Bochs-only assembly), `opennt-guest` (DOS/WOW guest images), `opennt-host`
(host capabilities), `opennt-bop` (BOP source mirrors), `opennt-softpc`
(original SoftPC firmware/ROM and machine-contract inputs), `opennt-utils`
(selected original utility packages), `adapter-softpc`
(Bochs-backed SoftPC/CCPU interface recovery), `adapter-win32` (unavailable
Win32 compatibility), `adapter-bop` (selector-blind copied-frame BOP
ingress/completion), and
`app` (CLI and final composition), and `session` (the dependency-free
per-VDM lifecycle, neutral resource and event foundation). `app` owns
composition and creates the session; `session` is not another composition
layer. This separation exists to make maximal
original-source reuse and minimal source intrusion mechanically enforceable.

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
