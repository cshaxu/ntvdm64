# T198 S5 P2 Core/Mantle Generic #UD Hook 001

P2 connects the P1 fixed mantle record to `BX_CPU_C::exception` behind the
new default-off `BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE` gate.  When enabled, only a
`#UD` copies scalar CPU state and a bounded instruction window into the mantle
record.  A synchronous accepted outcome can either apply the fixed register/CF
delta and resume, or set `bx_pc_system.kill_bochs_request` then return through
the original CPU-loop `longjmp` path.

No call to `SIM->quit_sim`, environment selection, selector/service decode,
adapter include, transaction, host pointer or product device path appears in
this new gate.  The uncomposed mantle bridge declines, preserving original
exception handling.

The r2 CPU5/MSVC x86 syntax probe compiled `exception.cc` with
`BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE=1` and the pinned non-x86-64 configuration.
P2 deliberately leaves the older separately gated historical composition
blocks untouched.  P3 must remove those blocks and their build/test shims;
until then the core's complete lexical boundary is not closed.
