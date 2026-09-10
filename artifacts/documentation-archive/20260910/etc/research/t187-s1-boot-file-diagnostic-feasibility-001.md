# T187 S1 boot-file diagnostic feasibility 001

Date: 2026-08-12  
Packet: M0 T187 S1  
Disposition: attempted predicate decomposition rejected; no source change retained.

## Question

Can the current COMMAND boot-file route (`54:0C/0D`) be made individually
observable by replacing its short-circuit expression with ordered adapter
predicates, while preserving all existing dispatch and transaction behavior?

## Procedure

An uncommitted, adapter-local experiment kept the same provider, COMMAND-plane
gate, boot-file preparation and host-session queue functions, but recorded the
first false predicate and copied DS:DX.  A focused v5 profile fixture covered
one normal CONFIG request, one normal AUTOEXEC request, an out-of-aperture
prepare rejection, a second request while a transaction was pending, and the
uninstalled case.  No Bochs build or guest observation ran.

## Observation

The normal default-off CONFIG request committed.  The first explicitly
instrumented AUTOEXEC request also committed.  However, later explicitly
instrumented requests reached `PREPARE_DECLINED` rather than the expected
queue-rejection case, including after a fresh adapter installation.  This is
not an admissible diagnostic result: enabling or rearranging the proposed
observer changes the route that it claims only to inspect.

The experiment, its test additions and its prospective Bochs export were
removed before this record.  `git diff` confirms no adapter, Bochs or test
source remains from it.

## Interpretation and follow-up

The existing combined condition is the behavior to preserve.  A future
observation must not repeat `prepare_boot_file`, reorder its conditions, or
replace its transaction path.  The next design candidate is a passive
fixed-width record created only from values already produced by the unchanged
route, with an independently proven ownership/lifetime rule.  It needs a new
admission before any source change, and any renewed Bochs export must first be
registered as a separate exception.  T186's runtime loop remains
unclassified; this experiment neither explains nor changes it.

