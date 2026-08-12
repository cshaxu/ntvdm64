# T95 S6 Narrow Startup-Transaction Build Attempt 002

## Question

Does the corrected 18-object startup-transaction companion link once in a new
MSVC/x86 root while preserving the narrow Bochs/adapter boundary?

## Inputs

- Fresh r2 root:
  `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r2`.
- Generator manifest SHA-256:
  `FE532988788694CE86C4C7102620EE67EDF7CB4620AE3F58D260E39C4D3D54E0`.
- Shim SHA-256:
  `C2B8DB745BBB72353BB637066601D1F9DF1C37E50C21F23CA4AF91BE00E16467`.
- The sole named target under BuildTools 2022 x86:
  `nmake.exe /f ntdos64-native-startup-transaction.mak
  ntdos64-native-startup-transaction.exe`.

## Procedure

The r2 manifest/shim hashes, 18-object count, empty generator build command,
two local opt-in macros, and rejected-token absence were verified before one
invocation. The invocation's output and exit code were retained in r2. No
second invocation or executable launch occurred.

## Observations

The one target exited `0`. It rebuilt the two locally selected Bochs objects,
the copied CPU archive, and the 18 declared adapter/CLI objects, then linked:

| Artifact | Size | SHA-256 |
| --- | ---: | --- |
| `ntdos64-native-startup-transaction.exe` | 2,500,608 | `7B261E7F2F9D6CA28E70A6758845C40112DC982A80A248F143B1DCDC7A3A041E` |
| `ntdos64-native-startup-transaction.map` | 2,477,988 | `73A41C1237A3D415AB2CCF589D00BE98F5C5E2F65C89EBAB239C8927E7654165` |
| `cpu/libcpu.a` | 2,047,060 | `DF03B1B8E04CEB0805A2245384B9A97DA81B93E7821FEBEFD1C14B305F01D11D` |

The build log SHA-256 is
`92C9617E19B08591D6080C859B5B90ED057926A3C903AFAA00CFD70F090C38FA`; the
exit record SHA-256 is
`F0CD2BE81098275C345E58CA0CEF5486272045A97FCAAAFEB7D3FD8C9A8E9588`.
The final shim scan found none of the broad interceptor, adapter runtime,
host-session, read/gather/observation/multi-write, OpenNT, or SoftPC tokens.

## Interpretation

The corrected closure is available as a **same-island MSVC/x86 link**. It
proves only that the named copied-root companion can link with its declared
Bochs archives and fixed-width adapter ABI objects. It does not prove reset,
firmware initialization, bounded execution, #UD interception, host-service
write, DOS, filesystem, search, CLI integration, or any runnable NTVDM claim.

## Follow-up

Any execution observation must be separately admitted as one controlled
reset-to-stop fixture with an exact lifecycle/stop predicate. It must not run
the linked executable merely because it now exists.
