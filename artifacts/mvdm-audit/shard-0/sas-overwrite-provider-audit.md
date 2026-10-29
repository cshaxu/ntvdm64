# SAS overwrite provider: selected owner and stale attribution

The older S1 unselected-source record attributes the live overwrite wrapper to
`ccpu386/ntstubs.c`. The r10 NTVDM map instead selects
`cvidc/accessfn.c::sas_overwrite_memory` and `ccpu386/ccpusas4.c::c_sas_overwrite_memory`
(see `artifacts/mvdm-audit/findings.md`). Source tracing explains the difference:

1. Selected `host/src/sim32.c::Sim32FlushVDMPointer` invokes the public
   `sas_overwrite_memory` after translating the address. Other original callers
   include the EMM manager; build selection and each caller's runtime execution
   are separate questions.
2. The selected C-VID `accessfn.c` wrapper forwards address and length through
   `Sas.Sas_overwrite_memory`. `mvdm_cvidc_bind_vectors` sets this slot to
   `c_sas_overwrite_memory`; it is called from the selected C-VID
   `setup_vga_globals`, at the original post-CPU setup call in `main.c`.
3. The CCPU implementation explicitly does nothing because this interpreter
   has no compiled-code cache to invalidate. The unselected `ntstubs.c` wrapper
   also delegates to that exact CCPU implementation. Thus the named leaf is
   shared, while the actual r10 wrapper and dispatch path differ from S1.

`MVDM-HOST-DIV-092` in `sas.h` and the registry describes the selected
overwrite as direct and not a vector member. That historical description does
not describe the current r10 C-VID selection; DIV-189 correctly records its
vector assignment. Keep both original source and current implementation;
correct this attribution before relying on the registry for future cleanup.
This is source-and-map evidence, not proof of every EMM or host pointer-flush
call, all initialization ordering, dirty video handling, or guest behavior.
A vector can be uninitialized before its binding point; this audit does not
prove whether an early caller exists. The complete executable-dependent
call graph and a real guest runtime probe remain outstanding.

Historical corroboration: `tests/mvdm-host/cvidc_vector_binding_fixture.c`
asserts the slot points to the CCPU provider. T473 S5 records that fixture
passing (`docs/etc/evidence/m0-t473-s5-softpc-device-lifecycle.md`); this
is a host binding test, not evidence that a guest exercised an overwrite.

## Subsequent source-comment correction

The current divergence registry already distinguishes the external wrapper
from its C-VID vector dispatch. The stale explanatory comment in `sas.h` has
now been corrected to describe that same chain and refer to DIV-189. This is a
comment-only change; it supplies no new execution or early-initialization
evidence. The source remains a partial mirror-diff row and its current hash
and text-retention estimate are recorded in the mirror-diff table.
