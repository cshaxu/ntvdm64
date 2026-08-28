# M0 T309 S1 — DEM/COMMAND vertical-slice boundary recovery

## Objective

Recover the narrow, source-shaped boundary that lets the original DEM and
COMMAND dispatcher packages consume the closed Base VDM broker and Bochs-backed
machine forms. This is package recovery, not a trace-selected BOP patch.

## Baseline

Original `demdisp.c` and `cmddisp.c` already compile on x86/x64. The historical
aggregate `nt_bop.c` cannot be linked wholesale because it brings the excluded
SoftPC PIC/second-executor product body. T308 has now fixed the monitor-control
form: `VdmQueryDir` is local and all remaining kernel-VDM services have a
unique deferred owner.

## Admitted work

1. Audit the original `MS_bop_0`/`MS_bop_4` entry sequence and identify the
   smallest source-shaped, selector-blind extraction/binding route that does
   not import the aggregate PIC executor.
2. Reconcile that route with the existing Base VDM facade, SoftPC-shaped
   lease/register forms and copied machine-event ingress. Each direct external
   form must retain one original declaration and one adapter owner.
3. Attempt an original DEM/COMMAND local closure on x86/x64. Preserve
   dispatcher ordering, original handler tables and failure results. Any
   unavailable historical child/product-shell branch receives an explicit
   disposition rather than a new app provider.
4. Add focused local tests only after the original package boundary is fixed.

## Exclusions

No full historical `nt_bop.c` aggregate link, second SoftPC/PIC executor,
kernel VDM/CSRSS reconstruction, new BOP service semantics, guest trace
repair, XMS/DPMI/Redirector/WOW/VDD/debugger recovery, or app-owned command
dispatcher.

## Exit

The selected original dispatcher/ingress route is either formally composed on
x86/x64 or has one exact remaining original boundary with a next-owner plan.
No selector or handler is authored merely to bypass the original package.
