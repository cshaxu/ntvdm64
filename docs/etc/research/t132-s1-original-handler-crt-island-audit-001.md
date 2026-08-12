# T132 S1 original-handler CRT-island audit 001

## Question

Can T130's original `unexpected_int` handler island be treated as a supported
general composition pattern for additional historical handlers?

## Inputs and procedure

Read-only inspection used the retained T113 object, T130 machine component and
CPU object, T130 final PE imports, map, and build logs. No source, compiler
flag, link input, executable, or runtime trace was changed.

## Observations

| Input | Directives or evidence | Consequence |
| --- | --- | --- |
| T113 `unexp_nt.c.obj` SHA-256 `17C693BE...E5CB12E` | `/DEFAULTLIB:msvcrt.lib`, `/DEFAULTLIB:oldnames.lib` | The clang-cl historical object requests the dynamic MSVCRT import library. |
| T130 `cpu/exception.o` | `/DEFAULTLIB:LIBCMT`, `/DEFAULTLIB:OLDNAMES` | Bochs's admitted native target is statically CRT-shaped (`/MT`). |
| T130 machine component | `/DEFAULTLIB:LIBCMT`, `/DEFAULTLIB:OLDNAMES` | The mechanics component matches Bochs, not the original object. |
| T130 link | Retained `LNK4098` warning for `msvcrt.lib`, plus explicit `vcruntime.lib` | Link success does not prove a coherent CRT model. |
| T130 PE imports | `VCRUNTIME140.dll`, `KERNEL32.dll`, `bcrypt.dll`; no `msvcrt.dll` import | Absence of the DLL import does not remove the conflicting static default-library resolution. |

## Disposition

The `unexpected_int` island is **research-only-with-risk**. T130 proves its
source mechanics and bounded invocation, not a general CRT-safe historical
object pattern. It may remain as the sole individually registered and observed
exception; no future machine handler may reuse its compile/link recipe or infer
CRT safety from the T130 PE import list.

Before any additional original handler island, a separate task must establish
one supported x86 CRT/runtime contract for the precise compiler, object,
component, Bochs target and final link. That task may not silently suppress
default libraries or use `/NODEFAULTLIB` as a workaround without proving all
runtime support and ownership edges.
