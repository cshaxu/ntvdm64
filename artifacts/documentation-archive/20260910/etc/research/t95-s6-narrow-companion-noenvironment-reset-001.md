# T95 S6 Narrow Companion No-Environment Reset 001

The linked r2 companion was invoked exactly once with the accepted
PC-reset-floor-v1 profile and with `NTDOS64_ADAPTER_PROFILE` and
`NTDOS64_ADAPTER_ROOT` cleared by the root-local wrapper.

The retained observation JSON records the exact companion hash
`1B7283123409720FB0700BCCC526E9563E54A5C089A70C33F45BD398E113CBED`,
the accepted profile hash, a 3,156 ms elapsed time, no watchdog termination,
and native nogui exit `1`. Its stdout contains original hardware reset,
benchmark-mode activation, the bounded CPU-loop stop at tick `10,000,005` and
a real-mode-halted CPU.

The initial helper reported failure only because it treated the word `adapter`
inside `ntdos64-native-adapter-observation.exe` as a forbidden guest input.
The wrapper and command include no profile/root value, BYOB option or payload
identity; the helper predicate is corrected for later runs and this evidence
root is deliberately not rerun.

Conclusion: the opt-in companion preserves r4's no-environment original
no-plan reset-floor behavior. It does not show a startup-session plan being
prepared, a RAM write, entry-state change, NTIO execution, NTDOS/COMMAND load,
or any host capability.
