# M0 T310 S30 — host-VDD binding closure

S30 closes the selected original SoftPC host-VDD binding cluster for Win32/x86
CCPU40.  The original `nt_vdd.c` installation, IRQ and DMA service bodies are
in the formal source closure; no hand-written VDD provider replaces them.

The only reached fixed-width host-object bridge is the previously admitted
SFT/JFT synchronous shadow.  Its identity facade delegates to the session's
single host-resource mapping-manager instance, preserving the project-wide
one-mapping-manager rule.

The selected CCPU40 candidate and forced-link closure completed with 381
edges, and the existing x86 VDD shadow fixture completed successfully.  S30
does not enable arbitrary VDD products, MONITOR/kernel VDM, Bochs fallback, or
integrated device behavior.

Evidence: [S30 P1](../etc/evidence/m0-t310-s30-p1-host-vdd-binding-closure-001.md).
