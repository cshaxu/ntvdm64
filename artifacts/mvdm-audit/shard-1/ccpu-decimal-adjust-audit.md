# CCPU decimal-adjust pair: actual selected bodies and native oracle

The frozen `aaa.c` and `aas.c` source hashes exactly match the OpenNT copy:
AAA `43583535E19A181D7A4C665C5FBA5C00109EB1079B65D462F2B8A3D92CE2C052`;
AAS `F58060B5F0732CA174B1124B2622AAAFA9D7341774CD71427509F2548ACE4672`.
`c_main.c` decodes opcode 0x37 to AAA and 0x3F to AAS; the selected worker
build compiles their objects into `original-ccpu386.lib`, and the snapshot map
attributes `_AAA` to `aaa.obj` and `_AAS` to `aas.obj`. This proves static
selection and call targets, not end-to-end guest dispatch or prefix handling.

An offline 32-bit native C fixture directly linked copies of the actual
selected MSVC-built objects and provided only CCPU register pointer arrays
and flag storage. The source `aaa-aas-native-oracle.c` executes genuine native
32-bit `AAA`/`AAS` on the host and checks the defined AX, AF and CF results
against the selected body. It exhausts 65,536 initial AX values and both
initial AF and CF for both operations (524,288 cases), and returned:

```text
PASS AAA/AAS: 524288 AX+AF+CF inputs against native 32-bit CPU; defined AX CF AF only
```

Host reports `AMD64 Family 26 Model 36 Stepping 0, AuthenticAMD`. The harness
uses x86 process execution on that host, not a guest. OF, SF, ZF and PF are
architecturally undefined for these operations and deliberately were not
compared; this selected body writes its configured undefined values. It does
not cover instruction decode, fault/LOCK prefixes, multithreaded CPU state or
another hardware vendor.

The direct objects were copied with only their `.drectve` section removed to
avoid MSVC default-library linkage while linking the offline MinGW fixture;
no executable section was edited. Original object hashes are AAA
`B1BFB3DA27803D66CC8DE3BFB0329660A6DF64F915DD8483E3B2BEBCA484E3AE`
and AAS `E22E475F78DF528A105C4D8D0E2857D67867825FF9DFE22B022FECEE65D830EC`;
their stripped copies hash respectively
`1AC1351BD289CAF680CB5C86984B86ED0D54184B69852152D31E6E959A167DE9` and
`1D3322B664EA8BF9A7F70BE2EA63C07E235C3DB5A9E58227F1B3BFB209A07A81`.
Fixture exe SHA-256
`438A88FE8BF78CE9B3E6F471AB30047E0327342300A297BDC905292DC2EA9B25`.
Fixture source SHA-256
`294F20D1B1973640CB242B1E4638CF7FE2C1F6C08372BB3741491DBCEF9DACF8`.
No product or guest process was started, no NTSRV was contacted, and no
runtime binary was changed. Both paths remain pending product-level decode
and lifecycle evidence rather than being marked fully covered by body tests.
