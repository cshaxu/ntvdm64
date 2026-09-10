# M0 T276 S25 XMS final expected-integration dispositions

## Question

What is the final product treatment of every selected `xms.486` path while
preserving original XMS provider ownership and excluding the obsolete
direct-host-address backend?

## Conclusion

- The six selected original provider bodies (`xms.c`, `xmsa20.c`,
  `xmsblock.c`, `xmsdisp.c`, `xmsmisc.c`, `xmsumb.c`) are `adapter-backed`.
  They retain XMS dispatch, allocation, A20, UMB and INT 15 semantics; their
  machine calls retain original interface shape through
  `adapter-softpc -> adapter-bochs`.
- `xms.h` is `binding-only` original declaration input.
- `i386/xmsmem86.c` is `not-host-runtime`: its original direct pointer body
  cannot be linked on either supported host architecture.  Its three
  same-shaped contracts are provided by `adapter-softpc`, resolving numeric
  XMS addresses via the session `guest_memory` mapping-manager instance and
  checked Bochs ranges.
- `xmsmemr.c` is a non-x86 unselected backend and is `not-host-runtime`.
- The six original build-description paths are `not-host-runtime` evidence.

The conclusion does not enable the package.  In particular, `xmsumb.c` awaits
the existing durable bounded UMB machine seam; that is an enablement
prerequisite, not an ambiguity about its final owner or disposition.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Set-T276S25XmsFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S25XmsFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```
