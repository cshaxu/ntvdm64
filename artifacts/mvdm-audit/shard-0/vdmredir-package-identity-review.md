# VDMREDIR cached and delivered image identity

The retained observer compares the cached x86 worker-build VDMREDIR, the
release asset and the installed VDMREDIR. At this snapshot release and installed
whole-file hashes match; the cached whole-file hash differs. The two image
`.text`, `.data` and `.reloc` raw section bytes match, while `.rdata` and
`.rsrc` differ; PE timestamps also differ. The differing resources and
read-only data have not been interpreted, so timestamp alone is not asserted
as the reason and semantic equivalence is not claimed.

The [VDMREDIR source review](../shard-2/vdmredir-host-roles-review.md) records
current graph and source evidence. Bind any claim about the delivered DLL to
its actual PE bytes and relevant object relocations, and account for the
different data/resource inputs before relying on cached-image results.
Real network, interrupt, failures and lifecycle behavior remain S17 work. This
check only reads files; no DLL, guest or product was built, changed or run.
