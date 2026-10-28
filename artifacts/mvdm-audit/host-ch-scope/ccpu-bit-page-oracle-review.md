# Native x86 bit index and page protection oracle

The standalone [oracle source](ccpu-bit-page-oracle.c), SHA-256 `5CB45B0A1289DA1F93678B6A3F3294312B4B97A3A8F089FF147BD43CB5FE0E21`, executes actual x86 instructions in an independent 32-bit Windows process against three `VirtualAlloc` pages. The independently rebuilt replay binary SHA-256 was `2CE06DB51B53773FA723DC4BF43701EF018BEC78E29BD235B6AB0C42C7DE3514`. The run returned zero with:

```text
negative carry=1 near=00000000 base=00000001
negative_guard code=c0000005 access=1 delta=-4
readonly_bt code=00000000 carry=1 value=00000001
readonly_btc code=c0000005 access=1 delta=0 value=00000001
```

For 32-bit operand/address BTC with ECX=-1, the modified dword was four bytes before the base and CF was the old high bit. Making that preceding page inaccessible caused `STATUS_ACCESS_VIOLATION` at base-4 and the recorded Windows access kind was 1 (write); after restoring access both dwords retained their pre-instruction values. On a read-only base page, BT read and returned carry, while BTC faulted with write kind 1 at the base and left the dword unchanged. Here the bit operand lies in the preceding protected page: this is a negative-index **page boundary** case, not proof for an individual operand spanning two pages. RMW hardware/Windows reports a write access even for the no-access page; do not infer microarchitectural read ordering from this field.

The fixture is a native hardware comparator, not a test of the CCPU decoder or actual guest. It does not prove CPU flag restoration after exceptions, segment limit/fault precedence, concurrency/LOCK, 16-bit operands, all four bit opcodes or positive large indexes. Independent decoder tests must compare both outcomes and fault/commit state.

Replay from the repository root, using a disposable build directory:

```powershell
New-Item -ItemType Directory -Force build/t473-s10/bitwrap-probe | Out-Null
Copy-Item artifacts/mvdm-audit/host-ch-scope/ccpu-bit-page-oracle.c build/t473-s10/bitwrap-probe/probe.c -Force
Push-Location build/t473-s10/bitwrap-probe
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul && cl /nologo /TC /MT /W4 /Fe:probe.exe probe.c && probe.exe'
Pop-Location
```

No product or guest source or image was changed.
