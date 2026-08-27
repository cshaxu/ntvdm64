# M0 T284 S1 — historical-interface adapter convergence inventory

## Objective

Create a complete, source-first per-file and per-public-symbol inventory for
the current `adapter-win32`, `adapter-softpc` and `adapter-vdm-monitor` roots,
the source-proven Redirector/WOW/VDD/debugger interface families, and their
selected `mvdm-host` consumers. It is the sole admission input for the later
`adapter-mvdm-host-out/{win32,softpc,monitor,redir,wow,vdd,debugger}`
migration.

## Scope

S1 is read-only with respect to production interfaces: it inventories source
identity, original caller/declaration, current consumer, interface family,
same-shaped binding, failure disposition, x86/x64 build selection and final
target location. It also records an explicit absent-current-implementation
record for each source-proven added family and which existing files are deleted
instead of moved.

## Non-goals

No adapter root is created or renamed; no OpenNT body moves; no BOP/provider,
machine, monitor, filesystem, child-process or broker behavior is enabled.

## Exit

Every live file and public header of the three roots, plus every reached
source-proven interface of the added four families, has exactly one final
owner/deletion disposition; every selected host consumer is enumerated; and
the inventory proves the intended `adapter-mvdm-host-out` subfamily dependency
directions without changing a production include.
