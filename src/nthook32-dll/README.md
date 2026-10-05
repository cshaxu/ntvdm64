# nthook32-dll

Owner-admitted T431 S2, x86 /MT only. `nthook32.dll` is the sole injected
product DLL. NTVWM and the DLL select the same suspended-child installer;
run16 selects the copied-context reader without loading the interception DLL.
Native GUI propagation does not grant character-frontend authority.

Application discovery uses the same common/application_search object as
run16: CWD then each PATH directory, COM/EXE/BAT/PIF precedence and exact
explicit paths. The selected original OpenNT classifier follows discovery.
No independent argv[0]/basename identity gate remains in the DLL; actual
CMD's MEM.EXE application and extensionless `mem` command are valid together.
The wrapper retains its original A/W creation, attributes and parameter tail.

`detours/` is the unmodified MIT-licensed Microsoft Detours v4.0.1 slice,
commit e4bfd6b03e50de46b47abfbd1e46b384f0c5f833. Its LICENSE.md is retained.
Selected translation units: detours.cpp, modules.cpp, disasm.cpp, image.cpp,
creatwth.cpp; the latter includes uimports.cpp. No examples/helper are selected.
Use only direct same-width DetourUpdateProcessWithDll, never the
DetourCreateProcessWithDll wrappers which can start a helper.

The installer never resumes or closes borrowed creation handles. Installation
failure may leave import/payload allocations in an unpublished suspended
child: its creator must abort that child and close its creation handles.
No partially modified child is resumed or returned, and no previously handed
off process or descendant is terminated by this mechanism.

## Pinned source SHA-256

| File | SHA-256 |
| --- | --- |
| creatwth.cpp | 062533627BFA775246948FAD92F9F3C0EF7841CC1A83C15DB9178BE12F2DBF4F |
| detours.cpp | DF415E05F985AFE88F4B03BA34D6F5ABCE47EDDA11CD66D4F72902B801113EB7 |
| detours.h | 7B498657D8DB4CFF2D488B1E65CFEEB5B1305DE7F32D23588B1FA86AAA46FCF7 |
| disasm.cpp | 6C44283AC6987D77D92ACA93A6A8E9F0DC1F7CAF8A2B310E4B374853E9136236 |
| image.cpp | 8E7C94B335237565F3DCE14B6DFD9DD88876CF10FB55B9C910F12AE9C38BD190 |
| LICENSE.md | B301808B732CFAA60DF2B4B422D78CD97D2A15058B207E7E33F0535BA5170DD6 |
| modules.cpp | 4A3F970EB539A4379996EB17E14BD75902FE4F34D566CFD19682FF19315CBC6F |
| uimports.cpp | 2B2D92DF4C5412906D7E6830A7C2836D78E368A6BEF509BE028566CBD921E83B |

The copied payload is private bootstrap data, not RPC or authorization.
Existing NTSRV object authentication remains authoritative. No broker call,
wait, process creation or session lookup is permitted in DllMain.
