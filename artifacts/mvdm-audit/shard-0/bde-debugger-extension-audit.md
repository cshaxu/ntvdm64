# BDE debugger extension: stack and memory helpers

The frozen union records `bde/bdestack.c` and `bde/bdeutil.c` as identical
to both selected OpenNT editions; both remain pending. This audit identifies
their source role and boundary, not all debugger functionality.

`bde/sources` declares a separate `DYNLINK` target named `bde`, with the
major component `ntsd` and minor component `ntsdexts`. Its module-definition
file exports debugger commands such as `k`, `kb`, `r` and memory dumps.
`bdecmds.c` receives a debugger-supplied `WINDBG_EXTENSION_APIS` table and
its stack commands call `WalkStack` and `WalkStackVerbose` in `bdestack.c`.
The latter reconstructs guest frames and uses helpers in `bdeutil.c`; those
choose supplied debugger memory/context operations or process APIs and use
NTVDM-specific state and symbols. This is an externally invoked debugger
extension, not the guest CPU execution loop or a callback automatically
required by a running DOS task.

The frozen product graph marks these source paths unselected. Searches of
the current package-scoped worker Ninja graph did not identify either
filename; the cached worker map is not identity-equal to package-c, so it
cannot establish exact deployed linker ownership. No BDE DLL export or
functional extension invocation has been demonstrated in the published
package. Existing session `debugger` bindings cover separately reached
boundaries and do not establish equivalent stack-walking/debugger commands.

The prior generic assertion that BDE requires a separate debugger admission
is consistent with the source role, but it is not an evidence-backed
exclusion from the user's whole-source capability inventory. Determine
whether independent external debugger-extension tooling is in the product
contract, trace required original context/symbol interfaces against CCPU40
and modern public APIs, then separately decide recoverability and verify
command invocation and lifecycle. Do not infer that guest execution or all
debugger services are missing from the absence of this extension. No source,
guest, executable or test was changed by this audit.
