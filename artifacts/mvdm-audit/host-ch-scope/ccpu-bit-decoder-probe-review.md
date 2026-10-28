# Real CCPU decoder: 16-bit address negative bit index

The isolated non-guest fixture derives a temporary source from the unchanged `tests/mvdm-host/ccpu_bounded_execution_fixture.c` using [derive script](ccpu-bit-decoder-probe-derive.py). Relative to that fixture it removes the separate PIC INTACK self-check, inserts two bit-instruction streams after the existing CPU/SAS initialization and normal execution/re-entry checks, and exits after the new observations instead of executing the later high-linear-page check. No product or guest source is modified. Each stream enters through the original `host_simulate`, not a copy of the address algorithm. The original CCPU decoder and bit leaf objects are linked.

The fixture uses real-mode DS=0, address-size 16, operand-size 16, BX=0, CX=FFFF, near word `[FFFE]=8000`, far word `[1FFE]=0000`, followed by `SETC` into controlled RAM and original `D6 FE` bounded exit. Observed output:

```text
BIT16 op=BT near=8000 far=0000 carry=0
BIT16 op=BTC near=8000 far=8000 carry=0
```

The independent [native x86 oracle](ccpu-bit-address16-oracle-review.md) establishes that the wrapped near address is FFFE. Consequently BT should report carry 1 with near unchanged; BTC should clear near, leave far zero and report carry 1. The results show this selected CCPU decoder chooses the far +8190 word instead. They are deliberate reproduction of an unfixed defect, not a test pass for the emulator.

The generated fixture map binds `_ccpu` to `original-ccpu386:c_main.obj`, `_BT` to `original-ccpu386:bt.obj`, and `_BTC` to `original-ccpu386:btc.obj`. Observed SHA-256: original fixture source `0324C8593240DFC4B1248625075A22576CAEA8F2367A6856DE74008A33E35627`; derived temporary fixture `2B2F5936A6DA85447AFECB215528E796B376364F37F39511A650673FE8201D5C`; selected `c_main.obj` `6FF59175D530E65B489B2C1BACB93B7C5AC459E049FE75B28EC0372AFA1C5986`, `bt.obj` `018D176132AEDCECE3CD5D9C3449F2E80218C59AA134A425ACF934F0928BFB27`, `btc.obj` `32A687FDC4B772C322B3A70A1C718C46750EC1BDE117FFD0B728D677BDD38775`; observed probe EXE `C46818E658EB36D7032A986B496DB7FAF6D4C8C9022C2119CA39501373FAC712`. The binary hash can vary on relink; bind the actual result to current object and executable hashes.

Linking follows the existing worker fixture command, including `/force:multiple` and existing duplicate-definition warnings. The test additionally links existing `ccpu_host_fixture_seams.obj` to satisfy three otherwise unresolved BaseClient calls; these seams fail closed and are not CPU instruction/address substitutes. The independent PIC check was omitted because it failed before reaching this probe. This is scoped selected-object/fixture evidence, not a complete worker, real guest, page-permission, segment-limit, fault-order, positive-overflow or release acceptance test. `host_simulate` return and the two recorded RAM/flag results are required; none proves all bit-op forms.

Before omitting the check, one run with both fixture seam objects printed `original timer IRQ did not traverse original PIC INTACK: vector=-1 eoi=0 wow=0` and exited 1 after CPU/SAS initialization. Its output existed only in the interactive tool record and the derived executable was subsequently replaced, so this is not a retained reproducible PIC result or evidence of a product regression. S13 must separately exercise the original PIC/IRQ fixture and establish the cause if it recurs; this bit probe cannot be counted as its pass.

The subsequent [same-binary readiness control](ccpu-pic-readiness-review.md) reproduced a blocked IRQ while the request remained in IRR, and delivered it when the test explicitly simulated the existing keyboard-ready boundary. This identifies the observed fixture initialization condition; it does not prove a product PIC regression or pass the separate reset/EOI acceptance.

Reproduce from repository root with the populated selected worker build and VS x86 environment:

```powershell
python artifacts/mvdm-audit/host-ch-scope/ccpu-bit-decoder-probe-derive.py
python artifacts/mvdm-audit/host-ch-scope/ccpu-bit-decoder-probe-run.py
```

The runner obtains actual compile/link commands from Ninja, redirects all new compiler/link outputs into the separate `build/t473-s10/ccpu-bit-decoder-probe` directory, checks process exit and the two **baseline-failure** observations, and stores full output there. An independent rebuild and run in that directory produced the same two observations; its EXE SHA-256 was `B4E4079CA718D54ECF29BF94E107B70543841638175866E0947977539A9D5574`, with the same derived source hash above. The checked-in derive and runner SHA-256 values are `99EE777CFABC7AC80317BF00933BF795248C7774D054C94A0592C4CAE9ED68C3` and `D0C6C81583C8AA65F3A4BBE3F2C9C22D5FD4B920F3B2A8492BE20FFB951A780D`; the runner's only subsequent change was trailing-blank-line removal. A successful future correction changes those observations and intentionally makes the baseline assertion fail; update it into a positive regression rather than treating that as a product regression.
