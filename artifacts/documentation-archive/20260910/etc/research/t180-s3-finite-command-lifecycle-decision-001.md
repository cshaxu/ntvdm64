# T180 S3 finite COMMAND lifecycle decision 001

Date: 2026-08-12  
Packet: M0 T180 S3  
Disposition: closed decision; admits a separate finite declared-target-sequence package.

## Decision

The current non-invasive CLI composition will not rehost the historical
`cmdGetNextCmd` / `GetNextVDMCommand` protocol, and will not manufacture an
empty successful answer to the second `54:01` request.  It instead needs a
finite, profile-declared sequence of guest targets.  Each ordinary
post-target `54:01` consumes exactly one next immutable declaration; the
last declaration is a source-built `QUIT.COM` whose existing `C4 C4 FE`
controlled-stop contract ends the run before another no-next request is
needed.

This is not a per-BOP workaround.  `54:01` remains the original COMMAND
command-acquisition ABI, while the replacement for the unavailable NT host
broker is one bounded provider with a finite input model.

## Evidence and rejected alternatives

T180 S1 maps `54:01` to `cmdGetNextCmd` in the retained COMMAND source.  Its
normal implementation builds command and VDM state, invokes
`GetNextVDMCommand`, synchronizes directory/environment state, and uses NT
event/console and termination facilities.  That historical host broker is
not an independently composable CLI provider.

T180 S2 proves that the first `54:01` at `95AB:03C1` delivered the declared
target, then a target normal return reached the same `54:01` again.  The
one-target provider correctly declined it; Bochs/SoftPC vector-06 handling
then owned the resulting machine path.  `54:11` was not observed.  Therefore
neither a synthetic CF-clear no-next response nor CLI result transport has a
source or runtime basis.

The alternative selected here has these bounded properties:

- The profile owns an ordered, immutable list of guest file identities and
  command tails; it is not an ambient host command queue.
- The namespace provider makes only those declared guest files available.
- The existing `54:01` transaction ABI remains the sole way a slot is
  delivered; no COMMAND semantics enter Bochs and no DOS semantics enter the
  generic adapter ingress.
- The final, source-built `QUIT.COM` uses the already defined controlled-stop
  BOP rather than pretending to be the historical `54:11`/exit-result path.

## Transfer

M0 T181 is admitted to specify this whole provider before any implementation:
the exact slot state machine, profile/schema ownership, materialized guest
namespace, `54:01` repetition contract, `QUIT.COM` terminal boundary, and
negative cases for undeclared, repeated, or out-of-order requests.  It does
not authorize a host broker, host process launch, an ambient namespace,
`54:11` result transport, Bochs device changes, or a new runtime observation.
