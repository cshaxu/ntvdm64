# T182 S2 finite-sequence derivative recipe 001

Date: 2026-08-12  
Packet: M0 T182 S2  
Disposition: recipe defined and generator-only verified; no compile, link, or observation occurred.

## Generated closure

`tools/New-T182S2FiniteSequenceEngineDerivative.ps1` generated the fresh
derivative `artifacts/build/current/t182-s2-finite-sequence-r2` from the
retained T177 machine root. Its generated manifest reports:

| Check | Result |
| --- | --- |
| Retained machine binary | T177 SHA-256 `8758F4335CB32B4FB97688ED3860E8B0C9E86D8155E6AA316E54F346177E8BFA` verified before copy. |
| Current adapter sources | 67 |
| Current CLI sources | 5, including `byob_launch_plan_v2.c` |
| Current source-object rules | 72 |
| New Bochs replacements | 0 |
| `main.o`/`cpu\\exception.o` build rules | 0 |
| Bochs archive prerequisites on final target | 0 |

The fixed retained inputs include the already-built T177 `main.o`,
`cpu\\exception.o`, the four retained Bochs archives, and the three retained
machine-composition objects. The sole final target is
`ntdos64-t182-finite-sequence.exe`; it may be built only with the manifest's
explicit `nmake /f ntdos64-t182-finite-sequence.mak
ntdos64-t182-finite-sequence.exe` command.

## Recipe boundary

The generator reads the retained manifest only to recover the verified 67
adapter and four base CLI *names*. It adds exactly
`byob_launch_plan_v2.c`, copies every current header/source by SHA-256, and
generates fresh object rules. It does not copy current Bochs sources, define
Bochs macros, or generate a rule for a Bochs object or archive. Thus header
changes cannot silently leave an old adapter object in the fresh derivative.

The first generator attempt (`r1`) copied no compiler output but rejected its
own CRLF-insensitive object-rule check. The checker was corrected; `r2` is the
accepted generator-only artifact. No compiler, linker, executable, or guest
process was invoked in either attempt.

## Next admission

S3 may perform exactly one named build of r2. Only if that build succeeds may
it perform one watchdog-bounded launch through the current source-built v5
CLI runner. A build failure closes the observation branch; an observation must
not retry, alter machine inputs, or establish `54:11`/DOS-result transport.
