# Native x86 16-bit bit-string address wrap observation

The [standalone probe](ccpu-bit-address16-oracle.c) executes actual IA-32 BT encodings with 16-bit address size and either 16- or 32-bit operand size in a 32-bit Windows process. It uses a structured exception filter to capture the accessed linear address from `STATUS_ACCESS_VIOLATION`; the tested low addresses must be unmapped. It is independent of NTVDM and does not build, edit or run a guest. The independently rebuilt source SHA-256 is `6DB957E060FAA5595377889EFAE309921A9A957F540D3424C4E0921FBFE54A30`; the replay EXE SHA-256 was `052EE41E701E69D03430070D012D372EB031005B3CF313EAB248EA4863AE218A`.

| Operand | Base BX | Index | Observed exception | Observed address |
| --- | --- | --- | --- | --- |
| word | 0000 | CX=FFFF (-1) | C0000005 | 0000FFFE |
| word | FFFE | CX=0010 (+16) | C0000005 | 00000000 |
| dword | 0000 | ECX=FFFFFFFF (-1) | C0000005 | 0000FFFC |
| dword | FFFC | ECX=00000020 (+32) | C0000005 | 00000000 |

All four checks passed on the recorded host. The byte sequences are `67 66 0F A3 0F` (word) and `67 0F A3 0F` (dword), using `[BX]` as the 16-bit effective-address base and CX/ECX as the bit index. Both signed negative and positive overflow cases show that the bit-string displacement is added with 16-bit effective-address wrap before the captured memory access. This supports using the existing `c_addr.c::address_add` after computing a defined signed delta in the selected bit-memory decoder; it is not by itself proof that every possible CPU/fault case is correct.

The process uses its ordinary flat DS segment, not a controlled short-limit descriptor. These observations **do not establish segment-limit checking or segment-versus-page fault priority**, cross-element bounds, write permission, BTS/BTR/BTC behavior, 32-bit address overflow, or architectural effect on flags after faults. A separate controlled descriptor/guest observation or suitable independent hardware test remains necessary for those questions. The probe requires the target low addresses to remain inaccessible; it deliberately fails rather than interpreting an accessible address as a pass.

Replay from the repository root (this emits compiler artifacts only in the disposable build folder):

```powershell
New-Item -ItemType Directory -Force build/t473-s10/bitwrap-probe | Out-Null
Copy-Item artifacts/mvdm-audit/host-ch-scope/ccpu-bit-address16-oracle.c build/t473-s10/bitwrap-probe/probe.c -Force
Push-Location build/t473-s10/bitwrap-probe
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul && cl /nologo /TC /MT /W4 /Fe:probe.exe probe.c && probe.exe'
Pop-Location
```

No product source or guest bytes were modified.
