# T198 S12 P1 Adapter Composition Witness 001

## Question

Can an explicit adapter-owned session synchronously compose copied generic-UD
events, the S10 boot-namespace plane, existing mantle mechanical actions and
the generic typed outcome without adding Bochs semantics?

## Inputs And Procedure

The i686 closure rebuilds the finite BYOB/adapter/provider plane set, then
builds the new composition source and its test with `-Wall -Wextra -Werror`.
The test supplies a local implementation of the existing mantle action ABI;
it validates every range and copies only its declared bytes to/from a 1 MiB
fixture RAM array. This is an executor boundary fixture, not a replacement
mantle implementation.

The test verifies: unbound-session decline; explicit initialize/bind;
COMMAND `54:0C` write-action execution and resume at `fault+4`; DEM open
`50:12` read/action completion and returned token; DEM seek `50:00` direct
resume; action-executor failure; and decline after explicit unbind.

## Observation

The resulting i686 executable exited `0`. The newly added composition and
test compile warning-free under `-Werror`. The retained `byob_profile.c`
emits its pre-existing unused `json_skip_value` warning, and
`bx_ntvdm_command_plane_v1.c` emits its pre-existing misleading-indentation
warning; neither is in this change and neither prevents the source-built
closure.

The COMMAND branch now copies its already-created transaction result to the
plane output after producing a write action. The test consequently observes a
generic `RESUME` outcome rather than a second #UD at the same boundary.

## Interpretation And Confidence

The session state, global active binding and all BOP/provider terms remain in
`bx-vdm`. The only mantle call is the pre-existing selector-blind mechanical
action function with its fixed record. The fixture has not proven native CPU5
`UD2` execution or a production CLI initializer; it proves the complete
adapter-side synchronous transaction and negative lifetimes. Confidence is
high for this adapter boundary and moderate for the later native link/run
step.

## Follow-up

S12 must next replace the classification-only alternative bridge with this
session handler in an explicitly composed i686/MSVC-x86 object closure, then
attempt a bounded native `UD2` fixture. No core/mantle change is admitted.
