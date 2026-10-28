# Real CCPU bit decoder: address16 matrix

The [derive](ccpu-bit-matrix-derive.py) and [runner](ccpu-bit-matrix-run.py) build an isolated fixture from the real `ccpu_bounded_execution_fixture.c` and selected worker libraries. The original PIC self-check remains and is not suppressed: the test explicitly sets `DelayIrqLine=0` before requesting IRQ0, as justified by the [PIC readiness A/B](ccpu-pic-readiness-review.md). Each of seven cases runs in a separate process with the same EXE, checks the original PIC result, emits one bit observation, and returns zero only if fixture execution reaches the observation. The original fixture seam for `host_exint_hook` is replaced only in a temporary copy by the same setjmp observation technique used in `ccpu_stack_transition_test.c`; a real exception is recorded, not swallowed as a success. Production and guest sources/images are unchanged.

[Captured result](ccpu-bit-matrix-observation.json) binds the seven observations and EXE. `fault=-1` means the test exception hook was not called; it is not a Windows exception code. Each case reports `PIC16 vector=8 eoi=0 wow=1`.

| Case | Actual instruction and initial state | CCPU observation | Native comparison |
| --- | --- | --- | --- |
| 0 | BT word `[BX]`, address16; BX=0, CX=-1; `[FFFE]=8000`, `[1FFE]=0` | CF=0, near unchanged | [native address16 oracle](ccpu-bit-address16-oracle-review.md) selects FFFE; its old bit is 1, so expected CF=1. |
| 1 | BT dword `[BX]`, address16; BX=0, ECX=-1; `[FFFC]=80000000` | CF=1, near unchanged | Native selects FFFC and bit31=1; CF agrees, but CF alone cannot establish address identity. |
| 2 | BT word `[BX]`, address16; BX=FFFE, CX=16; `[0000]=1`, `[10000]=0` | CF=0 | Native wraps to 0000; expected CF=1. |
| 3 | BT BX,CX; BX=8000, CX=-1 | CF=1 | Register-destination modulo-width control; no memory-address conclusion. |
| 4 | BT word `[BX]`, imm8=FF; BX=2000, `[2000]=8000` | CF=1 | Immediate-index control; it does not establish all immediate or fault forms. |
| 5 | BTC dword `[BX]`, address16; BX=0, ECX=-1; `[FFFC]=80000000` | CF=1, near dword stays 80000000 | Native address16 selects FFFC; together with the independently observed native dword BTC bit semantics, expected near dword becomes zero. Actual CCPU write target is **not identified** by this fixture. |
| 6 | BTC word `[BX]`, address16; BX=FFFE, CX=16; `[0000]=1`, `[10000]=0` | CF=0, `[0000]=1`, `[10000]=1` | Native wraps to 0000; expected carry 1 and `[0000]=0`. This directly identifies the CCPU write beyond the 16-bit wrap boundary. |

For case 5, combining native address16 selection and native BTC behavior is an inference from two independent probes, not a single direct native address16/BTC witness. An apparently correct case-1 carry likewise must not be mistaken for a validated address. Other registers, high positive/negative extremes, immediate BTC, operand32 positive overflow, protection, segment-limit ordering and exception/flag atomicity remain untested here. The fixture is not an installed-product or guest execution test.

At this run, derived fixture SHA-256 `79BFC4A85C3172C60A3FF943FA256219AA09191A0BE0EFDC644348D266006C1E`, derived seam `F635DA8623BCC86E150DEC3D6B21C45293C1B454622EDC515C89A2E859F1495F`, real selected decoder `c_main.obj` `6FF59175D530E65B489B2C1BACB93B7C5AC459E049FE75B28EC0372AFA1C5986`, EXE `A992DF31CA9F4416F71272A5176C50E679C381C1580366E62DA88383AF7010D0`. Actual map, per-case captures and build output are in `build/t473-s10/ccpu-bit-matrix-probe`. Link inherits `/force:multiple` and existing duplicate-definition warnings. The process-level 30-second diagnostic timeout bounds only the isolated test harness, not product behavior.

An independent rebuild and replay produced identical seven case results, with EXE SHA-256 `F09176AA01AA8CF2C6474AF6BC86102ED223F34E1014F282042F407EC2055577`. The runner checks all expected compiler and linker command fragments before redirecting outputs, preventing an unexpected Ninja command from writing the original cached targets.

Replay, with the selected x86 worker tree and VS x86 tools populated:

```powershell
python artifacts/mvdm-audit/host-ch-scope/ccpu-bit-matrix-derive.py
python artifacts/mvdm-audit/host-ch-scope/ccpu-bit-matrix-run.py
```

The runner currently records baseline outputs and requires every case to reach its result and original PIC assertion; it does not declare unfixed CCPU results a semantic pass. Convert it to compare positive native expectations when implementing the repair.
