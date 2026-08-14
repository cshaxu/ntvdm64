# M0 T198 S123: Zero-Byte Result Transaction Fix

S122 proves the empty AUTOEXEC file is open at `4E534005`.  The read provider
already creates the source-derived resume result `AX=0`, CF clear and a valid
zero-byte bulk transaction.  The composition then incorrectly tries to run an
empty mantle WRITE; the mantle mechanical contract rejects that intentionally
range-less action and the BOP declines.

S123 changes the common composition seam: a zero-range, zero-payload WRITE is
a result-only transaction and skips mantle execution.  Its typed CPU result
still crosses the ordinary outcome route.  No service selector, provider,
guest byte transfer, host capability, Bochs code or machine behavior changes.

The changed source compiles CPU5/x64 in
`artifacts/build/t198-s123-zero-byte-result-compile-r1`.
