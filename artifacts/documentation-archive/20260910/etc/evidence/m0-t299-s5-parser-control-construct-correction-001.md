# M0 T299 S5 — parser control-construct correction

The original-source scanner now classifies C/C++ and MSVC control constructs
(`try`, `catch`, `__try`, `__except`, `__finally`) as language syntax, never
as function definitions or direct call candidates.

After reproducible regeneration from the two original OpenNT trees:

- the first-degree selected-file closure contains 128 physical definitions;
- the second-degree ledger contains 743 cross-file candidates;
- zero first-degree definitions and zero second-degree rows have any of the
  excluded control-construct spellings;
- the generator still rejects any second-degree row that resolves in its
  caller file or another selected first-degree file.

This is an audit-parser correction only. No mirror, adapter, package
selection, build input or runtime behavior changed.
