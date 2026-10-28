# Combined real PIC readiness and CCPU bit-decoder observation

This fixture keeps the original `ccpu_bounded_execution_fixture.c` PIC setup and assertion intact. The [derivation script](ccpu-bit-ready-probe-derive.py) inserts a **test-only** `DelayIrqLine=0` immediately before requesting IRQ0, representing the existing SPCKBD/IVT-ready boundary explained in [the readiness A/B](ccpu-pic-readiness-review.md), and prints the actual INTACK result. It also inserts the two original bit decoder streams after the existing CPU startup/re-entry checks, without replacing original CPU or memory algorithms. The remaining original high-linear-page call is left in place; this record does not independently prove its completion or all fixture checks. No production or guest source is changed.

The [runner](ccpu-bit-ready-probe-run.py) reconstructs the selected worker compiler/link commands, uses the already established BaseClient fail-closed test seams, and preserves the original fixture's `/force:multiple` link setting and warnings. Map confirms `_ica_intack` from `original-softpc-system:ica.obj`, `_ccpu` from `original-ccpu386:c_main.obj`, `_BT` and `_BTC` from their original CCPU leaf objects; `DelayIrqLine` comes from `nt_eoi.obj`. An unmodified PIC assertion failure or missing observation makes the runner fail; it does not suppress or skip that self-check.

Recorded output (process exit 0):

```text
PIC16 vector=8 eoi=0 wow=1
BIT16 op=BT near=8000 far=0000 carry=0
BIT16 op=BTC near=8000 far=8000 carry=0
```

PIC passes in the explicitly prepared test state. The BT/BTC observations reproduce the known unfixed defect: the native address-size-16 oracle selects FFFE for BX=0/CX=-1, so its expected BT carry is 1 and its expected BTC result is near=0000, far=0000, carry=1. These baseline-failure observations are deliberately asserted by the runner to keep the diagnosis replayable; when the production fix is made, convert the test to assert native semantics instead of treating changed baseline as a regression.

Recorded SHA-256: derivation script `6A0A35B0DAD590199C210B1D372C9A09C380761A9C2865EEFD316B29FB8D6855`; runner `C0D24B5DBE98F4648501998CA21A546FBADA72CF5739C76128656D9F1CEEB27D`; temporary derived source `2F31CB63D9579957FD47D6223C478B1A1B450A958D6D8AA77C8AC600654CA35D`; selected `c_main.obj` `6FF59175D530E65B489B2C1BACB93B7C5AC459E049FE75B28EC0372AFA1C5986`; independently rebuilt EXE `B2EAED7B26005ABD32C646FCB863A80739B782F999979ADBB16BB29FE3ACA3E5`. The runner checks that every expected command fragment exists before redirecting output, preventing a changed Ninja command from silently writing the original target. Full link and runtime output and map live under `build/t473-s10/ccpu-bit-ready-probe`; regenerated executable hashes may vary.

Replay from repository root, with the selected x86 worker libraries and VS tools available:

```powershell
python artifacts/mvdm-audit/host-ch-scope/ccpu-bit-ready-probe-derive.py
python artifacts/mvdm-audit/host-ch-scope/ccpu-bit-ready-probe-run.py
```

This is not an installed product or guest test, a production readiness bypass, or proof of segment limits, cross-page fault priority, other opcodes, operand/address sizes or all CPU state. The PIC precondition is an explicit test control only.
