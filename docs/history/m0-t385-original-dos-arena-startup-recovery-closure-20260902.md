# M0 T385 closure — Original DOS arena startup recovery

## Closed result

T385 closes with a source-backed correction to its predecessor hypothesis.
The DOS MCB allocation and SoftPC conventional-memory chain were not the
fault: the first `54:0F` allocation returned valid `ES=049F`. The actual
hazard was the original COMMAND `EndInit` grow-and-retry environment path:
the modern inherited environment required `0x270` paragraphs and overwrote
the still-live transient BOP-return instruction at `03F4:2128`.

The selected recovery (`MVDM-HOST-DIV-200`) uses the original guest's existing
`BX=0`/`copy_old_env` fallback at that unsafe capacity gate. It preserves the
initial DOS environment rather than expanding it through the active transient.
No CPU, SoftPC allocator, DOS allocator, guest binary, BaseSrv, or BOP routing
semantics were replaced.

## Verified result

The formal CPU40/x86 product links successfully. In the one fixed non-debug
startup observation, `54:0F` returns `BX=0`, the original COMMAND flow reaches
the first `54:01` command-acquisition boundary, and the product exits `0`.
The earlier `03F4:2128` illegal-op corruption does not recur.

This is not a claim that arbitrary DOS programs or native children execute.
It establishes the pure-DOS startup predecessor required before the next
original COMMAND `cmdExec32` stage-to-return package.

## Evidence

- [T385 S1–S4 environment/transient recovery ledger](../etc/evidence/m0-t385-s1-s4-command-environment-transient-recovery-001.md)
- [T384 predecessor closure](m0-t384-cpu40-illegal-operation-startup-recovery-closure-20260902.md)
