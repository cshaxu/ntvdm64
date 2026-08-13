# T198 S36 chained-step history witness 001

## Objective and boundary

S36 replaces S35's rejected trace-entry observation with the original Bochs
chained-handler per-step transition.  The admitted intrusion is `BX-CORE-071`:
the core copies only version, CPU id, CS, RIP and the existing native
instruction count into a fixed record.  The mantle owns the bounded ring.

The core has no BOP, OpenNT, DOS, service, guest-memory, device, firmware,
debugger or product-lifecycle knowledge.  It neither inspects nor retains the
instruction object.  The test bridge copies the ring at the already admitted
generic `#UD` boundary, before the finite fixture's later lifecycle cleanup
can overwrite the final entries.

## Mechanism

`src/bx-core/cpu/instr.h` adds the default-off record immediately before the
original `BX_INSTR_BEFORE_EXECUTION` in `BX_EXECUTE_INSTRUCTION`.  The original
`BX_NEXT_INSTR` and `linkTrace` paths both enter through this macro.  The
existing CPU-loop first-trace calls in `src/bx-core/cpu/cpu.cc` add the only
entry not reached through that macro.  `src/bx-mantle/bx_ntvdm_instruction_history.*`
provides only copied fixed-width configuration, storage and query.

## Reproducible build and run

The named build recipe uses Visual Studio 2022 with `-arch=x64 -host_arch=x64`
and `/MT` throughout.  It sets only
`BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1`, rebuilds the complete current
CPU5 source closure (95 CPU source objects), builds the mantle ring and exact
fixture, then links the existing x64 finite executable:

```text
cmd /c artifacts\build\t198-s36-chained-step-history-r1\build-and-link.cmd
artifacts\build\t198-s23-native-ntio-run-r1\command-r1.exe
```

The completed build log lists the CPU, FPU and disassembler source objects and
contains no compiler or linker error.  The exact run exits zero.

## Observed result

The existing source-built startup path remained intact: `50:11`, `50:3B`, DTA,
system-symbol and drive transactions all resumed, then the first non-BOP
generic event remained real-mode `0000:0A84`, vector 6, all-`FF` window.  The
bridge-time fixed ring reports:

```text
t198-s23 history-tail count=8 predecessor=0000:0a82 seq=aad7 fault=0000:0a84 seq=aad8
```

The fixture asserts ring capacity/rejection behavior, eight-record wrap,
record version, final `0000:0A84`, and consecutive `AAD7 -> AAD8` counts.
This establishes `0000:0A82` as the immediate executed predecessor of the
first generic `#UD` under this exact CPU5 x64 `/MT` closure.

## Interpretation and follow-up

This is not a missing BOP handler: the event's copied window remains
`FF FF FF FF`, and S30/S31 already prove the physical low-RAM area differs
from the source-loaded NTIO image.  The result narrows the next work to a
source-led provenance audit of the control transfer/mutation that enters the
`0000:0A76..0A84` sequence.  It must compare the reached OpenNT/NTIO flow and
the minimal machine's low-RAM behavior before enabling any device, inventing
a service, or changing the adapter.
