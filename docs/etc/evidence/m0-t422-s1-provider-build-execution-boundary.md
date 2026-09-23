# T422 S1 WOW32 provider-build execution boundary

## Question

Can the S1 provider graph produce fresh x86 object/link evidence, including
the conditional `Sim32FlushVDMPointer` removal test?

## Inputs and procedure

The graph was generated at
`build/M0-T422/S1/wow32-provider-r1` by the existing provider generator.  It
selects 77 original WOW32 bodies, 19 original non-MVDM link objects and 21
target-local binding objects.  The focused target was `obj/wow32.obj`.

Three execution forms were checked:

1. direct Ninja without an x86 environment;
2. `cmd` invoking the graph's `msvc-x86.cmd` then Ninja; and
3. a PowerShell process populated from `VsDevCmd.bat` before direct Ninja.

## Observations

| Form | Observation | Interpretation |
| --- | --- | --- |
| Direct Ninja | `CreateProcess failed: The system cannot find the file specified.` | Ninja has no `cl.exe` on PATH without the x86 environment. This is a host invocation error, not a source compile diagnostic. |
| `cmd` environment then Ninja | `obj/wow32.obj` is emitted (18,671 bytes), then Ninja remains live at negligible CPU after `cl.exe` is gone. | The compiler reached a successful object write; the parent-process wait does not return. |
| Imported VS environment then Ninja | The same object is emitted, then the Ninja/parent pair remains live after `cl.exe` exits. | The delayed process reaping is independent of the missing-PATH form. |

The PowerShell import must preserve the uppercase `PATH=` returned by
`VsDevCmd.bat`; assigning both the returned `PATH` and the caller's `Path`
causes the caller's non-VS path to overwrite the compiler path.  This explains
the direct-Ninja create-process result, but not the later wait hang.

Only diagnostic Ninja and parent processes were ended.  No user runtime
`ntvdm.exe` or `basesrv.exe` process was touched.  Build output remains below
the required task-local `build/` root and is not a candidate artifact.

## Conclusion

There is currently no fresh **linked** provider DLL result, so S1 does not
claim a provider build pass and does not delete the conditional 24-line
`wow_sim32_pointer_compat.c` candidate.  There is also no C compiler error
from `wow32.c`: a fresh object was written before the external wait failure.

The next build attempt must use an execution host that correctly reaps the
compiler child (for example a normal local terminal invocation), then run the
whole `wow32.dll` target and record the exact link result.  It must not disable
Defender, add exclusions, alter guest media, or substitute an older DLL as
fresh evidence.  The static provider graph verifier remains valid only for
composition, not this missing link proof.
