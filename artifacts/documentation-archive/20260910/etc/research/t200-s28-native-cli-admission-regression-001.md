# T200 S28: native CLI admission regression

The S27 native CLI was rebuilt and executed by
`tools/probe/Invoke-T200S27NativeCliProbe.ps1` at
`artifacts/build/t200-s28-native-cli-r2/`.  Its normal admitted run still
returns `terminal=5 detail=1` without a child engine, Bochs shell, or
environment transport.

The same built executable then established these bounded CLI cases:

| Invocation | Result |
| --- | --- |
| declared target, `--validate-only` | request accepted without engine entry |
| `--include-drives c --exclude-drives c --validate-only` | `include=00000004`, `exclude=00000004`; snapshot ownership applies exclusion precedence |
| `COMMAND.COM` instead of declared target | exit `3`, rejected before engine entry |
| `--include-drives ""` | exit `2`, usage rejection |

The earlier attempt to execute the fully excluded-drive profile reached an
unbounded guest path and was stopped by the outer 120-second harness.  It is
not treated as CLI success or repaired here: it is retained integration risk
for the owning guest/machine package.  The validation route proves CLI request
semantics without re-running that unsafe guest combination.
