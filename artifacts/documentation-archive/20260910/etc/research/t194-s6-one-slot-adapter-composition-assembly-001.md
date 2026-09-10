# T194 S6 one-slot adapter composition assembly 001

Date: 2026-08-12  
Packet: M0 T194 S6  
Disposition: source/test/build closure complete; no guest execution.

## Question

Can the existing adapter installation assemble both explicitly declared COMMAND
lifecycle profiles without retaining an implicit two-slot requirement?

## Change

`bx_ntvdm_adapter_runtime_v1_install` now always obtains and parses the
immutable launch-plan environment value. It requires its slot count to equal
the profile's declared target count, validates the plan target kind against
the declared TARGET artifact, and always loads that target. `QUIT.COM` is
loaded and identity-checked only when the profile declares the second,
terminal slot. The same existing boot-namespace provider is initialized for
either declared count, receiving no terminal image for the four-file v6
namespace and the validated QUIT image for the five-file v5 namespace.

No BOP dispatch, guest-memory ABI, guest instruction, Bochs source, device
selection, host capability, process execution, or result transport changed.

## Verification

The owned MinGW CMake target `bx-ntvdm-adapter-runtime-test` was rebuilt in
`artifacts/build/current/t181-local-mingw` and its focused modes were run:

| Mode | Result |
| --- | --- |
| `--t181-v5-install` | Existing two-slot profile installs with a five-file provider and retained QUIT behavior. |
| `--t194-v6-install` | A v6 four-artifact profile with `2,1,c,00` installs with a four-file provider. The same v6 profile with `2,2,c,00` is rejected as a launch-declaration mismatch. |

Both exits were zero. This proves profile/plan/provider assembly only. It does
not prove CPU-loop startup, a guest `54:01`, or a guest `54:11` return.

## Follow-up

The next slice must use the already-built, unchanged engine boundary to
determine whether a v6 assembled profile can reach the declared bounded
startup handoff. Any fresh link, Bochs change, or CPU-loop observation needs
its own admission; it must not be folded into this assembly closure.
