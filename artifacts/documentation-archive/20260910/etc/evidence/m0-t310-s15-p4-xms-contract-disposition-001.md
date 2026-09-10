# M0 T310 S15 P4 — XMS provider and declaration dispositions

## Question

Does every selected XMS declaration/definition and selected caller have one
owner and a non-synthetic disposition under the selected SoftPC CCPU40 path?

## Inputs and procedure

Read the six original `xms.486` manifest members, `xms.h`, the original
`nt_msscs.c` initialization call and `nt_bop.c` dispatch call, then compare
the exact `PSAMEMORYMOVEROUTINE` form in original `mvdm-support/inc/suballoc.h`
with the selected XMS memory binding.  Record the result in the accompanying
[owner-contract ledger](m0-t310-s15-xms-owner-contract-ledger-001.tsv).

## Observations

- `nt_msscs.c` calls `XMSInit` in its original startup order. `nt_bop.c`
  routes the XMS BOP byte to `XMSDispatch`; neither requires an invented
  alternative dispatcher.
- `suballoc.h` defines the move callback as `Destination, Source, Size`.
  The direct historical `xmsmem86.c` implementation and the selected session
  binding have that same operational order. The opposite parameter *names* in
  the public `xms.h` declaration are a historical declaration discrepancy,
  not authority to swap the callback's actual arguments.
- The three exported declarations `xmsGetMemorySize`, `xmsGetDefaultVDMSize`
  and `sas_manage_xms` have neither a selected caller nor a selected original
  definition. They remain explicit original-unavailable declarations; no
  replacement was created.

## Conclusion

All selected XMS operation bodies and callback edges now have a single
original/binding/adapter-backed disposition. The two actual runtime entries
remain the original SoftPC startup and BOP routes. This closes code and
source-contract classification, but not behavioral lifecycle proof: that
requires a reachable active SoftPC session with original register, UMB and
machine initialization owners present.
