# adapter-win32

Same-shaped facade for unavailable historical Win32/NTDLL calls using public
Win32 APIs. Existing project code is recovery evidence until audited.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-WIN32-001 | NT4 exported RTL string, heap, TEB/PEB and current-directory helpers to `oemuni`. | Modern public SDKs do not expose the historical private product composition safely on both x86 and x64. | Preserve source-facing names and counted-string/NTSTATUS contracts; use public Win32 conversion, heap and directory APIs with adapter-private TLS state. | `include/nt.h`, `include/ntrtl.h`, `include/nturtl.h`, `source/opennt_support_rtl.c` |
