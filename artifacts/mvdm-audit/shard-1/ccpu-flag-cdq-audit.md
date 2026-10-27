# CCPU flag and sign-extension object differential

Frozen `stc.c`, `lahf.c`, `cdq.c` are byte-identical to OpenNT. Selected
worker map links `_STC`, `_LAHF`, `_CDQ` from corresponding CCPU objects.
`c_main.c` dispatches F9 to STC, 9F to LAHF and 99 to CDQ only when the
operand size is 32 bits (the 16-bit branch dispatches CWD). Static call-chain
identity is not a guest decode test.

Offline `flag-cdq-native-oracle.c` links actual selected object copies,
supplying CCPU flag/register globals; it compares real 32-bit native x86 on
AMD64 Family 26 Model 36 Stepping 0, AuthenticAMD. It checks:

- STC: all 64 initial combinations of CF/PF/AF/ZF/SF/OF, including all six
  resulting flags.
- LAHF: the same 64 flag combinations crossed with all 256 AL byte values,
  comparing AX and the six flags, 16,384 cases.
- CDQ: 100,000 deterministic EAX cases including 0, 7fffffff, 80000000 and
  ffffffff, comparing EAX, EDX and the six flags against native `cltd`.

Total output: `PASS LAHF/STC/CDQ 116448 cases; selected flags and register
outputs match native 32-bit CPU`. Sampling is not full 32-bit CDQ input
coverage. Other flags, unrelated registers, actual guest mode/operand-size
selection, prefixes/faults and lifecycle are not covered. Both source and
body matches are insufficient to classify full integrated support. The
fixture did not start NTSRV/NTVDM or execute/alter guest.

Selected object SHA-256: STC
`959F3D2D8EC5A538EC7D0E794AEBA967BF9C6A1F0547BC76E2E23EFDA103E1C3`, LAHF
`8FD0B9E26855DAB6D13E82CE57BA15368378B06B8BFC670383C28AE001641D2C`, CDQ
`06FF602DF1F473BB1FF8856A8881ECDB242A5604CE92DA31EE04D7AA8B0B6AA3`.
Only `.drectve` is removed from link copies. Fixture executable SHA-256
`F94346251C7FAD735362F7DB4CBA1331D1016E8B39760606B7FCB47F587D0128`, source
`C0425878779846AB23D0512C91BA0747F005B8CF9499B77A40BDE501314945AA`.
