# T95 S6 Synthetic HLT Startup Observation 001

The r2 companion was invoked once with a generated internal-only BYOB package.
Its `NTIO.SYS` is exactly one byte `F4` (`HLT`), SHA-256
`2017ff3461395672aa0aa4f64894fd2f95a4b120e2690e8951656d79adc2eed2`.
`NTDOS.SYS` and `COMMAND.COM` are one-byte zero placeholders, both verified
for identity only and never placed into guest RAM.

The retained observation records: plan applied, `CS:IP` reached the loaded
payload (`0070:0000`) and HLT advanced to `EIP=00000001`, bounded benchmark
stop, no watchdog, and native nogui exit `1`. This is direct evidence of the
narrow endpoint, existing checked RAM copy and real-mode-entry delta working
together.

It is not a DOS, NTIO, NTDOS, COMMAND, filesystem, BOP/DEM or host-capability
result. The fixture is internal test data below `artifacts/analysis` and is
not a reusable guest distribution.
