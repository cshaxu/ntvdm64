# T483 S11 closure

Owner accepted S11 after the P7 delivery at 81f4cef3d.

P7 delivered the compat boundary cleanup. The owner retained two B-to-C exceptions in the WOW32 build, `_GetDlgItem` and `gabObjectCreateFlags`. The four other WOW units have moved to their WOW32 owner. The three character conversion services now build in compat. The native Console provider retains keyboard layout and code page ownership. The x86 preflight, formal WOW32 graph, control string and dialog creator fixtures passed. The owner approved the XACTSRV net increase of 10 raw mirror lines and the NT SDK subset of 53 raw mirror lines. Both are in their original non-MVDM locations. All seven XACTSRV trial objects matched the forty production code sections. Cached x86 and x64 builds passed. Both release destinations matched all eleven manifest hashes. The Full suite passed all twenty gates, including the version rejection fixtures and DOS4GW/NESticle. The [P7 dependency and delivery record](../../../artifacts/mvdm-audit/t483-s11-p7-compat-boundary.json) identifies the two exceptions and the checked production graphs.

S11 P6 moved wow_control_brush, wow_control_fnid, wow_control_state and
wow_dialog_memory to wow32-dll/source. Each source and all seven object code
sections match the preceding build byte for byte. Cached x86/x64 builds and
the formal WOW32 link check passed. Four fixtures exercised FNID, brush,
edit state and dialog callbacks with zero failures. The six distinct WOW32
provider symbols previously bound by those compat units now resolve within
WOW32. Six remaining compat units still read WOW32 headers. Package
e0b93bbc-5a9f-43c3-a1c0-09d527bd1f40 matched both release destinations and
all eleven PE text sections matched P4. Receipts are under build/t483-s11-p6.

S11 P4 isolates A-library compilation in the x86 worker and native x64 product
graphs. The generators select library flags by source ownership and keep
service IDL prerequisites on product code. The cached x86 and x64 release
builds passed. The [production library boundary check](../../../tools/audit/verify-library-build-boundary.py)
verified 110 library object contexts across eight graphs. The link-map and
COFF review found zero outward library symbol bindings. Targeted CRT,
thread, pipe, launch, file identity, handle, process attribute and frontend
window tests passed. The frontend test required building its explicit Ninja
target before running it. Package 6e1d0b82-69a3-4f27-9ee2-b10f150edeb6 was
verified at both release destinations. All eleven PE text sections match P3.
Build, publication and targeted test receipts are under build/t483-s11-p4.

S11 P3 moved the [RegisterClassWOWA unit](../../../src/compat/wow_class_client.c) from compat to wow32-dll with identical source contents. The WOW32 production graph now builds the object under its binding owner. Cached x86 and x64 release builds and the formal graph check passed. The class-client fixture reported zero errors across five version domains, server_owner=1 and cpd=2. All eleven images and the manifest of package 5116116b-cf58-4523-8835-bf63c97248a6 matched the staged hashes in assets/release and O:/winnt/system32. Receipts are under build/t483-s11-p3.

S11 P2 isolates the WOW32 production CRT object from the BC headers. Its native flags retain the x86 CRT, calling convention and optimization settings. No declaration or implementation moved. The cached x86 and x64 release builds passed. The [CRT regression](../../../test/lib-win32/crt-compat/verify-crt-compat.ps1) checked the production command, compiler dependency record, both link maps, formatting with the actual WOW32 object, and missing/duplicate provider failures. All eleven images of package 1941b6a0-3dc9-4549-a420-64c4a06c13b2 were hash-verified at both release destinations. Build, test and publication receipts are under build/t483-s11-p2. S11 remains active for the other A-library compilation contexts.

S10 delivered the frontend-base and nthook-dll layout at 14969ed80. S11 admission starts from the accepted closure at 49d0ce087.
