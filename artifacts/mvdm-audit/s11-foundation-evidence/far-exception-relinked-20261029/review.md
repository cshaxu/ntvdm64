# Far-transfer selected-object recheck

The cached x86 worker graph marked the far-exception fixture for relinking.
It was relinked with the existing selected CCPU and fixture objects, then
`verify-ccpu-far-exception.py` passed all four trace-off, matching-selector,
other-selector and valid-destination cases. The fixture, map, selected
`c_xfer.obj` hashes and per-case results are in `result.json`; source, runner
and cached worker image hashes are in `input-hashes.txt`. A subsequent Ninja
dry run reported no pending fixture work.

This proves only the selected fixture's asserted fault/result and diagnostic
filter behavior. The existing link deliberately uses `/force:multiple` and
reports duplicate-symbol warnings; the runner checks the far-transfer and
exception-hook owners in the map, not every duplicate symbol. It is not a
real guest fault-delivery test or a Full product gate, and the cached worker
hash is provided as context rather than proof that the fixture's entire link
matches that worker. No guest or runtime source was modified by this recheck.

The verifier now additionally checks seven relevant original CCPU symbol
owners in the selected fixture map: far validation, descriptor type,
translation, physical write, SAS init/term and CPU init. The four cases pass
with those checks (`owners-result.json`). A separate copied-map negative
control changed the descriptor type owner to a fictitious fixture object;
the verifier rejected it before running the fixture. This strengthens link
identity evidence only: it does not eliminate other duplicate-symbol risks
or establish the whole fixture is equivalent to the product worker.
