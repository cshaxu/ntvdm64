# adapter-win32

Same-shaped facade for unavailable historical Win32/NTDLL calls using public
Win32 APIs. Existing project code is recovery evidence until audited.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-WIN32-001 | NT4 exported RTL string, heap, TEB/PEB and current-directory helpers to `oemuni`. | Modern public SDKs do not expose the historical private product composition safely on both x86 and x64. | Preserve source-facing names and counted-string/NTSTATUS contracts; use public Win32 conversion, heap and directory APIs with adapter-private TLS state. | `include/nt.h`, `include/ntrtl.h`, `include/nturtl.h`, `source/opennt_support_rtl.c` |
| ADAPTER-WIN32-002 | Reached OEM calls reuse `TEB.StaticUnicodeString` after temporary `RtlInitUnicodeString` rebinding. | A real modern TEB cannot provide the historical scratch field safely. | Restore only the adapter-owned TLS buffer before a non-allocating OEM-to-Unicode conversion. | `source/opennt_support_rtl.c` |
| ADAPTER-WIN32-003 | `demsrch.c` reaches the NT4 `NtVdmControl(VdmQueryDir, VDMQUERYDIRINFO)` control contract, while also including private `winbasep.h` without using its declarations. | Modern Windows exposes no supported kernel VDM control service; importing the full private Base/VDM headers would violate the mirror boundary. | Preserve the reached VDM enum/structure/function spelling. Route only `VdmQueryDir` through the bound session's synchronous generic control seam; other VDM services explicitly return `STATUS_NOT_IMPLEMENTED`. `winbasep.h` is a behavior-free same-name include carrier. | `include/vdm.h`, `include/winbasep.h`, `source/vdm_control.c` |
