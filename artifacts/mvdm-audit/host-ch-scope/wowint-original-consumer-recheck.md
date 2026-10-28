# WOW internal table declaration: original consumer recheck

`inc/wowint.h` contains only `FUN_SETTABLE`, the packed `WOWSETTABLE16` pointer argument and its pointer typedef. The current file and both frozen original editions have identical SHA-256. It is a declaration carrier, not the dispatcher or an implementation of an internal table service.

A case-insensitive literal search for the header name and both declared identifiers across current `src`/`tools`, `OpenNT/base/mvdm` and `OpenNT-4.5/nt/private/mvdm` finds only this header's own name and definitions in each MVDM tree. The previously recorded worker/WOW32 dependency census has no exact selected edge, as documented in [the profile follow-up](../host-inc/wow-unselected-profile-followup.md). These are source and captured-graph observations, not proof that external, dynamically generated or unbuilt consumers cannot exist.

Classify this exact unused declaration carrier outside the observed NTVDM host source selection. Do not delete it or conclude that all WOW table-setting semantics are absent or restored: any reachable original internal service and guest ABI remain S14 obligations. Reopen this file's role if a supported provider or generated consumer is identified. No product or guest modification or execution was made.
