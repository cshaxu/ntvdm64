# mvdm-support

Canonical original MVDM support mirror. Its final retained upstream-relative
package set is `dirs`, `makefil0`, `inc`, `oemuni`, and `suballoc`.

The current T276 audit keeps the byte-exact source union temporarily under
`src/mvdm-host`; no source is copied, changed, linked, or deleted by
this skeleton. A dedicated hash-verified `git mv` packet must establish this
component before a support source becomes a formal build input.

T301 S1 P21 preserves any missing byte-exact support build products from
`opennt-src-2` under their original-relative paths.  They are provenance
evidence only and do not enter a host build or link graph; see
[`m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv`](../../docs/etc/operations/m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv).

## Divergence register

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| MVDM-SUPPORT-DIV-001 | Model `DEMEXTERR.ExtendedErrorPointer` as a 32-bit field in the packed DOS data record. | The original x86 `PUCHAR` happens to be 32 bits; on x64 it widens and changes the guest ABI. | Preserve the source record's nine-byte packed layout by storing the guest pointer numerically as `ULONG`. | `inc/dossvc.h` |
| MVDM-SUPPORT-DIV-002 | Model the packed `DOS_CALL_NAMED_PIPE_STRUCT` 16:16 pointer fields as native pointer typedefs. | The original x86 typedefs happen to be 32 bits; on x64 they widen and move every subsequent guest field. | Preserve the original four-byte physical 16:16 values as `DWORD`; unchanged `READ_FAR_POINTER` source decoding retains the original name, parameter and result semantics through the bounded Redirector pointer scope. | `inc/vrnmpipe.h` |
| MVDM-SUPPORT-DIV-003 | Model the packed `DosWriteMailslotStruct` buffer location as a native pointer. | The original x86 pointer is four guest bytes and would widen on x64. | Keep a four-byte numeric 16:16 location, decoded only by the original `READ_FAR_POINTER` flow inside a bounded Redirector pointer scope. | `inc/apistruc.h` |
| MVDM-SUPPORT-DIV-004 | Model packed `DOS_ASYNC_NAMED_PIPE_STRUCT` far-address members as native pointer typedefs. | The original 16:16 request is a fixed guest ABI; x64 pointer widening changes offsets before original Redirector code can inspect it. | Keep each location as its original four-byte numeric value. The original source decodes it only under a bounded Redirector pointer scope; cross-call completion still requires a separately admitted safe completion binding. | `inc/vrnmpipe.h` |
