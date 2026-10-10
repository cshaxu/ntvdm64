# T442 S1 withdrawn native Console-carrier experiment

## Finding

`mysmb64` owns the observed behavior. Its source treats only `cmd.exe`,
`powershell.exe`, and `pwsh.exe` as text-shell parents. When launched under
`ntvwm.exe`, it selects graphical startup, calls `FreeConsole`, and later uses
ordinary `AllocConsole` when switching into its own text mode.

## Disposition

An unpublished prototype added a service query and hook interception to map
that `AllocConsole` request back to NTVWM's private carrier. This changes one
target application's Console policy and is not a product-owned lifecycle
repair. It was removed without publication. Its compilation and fixture
results are non-delivery evidence only.

## Current scope

T442/S1 now diagnoses outer visible-Console loss: whether NTCON's anchor
detects the outer CMD exit and whether NTSRV then retires the root and asks its
workers to stop.
