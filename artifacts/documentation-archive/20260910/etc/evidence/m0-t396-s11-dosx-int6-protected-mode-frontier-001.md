# M0 T396 S11 — DOSX Int6 / protected-mode frontier 001

## Admission

S10 is closed for its own narrow owner: it replaced the reached private WOW
error-display dependency with source-shaped visible public-Win32 routes.  Its
complete-archive comparison establishes a new, non-terminal execution
frontier; it does not prove WOW32 loading.

## Reached frontier

The same Console-owning `system32\\krnl386.exe` run has these contrasting
facts:

| Product | Observation |
| --- | --- |
| S8 current archive | after 4906 ms, `wow:private-hard-error` / `0x78` |
| S10 current archive | no such termination through 30000 ms; original guest reaches `05D8:219A`, `21C0`, `22BF`, `22CD`, `2439`, and `243A` opcode `63h` sites; `WOW32.DLL` is not yet loaded |

`c_main.c` proves those are original CCPU `ARPL Ew,Gw` dispatches.  It calls
`Int6()` whenever `GET_PE()==0` or `GET_VM()==1`; the reached reports show
`pe=0`, `vm=0`.  Original `c_xcptn.c::Int6` then calls
`benign_exception(I6_INT_NR, INTERNAL, -1)`.  The active question is the
source-owned #UD-vector continuation that DOSX uses before protected-mode
entry—not the arithmetic implementation of `ARPL` and not a new CPU policy.

## S11 objective

Trace and recover the exact original `Int6 -> benign_exception -> vector`
continuation for this DOSX bootstrap until it either reaches the original
protected-mode/DPMI handoff and `51h`/`WOW32.DLL`, or establishes the next
source-owned unavailable boundary.

## Scope and exclusions

- `softpc.new/base/ccpu386/{c_main.c,c_xcptn.c}` and their reached continuation
  helpers; selected DPMI/DOSX BOP boundaries; a default-off scalar observation
  only if static source audit cannot identify the target.
- No app launch-declaration change, classification change, guest/media patch,
  direct `GET_PE` write, synthetic #UD handler, BOP result fabrication,
  CPU instruction shortcut, or change to S10 dialog result behavior.

## Exit criteria

1. Map the reached `05D8` #UD/`ARPL` state to its original vector and
   continuation owner, including register/stack and resume semantics.
2. Compile the affected original/adapter units in the selected x86 graph and
   run a fixed-container observation.
3. Either observe source-owned advance to the `51h`/WOW32 load frontier, or
   record the next exact owner and failure contract without converting it to
   a guessed successful continuation.
