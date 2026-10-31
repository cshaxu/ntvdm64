# S11 V86 INT3 and INTO exception routing

NT4 `ntos/ke/x86/trap.asm` routes a V86 one-byte `INT3` through
`Ki386VdmReflectException_A(3)` and a taken `INTO` through the same exception
owner for vector 4. Those are distinct from a decoded `CD 03` or `CD 04`
software interrupt. The selected CCPU `c_main.c` previously called `INTx` or
`INTO`, both of which invoke the host software-interrupt hook when PE is set.

The real selected CCPU fixture installs separate exception and software-hook
owners. Before the candidate, cases 157 and 158 invoked software vectors 3/4
and failed to enter the V86 exception target; case 159 (INTO with OF clear)
passed. The candidate first calls the already installed exception hook for
one-byte INT3 and taken INTO in the unpaged, direct-IF V86 domain implemented
by the existing DPMI fault owner. An unhandled hook, non-V86 execution,
virtualized IF, and paged V86 retain the previous selected CCPU path.

The selected real-CCPU gate covers those three controls, explicit `CD 03` and
`CD 04`, absent-hook fallback, and separate virtualized-IF and paged negative
controls; all 158 selected cases pass. These tests establish instruction
origin, selected frame and fallback behavior under the tested configuration;
they do not establish production guest incidence, debugger behavior, host
exception fallback, virtual-IF or paged fault delivery. `MVDM-HOST-DIV-489`
records the source exception. No guest image was changed.

The same ten-component package passed the Full 16-gate suite, Winmine, SOL,
WRITE, four SOL Options rounds, and Win3.1 standard/enhanced × DOSONLY 0/1.
The 86 monitored guest binaries in both original and isolated trees remained
unchanged. It was published to both destinations with all 20 component
hashes matching the tested manifest. [Evidence](s11-v86-trap-evidence/).
These checks do not prove a real guest hit these instruction cases. The
published baseline is not a complete V86 or NT4 exception implementation.

The mirror ledger retains `c_main.c` as partial, with 99.03% original code
lines matched by the text estimator. This is not behavioral coverage. The
updated C/H inventory check found no missing or stale diff estimates; the
existing scope and reachability limitations remain.
