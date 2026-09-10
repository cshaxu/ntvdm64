# T228 S1 P14 — Environment-Size First-Fault Boundary Result

## Question

Does the P13 environment-size split reach a distinct early non-UD processor
fault, such that a bx-core/bx-mantle mechanical repair would be indicated?

## Fixed Inputs And Diagnostic Boundary

Both runs used the P14-admitted native executable
`build/M0-T228-S1/cs-transition-ring-20260818a/bin/ntdos64-native.exe`
(SHA-256 `f9d66c5498b68f63ba41692822c5759b447ecf63d209e8c150e214f07f0fe9ff`),
its manifest-recorded MSVC x64 `/MT`, CPU5/Pentium-MMX history-diagnostic
configuration, and the hash-locked T228 four-image input manifest
(SHA-256 `c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`).

The existing first-fault observer was explicitly armed with
`--observe-first-fault`.  It remains selector-blind and copies only its
registered typed fault and segment-access records.  No BOP observation,
provenance/guest-byte capture, source filter, guest change, provider change,
or CPU semantic change was used.  Each Direct run had a one-million
instruction budget and the paired startup-source paths from the immutable
input root.

## Child-Only Environment Forms

The process environment was cleared before each child start.  The minimal
form contained exactly `SystemRoot`, `ComSpec`,
`NTDOS64_STARTUP_CONFIG_SOURCE`, and `NTDOS64_STARTUP_AUTOEXEC_SOURCE`; their
value lengths were respectively 10, 27, 85, and 87 UTF-16 characters.  The
controlled-large form was identical plus `OPAQUE_A` through `OPAQUE_D`, each
with a distinct 1,000-character value.  Thus it changes size without adding
an interpreted host capability or a path/content dependency.

The complete commands, output hashes, and the child-environment name/length
ledger are in the disposable
`build/M0-T228-S1/environment-first-fault-comparison-20260818a/observation.json`.

## Results

| Form | Lifecycle ledger | First-fault / segment-access observer | Terminal |
| --- | --- | --- | --- |
| Minimal | Three records: parent start, retried start, child/return phase | Both unavailable: no accepted non-UD fault before completion | exit `0`, ordinary guest completion |
| Controlled-large | One parent-start record | Both unavailable: no accepted non-UD fault in the bounded run | exit `4`, execution-budget terminal |

The stderr streams are byte-identical reset-only Bochs output.  The two
stdout streams differ only in the lifecycle/terminal outcome stated above.

## Interpretation

The environment-size branch is reproduced without an early observed non-UD
fault.  Therefore the previously retained `0C41:2357` first-fault fact is not
the first mechanical divergence of this P13 scenario, and P14 does not admit
a bx-core, bx-mantle, machine, or exception-delivery repair.

This result does **not** prove that a non-UD fault can never occur later.  It
only proves that, before the large-environment one-million-tick budget
terminal, the registered observer does not identify one.  It leaves the
complete original guest COMMAND image/lifecycle domain selected by P13
unchanged: environment reallocation, NTDOS allocation interaction, and
transient placement must be examined as one owner package.

T145 remains controlling: no retained exact listing/map binds a controlled
COMMAND.COM offset to a source label, so neither the typed result nor a
heuristic disassembly may be used to name a COMMAND source instruction.
No BOP leaf work, environment truncation, synthetic guest write, or
address-specific Bochs workaround follows from P14.