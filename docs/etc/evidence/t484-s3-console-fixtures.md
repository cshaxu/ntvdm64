# T484 S3 P18: Console and font producers

Three old generators are replaced: T361 font/toggle, T388 Console contract
and T310 host input. T388 and T310 selected the same fixture; there is one
native target, not two maintained graphs. The font target uses current
session/host-context owners on x86/x64. Console targets reuse the current
worker fixture providers on x86. No unresolved-symbol or duplicate-symbol
link suppression was introduced. No product/mirror/guest code changes.

The old Console fixture required three corrections before it could pass:

- It expected Alt+Enter to disappear. Current ReadConsoleInputExW and the
  separate toggle fixture require down/up/next-character preservation. The
  assertion now verifies all three records, including peek/nonconsumption.
- It bypassed product initialization and had no text-route provider, causing
  a deliberate fail-fast. Its explicit native-Console route now binds the
  existing interface; guest VGA operations abort if reached. This is a
  native cell/input test, not a guest VGA algorithm test. Duplicate broker
  seams are removed in favor of existing fail-closed worker fixture seams.
- Its active backing buffer was resized by the host terminal from requested
  120x50 to actual120x30 before row48 readback. Captured empty rectangle was
  78,48,77,47. The test now owns an inactive backing buffer and retains all
  cell/attribute/bounds/reuse assertions, avoiding unrelated viewport resize.

Initial missing-provider links, duplicate-seam links, expired-assertion exit20,
missing-binding fail-fast and geometry failure are not reported as passes.
The final `verify-console-contracts.ps1` entry builds through CMD and runs
four successful cases: font x86/x64, Console toggle and complete input/text
contract. Console runs use the existing owned private-desktop observer, with
both observer success and child exit0 required. No Computer Use or guest GUI
acceptance is claimed.

[Results](../../../artifacts/build-migration/t484/s3-console-fixtures/result.json)
record final binary hashes. Adjacent observer reports, build receipt and
identity receipt retain actual evidence. All11 raw host inputs and22 published
hashes are unchanged under package d8983d2b-3fe1-4d18-8ea9-64613c84346c;
there is no changed product image to republish. The containing commit is P18.
Seven literal Ninja generators remain, excluding inline producers and release
scripts. S3 is not closed; S4/S5 remain outstanding.
