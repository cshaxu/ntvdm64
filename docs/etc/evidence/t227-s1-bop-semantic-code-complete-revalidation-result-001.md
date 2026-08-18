# M0 T227 S1 — BOP semantic code-complete revalidation result

## Scope

This result revalidates the actual local provider semantics behind the retained
203-entry BOP inventory. It does not run a native guest image or claim that a
historical optional host runtime (WOW, DPMI, Redirector networking, debugger,
or virtual device) exists.

## Formal inputs

- Current 203-entry inventory: `docs/etc/operations/t226-s1-bop-local-conformance-manifest-001.json`.
- Formal graph generator: `tools/build/New-T225S7FullNinjaGraph.ps1`.
- Fresh disposable root: `build/M0-T227-S1/dem-semantic-20260817a`.
- Toolchain: MSVC x64 `/MT`; projected Bochs CPU5 configuration SHA-256
  `23B0A094B911C8663587BB62AE6A6E1AD4E109023F2D1EA6C7A555D032EE2877`.

## Procedure and results

1. Regenerated the full formal Ninja graph at the declared root.
2. Built and executed the 15-fixture DEM package: all returned zero.
3. Built and executed every one of the 40 retained T226 owner-package
   fixtures: all returned zero.
4. Ran `Test-T226S1BopConformanceManifest.ps1 -RepositoryRoot
   O:\repos.hobby\ntdos64`: the deterministic 203-row manifest passed.
5. Ran `ninja -C build/M0-T227-S1/dem-semantic-20260817a all`: completed
   successfully.

The tested distribution is DEM 15, COMMAND 15, XMS 5, DPMI 1, Redirector 1,
and Debugger/top-level-machine/WOW 3. These fixtures cover the selected
source-preserving provider behavior or explicitly source-proven
no-op/unavailable/deferred/typed-stop outcomes for Direct and Readonly.

## Build limitation

One first attempt to link the COMMAND bootstrap-provider fixture reported
`LNK1104` for its Ninja response file. A same-root literal-target retry
linked and executed it successfully; all 40 fixtures then executed with zero
exit status and the final `ninja all` succeeded. The event is recorded as a
non-reproduced build-host response-file anomaly, not as a BOP semantic or
product-code failure. No source change was made to conceal it.

## Conclusion

The current BOP owner-package code is locally code-complete for the declared
Direct/Readonly contract. The next work is intentionally integrated Guest DOS
execution: it must use immutable source-built images and may reveal a complete
owner-domain defect, but it must not reopen BOP work merely from trace order.