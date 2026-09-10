# T95 S6 Narrow Startup-Transaction Build Attempt 006

## Question

Does the frozen 18-object MSVC/x86 companion containing the registered generic
fault `CS:EIP` confirmation link once with a numeric, independently retained
exit record?

## Inputs and procedure

- Fresh root: `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r6`.
- The current copied `cpu/exception.cc` hash was
  `4B7DED81C66C36F7BB840C179D67D4D8ABA4853B7EAE73BCE3919840E3324886`.
- The unchanged generator declared 18 closure objects, exactly
  `BX_NTVDM_ENABLE_EXECUTION_PLAN=1` and
  `BX_NTVDM_ENABLE_STARTUP_TRANSACTION=1`, and no broad-interceptor macro.
- The verified root-local helper invoked exactly
  `nmake.exe /f ntdos64-native-startup-transaction.mak
  ntdos64-native-startup-transaction.exe` once under BuildTools x86.

The helper's delayed-expansion wrapper wrote a numeric exit record after the
named target. No second invocation or executable launch occurred.

## Result

The recorded exit was `0`; the wrapper process exit matched it. The post-build
audit found no `nmake`, `cl`, `link`, or wrapper `cmd` residual.

| Artifact | SHA-256 |
| --- | --- |
| build log | `3B3DC324A96006158B067647ABC57632C9C7DDD4BFCF64A1E2155E198E5CD698` |
| wrapper | `5DB03917D14AB819043819BCE1B9D36E7A724F5F77349ECE699C15597919819C` |
| executable | `8AE9A77A68CA34154CB9992EE8E995FF68896963F9A679396CAEF476D4F4AC44` |
| map | `728778000C2E4426B8104BAE9A337A2A09A00E291D631EDEEA85423D4A221FD4` |
| CPU archive | `0E1934CE06C6134487AD9262D1E1E7B6619C53251601DA0D3E2DAD86F3C65255` |

## Interpretation

This proves only the same-island Bochs/adapter link closure: the default-off
generic #UD/write/resume mechanics and its copied fault-state confirmation can
be linked with the declared narrow objects. It does not prove a reset,
firmware, #UD transaction, post-resume state, DOS runtime, host capability, or
CLI product behavior. Those require a separately admitted bounded observation.
