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

## S10 guest-probe handoff: exact call paths and remaining proof

In `c_main.c` opcode F9 calls STC, 9F calls LAHF, and 99 calls CWD for USE16 or CDQ for USE32; none of these three is an inline assignment. At adjacent opcode 98, USE16 calls CBW and USE32 calls CWDE. `_STC`, `_LAHF`, `_CDQ`, `_CBW`, `_CWDE`, `_CWD` appear in current worker map from corresponding original CCPU objects. Source and map prove dispatch design and linkage, not that a packaged DOS guest exercised that decoder.

- STC: inputs are CF plus all other CPU flags; writes CF=1 and does not write the other source flag entries. Direct-object fixture covers all 64 combinations of CF/PF/AF/ZF/SF/OF, but not other production flag globals. Guest can check both incoming CF states and preserved observable flags/register sentinels.
- LAHF: reads SF/ZF/AF/PF/CF, writes AH = bit7/6/4/2/0 with bit1=1 and bits5/3=0; AL and upper EAX remain intact; flags unchanged. Object test covers all 64 combinations of six selected flags and all 256 AL values, with a test-local pointer for AH; it does not establish production CCPU_BR alias wiring or upper EAX. Guest should explicitly preserve an EAX high-word sentinel and use SAHF or arithmetic to produce the inputs; distinguish writeback to the guest register from the test's local pointer.
- CDQ: USE32 sign-extends EAX bit31 into EDX=0 or FFFFFFFF and keeps EAX/flags. Object test uses 100000 seeded and boundary EAX values, six flags, local register arrays. Since source depends only on the sign bit, the two sign classes are the finite functional partition, but actual production register mapping and guest decode still need direct evidence. Test EAX 00000000/7FFFFFFF/80000000/FFFFFFFF with distinct EDX, all observable flags preserved.

For adjacent cross-shard instructions, CBW preserves upper EAX and sign-extends AL into AX; CWDE replaces all EAX by sign-extended AX; CWD replaces only DX preserving EDX upper 16; none modifies arithmetic flags. A real-mode 16-bit guest should exercise opcode 98/99 both unprefixed and with 66h and verify sign boundaries (AL 00/7F/80/FF; AX 0000/7FFF/8000/FFFF), upper-word sentinels and unchanged flags. CBW/CWD/CWDE are owned by other shards: this file is guidance only, no change to their ledger.

A bounded actual guest success would close the decisive decode/alias/publication gap for tested context; decide full classification on the defined instruction contract and its realistic independent preconditions, not demand arbitrary failure or reentry cases for register-only operations. Still explicitly record untested protected-mode/default-size and repeated-prefix context rather than claiming them covered by a real-mode run. Current packaged worker/map identity must match the test; source equality or old object proof alone cannot establish that identity. No guest was started in this handoff.

## Independent S10 leaf classification review

Rechecked frozen/current original source hashes for STC, LAHF and CDQ and package-c NTVDM SHA256 `01A5B459072CA23F49AA43285580BD9017395C936F9DA8B2F18EE7223A52D1F6`. Read the actual COM probe, positive/negative guest record, and PE object/decoder relocation evidence. The tested x86 package executes the complete selected leaf bodies and the originally mapped canonical flags/register aliases; negative control rejects an intentionally wrong guest expectation and a wrong-image control rejects WOW32.DLL. The earlier direct-object x86 comparison covers independent arithmetic partitions. Taken together, these support 100% restoration of each unchanged **selected leaf file contract**, not all ISA or decoder behavior.

Real-mode default 16-bit and 66h use are covered; protected-mode default-size, invalid/repeated prefixes and general exception/worker lifecycle remain separate decoder and runtime obligations. This review did not re-run the guest or modify source/guest/binaries.
