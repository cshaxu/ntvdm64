# M0 T370 S3 — formal native-child link and focused proof

## Formal CPU40/x86 link

The formal graph was generated at `build/M0-T370/S3/x86` with the selected
CPU40/x86 MSVC environment.  The following command completed successfully:

```text
ninja -C build/M0-T370/S3/x86 -j 8 original-softpc-process.exe
```

`ninja -t query original-softpc-process.exe` proves the final link includes
both `original-mvdm-command.lib` and `softpc-bindings.lib`; the latter contains
the selected `mvdm_command_native_child.c` binding.  The final link also
retains `basesrv-bindings.lib` and `session.lib`, which own the S2 re-entry
wait and snapshot lifetime respectively.

## Focused lifecycle proof

The generated focused x86 graph rebuilt and ran
`command_native_child_fixture.exe` successfully after the final source edits.
The fixture covers bounded input capture, CR-to-NUL guest mutation,
environment/stream snapshots, active Base VDM wait, worker re-entry
increment/decrement, no synthetic command delivery, and snapshot cleanup.

## Scope result

S3 establishes code and formal-link closure for the selected local native
child cohort.  It does not establish that the fixed guest workload reaches
`54:08`, `54:0A`, or `54:0B`; that single observation remains S4.
