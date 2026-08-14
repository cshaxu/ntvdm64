# M0 T198 S132: COMMAND Current-Directory Composition Closure

S132 adds the existing finite `54:04` current-directory provider to the T198
composition only after common ingress, provider selection and COMMAND
`LAUNCH` classification. It receives the drive availability solely from the
immutable composition snapshot and uses the existing generic multi-write
mechanics for an admitted root response.

The focused MSVC/x64 adapter-only closure at
`artifacts/build/t198-s132-current-directory-composition-test-r3` compiles,
links and runs successfully. Its test proves:

- admitted C drive writes the root response at DS:SI, clears CF and resumes
  after four bytes;
- an unadmitted D drive returns the source-derived direct CF/AX-zero result
  without a mantle RAM write; and
- protected-mode `54:04` declines.

The first r1/r2 focused builds found one composition-boundary defect: the
generic multi-write executor deliberately requires a nonempty write, whereas
the provider's invalid-drive result is a valid typed CPU result with no write.
S132 now invokes the generic executor only when the transaction contains a
write; it does not weaken the generic mechanical-action ABI.

No host-drive scan, environment mutation, path search, filesystem operation,
legacy runtime gate, Bochs change or native guest run entered S132.
