# T420 S21 guest Console witness

## Question

Can the CCPU capability runner distinguish an actual guest success from a
launcher exit that merely appears successful?

## Inputs and procedure

The admitted S21 generator creates DOS fixtures only under
`build/M0-T420/S21/`.  Each successful fixture writes its own
`CCPU-<case>-OK` marker through DOS `INT 21h/AH=09` before `INT 21h/4C00`.
`VerifyCcpuIpProfile.ps1` reads both the observer record and its captured
`.console.txt`; it requires exited/zero **and** every fixture's declared text.
The COMMAND regression verifier likewise requires the real `MEM` and `VER`
screen strings, rather than trusting the outer process code.

Formal x86 product build:

```text
ninja -C build/M0-T420/S21/formal-x86-001 -j4 product-programs
```

Observed package logs below `O:\winnt\logs`:

```text
t420-s21-final-fpu-text-r1: FPU64, FPUI32, FPUCW, FPUSW, TIMBUSY, TRAP, FAULT
t420-s21-final-fpu-text-r1: HLTWAIT expected CPL3 privilege boundary
t420-s21-command-text-r1: interactive MEM, nested MEM, direct MEM, COMMAND /c VER
```

## Observations

All five positive CCPU fixtures exited zero and emitted their guest markers.
`TIMBUSY` additionally installs a guest INT 1Ch handler and waits for three
real clock callbacks; its marker proves timer/PIC delivery from guest code.
The four COMMAND/MEM routes emitted their required real `MEM`/`VER` text.

`HLTWAIT` prints `CCPU-HLTWAIT-BEFORE` and then times out.  A temporary,
uncommitted diagnostic observed original CCPU `GET_CPL()==3` at `HLT`; the
original `c_main.c` therefore raises the HLT access GP before its wait loop.
The fixture is retained as a negative control: it must not emit `AFTER` or a
success marker.  Direct HLT wakeup requires the retired kernel V86-monitor
virtualization contract and is not a selected standalone CPU40 feature.

## Worker-thread lifecycle witness

The guest Console gate applies to DOS execution.  CCPU's per-thread TLS
allocation and release are host-side mechanics, so they have a separate,
explicit host witness rather than pretending to be guest text.  The formal
generator now includes `ccpu-thread-lifecycle-test.exe`.  Its test-only entry
uses the selected original `host_CreateThread`, enters original CCPU through a
two-byte direct-unsimulate BOP, returns through that thread's original CCPU
TLS simulation frame, and exits with original `host_ExitThread`.

The current formal library set was compiled x86 and the compatible direct
fixture link/run at `build/M0-T420/S21/ccpu-thread-lifecycle-direct-r2/`
printed `CCPU thread lifecycle OK` and exited zero.  This distinguishes a
real initialized-and-released CCPU thread frame from a thread that merely
starts and returns without ever entering the simulator.

## Interpretation and follow-up

The text gate prevents false green acceptance when a wrapper exits zero after
guest failure.  It does not claim S21 closure: the remaining CCPU family and
worker-lifetime matrix must still be completed.  The HLT result is an explicit
source-shaped CPL3 boundary, not a reason to alter original CCPU privilege
semantics or introduce a CPU30/V86 monitor.
