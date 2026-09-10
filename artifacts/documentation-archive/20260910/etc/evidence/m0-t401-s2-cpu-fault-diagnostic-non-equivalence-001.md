# M0 T401 S2 — CPU-fault diagnostic non-equivalence

## Scope

This record tests the pre-existing, default-off CPU fault report in the fixed
T398 x86 stage after the normal `WRITE.EXE` run reached the Kernel31 general
protection fault.  It is an observation validity result, not a product
acceptance result and not a repair proposal.

The test rebuilt only the external Console-owning observer from
`tests/observation/console_startup_observer.c`.  It did not build, alter, or
replace `WRITE.EXE`, Kernel31 guest media, WOW32, or `ntvdm32.exe`.  The
observer launched the fixed `O:\\t398\\ntvdm32.exe` with its ordinary declared
target `system32\\WRITE.EXE`, suppressed its own normal diagnostics, and set
only the already-supported absolute `MVDM_CPU_FAULT_REPORT_PATH` sink.

## Result

The result is not equivalent to the clean run:

| configuration | visible stop | WOW32 state | disposition |
| --- | --- | --- | --- |
| clean external state observation | original Kernel31 GPF after WOW load | loaded | valid product-path evidence |
| `MVDM_CPU_FAULT_REPORT_PATH` enabled | `The NTVDM CPU has encountered an illegal instruction. CS:05d8 IP:20d6 OP:63 69 65 73 5c` | not loaded | diagnostic perturbation |

The diagnostic file contains only early `MVDM-CPU-MOV-SR` records followed by
`MVDM-CPU-ARPL cs=05D8 ip=20D6 pe=0 vm=0`; it contains no `MVDM-CPU-FAULT`
general-protection record for the clean Kernel31 failure.  The observer timed
out at 30 seconds with the altered illegal-instruction panel still visible.

This repeats the established rule in
`m0-t397-s4-cursor-resource-frontier-001.md`: enabling a report-path
environment variable can alter this bootstrap before WOW32.  Consequently the
current CPU-fault sink cannot establish the normal WRITE GPF coordinate and
must not drive a repair choice.  The result does not implicate
`LocalHandleDelta`, CCPU fault delivery, or a missing WOW32 function.

## Cleanup

The external observer executable, report files, and its one compiler object
were task-owned disposable artifacts and were removed after reading them.  No
product or guest artifact was removed.
