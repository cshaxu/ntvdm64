# M0 T442 withdrawn mysmb Console experiment

This packet is retained solely to explain a withdrawn, unpublished S1
experiment. It is not an active plan and introduces no product requirement.

`mysmb64` chooses text startup by inspecting its immediate parent name. Under
the project topology its parent is `ntvwm.exe`, so it deliberately detaches
and later calls the ordinary Windows `AllocConsole`. Under direct CMD it keeps
the inherited Console because its parent-name policy recognizes `cmd.exe`.

This is application behavior, not a missing NTVWM/NTCON lifecycle feature.
The attempted hook/RPC redirection of `AllocConsole` was removed before any
release. The active T442/S1 work is the visible-Console-loss lifecycle
diagnosis in `docs/states/CURRENT.md`.
