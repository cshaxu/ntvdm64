# S10 generic thunk: S9 failure evidence bound to published code

This supersedes the limited image-binding qualification in `wow-generic-thunk-call-convention-review.md`; it does not amend the original report. No products, guest, tests or binaries were modified or executed. Evidence captured by `generic-thunk-package-observer.py` and `generic-thunk-package-observations.json`.

1. `shard-0/wow-generic-thunk.md` documents the S9 controlled mismatch test using actual `wkgthunk.obj` SHA-256 `50E3000F28A6A6C943B7B521BDCD39E6DF1B61D2EC8C07A42C1D8599811C83B5`: stdcall-advertised / cdecl actual callee terminated with `0xC0000409`; matching stdcall and original-helper control completed. This is the **same hash** as the current cached object verified in the earlier shard-3 report.
2. Its isolated Win16 nested-callback baseline used full DLL SHA-256 `C97C512C6F6BD6F16A6FB68F340760F551CD2186B5262B1A6AE892FC8A06BC58`, exact match for the current cached producer. The baseline exited `0x42b` after phase 0; a separate candidate using original helper and EBX-preserving adapter completed its specified phases. These are historical runs, not new tests, and the guest baseline did not independently identify worker exception cause.
3. `video7-pointer/publication.json` records the producer `C97C...` and published WOW32 `FF5AAD62B628F0AF28DF23255D97059ABF089CBBA9226CE46091412F4093E431`. Today both `assets/release/WOW32.DLL` and `O:/winnt/system32/WOW32.DLL` hash to that published identity. Their `.text`, `.rdata`, `.data` and `.reloc` raw bytes, section virtual addresses and sizes match producer; `.rsrc` differs. In particular the complete `.text` SHA-256 for all three is `E2917CC9D47853D3157D6FBCF911F5C9E75E2073FE35CA505207F6F9FD30DF51`.

Thus the currently installed/published executable code and the earlier reproduced baseline are **byte-identical in their PE code section**, and the tested object hash is the current cached object. The published image has a distinct whole-file hash and the S9 guest execution used C97, not the resource-modified FF5 file; do not claim the latter full file was itself run in the S9 experiment. Equal PE code sections do not by themselves prove package context, dynamic imports/resolution, resource interactions, or every application failure. On this concrete route the prior report now establishes an actual selected-object mismatch failure and a focused guest baseline/candidate differential; wider calling conventions, failures, stack unwinding, task teardown, reentry, and candidate package acceptance remain S14 obligations.

Replay from repository root without build:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'
python artifacts/mvdm-audit/shard-3/generic-thunk-package-observer.py
Get-FileHash build/.ninja/msvc-x86-release/wow32/obj/wkgthunk.obj -Algorithm SHA256
Get-Content artifacts/mvdm-audit/shard-0/wow-generic-thunk.md
Get-Content artifacts/mvdm-audit/video7-pointer/publication.json
```
