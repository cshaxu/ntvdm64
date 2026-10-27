# S10 DEC native oracle contract and static review

The shared producer is `tests/observation/ccpu_alu_oracle.c`. Frozen path
partition assigns `softpc.new/base/ccpu386/dec.c` to shard 2; ADC, AND and INC
are shard 3. The producer requires MSVC x86 and executes actual host `dec al`,
`dec ax` or `dec eax`. The `UNARY` macro loads lhs, uses `bt carry,0` to set
input CF immediately before DEC, captures EFLAGS with `pushfd`, and preserves
that CF. Other arithmetic flags compared are written by DEC. Result is masked
to operand width; RHS is a record-only value and is not a DEC operand.

Each 14-byte record is lhs/rhs/result as little-endian u32 followed by flags
as little-endian u16. DEC flags use `0x08d5` (CF/PF/AF/ZF/SF/OF). Logical
operations mask undefined host AF with `0x08c5`; that separate behavior does
not change DEC's mask. Output names are `DEC{0,1}_{8,16,32}.BIN`, where the
number is initial CF. Eight-bit files hold 65,536 ordered pairs (917,504 bytes);
16/32-bit files hold 14×14 boundary pairs plus 1,000 deterministic LCG pairs
(1,196 records, 16,744 bytes), with each value masked to width. Seed is
`0x12345678`, recurrence `1664525*state+1013904223` modulo 2^32, two updates per
pair. The output directory must exist; open, write and close failures return
nonzero. Each file restarts the same sequence.

The main-session receipt `ccpu-alu-records.json` records the six DEC file lengths and hashes, and `ccpu-alu-guest.json` records a real guest pass of all selected forms and both CF inputs. The independent negative data control mutates ADD8 rather than DEC. The selected-image record matches the full DEC leaf and decoder references; this shard independently rechecked source, object, decoder and package hashes. The oracle does not prove decoder-to-image identity, upper-register
preservation, other flags, protected mode, prefixes, faults or guest execution.
No product code or original guest was changed.

Independent review: the current producer SHA-256 is `CFB5909506354617AF76E53F691078006C7309044E3275EE364D00424A570B35`. DEC source is byte-identical to both original comparisons. Width-dependent 16/32 domains remain sampled, and 100% applies only to the complete selected leaf implementation rather than all possible test inputs or the decoder.
