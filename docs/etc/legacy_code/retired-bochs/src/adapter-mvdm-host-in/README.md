# adapter-mvdm-host-in

Selector-blind copied-frame transport only. This component is the renamed
production home of the former machine-to-BOP boundary; it recognizes neither
OpenNT selectors nor services.

## Present source inventory

- `generic_ud_bridge.{h,cc}` supplies the fixed-width #UD event/result record
  and default mechanical decline/observation behavior.
- `bop_ingress.{h,c}` owns the app-registered, in-process opaque callback and
  no provider routing policy.
- `frame_transaction.{h,c}` is retained as a source-level migration unit. Its
  former CPU-result and checked-RAM ABI carriers were retired during adapter
  convergence, so it is deliberately excluded from an enabled runtime until a
  later task inventories and reconnects those same-shaped carriers. It is not
  a fallback implementation and it is not a `src.old` input.

The later inventory must either reconnect `frame_transaction` through named
current adapter contracts or remove it with a replacement/disposition record;
it may not revive the former component root or introduce selector semantics.

## M0 T304 S2 fixed-width contract

`generic_ud_bridge.h` is the sole public definition of the generic copied
machine-event and result records. Its sizes and critical offsets are asserted
for x86 and x64. `adapter-bochs` does not include this header or attach
provider meaning; it only copies bounded opaque bytes between the private
Bochs overlay record and this public contract.
