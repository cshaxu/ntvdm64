# DOSX guest image and historical build inputs

The original OpenNT-4.5 DOSX image matches the packaged and installed image,
so it is recorded as partial pending execution evidence. Nine distinct source
or script file roles are historical guest build inputs, excluded only from the
checked host build. The original generic and x86 response-file distinctions
are preserved in the [source review](shard-2/dosx-guest-build-review.md).

The root `dxstrt.asm`, `dxhpbios.asm` and `dosx.def` remain pending because
their precise selected consumption was not established. DOSX image identity
does not prove source-to-binary lineage, protected-mode transitions, memory,
callbacks or error/teardown behavior. No guest or product was modified.
