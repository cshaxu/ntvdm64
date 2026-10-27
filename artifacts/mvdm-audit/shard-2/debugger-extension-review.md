# S10 shard 2: external debugger-extension group

Frozen selected-graph and current source searches show the following three
original host inputs are not selected into the product; all retain pending
status until the historical external debugger tooling scope is decided.
No source identity alone proves restored behavior or justifies excluding a
requested tool.

- `ieuvddex/help.c` SHA256
  `F27DDDEE55C29F080A4679BC1653384C28147961D497D04F2A7F85C89F3F13E4`
  implements exported WinDbg extension `help`, initializes caller-supplied
  debugger API pointers and prints selector help then delegates to the
  architecture-specific `helpp`. Original `ieuvddex/ntvdm.def` exports help;
  `ieuvddex/sources:31-47` builds an extension DLL named ntvdm, not the VDM
  executable. No product/test references to ieuvddex were located in the
  searched CMake and test trees. Existing product startup is not evidence for
  this interactive command or processor variants.
- `ieuvddex/trace.c` SHA256
  `B1F435775188A93D9BDD20CFDF4DE08E0194D6AA86B695F092336E441A6B9143`
  supports the debugger's DPMI trace command. `procdep.c:548` enters trace;
  the helper reads debuggee symbols such as `ntvdm!DpmiTraceTable`,
  `bDpmiTraceOn`, count/index/capacity via the supplied debugger expression
  and process-memory APIs, checks count/index bounds, then decodes entries.
  It depends on the debuggee's original trace instrumentation and symbols;
  the existence of current CPU40 or DPMI functionality does not establish
  this debugger extension is runnable. Neither tracing output nor diagnostic
  write/control behavior was exercised in this review.
- `softpc.new/debugger/vdmdebug.c` SHA256
  `38A25FA9784C466C56EE7024B33DFB7E3A419E6E415AEDBB92AF32F61FA24C77`
  is an independent console program (`debugger/sources:35-46`), creating a
  debugger named pipe and Yoda event then serving a SoftPC debugging shell.
  The original executable is not in the selected runtime graph. No test was
  found invoking this filename or ieuvddex in the searched `tests` tree.
  Absence of those test references is bounded search evidence, not proof that
  a debugger protocol is unnecessary or equivalent to existing observers.

The source paths and hashes identify frozen mirrored files, not verified live
external debugger binaries. No interactive debugging, guest modification or
runtime test occurred; comparisons and command paths are source-level only.
