# M0 T194 S47: Metadata Provider Observation Admission Audit

S46 bundle engine hash is
`92A1C09EAA96FCC8D2EEFED473FB54240A2F5224551D122C793D197511CE388C`.
The v6 runner, shim, profile and target hashes remain respectively
`706882B0EE453CB5E4A64A81A9E357375766DB4B99BDBAD74932577C1570CB37`,
`57AD850F46782FFF798DBEA39EFE6C7EB69FAD3B192DDF2DA5850E696F3F6A41`,
`F91B27EF63FEEEB64A7DCB18F25FCBF2B626DE05CFC1961F4D96CF21B4262548`, and
`69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`.

The controller remains no-retry, rejects pre-existing evidence roots, uses a
15-second caller-selected watchdog and writes a final record with null hashes
if a log is locked. No `ntdos64` or `bochs` process was present during audit.

The controller still cannot guarantee `taskkill` authorization. S36 proved
the remaining risk: a denied tree kill can leave children after the terminal
record is written. S48 is admitted for exactly one invocation only if any
residual tree is verified by wrapper PID and manually terminated at that exact
PID; no name-based cleanup or second invocation is permitted.
