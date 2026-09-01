# M0 T337 S2 — Original system-root binding rebaseline

## Question

Does the selected original COMMAND configuration producer need a new guest-root
interceptor, or does an existing source-shaped MVDM system-root binding already
provide its required host-path input?

## Inputs

- `src/mvdm-host/dos/command/cmdconf.c`
- `src/mvdm-host/dos/dem/dem.c`
- `src/mvdm-host/softpc.new/host/src/{nt_msscs.c,nt_pif.c}`
- `src/adapter-mvdm-host-out/softpc/mvdm_softpc_firmware.c`
- `src/app/package_layout.c`
- `tools/build/Stage-OriginalSoftpcRuntime.mjs`

## Procedure and observations

1. Read the original `cmdconf.c` producer rather than inferring a later guest
   lookup. Its retained body expands `COUNTRY=` and `SHELL=` with the original
   `%SystemRoot%` layout and names `\\system32\\country.sys` and
   `\\system32\\command.com`.
2. Confirm the one registered divergence (`MVDM-HOST-DIV-158`) changes only
   acquisition of that system-root string. It preserves original short-path
   normalization, configuration parsing, generated-line ordering and failure
   direction.
3. Confirm the same selected root already feeds original `NTIO.SYS`, NTDOS
   directory initialization, and PIF default configuration lookup through the
   named SoftPC binding.
4. Confirm the package stage carries the matching `system32` cohort. No
   separate guest-root interceptor, virtual drive, or host-drive substitution
   appears in the selected route.

## Interpretation

S1's initial scan omitted `cmdconf.c`, the actual source producer. The product
already had the correct recovery rung: an app-selected immutable system-root
string supplied through a same-shaped adapter to selected original bodies.
The remaining task is packaging cleanup: use one `mvdm/` root for DOS and
Win16 media, place system media under `mvdm/system32/`, and keep external
SoftPC resources in `mvdm/softpc/`. This is a source-shaped deployment
composition, not a guest-drive policy.

## Confidence and limits

High confidence for COMMAND configuration and selected startup media. This
does not claim that direct DEM host-file access is redirected into media, that
guest `C:` is virtualized, or that general guest execution has completed.
