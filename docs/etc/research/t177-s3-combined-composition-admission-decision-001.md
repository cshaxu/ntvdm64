# T177 S3 combined composition admission decision 001

## Decision

Admit one exact combined native-POST/machine-composition build and one bounded
runtime observation as S4 of this packet.

## Basis

T177 S2 proves that the prior machine-composition observation retained direct
startup and therefore could not test the source-proven POST/INT10
precondition. The existing derivative generator already declares the required
combination through `-DeferredStartupPlan -MachineComposition`; it replaces
only `main.o` and `cpu\\exception.o`, links the existing machine component
objects, and retains all Bochs archives. T160 separately proves the native
POST/deferred half of that exact two-object shape.

This is a composition verification, not a repair. It creates no new BOP
provider, device, firmware, IVT value, host capability, adapter behavior, or
Bochs semantic change.

## S4 execution envelope

1. Generate a fresh derivative using both existing flags.
2. Verify from its manifest and makefile that exactly `main.o` and
   `cpu\\exception.o` are rebuilt, all Bochs archives remain retained, and the
   machine component object set is the existing T176 set.
3. Perform exactly one MSVC/x86 link and exactly one bounded observation using
   the existing native-POST profile/observer path.
4. Record whether native POST is reached and whether machine-composition
   selector `06h` is observed or remains absent. Any terminal condition is
   descriptive only; no CLI-completion claim is permitted.

## Stop conditions

Stop without broadening if the combination requires a third Bochs object,
archive rebuild/repack, source edit, device/firmware change, BOP-provider
extension, ABI workaround, a second observation, or a synthetic vector.

## Expected value and limits

The highest-value result is a clean separation between lifecycle and machine
BOP reachability: a native-POST run with no `06h` says the BOP remains
unreached; a recorded `06h` gives the first valid context for its already
admitted original handler. Neither outcome authorizes further mechanics during
this packet.
