# T422 S1 WOW32 provider-build execution boundary

## Question

Can the S1 provider graph produce fresh x86 object/link evidence, including
the conditional `Sim32FlushVDMPointer` removal test?

## Inputs and procedure

The graph was generated at
`build/M0-T422/S1/wow32-provider-r1` by the existing provider generator.  It
selects 77 original WOW32 bodies, 19 original non-MVDM link objects and 21
target-local binding objects.  The focused target was `obj/wow32.obj`.

Four execution forms were checked:

1. direct Ninja without an x86 environment;
2. `cmd` invoking the graph's `msvc-x86.cmd` then Ninja; and
3. a PowerShell process populated from `VsDevCmd.bat` before direct Ninja.
4. the same Ninja graph expanded with `ninja -t commands wow32.dll`, with each
   generated command run serially inside the graph directory through its x86
   MSVC environment.

## Observations

| Form | Observation | Interpretation |
| --- | --- | --- |
| Direct Ninja | `CreateProcess failed: The system cannot find the file specified.` | Ninja has no `cl.exe` on PATH without the x86 environment. This is a host invocation error, not a source compile diagnostic. |
| `cmd` environment then Ninja | `obj/wow32.obj` is emitted (18,671 bytes), then Ninja remains live at negligible CPU after `cl.exe` is gone. | The compiler reached a successful object write; the parent-process wait does not return. |
| Imported VS environment then Ninja | The same object is emitted, then the Ninja/parent pair remains live after `cl.exe` exits. | The delayed process reaping is independent of the missing-PATH form. |
| Serial generated commands | All 128 commands complete, including resource compilation, the four legacy import libraries and final link. | This bypasses only the observed Ninja child-reaping failure; it uses the unmodified generated command lines and is valid fresh x86 link evidence. |

The PowerShell import must preserve the uppercase `PATH=` returned by
`VsDevCmd.bat`; assigning both the returned `PATH` and the caller's `Path`
causes the caller's non-VS path to overwrite the compiler path.  This explains
the direct-Ninja create-process result, but not the later wait hang.

Only diagnostic Ninja and parent processes were ended.  No user runtime
`ntvdm.exe` or `basesrv.exe` process was touched.  Build output remains below
the required task-local `build/` root and is not a candidate artifact.

## Fresh-link result

The serial generated-command run produced `wow32.dll` (561,664 bytes) with
SHA-256 `3966DD21E2286172594EDA2092F02542172FD229153099F70AE4F528B5B4AF63`.
`dumpbin` reports x86 machine type, exactly 27 exports (including original
`W32Init`, `W32Dispatch`, `GetCommHandle` and `GetCommShadowMSR`) and imports
from `ntvdm.exe`, `KERNEL32.dll`, `USER32.dll` and `GDI32.dll`. The linker
emits only its expected ignored `DESCRIPTION` warning and two duplicate-alias
warnings for the generated stdcall aliases; it emits no unresolved external.

The resulting DLL imports 91 named functions from its one worker parent,
`ntvdm.exe`; the import table is finite and contains no copied session or CCPU
archive. Their source-shaped allocation is:

| Parent-import group | Count | Sole owner / later receiver |
| --- | ---: | --- |
| worker runtime/TLS/TEB exit | 7 | `ntvdm-exe`; S2 verifies user-domain entry and retirement without a second TLS owner. |
| guest-memory/page-domain | 8 | `ntvdm-exe`; S2 owns publication/retirement usage, not a second mapping manager. |
| CPU descriptors/register access | 33 | existing one CCPU worker; S2--S7 consume it but do not own it. |
| VDD/DOS/timer/thread services | 20 | existing worker and original VDD/DOS owners; S6/S7 exercise reached callers. |
| OEM file/environment services | 14 | S6's existing worker/file boundary. |
| debugger notifications | 3 | completed T420 debugger boundary. |
| error/allocator/PEB/status helpers | 6 | their preexisting worker compatibility owners; S7 audits reached paths. |

This is an import boundary, not a promise that all 91 services are fully
functional for WOW16 yet. Its value is that S2--S7 can close each original
family against a bounded parent ABI rather than silently linking a second
worker implementation into the DLL.

This establishes the fresh provider **build** boundary. It does not establish
runtime loading, callback behavior, desktop publication or WOW16 acceptance.

## Pointer-flush removal experiment

The final link command was repeated in the same build root with only
`obj/wow32-dll/wow_sim32_pointer_compat.obj` removed and an alternate output
name. It fails with `LNK2001 _Sim32FlushVDMPointer@16` from 38 original WOW32
objects, including `wow32`, `wcall16`, `wusercli`, `wuser`, `wuwind`, `wkman`,
GDI, resource, shell and dialog consumers. This is decisive build evidence:
the current selected headers retain the historical external spelling in the
late DLL, even though the CPU40 host header provides an in-process no-op macro.
The 24-line ABI-compatible no-op remains required; it is not autonomous
pointer-management behavior and must not be deleted merely for a smaller line
count.
