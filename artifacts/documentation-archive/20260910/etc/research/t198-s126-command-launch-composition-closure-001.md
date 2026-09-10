# M0 T198 S126: Finite COMMAND Launch Composition Closure

S126 assembles the existing source-derived `54:01` provider as one component:
explicit CLI launch plan, existing `54:05` registration, immutable drive
snapshot, CMDINFO gather, checked response writes and commit state.  It does
not link OpenNT `cmdGetNextCmd` or add a host queue.

The MSVC/x64 adapter-only probe at
`artifacts/build/t198-s126-command-composition-test-r7` compiles, links and
runs the complete closure.  Its focused composition fixture proves:

- missing plan/snapshot remains protected by component predicates;
- `54:05` supplies the required registration;
- one `54:01` gathers the 36-byte CMDINFO and writes the one-slot TARGET
  response with CF clear and normal four-byte resume;
- commit advances delivery exactly once;
- a second `54:01` declines under the immutable one-slot plan.

The explicit plan setter validates by round-tripping the existing CLI wire
format.  Manifest/probe source lists include only its direct adapter/CLI
dependencies (`cmd_get_next`, CMDINFO codec, launch-plan and declaration),
not a host broker or Bochs source.
