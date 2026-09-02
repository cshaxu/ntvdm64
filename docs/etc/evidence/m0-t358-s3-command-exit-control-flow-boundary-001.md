# M0 T358 S3 — `/C EXIT` control-flow boundary

## Fixed observation

- Formal CPU40/x86 product SHA-256:
  `c92e4cc21f872adc6abc766da94f583fdb751b416d1ed19ce73c457b8328fa65`.
- Fresh capacity-valid stage: `O:\t358`; its format-3 manifest retains the
  sixteen immutable MVDM media assets.
- Workload: the app's fixed `--command EXIT` declaration.
- Observer: `tests/observation/console_startup_observer.c`, built as the
  existing x86, non-debug, console-owning observer.  The observer copies only
  the original host-stack `CS:... OP:` text already formatted before a modal
  error dialog; it does not read guest memory or alter the child.

The initial fixed observation timed out after eight seconds inside the
original modal error path.  One observer-only rerun with the identical formal
product, stage, command and timeout copied the source-owned fault text below.
Its durable BOP record proves that original configuration completed and that
execution continued after `54:0F`; it reached `50:11`, `50:3B`, `50:0F` and
`50:1B`.  It did **not** reach `54:01`, `54:00`, or a controlled COMMAND
completion.

```text
MVDM-BOP-RETURN 54:0F cs=03F4 ip=03CF ax=049F cf=0
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
stop-fault=CS:03f4 IP:2171 OP:63 69 66 69 65
```

## Source attribution

`illegal_op_int` in original
`mvdm-host/softpc.new/base/system/illegalp.c` obtains the fault CS:IP from
the INT 6 frame, reads five bytes through original SAS, formats the shown
diagnostic, and invokes original `host_error`.  The captured host stack was
then inside original `ErrorDialogBox`; the timeout is therefore a consequence
of its original modal error behavior, not a watcher failure.

For a `.COM` image, the loaded instruction pointer `0x2171` corresponds to
file offset `0x2071` (`IP - 0x100`).  The immutable original
`COMMAND.COM` has at that offset the help-text bytes:

```text
H  [drive:]path    Specifies the directory containing COMMAND.COM file.
```

Thus the CPU attempted to execute COMMAND data.  This is a real guest
control-transfer failure after original COMMAND initialization prerequisites,
not an unimplemented `54:0F` handler and not proof that `/C EXIT` was
delivered.

## Disposition

All direct COMMAND/DEM owners on the reached path are already selected in the
formal graph.  The next owner is therefore the selected CPU40 control-flow
cohort: original CCPU instruction/return/interrupt state together with the
SAS/guest image layout it consumes.  It must be recovered and observed before
T358 resumes.  No COMMAND BOP, batch, DOS EXEC, guest-media or app command
input is changed by this result.
