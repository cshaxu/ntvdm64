# Remaining historical build recipe roles

This checkpoint reviews all 36 then-pending build-control paths named
`sources`, `dirs`, `makefil*` or `_objects.mac` against ledger base
`35454cc49`. It changes **physical recipe roles only**. It does not exclude
any program, driver, debugger, printing, sound, CD-ROM, DDE, declaration or
firmware capability from the T.

The [per-file dispositions](historical-build-role-dispositions.csv) record
the actual original target/recipe, current composition boundary and retained
source-capability obligation. They cover NT directory recursion, generated
architecture object lists, shared `NTMAKEENV` trampolines, independent
tool/sample manifests, Win16 build/link rules and h2inc conversions. The
review read their complete contents, including the multi-architecture
host object list and the header-conversion tail; basename matching alone
is not their exclusion evidence.

## Configuration ownership and scope

Current `CMakeLists.txt` delegates the worker/reference composition to
`New-T310OriginalSoftpcNinja.ps1`, WOW32 to
`New-T404S5Wow32ProviderNinja.ps1`, and x64 native components/hooks to their
reference-graph consumers. T310 reads the selected original C source
manifests through `Get-OriginalSources`; T404 reads its WOW32 source manifest
through `Get-SelectedWow32Sources`. These are real configuration-time inputs,
so compiler dependency absence would not justify excluding them.

The [observations](historical-build-role-observations.json) name all 19
selected original source manifests and bind the five reviewed composition
owners. None of the 36 recipes is in that set. The native x64 generators
consume the composed reference graph rather than executing NT directory
recursion or these old object lists. Current graph and valid dependency
snapshots are separately hash-checked for exact path absence. This is a
source-reviewed current configuration boundary, not a claim that those files
could never be used by historical builds, future profiles or external tools.

Examples of the retained obligations are explicit:

- `inc/makefile` owns historical h2inc rules, not the acceptance of the
  generated guest ABI layouts; the corresponding declarations remain reviewed.
- Root `makefil0` includes guest builds and ROM/profile binplacement; its
  exclusion does not accept their payload identities or authorize rebuilding
  guests. Current package staging retains its own separately audited contract.
- `vdmutils/win/sources`, GRAFTABL, the Unix EMM manifest, debugger MIPS
  fragment and VDD samples remain separate from their actual source behavior.
- DDE test makefiles describe original clients, servers and demos. Their
  exclusion neither proves DDE nor removes its testing obligation.

## Provenance, proportions and verification

Twenty-seven recipes match a named original OpenNT/OpenNT-4.5 file exactly.
The nine generated `_objects.mac` files match the already registered
OpenNT-src-2 supplement, its original import manifest and frozen identities.
The initial merge attempt stopped before writing ledgers because it checked
only the two principal original roots; the corrected attempt verifies the
existing supplemental provenance through the common checker. This was an
audit-input omission, not a product failure or new source admission.

All 36 original texts remain 100 percent retained and unchanged. An executable
restoration ratio is not applicable to an excluded declarative recipe; no
runtime behavior percentage is inferred. All constituent source, resource,
guest and utility decisions remain independently in scope.

The existing [checker](guest-source-role-checkpoint.py) verifies exact cohort
membership, original/frozen hashes, supplemental attribution, selected
configuration/input identities, the 36-row delta, all unrelated rows across
four shards, retained full rows and existing mirror drifts:

```text
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/historical-build-role-verification.json --base 35454cc49 --prefix historical-build-role- --expected-count 36
```

The [result](historical-build-role-verification.json) retains 1,768 pending,
319 partial and 52 full paths, with 2,545 excluded physical file roles.
This changes no production source, guest, configured graph or binary.
