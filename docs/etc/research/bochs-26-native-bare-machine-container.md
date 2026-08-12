# Bochs 2.6 Native Bare-Machine Container

## Decision

T95 adopts the imported Bochs 2.6 runtime as an intact internal machine
container. It does not pursue the earlier micro-M0 idea of supplying
project-owned substitutes for Bochs `SIM`, memory, device, or exception
frameworks.

The micro-M0 closure record remains valuable negative evidence: its
triple-fault path demonstrated that peeling Bochs away from its own runtime
would incrementally recreate a virtual-machine framework. That is outside
this project.

## Native Initialization Boundary

The first container invokes the existing Bochs ownership chain unchanged:

```text
bx_init_siminterface()
  -> bx_init_main(argc, argv)
  -> bx_begin_simulation(argc, argv)
```

`SIM`, parameter storage, exception behavior, PC-system reset, and the
unmapped-port/default-device stubs remain inside that Bochs boundary. The
project supplies only command-line/configuration input and a narrow
adapter-facing lifecycle wrapper around native entry and termination points.

## Bare-Machine Profile

| Area | Initial disposition |
| --- | --- |
| GUI | Original `nogui` implementation only; no project GUI replacement. |
| CPU and exceptions | Original Bochs logic unchanged. |
| Memory and reset | Original Bochs lifecycle. A bounded test ROM is test input, not firmware implementation. |
| Devices | No device is admitted merely for this profile. Native default stubs/router remain internal Bochs support. |
| Plugins | No optional plugin is admitted for the first profile. Native loader/configuration support remains only if initialization requires it. |
| OpenNT | Absent during container bring-up; later it crosses only the typed adapter. |

## Assembly-Boundary Research Conclusion

Two possible boundaries were compared against the imported Bochs 2.6 source.

| Candidate | Source result | Decision |
| --- | --- | --- |
| Adapter constructs CPU, memory, ROM, timers and a selected hardware set | `main.cc` native startup owns the ordering and implicit state: `SIM` initialization/configuration, plugin startup, `bx_pc_system.initialize`, `BX_MEM::init_memory`, ROM/RAM loading, CPU initialization/state registration, `DEV_init_devices`, hardware reset, and timer start. Recreating that order outside Bochs would recreate its machine framework. | Reject. |
| Bochs constructs its native machine from externally supplied configuration | The native chain accepts command-line/configuration input before it creates the hardware objects. `nogui` is selected by the original display-loader path. | Adopt. |

The actual reusable boundary is therefore hybrid but unambiguous:

```text
adapter: materialize a versioned declarative bare-machine profile
  -> Bochs: parse the original profile and assemble/reset its own machine
  -> adapter: observe only approved lifecycle/mechanical events
  -> OpenNT: receive only typed, separately admitted bridge records
```

The adapter may own profile **data** (selected CPU level, memory size, `nogui`,
ROM test input, and explicitly disabled features), but it does not instantiate
or wire Bochs objects. This preserves Bochs as a reusable contained machine
while still giving the adapter a precise, portable configuration boundary.

## UD/BOP Interception Boundary

Bochs 2.6 already has optional instrumentation callbacks for decoded opcode
bytes and exceptions. They are observation-only (`void` callbacks): they can
record an opcode or `#UD`, but cannot suppress the subsequent native
`BX_CPU_C::exception` path. They therefore support trace evidence but not a
resumable BOP handoff.

The minimal viable mechanism is a single generic native-exception interception
extension, disabled by default. It is deliberately **not** a BOP feature in
Bochs:

```text
Bochs sees an ordinary #UD
  -> neutral exception-intercept callback receives vector + fault location
  -> bx-ntvdm-adapter reads the profile-declared marker bytes and decides
       pass-through | handled
  -> pass-through: original Bochs #UD path remains unchanged
  -> handled: adapter performs the separately admitted OpenNT bridge and
       returns an explicit resume state
```

The Bochs-side patch may know only that an external handler accepted an
exception. It may not recognize BOP bytes, service numbers, DOS/WOW/DEM
semantics, or OpenNT layouts. Conversely, the adapter may use versioned
register/memory records and an explicit resume disposition, but it must not
replicate Bochs exception delivery, decode, CPU state transitions, or memory
rules. An unrecognized marker always falls through to original #UD delivery.

This is a new, narrowly scoped extension rather than a core-to-peripheral
link-severance patch. It requires explicit approval and a patch/evidence/
negative-test record before implementation.

## Guardrails

- Do not replace `SIM`, the decoder, exception paths, memory manager, or
  device framework with project code.
- Keep Bochs source changes near zero. A change is admissible only to sever a
  demonstrated core-to-excluded-peripheral hard link, never to host NTVDM
  policy; record it as a separate patch with a removal test.
- Put Bochs/NTVDM integration in `src/bx-ntvdm-adapter`; its typed ABI is the
  actual reusable boundary for `ntvdm64` and future consumers.
- Do not use a successful native Bochs build as proof that OpenNT/NTDOS runs.
- Add a Bochs feature only from a reached OpenNT caller and record its boundary
  effect.
- Stop for user direction if making this container callable needs a semantic
  Bochs modification instead of a thin embed/build wrapper.

## UCRT Build-Closure Evidence (2026-08-10)

The one admitted native configuration is recorded in
`artifacts/build/bochs-2.6-native-nogui-ucrt-r3/`:

- toolchain: MSYS2 UCRT64 GCC/G++ 16.1.0 and GNU Make 4.4.1;
- profile: original Bochs `nogui`, CPU level 3, with the exclusions recorded
  verbatim in `ntdos64-native-container.json`;
- source patches: none.

`configure` completed successfully. A single controlled `make` compiled the
Bochs CPU archive (`cpu/libcpu.a`), CPU database, memory archive, and GUI/SIM
archive, including the original `nogui` implementation. The final program did
not link because GCC 16 rejects this original Bochs 2.6 source expression:

```text
config.cc:3261:55: error: ordered comparison of pointer with integer zero
```

The failing source expression is an old C++ pointer comparison in the original
configuration writer, not a CPU, memory, device, BOP, or OpenNT boundary.
This project intentionally does **not** apply a Bochs compatibility patch for
it: the agreed source-change exception is only a demonstrated hard
core-to-excluded-peripheral link. `configure.log` and `build.log` are the
reproducible evidence. Earlier `r1` (temporary-directory script defect) and
`r2` (interactive execution timeout) directories are retained as diagnostic
records and are not alternative configuration attempts.

The owner-approved `BX-BUILD-002` compatibility correction was subsequently
applied and recorded in the external-intrusion register. With the identical
profile, `artifacts/build/bochs-2.6-native-nogui-ucrt-r4/` completed configure
and `make` successfully. It contains `bochs.exe` (14,991,828 bytes;
SHA-256 `1DBAC02A6671735E168E3AF515DC2A8E2671E3DF3BCE0E770F576AB2CBEBB01E`).
`bochs.exe -help` identified itself as Bochs 2.6 and exited with code zero.

This proves only that the native Bochs container is configuration-closed and
link-built. It does not yet prove reset, bounded execution, BOP handling, the
adapter ABI, or any OpenNT runtime behavior. The r3 stopped build remains
evidence for why `BX-BUILD-002` was necessary.

## Native Reset And Bounded-Stop Fixture (2026-08-10)

`tools/Invoke-Bochs26NativeResetFixture.ps1` materializes a test-only original
Bochs configuration below the r4 build directory. It selects the imported
Bochs BIOS and VGA ROM, the original `nogui` and `textconfig` implementations,
4 MiB RAM, no guest disk, and `-benchmark 1`. It neither loads OpenNT nor
installs the #UD callback.

The fixture completed with these log predicates:

```text
Bochs benchmark mode is ON (~1 millions of ticks)
cpu hardware reset
exception(): 3rd (13) exception with no resolution ... resetting
cpu loop quit, shutting down simulator
```

The expected no-disk BIOS path reaches original triple-fault reset more than
once before the benchmark timer stops the CPU loop. This is evidence that the
native Bochs reset/triple-fault mechanics are retained, not a guest boot
success. Bochs 2.6 `nogui` returns process code 1 after this controlled stop;
the fixture accepts it only when both benchmark-start and CPU-loop-stop log
predicates are present, so a startup/configuration failure cannot pass.

With `-UdResumeFixture` against r5, the same tool generates a temporary BIOS
whose reset vector is `UD2; HLT`, sets the default-off test environment switch,
and requires the log record `ntdos64 #UD test interceptor resumed execution`
in addition to the benchmark predicates. The run completed. This proves only
that the generic exception seam can decline native delivery, set a supplied
resume RIP, and return to Bochs execution. It proves neither BOP recognition
nor any OpenNT bridge behavior.

## Immediate Work

1. Preserve this stopped UCRT build result; do not start another configuration
   or source-patch loop.
2. Obtain explicit direction on whether the narrowly identified compiler
   compatibility repair is permissible under the Bochs preservation rule.
3. Only after a link-built native container exists, exercise native reset and
   deterministic termination before designing the typed OpenNT adapter.
