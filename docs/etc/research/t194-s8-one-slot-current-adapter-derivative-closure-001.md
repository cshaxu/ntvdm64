# T194 S8 one-slot current-adapter derivative closure 001

Date: 2026-08-12  
Packet: M0 T194 S8  
Disposition: source/build closure complete; no guest execution.

## Inputs and procedure

The T98 current-adapter derivative generator was used with a fresh r1 root.
Its first link compiled adapter/CLI objects only and stopped at one unresolved
symbol: `byob_launch_plan_v2_from_environment`. The current adapter runtime
now calls that function, but the generator's CLI closure still listed only
`byob_launch_declaration_v1.c`.

The generator was corrected to include the adjacent owned
`byob_launch_plan_v2.c` unit. A fresh r2 root was then generated from the
same accepted r5 Bochs base and linked in the documented VS2022 BuildTools x86
environment.

## Observations

- r2 manifest: `bochsReplacementCount=0`, 67 adapter sources, 5 CLI sources.
- The generated makefile has no `main.o` or `cpu\\exception.o` compile rule.
- `ntdos64-t98-current-adapter.exe` and its map were produced successfully.
- The link map contains the formerly unresolved launch-plan import through the
  fresh CLI object.

The r1 failure is retained as a closure diagnostic, not retried in place.
r2 is the only valid source-built derivative for a future v6 observation.

## Boundary result

The only repaired dependency is adapter-runtime to owned CLI launch-plan ABI.
No Bochs source/object/archive, machine configuration, BOP dispatch, guest
payload, or host capability changed. The binary has not been executed.

## Follow-up

Any v6 run must be admitted separately as one bounded process-tree observation
using r2 and a fully identity-validated v6 profile/root/target bundle. Its
result may classify startup, BOP/transaction reachability, an existing stop,
or a watchdog termination; it cannot claim normal COMMAND return without an
observed `54:11`.
