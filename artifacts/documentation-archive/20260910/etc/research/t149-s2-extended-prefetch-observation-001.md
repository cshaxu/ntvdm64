# T149 S2 extended prefetch observation 001

## Question

Did the immutable T148 single-member CPU derivative reach the late real-mode
prefetch fault when the observer budget was raised from one million to ten
million emulated ticks?

## Inputs

- Executable: `artifacts/build/current/t148-single-member-r1/ntdos64-t148-single-member.exe`
  (the T148 source-built derivative; SHA-256
  `8A5E67F9546E4BCE9825BC6E04E6322F4D03EDCA2BCB1458EB7A2182D03FD760`).
- Fixed profile/ROM observer and one fresh output directory:
  `artifacts/analysis/t149-s2-10m-prefetch-observation-001`.
- The one admitted invocation used the existing observer with `-Benchmark 10`.
  No source, archive, linker input, device, firmware, adapter, BOP/provider,
  or guest change was made.

## Procedure

1. Run the immutable T148 executable once with the existing fixed observer,
   ten-million-tick budget and outer watchdog.
2. Inspect the resulting `bochs.log` for the registered `BX-TRACE-056`
   record, native real-mode vector trace, selector `02h` observation and
   machine-composition result.
3. Count each marker, check whether the observer produced a completion JSON,
   and check for a remaining `bochs` process.

## Observed result

`bochs.log` is 230,226,223 bytes. Its first relevant cycle begins at tick
`1,030,692`:

```
prefetch-limit cs=0bc1 prev-rip=ffff eip=00000000 limit=00010000
prefetch: EIP [00010000] > CS.limit [0000ffff]
real-mode vector vector=0d old-cs=0bc1 old-ip=ffff old-ss=0bc1 old-sp=fff8 target-cs=0b43 target-ip=0088
... selector=02 ... cs=073b ...
machine composition outcome=1
```

The four corresponding marker counts are all exactly `309782`. The final
complete cycle is at ticks `31,079,352` through `31,079,479`, with the same
fields. Thus the first one-million-tick cutoff identified by T149 S1 was an
observer-budget artifact; the late fault is reachable in this exact T148
composition and repeats after the original BOP `02h` path returns.

The outer watchdog terminated the run before the observer emitted
`observation.json`; no `bochs` process remained after the run. Therefore this
is valid *reachability and repetition* evidence, but it is not a clean
completed ten-million-tick observation and makes no continuous-execution
claim.

## Interpretation and disposition

The record corroborates T134/T135: `PREV_RIP=FFFFh` is the recorded Bochs
commit-position field at the existing segment-limit check. It is not an
instruction decode or, by itself, proof of the last successfully executed
instruction. The fixed one-for-one cycle also rules out treating BOP `02h`,
the adapter BOP plane, or a missing host provider as an independently
advancing predecessor.

The next admissible work is static source correlation of the original INT 0Dh
handler, its BOP `02h` continuation and IRET return-frame contract. It must
not add a runtime retry, device, firmware, BOP/provider, adapter or guest
change.
