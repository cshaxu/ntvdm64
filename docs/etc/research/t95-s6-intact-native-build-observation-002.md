# T95 S6 Intact Native Build Observation 002

## Question

Does the first detached MSVC/x86 intact-native build after `BX-EXEC-032`
reach an adapter-free final link, and if it stops, what is the first exact
closure failure?

## Inputs

- Frozen post-`BX-EXEC-032` manifest:
  `artifacts/analysis/t95-s6-intact-native-tuple-manifest-002-20260811-001/intact-native-tuple-manifest.json`.
- Fresh build root:
  `artifacts/build/bochs-2.6-native-intact-msvc-r3`.
- Generated wrapper and terminal observation record in that root.
- Registered exception `BX-EXEC-032`.

## Procedure

The detached launcher created the fresh hash-verified root and launched its
single recorded command:

```text
nmake /f ntdos64-native-container.mak ntdos64-native-container.exe
```

The coordinator monitored only the generated observation record until its
atomic exit-code file appeared.  No command was restarted, no file in r3 was
edited, and the target was not run.

## Observations

- Wrapper process PID was `23356`; it reached terminal exit code `2`.
- The build compiled the original I/O, CPU, CPU database, memory, GUI,
  disassembly and FPU archive members, then invoked the renamed final link.
- The final link input contains no adapter object, adapter archive, CLI object,
  or OpenNT object.  The earlier `main.cc` missing adapter header does not
  recur, proving only the default-off compile/link isolation predicate.
- The first terminal diagnostics are four unresolved `bx_devices_c` members:
  `register_default_io_read_handler`, `register_default_io_write_handler`,
  `default_read_handler`, and `default_write_handler`.
- The source archive recipe lists `devices.o` but not the previously isolated
  `minimal_port_space.o`; the four definitions were moved there by the
  registered `BX-IO-029`/`BX-IO-030` source-object separation.  This explains
  the complete four-symbol set without inferring a runtime device need.
- `ntdos64-native-container.exe` was not created.

## Interpretation and Confidence

The r3 observation is a complete, non-retriable build result.  It confirms
that default Bochs container compilation/linking is adapter-independent, but
the intact tuple is not link-closed because its original I/O archive manifest
does not follow the already accepted source-object relocation.  Confidence is
high: the archive recipe, exact moved definitions and final linker errors
identify the same four symbols.

`BX-BUILD-033` is registered before the corresponding narrow build-graph
correction.  That correction adds the existing Bochs-internal object to the
same original archive; it neither selects nor enables a device and makes no
runtime or guest-capability claim.

## Follow-up

After static verification of `BX-BUILD-033`, perform at most one new detached
fresh-root observation from a newly frozen tuple.  It must not reuse or retry
r3, execute the output, or treat a successful link as reset or guest-runtime
evidence.
