# T95 S6 Intact Native Build Observation 003

## Question

Does `BX-BUILD-033` restore the original I/O archive closure needed by the
previous source-object separation, without introducing an adapter or guest
dependency into the default native container?

## Inputs

- Fresh tuple manifest:
  `artifacts/analysis/t95-s6-intact-native-tuple-manifest-003-20260811-001/intact-native-tuple-manifest.json`.
- Fresh build root:
  `artifacts/build/bochs-2.6-native-intact-msvc-r4`.
- `BX-BUILD-033` source/template membership correction.
- Existing detached observer wrapper and generated observation record.

## Procedure

The focused static check first verified exactly one
`minimal_port_space.o` entry directly after `devices.o` in each matching
I/O archive list, with no prohibited boundary vocabulary.  The usual manifest
generator then froze the changed tuple.  The detached wrapper launched exactly
once:

```text
nmake /f ntdos64-native-container.mak ntdos64-native-container.exe
```

The coordinator monitored the record to terminal state and inspected only the
finished log and output metadata.  The executable was not run.

## Observations

- The static archive-membership check passed, and documentation governance
  verification passed before the observation.
- The r4 wrapper process (`PID 13688`) completed with exit code `0`.
- The original I/O archive command includes both `devices.o` and
  `minimal_port_space.o`; the prior four unresolved default-port symbols do
  not recur.
- The named target exists at
  `artifacts/build/bochs-2.6-native-intact-msvc-r4/ntdos64-native-container.exe`,
  with length `2,464,768` bytes and SHA-256
  `F8200EF546C6760816C28E76F0DF90CD93860844ECFE82F59AC7A21D7A3F9AFD`.
- The terminal link line retains the intact original archive family and
  platform-library shape.  The full build log contains zero occurrences of
  `bx_ntvdm`, `ntdos64_adapter`, `src/cli`, `src/opennt`, or `adapter`.
- The profile, firmware, target, reset path and guest payload were not
  executed.  Compiler warnings remain historical compilation diagnostics and
  are not silently classified as runtime behavior.

## Interpretation and Confidence

The MSVC/x86 intact-native container is now source-built and link-closed for
the frozen r4 tuple.  This establishes only a native build artifact with a
default adapter-free link boundary.  It does not establish process startup,
Bochs configuration acceptance, firmware reset, controlled stop, DOS/WOW16,
adapter dispatch, host capability, CLI composition, or an NTVDM runtime.

Confidence is high for the narrow build conclusion because r3's exact four
missing members correspond to the newly included existing object and r4 exits
zero under a hash-recorded, single-invocation wrapper.

## Follow-up

The next S6 gate is a design/admission review for one bounded native
reset-to-controlled-stop observation using this source-built container and its
declarative profile.  It must define the stop owner, timeout/termination
mechanics, evidence outputs and negative conditions before any execution.
