# T198 S49: post-drive NTIO continuation

## Result

S49 removed only the retained fixture bridge's deliberate stop after a
successful `50:0F` result.  The same clean current-source build then continued
through the existing DEM initialization sequence and stopped at selector
`50`, service `46` (`SVC_DEMGETDPBLIST`) at `9346:7588`.

The r1 witness is
`artifacts/build/t198-s49-post-drive-observation-r1/run.log`:

```text
observed-dta-resume=1 observed-hard-error-resume=1
observed-drive-resume=1 observed-first-generic-ud=0
observed-stop=1 next=50:46 terminal=1:9346:758a
```

Thus execution crosses `50:0F`, `50:1B`, and `50:32` without a generic #UD
or a machine/device fault.  The final stop is the fixture's ordinary
unhandled-BOP disposition, not a synthetic Bochs failure.

## Source correlation and composition finding

OpenNT `demdisp.c` maps `SVC_DEMGETDPBLIST` to `demGetDPBList`; its contract
is implemented in `demgset.c`.  The current source-derived provider already
exists as `src/bx-vdm/bx_ntvdm_dem_dpb_service.c`, with composition use in
`src/bx-vdm/bx_ntvdm_adapter_runtime.c` and
`src/bx-vdm/bx_ntvdm_startup_transaction_bridge.c`.

The S48 clean-rebuild recipe deliberately reads the historical S25 compile
file only as a membership manifest.  That manifest predates the DPB provider,
so it does not compile or link the current DPB/runtime composition members.
Accordingly, S49 proves a **clean-recipe composition omission**, not a missing
`50:46` implementation and not authority to write a new service.

## Next admission

The next task must first derive a current, explicit `bx-vdm` BOP-composition
membership manifest from tracked sources, including the existing DPB provider
and its documented routing path.  It must then rebuild and observe `50:46`
with the same snapshot contract.  It must not add a new provider, device,
firmware behavior, or host capability.
