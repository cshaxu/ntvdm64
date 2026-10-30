# Actual WOW32 notification boundary

A diagnostic-only copy of the selected original `wow32.c` records the process
ID and notification reason on entry to `W32DllInitialize`; its body and the
other selected provider objects are unchanged. The diagnostic DLL was linked
and sealed into a complete isolated ten-component package. It was not
published. The two immutable, hash-checked prebuilt Win16 guests were started
through `CREATE_SEPARATE_WOW_VDM` and completed normally in two distinct WOW
workers; the peer continued executing after the first worker exited.

For worker 36824 (`00008FD8`) the entry recorded reasons 1, 2, 3 and 0,
in order; worker 25468 (`0000637C`) recorded the same sequence independently.
These are process attach, thread attach, thread detach and process detach.
The observer verified each worker's process identity and exit code zero.
The test checks that these actual provider entry branches were reached in
these runs, not every notification, return value or internal cleanup effect.
Process detach on worker exit is not explicit `FreeLibrary`: the generated
bridge deliberately limits its project-owned bitmap/registry shutdown calls
to explicit unload. Guest DLL lifecycle remains a separate boundary.

The included inputs, native report, package manifest and notification records
bind the evidence to the diagnostic provider. Its unsealed input SHA-256 is
`963D1A31337F3A5C4B11FAF597782F1E46DF45A466CA19C7FF1C5C1447D29066`.
Writing a marker from a loader callback can affect timing; this is not a
performance or concurrency proof. An earlier attempt did not start a worker
because the copied package lacked a matching sealed identity; it was not a
product test. The complete resealed package passed the witness.
