# S11: Win386 mouse and V86 dispatch regression

The owner reports that `O:/w31/PATCH/WIN386.CMD`, a DOS-mode Windows 3.1
enhanced launch using V7VGA and mouse.drv, stops responding shortly after
physical mouse movement. The preexisting four-case startup/exit witness did
not exercise sustained mouse input, so it cannot disprove this report. The
original installation was not modified or rebuilt.

The controlled test uses a test-owned copy of that installation, matching
the V7VGA setup, an isolated runtime package and private desktop. Its
test-only hook delivers 600 relative motions over roughly six seconds to the
actual frontend input sink, then eight further motions; each sink
acknowledgment and each fresh frame is separately observed. This starts at
the component input sink and does not validate host Raw Input, physical mouse
or a user-owned live session. A lack of fresh frames is not by itself proof
of permanent CPU deadlock.

| Control | Result after burst | Evidence under build/t473-s11 |
| --- | --- | --- |
| Prior graph-owner ten-component package | 8/8 fresh frames | win386-mouse-burst-older |
| Previously published S11 CPU-monitor r10 | 0/8 | win386-mouse-burst-mid |
| Current pre-fix package | 0/8 twice | win386-mouse-burst-current, win386-mouse-burst-repeat |
| Current package, diagnostic disabled entire protected monitor | 8/8 | win386-mouse-burst-no-monitor |
| Current package, diagnostic disabled only monitor CLI/STI | 8/8 | win386-mouse-burst-no-if |
| Current package, diagnostic disabled STI immediate IRQ only | 0/8 | win386-mouse-burst-no-immediate-irq |
| Current package, diagnostic omitted setIF only | 0/8 | win386-mouse-burst-no-setif |
| Current package, diagnostic excludes V86 from protected dispatch | 8/8 | win386-mouse-burst-no-v86 |
| Same source change compiled as product candidate | 8/8 | v86guard-mouse-burst |

The no-monitor and no-IF test images are diagnostics, not acceptable product
fixes. A separate current run waiting ten seconds after burst then moving
away from the edge regained fresh frames: this documents a long response
stall under that workload, not a proof that every reported hang recovers.
A quiesced sample also observed CPU progress between V86 and protected VMM
contexts. One-shot controls establish a strong attribution to this tested
path, not a complete account of every physical mouse failure.

Original `ntos/ke/x86/trap.asm:_KiTrap0D` dispatches V86 traps to
`Ki386DispatchOpcodeV86` and protected-mode traps separately. The selected
CCPU `host_exint_hook` previously tested CPL but did not exclude VM=1, so a
V86 #GP could be consumed by the newly introduced protected-mode decoder.
Real CCPU cases 102/103 set V86 and arrange matching selector-table bytes:
with the prior actual hook, both CLI/STI are incorrectly handled and CLI
changes IF and VDM state; with the candidate both are passed to the existing
fault owner with EIP, flags and VDM state unchanged. Existing protected-mode
controls remain in the 93-case suite. The implementation adds only `!getVM()`
to that protected decoder guard and retains the preexisting fallback.

The remaining DPMI mapped-code and original exception-delivery boundaries
are independent and remain open; this change does not claim their resolution.

The selected 93-case CCPU suite, Full 16-gate suite, Winmine/SOL/WRITE and four SOL Options rounds, Win3.1 standard/enhanced × DOSONLY 0/1 and the high-rate mouse candidate witness passed. Original and isolated Win3.1 trees retained the 86 monitored binary hashes. All ten tested components were published to both destinations with 20 matching hashes. [Evidence and test-only probe sources](s11-win386-v86-mouse-evidence/). Physical mouse/Raw Input is not covered by the injected sink probe; owner validation remains distinct.
