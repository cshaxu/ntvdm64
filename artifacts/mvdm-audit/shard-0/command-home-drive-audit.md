# COMMAND home-drive source comparison

`dos/command/cmd.c` has one original function, `CMDInit`. The selected original
queries `GetSystemDirectory`, takes its first character when the returned
length is in the original accepted range, and writes `cmdHomeDirectory[0]`.
Current source substitutes `NtvdmGetSystemDirectoryA` while retaining the
conditional, drive assignment and return. `cmddata.c` initializes the drive
to `C:` and `cmdmisc.c` later calls `SetCurrentDirectory` with that value.
The existing project firmware helper projects the active session's product
root plus `system32`; therefore, when the host and selected product reside on
different drives, current home-drive selection intentionally differs from
the original host-system drive. That divergence is recorded as DIV-464.

The raw original/current line comparison matches 33 of 34 original lines
(97.06%); this is a text-retention estimate only, including comments and
blank lines. It is not 97.06% functional equivalence: the sole functional
query changes its authority and its effects require validation. A legitimate
behavioral restoration percentage for this one-function file is not presently
measurable. This mirror-different file remains partial.

`tests/session/softpc_media_resource_fixture.c` checks the session-root helper
and small buffer behavior, not full COMMAND startup. The historical
`command_initialization_fixture.c` compares against host GetSystemDirectory;
its source alone is not evidence that the current root-aware product was
tested with different host/product drives. Confirm current build selection
and test return/failure, exact capacity, differing drive roots and real DOS
startup before claiming the whole behavior restored. No guest or runtime
source was changed by this audit.
