# VDMREDIR cached and delivered image identity

The retained observer compares the cached x86 worker-build VDMREDIR, the
release asset and the installed VDMREDIR. At this snapshot release and installed
whole-file hashes match; the cached whole-file hash differs. The two image
`.text`, `.data` and `.reloc` raw section bytes match, while `.rdata` and
`.rsrc` differ; PE timestamps also differ. The differing resources and
read-only data differ in three bytes. The [resource observer](vdmredir-resource-observer.py)
and its [snapshot](vdmredir-resource-observations.json) show that the release
contains an additional 16-byte `NTVDM_PACKAGE_ID` resource; its version-resource
payload is byte-identical to the cache version. The resource-directory codepage
also differs. The three differing `.rdata` bytes all fall in the timestamp
field of the single PE debug-directory entry; the observer checks their exact
RVAs. This accounts for the observed section differences but does not establish
source-to-image provenance or prove runtime semantic equivalence.

The [VDMREDIR source review](../shard-2/vdmredir-host-roles-review.md) records
current graph and source evidence. Bind any claim about the delivered DLL to
its actual PE bytes and relevant object relocations, and account for the
different data/resource inputs before relying on cached-image results.
Real network, interrupt, failures and lifecycle behavior remain S17 work. This
check only reads files; no DLL, guest or product was built, changed or run.
