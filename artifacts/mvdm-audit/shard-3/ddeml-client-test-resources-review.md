# S10 shard 3: DDEML client test resource owner refinement

This read-only follow-up resolves the **file roles** of `wow16/ddeml/tests/src/client/conv.ico`, `list.ico`, `ddemlcl.dlg`, all pending in the earlier `parallel-review-current.md`. No guest/sample rebuild, test execution or shared-ledger change.

| File | Original identity and actual consumer | Recommendation |
|---|---|---|
| `conv.ico` | SHA-256 `01A39B852198DF2B119FE7123B8B149025E74647B15699AF3043FA9CB4C106BC`; `ddemlcl.rc:12` binds IDCONV to it. | Historical Win16 test UI resource, not product DDEML implementation. |
| `list.ico` | SHA-256 `40273A07C5926E76B4354C0C057B7A0E22F4CBA30C5D4297DD244C696707F0D9`; `ddemlcl.rc:13` binds IDLIST. | Same test-resource role. |
| `ddemlcl.dlg` | SHA-256 `69CA1F0FBA6F14116B2262ADED3A4709066347B14EEEE95D531777FC20776511`; `ddemlcl.rc:93` includes it. | Historical test dialog definitions, not runtime provider. |

`client/makefile.dos:18-19` explicitly compiles `ddemlcl.rc` with WIN16; lines 42-44 link the sample and attach its resource. This is an actual historical Win16 test-build chain for all three. The separate `client/sources` native WIN32 sample names `client.rc` and `client.res`; this checkout does not contain `client.rc`, and no inspected recipe proves it is generated or equivalent to `ddemlcl.rc`. That remains an independent native-test build gap, but it does not make the three files' demonstrated Win16 test-resource roles unknown. Their original hashes match both inventories as recorded in `parallel-review-current.md`.

Recommend classifying these three as **test-only resource/build inputs** for product-source accounting, retaining the files and explicitly preserving DDEML feature and test restoration obligations. This is not evidence that DDEML callbacks, message lifecycle, or test programs work; the native sample's missing resource and unexecuted 16-bit test remain unverified.

Replay:

```powershell
Get-FileHash src/mvdm/wow16/ddeml/tests/src/client/conv.ico,src/mvdm/wow16/ddeml/tests/src/client/list.ico,src/mvdm/wow16/ddeml/tests/src/client/ddemlcl.dlg -Algorithm SHA256
rg -n -i 'conv\.ico|list\.ico|ddemlcl\.dlg' src/mvdm/wow16/ddeml/tests/src/client/ddemlcl.rc
Get-Content src/mvdm/wow16/ddeml/tests/src/client/makefile.dos
Get-Content src/mvdm/wow16/ddeml/tests/src/client/sources
Get-ChildItem src/mvdm/wow16/ddeml/tests/src/client -Filter client.rc
```
