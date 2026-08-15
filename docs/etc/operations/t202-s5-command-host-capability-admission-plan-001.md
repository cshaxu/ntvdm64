# T202 S5 COMMAND host-capability admission plan

## Purpose

S5 is a complete-package admission audit for the callable OpenNT COMMAND
family (`54:00..10`). It re-evaluates the earlier contained-profile
dispositions against the approved non-invasive CLI definition: ordinary
Win32/NT user-mode APIs may integrate files, paths, devices, console, process
environment and registry where the caller has permission; no host system file,
kernel, installation or registry modification is admitted merely to restore a
missing NTVDM API.

## Required result

For every callable service, record one original owner/module, caller ABI,
host mutation class, profile disposition (`direct`, `readonly`, `overlay`,
`virtual`, `deferred` or source-shaped unavailable), and one implementation
path:

1. original OpenNT provider usable through a narrow ABI adaptation;
2. original provider with ordinary CLI capability replacement;
3. minimum source-derived rehost when the historical composition cannot link;
   or
4. explicit source-proven unavailable/deferred result.

The audit must distinguish a missing modern ordinary API from a historical
CCPU/SAS/VDD/GUI/process-composition dependency. A missing or unusable API is
recorded with its caller and failure form, then the audit proceeds to the next
COMMAND service; it is not a stop condition by itself.

## Boundaries

- Keep all selector/service decoding and provider routing in `src/bx-vdm`.
- Keep Bochs selector-blind; no BOP, COMMAND, DOS or SoftPC name enters
  `bx-core` or `bx-mantle`.
- Do not reuse the old contained unavailable responses as proof that a
  host-integrated provider is implemented.
- Do not implement an individual trace-reached `54:xx` endpoint, run a new
  native trace, or change host state during S5.
- S6, if admitted, will implement the whole provider only after this map and
  package regression design are complete.

## Sources and evidence

Primary sources are `cmddisp.c`, `cmdexit.c`, `cmdmisc.c`, `cmdredir.c`,
`cmdexec.c`, `cmdconf.c`, `cmdkeyb.c` and `cmdenv.c` under
`src/opennt/base/mvdm/dos/command`, plus `nt_bop.c` only as the historical
top-level routing reference. Existing T199 COMMAND maps and the T202 S4
closure are comparison evidence, not a substitute for the new host-capability
map.
