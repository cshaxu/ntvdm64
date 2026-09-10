# T184 S1 v5 observation input and controller inventory 001

Date: 2026-08-12  
Packet: M0 T184 S1  
Disposition: inputs are available; the existing CLI observer is insufficient for S3 until its child-tree watchdog behavior is closed.

## Immutable input set

The completed tuple is available: T183 current `ntdos64-run.exe` and
`ntdos64-bochs-engine.exe`; T183's four-member bundle containing T182 engine
and three ROMs; and retained source-built NTIO, NTDOS, COMMAND, SHARE target
and `QUIT.COM`. A fresh v5 profile/root must name the target as slot zero and
the exact three-byte QUIT component as slot one, with no host capability.

The profile must therefore be generated as evidence input, not borrowed from
the retained v4 observation root. Its one target is `TARGET.EXE` (SHA-256
`69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`),
followed by QUIT (SHA-256
`06A37DFF559DF7325DE8B003F4DF53C188F733E0CA312AAD961C34DAE48D7B83`).

## Controller disposition

`Invoke-CliNativeControlledStopObservation.ps1` does persist input hashes,
stdout, stderr and outcome JSON, but on timeout it calls `process.Kill()` only
on the runner PID. The current runner launches the shim, which launches native
Bochs; that behavior cannot prove a child-tree cleanup. It is not sufficient
for the single-run budget.

S2 must define a current controller that retains the same durable inputs and
streams but kills the known process tree with the existing explicit tree-watchdog
pattern. It must preserve an outcome record for launch failure or timeout and
must not retry. No engine process started in S1.
