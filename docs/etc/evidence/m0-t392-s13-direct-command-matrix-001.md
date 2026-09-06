# M0 T392 S13 — direct command matrix

S13 removes every interactive `COMMAND.COM` row from the clean-Console
fixture.  The fixture no longer writes Console keyboard events, waits for a
banner, or claims an interactive `MEM`/`exit` path.  It runs only these owner
specified direct product command lines once per architecture:

1. `ver`
2. `mem`
3. `command/c ver`
4. `command/c`
5. `command/c command /c ver`
6. `cmd/c ver`

Known rows assert their appropriate Windows, DOS, or conventional-memory
text. `command/c` has no invented text assertion; it records only its process
result. Every row owns one Console, inherits its `CONIN$`/`CONOUT$` handles,
has a 30-second bound, and terminates/reaps its launched child on timeout.
No user input, UI automation, guest-memory input, or second Console is used.

## Published-package run

The x86 and x64 fixtures were run in parallel from `O:\ntvdm64` against
`ntvdm32.exe` and `ntvdm64.exe`. Both runners naturally exited; final process
queries found no residual `ntvdm32.exe` or `ntvdm64.exe`.

| Row | x86 | x64 |
| --- | ---: | ---: |
| `ver` | pass (0) | pass (0) |
| `mem` | timeout (12) | timeout (12) |
| `command/c ver` | timeout (12) | nonzero child exit (13) |
| `command/c` | timeout (12) | timeout (12) |
| `command/c command /c ver` | timeout (12) | timeout (12) |
| `cmd/c ver` | pass (0) | pass (0) |

This is an exact automatic observation of the direct launch surface. It does
not claim that the timeout/nonzero rows are product regressions: the remaining
detached-Console boundary from S12 still applies. The fixture nevertheless
proves the requested rows were executed once, reports every result, and leaves
no product process behind.
