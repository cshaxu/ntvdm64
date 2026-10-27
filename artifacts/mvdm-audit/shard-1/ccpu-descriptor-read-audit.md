# CCPU SLDT, SMSW, SGDT object boundary

Frozen source reconciliation SHA256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. `sldt.c`, `smsw.c`, `sgdt.c` are identical to the frozen selected original. `c_main.c` dispatches 0F00/0 to SLDT after PE/VM validation and 0F01/4 to SMSW; 0F01/0 SGDT rejects register operand and chooses SGDT16 or SGDT32 by operand size. These are source dispatch paths, not evidence of guest execution. The original SGDT16 comment itself questions the upper base-byte behavior; no replacement or guest semantics are inferred here.

`ccpu-descriptor-read-object-fixture.c` links copies of selected objects with only .drectve removed and test-local CCPU_SAR, CCPU_CR and CCPU_STAR storage. It checks 65536 seeded values each for SLDT and SMSW and 100000 seeded GDT base / limit values for each SGDT16 and SGDT32, 331072 total. All match the source-defined reads and masks. This is a self-consistency check against expected source representation, not a hardware oracle: especially, it does not establish architectural 16-bit SGDT serialization or production storage initialization. It uses i686-w64-mingw32-gcc -O2 and temporary object copies.

Selected object SHA256: sldt B1D8FDF61DB7704C5D02EB51DA69132E6C7314F7B60D4AA318C8BE3ED80175A9; smsw A6DB95814D11F47355E79C958E173200317E474EC3FDF3181282B06514F4AE42; sgdt 8AE34F0B1FA70EE59580D552EB7E628C5A0138481B368CB7BA651C33A7707EC2. Fixture SHA256 BBF36A04C0532D2DED02FF299454E8595584D2FBE79395DC3618CA4B64ABE677.

Keep pending: current deployed executable identity, real guest instruction bytes and selected decoder, 16/32 SGDT memory layout including upper byte, selector/mode gating, page fault/no partial writeback, actual CCPU backing and lifecycle. No product/guest modified.
