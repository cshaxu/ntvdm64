# M0 T96 S4 COMMAND Boot-File Reconciliation 001

## Finding

The existing `cmd_boot_file_service` is a sound source-derived replacement
for the original paired CONFIG/AUTOEXEC result contract, but its current
runtime call site predates T96 and bypasses common ingress/registry/COMMAND
plane classification.

## Evidence

Pinned OpenNT `cmdconf.c` handles service 12 and 13 as one pair: materialize
CONFIG/AUTOEXEC into host temporary files, copy a bounded OEM path to `DS:DX`,
then allow ordinary DOS open/read continuation. The retained source trace
record `t95-s7-ntio-command-boot-file-bop-trace-001.md` proves the guest
continuations and the contained replacement path values.

`src/bx-ntvdm-adapter/bx_ntvdm_cmd_boot_file_service.c` accepts only real-mode
`C4 C4 54 0C/0D`; it validates the 64-byte real-mode `DS:DX` aperture and
queues an opaque generic multi-write transaction containing respectively
`C:\\CONFIG.SYS\0` or `C:\\AUTOEXEC.BAT\0`. The strings name profile-materialized
guest namespace files. It neither reads guest memory nor opens/creates a host
path or temp file.

`bx_ntvdm_adapter_runtime.c` currently invokes this service directly in the
legacy runtime chain and queues its transaction through the generic session
consumer. That is the sole T96 issue: the call bypasses COMMAND plane routing.

## Disposition

Services 12/13 remain one paired source-derived CONFIG component. Their
namespace replacement is retained; restoring historical temporary files is
forbidden. No additional direct recognizer may be added. S8 must migrate this
single runtime call through common ingress, registry, and COMMAND plane with
the paired component record, then prove both existing write transactions and
negative non-pair behavior. Until that migration, S4 does not claim runtime
COMMAND-plane integration.
