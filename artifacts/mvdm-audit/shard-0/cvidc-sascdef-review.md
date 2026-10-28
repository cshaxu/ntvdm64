# C-VID SAS vector source role

`softpc.new/base/cvidc/sascdef.c` defines `cSasPtrs`, as does the distinct
`softpc.new/base/ccpu386/sascdef.c`. The current CCPU40 worker manifest selects
the latter through `base/ccpu386/sources`; the worker map attributes `_cSasPtrs`
to `original-ccpu386:sasCdef.obj`. The two source files are not byte-identical.
Consequently, a name-only match of `cSasPtrs` cannot establish that the C-VID
file is the provider. The paired observer records source hashes, selection and
map evidence; replay it against the same worker build inputs.

This is a source-role observation, not a proof that the selected SAS vector's
entries are all correct or that guest memory and video behavior are complete.
The C-VID file's final disposition remains pending until generated and indirect
consumers and the complete intended original profile have been reconciled.
No source, build, executable or guest image was changed.
