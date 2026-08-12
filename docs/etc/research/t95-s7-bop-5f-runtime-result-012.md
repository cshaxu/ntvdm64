# T95 S7 BOP 5F Runtime Result 012

## Result

The single r13 MSVC/x86 trace binary
`6A3B49B95A734F112075C0FA00FD9045FC0D32DD633C9DFD3754ABDF1B706485`
reaches the admitted `5F` endpoint with source-correlated inputs:

```text
CS=8dc8 DS=073b SI=4454 DI=041d CX=0004 AX=beef
```

The adapter accepts it at `8dc8:45a6`, resumes at `45a9`, preserves its GPR
delta as empty, and sets CF.  This is the `JC isk_int9` x86 branch required by
`spckbd.asm:2108-2110`; it is not a SoftPC table decode or a device action.

## New observations

The next identity is selector `66` at `8dc8:79c5`, followed by repeated
selector `06` at `073b:0740`.  The full inventory resolves them without
guesswork:

| Selector | Inventory owner | Disposition |
| --- | --- | --- |
| `66` | `emm_init` in the SoftPC BIOS table | Listener-only pending EMS caller/owner audit. |
| `06` | `illegal_op_int` in the SoftPC BIOS table | Listener-only diagnostic evidence; it must not become a synthetic success path. |

The 15-second watchdog record has 430,013 observed BOPs and four prior generic
transactions.  The repeated `06` is a new contained stop condition, not proof
that EMS, keyboard, or Bochs is implemented.  In particular no EMS hardware,
page frame, host memory or new Bochs device is authorized by this observation.

## Next proof obligation

Trace selector `66` from its actual guest call site through `emm_init`, then
classify whether the first CLI profile requires EMS, can receive an
OpenNT-source-defined unavailable result, or must retain a native Bochs
mechanism.  `06` remains evidence-only until that classification is complete.
