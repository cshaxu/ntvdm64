# CCPU BTC and RCL source owners

The original `btc.c` and `rcl.c` match both selected original editions, are
selected in the current CCPU40 worker graph, and their object bodies bind to
the released NTVDM code image. The accompanying observer records the hashes,
build edges and object-to-image correspondence. This supports a partial source
role for each file, not complete instruction semantics.

BTC's body toggles the selected bit and sets carry, while signed memory-index
address formation is performed by the separate original operand path. The
[signed-index differential](../ccpu-header-macro-review.md#demonstrated-signed-memory-bit-index-difference)
demonstrates a fault in that complete path, not proof that this leaf alone is
the cause. S11 owns word/dword and register/immediate addressing, wrapping,
flags and fault ordering. RCL's original count masking and rotate-through-carry
also need actual guest verification across sizes, zero and boundary counts,
flags, permissions and exceptions. Identical source text and a linked body do
not establish these runtime contracts; neither feature is excluded.

Replay `python artifacts/mvdm-audit/shard-0/ccpu-bit-rotate-image-observer.py`.
No product or guest image was modified or executed by this source observation.
