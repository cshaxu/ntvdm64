# S12 — PIG branch with an existing project correction

The final simple positive PIG branch held from DIV-498 was individually
reviewed. `c_main.c::c_pig_interrupt` was excluded in the selected product
profile; it differed from OpenNT by the project-corrected dereference in
`setjmp(*ccpu386SimulatePtr())`, recorded in Git history. The whole branch is
replaced by the DIV-499 marker while retaining its original PIG guards. The
active non-PIG simulator and its frame handling remain unchanged.

[Ledger](s12-pig-modified-branch.json) records the previous Git baseline and
removed body hash. Cached NTVDM rebuild passed; after applying the published
identity, all ten outputs were identical to the released set except for link
timestamps. This is an inactive-source cleanup, not a change to CCPU V86
execution or a claim about PIG functionality. No guest was changed.
