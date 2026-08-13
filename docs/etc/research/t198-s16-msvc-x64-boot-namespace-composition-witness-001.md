# T198 S16 MSVC x64 Boot-Namespace Composition Witness 001

## Question

Can the existing bounded boot-namespace BOP composition close as a single MSVC
x64 `/MT` C program before it is attached to the already-proven native
generic-`#UD` mechanics?

## Procedure

`tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` source-builds the exact
adapter closure retained by the former i686 composition witness. It includes
the boot-namespace composition/plane/provider, shared BOP ingress and provider
registry, their direct adapter dependencies, and exactly three `src/cli`
BYOB startup-contract data-model sources. The existing composition test supplies
its local 1 MiB mechanical-action executor; it does not link `bx-core`,
`bx-mantle`, product Bochs, a CLI engine, MinGW, or an OpenNT runtime object.

The fresh successful record is
`artifacts/build/t198-s16-bx-vdm-x64-r11/t198-s16-bx-vdm-boot-namespace.json`.
It records 34 exact C source inputs, MSVC `cl.exe`/`link.exe`, x64, `/MT`, and
the only platform library input, `bcrypt.lib`.

## Result

The r11 fixture compiles warning-free with `/W4 /WX`, links, runs, and exits
zero. `dumpbin /headers` identifies the result as machine `8664` (x64) and
PE32+. The retained fixture covers unbound decline; bind; COMMAND `54:0C`
write/action/resume; DEM `50:12` open/read completion; DEM `50:00` seek;
executor failure; and post-unbind decline.

The two production source corrections are bounded-copy repairs only:

- `bx_ntvdm_readonly_namespace.c` replaces five unchecked `wcscpy` calls with
  a length-checked destination copy.
- `bx_ntvdm_search_request_v1.c` copies an already validated 8.3 component by
  its computed length and terminal element.

The fixture's five fixed wide literals likewise use fixed-size copies. No BOP
selector, service disposition, provider choice, DOS behavior, Bochs behavior,
or guest ABI changed.

`bcrypt.lib` is required by the retained BYOB identity verifier and session
token source. It is the standard Windows CNG library for the startup-contract
data model, not an OpenNT/NTVDM legacy runtime or host-capability backend.

## Bound

This proves the current source-derived boot-namespace adapter session in the
owner-selected MSVC x64 runtime. It does not prove native CPU execution of a
BOP, a CLI initializer, general host filesystem access, or any newly admitted
BOP provider. The next admission may attach this same source-built closure to
the existing selector-blind generic-`#UD` mechanical bridge.
