# Independent WOW32 DLL-entry return ABI review

The mismatch is real in the selected x86 bridge: generator declares `extern int __stdcall W32DllInitialize`, but wow32.h and wow32.c declare/define `BOOLEAN W32DllInitialize`; the witness uses the actual Windows SDK and compile-time checks `sizeof(BOOLEAN)==1`. A BOOLEAN return communicates its value in AL, whereas the generated caller consumes EAX as an int. Both use the same stdcall argument cleanup; this is a return-width mismatch, not a stack-argument or BOOL/BOOLEAN naming equivalence. `DllMain` itself must still return a 32-bit BOOL; reading the provider as unsigned char then widening to int preserves that contract.

The original body explicitly returns FALSE if either `CreateSmallHeap` or `HeapCreate` fails. A false low byte does not require the remainder of EAX to be zero by the BOOLEAN return contract. Thus a bridge that propagates the whole register can convert a valid false into a nonzero DLL attach result. The controlled witness is constructed to force that situation, not an observation that the compiled original body ever leaves those exact upper bits on either failure.

The `r2/result.json` reports actual selected `wow32-dll-entry-bridge.obj` against a controlled naked stdcall provider returning EAX A55A0000 / A55A0001: actual false attach is A55A0000; a candidate with only the provider declaration changed to unsigned char returns 00000000; nonzero attach remains nonzero and two explicitly tested detach cleanups execute. Both build and witness runs exit zero. This supports a real contract violation and a minimal candidate, for the controlled x86 object/inputs only. The provider is deliberately substituted, and `DllMain` is called as a function: it does not demonstrate actual loader behavior, an actual original heap failure, upper EAX values from the real compiler output, guest reproduction, or safety of the full runtime fix. It does not independently prove the selected object is present in a published PE.

No product or guest modification and no additional execution were performed in this review. The following input identities are independently hashed from disk:

- `tools/build/New-T404S5Wow32ProviderNinja.ps1`: `d8168ffd8919514c446f28511039f2b1122e2abf4f41b73f06009f15779c6962`
- `src/mvdm/wow32/wow32.h`: `06148a93a63faf7fc5480e4c9f80c98ddfe6e2a0a782eafe6a6fd1ccdd46de27`
- `src/mvdm/wow32/wow32.c`: `42ac4813868d734b26fedebca4841b6e1318ecac4be0d700d3d0712d47231eac`
- `artifacts/mvdm-audit/dll-entry-return-probe.c`: `b019ada4c13a7c5eb742ed974c564d10cedae3ae2317cbc92bb3f6ef5def3f3d`
- `artifacts/mvdm-audit/verify-dll-entry-return.ps1`: `9c7f88211995891473f0198f90d5a595f389f201cfe3506786332a67bb54add1`
- `build/t473-s10/dll-entry-return-r2/result.json`: `1f90d3c6869ca92093db685b7b4d5faf09281cf856f0090d04f0ecb7af7d098f`
