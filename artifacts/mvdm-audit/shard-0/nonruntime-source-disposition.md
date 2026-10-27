# Three original non-runtime/profile source dispositions

All three match frozen source hashes and are absent from four checked Ninja
input closures: NTVDM, WOW32, VDMREDIR and package-scoped NTVDM.

- suballoc/tsa.c: original SOURCES builds suballoc.c into the library and
  declares UMTEST=tsa. This standalone test main is excluded as runtime input;
  original allocator, XMS and DPMI behavior remain separately in scope.
- softpc.new/convert/convert.c: original PROGRAM convert opens named files
  in place, changes line endings and removes terminal control-Z. It is source
  maintenance tooling, excluded as runtime input. No guest/firmware algorithm
  is excluded through this decision.
- xms.486/i386/xmsmem86.c: the original i386 callback alternative directly
  treats supplied guest addresses as host addresses for native memory and
  move calls. The selected graph composes xmsmemr.c and bounded CPU40 movement
  instead. Exclude this competing profile input; selected XMS commit, free,
  move and failure behavior remain separately in scope.

The source files remain unchanged. No runtime feature is declared complete
through these exclusions. No original tool or guest was executed or modified.
