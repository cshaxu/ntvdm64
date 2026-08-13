# T197 S7 Physical Closure Reconciliation 001

## Question

Does the source-built CPU5 bare-machine closure physically implement the
declared `bx-core` / `bx-mantle` / `bx-vdm` architecture, with every direct
Bochs exception accounted for and no product shell or BOP semantics hidden in
the closure?

## Inputs

- T197 S1--S6 evidence and commits through `132bbc7`.
- Fresh r12 manifest and runtime witness under
  `artifacts/build/t197-s6-minimal-machine-link-probe-r12`.
- Current physical source roots and
  `adapter-external-intrusion-exceptions.md`.

## Procedure

1. Read r12 JSON and require its fixed CPU5/non-x86-64 whole-core flag and a
   successful link.
2. Check each declared source is physically under `src/bx-core` or
   `src/bx-mantle`; count source and object entries.
3. Scan live source/tests/tools for the retired `bx-ntvdm-adapter` path.
4. Inspect the r12 forbidden-input list and the current exception rows
   BX-MANTLE-062 through -066 and BX-CORE-064.
5. Re-run focused mantle/core boundaries, documentation governance and safe
   diff checks.

## Observations

- r12 records `wholeCpu5Core: true`, `linkSucceeded: true`, 129 declared
  source entries and 130 object entries.  Every declared source has the
  physical owner prefix `src/bx-core/` or `src/bx-mantle/`.
- Its forbidden input list names `main.cc`, `config.cc`,
  `gui/siminterface.cc`, `bochs.exe`, device archives, adapter and OpenNT.
  None is a manifest source.
- The live source/tests/tools scan has no retained
  `bx-ntvdm-adapter` component path.
- Current exceptions 062--066 and 064 all name their post-T197 physical
  paths and are marked implemented.  Their focused source tests establish
  finite SIM, no-device port space, no-product shell, RAM initialization and
  CPU5 profile limits.
- The only dynamic witness is the r12 finite lifecycle:
  `initialize(1 MiB, 1 MiB) -> CPU5 reset/APIC/CPUID -> cleanup()`.  It exits
  zero.  It does not enter `cpu_loop`.

## Interpretation And Confidence

High confidence that the first-stage physical split and native bare-machine
build/lifecycle closure are internally consistent.  `bx-core` owns adopted
CPU/FPU/memory/decode code; `bx-mantle` owns the finite SIM, product-shell
isolation, PC-time, RAM and empty port-space assembly; `bx-vdm` remains the
separate typed VDM/BOP adapter.  No BOP selector/service meaning is present
in the newly admitted core/mantle closure.

This does **not** prove guest execution.  Starting an instruction loop needs
a separately admitted reset-image/entry contract, a finite instruction budget
and a controlled-stop disposition.  It cannot be inferred from successful
CPU reset, and it must not reuse a product BIOS/device configuration as a
shortcut.

## Follow-Up

T197 may close after its final S7 authority/provenance reconciliation.  The
next candidate package begins only with a separate, source-backed native
execution-admission question.  The later holistic BOP package resumes only
after that native machine contract is explicit; it must use `bx-vdm` for all
selector/service/provider meaning.
