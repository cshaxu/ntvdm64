# Real WOW32 explicit unload on missing export

A diagnostic-only provider was linked from the selected WOW32 objects with
`W32Init` deliberately omitted from a copied export definition. Its copied
original initializer records process ID, notification reason and whether
the reserved argument is non-null. The complete isolated package was sealed
and its export table confirms `W32Init` is absent while `W32Dispatch` remains.
No guest, production source or published package was changed.

The real worker reached `init-export-missing`, the original `nt_bop.c`
branch that calls `FreeLibrary` before terminating. Both default and `--wait`
Run16 returned `ERROR_DLL_INIT_FAILED` (1114). For each actual worker the
initializer recorded process attach followed by process detach with a null
reserved argument: `00008D68:1:0`, `00008D68:0:0` and `00007EA0:1:0`,
`00007EA0:0:0`. This distinguishes explicit unloading from process-termination
detach. The staged provider hash is
`CFA1F6E5E241F347A1AC88C89A67E964DF4253A74C4CF9BDF8E9C34422837297`;
the separately linked input and export-definition hashes are in `inputs.csv`.

This proves that the actual failure branch reached the original DLL detach
entry and terminated with the expected launcher error. The source bridge
calls the project bitmap and registry shutdown functions on this reserved-null
path; this witness did not instrument those functions or prove their cleanup
effects. The controlled bridge fixture separately verifies both calls, but
does not substitute for a live cleanup-state test. The file and loader
instrumentation may alter timing and is diagnostic only.
