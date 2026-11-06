# T478 S25: selected fax failure contracts

S25 owns finding 57 under the original-semantics ceiling. Parent is
1d8bae55c (S24). The selected WOW client remains separate from the unselected
WOWFAX GDI driver and UI. This delivery fixes host client defects; it does not
claim a deployed or end-to-end working fax product.

## Original and selected ownership

The five audited release bodies (SortedInsert, BuildStrList,
GetSupportedFaxDrivers, BuildPath and InstallWowFaxPrinter) in the parent
match both pinned OpenNT families after comments, line markers and DBG-only
code are excluded. Their defects are original bugs, not missing original
wiring or project replacements. The current graph selects wow32fax.obj;
WOW32FaxHandler and InstallWowFaxPrinter call the existing project Registry
bindings. The parent Registry admits the exact WOW/WowFax subtree, so a
configured supported-driver list is not an unreachable source-only branch.

The image observer checks all twelve mapped fax functions in the actual compiled object against the frozen
WOW32 image and resolves Registry call relocations. This is static selection
and byte identity evidence, not actual fax driver execution. wkman dispatch,
wkernel profile interception and existing spoolerapis remain unchanged.
ClosePrinter is already an entry in the same loaded API table.

## Minimal repair (DIV-879)

- Initialize the returned list and its backing pointer. On failed allocation,
  test the pointer before zeroing it; return NULL/count zero and release the
  untransferred MULTI_SZ buffer. Empty lists and second-query failures now
  retire that buffer. A successful list still retains its string storage.
- Assign the AddPrinterDriver result before reading it. Preserve original
  already-installed-driver and already-existing-printer success semantics.
- Retain and close the handle returned by successful AddPrinter using the
  existing ClosePrinter entry. No port/driver rollback or retry is introduced.

The pointer array's exact count is intentional: each insertion consumes one
remaining zero slot, and consumers use the explicit count. An extra sentinel
slot is unnecessary. WowFaxWndProc already closes its mapping handle when
MapViewOfFile fails; that path needs no repair.

## Controlled counterexample and limits

`test/integration/verify-wow-fax-failure.ps1` extracts five exact current or
baseline bodies and compiles them with x86 MSVC. Registry, heap, spooler, file
copy and resource-string boundaries are isolated providers; the fixture never
calls a real registry mutation, driver install, printer install or file copy.
Its SEH boundary records the old NULL-write exception without crashing the
observer. All heap blocks belong to the fixture and are retired between cases.

Baseline: 132 checks, 15 failures. Final fixture: 135 checks, zero failures.
The differing check count includes successful-path provider callbacks. The
old uninitialized return is undefined; its observed value is a bounded
counterexample, not a stable old API contract. Coverage includes sorted normal
list, absent key, first/second query failure, wrong type, empty list, both
allocation failures, all nine driver/printer success/already-exists/error
combinations, and early open/create/directory/API-load failures.

Successful list storage remains process-lived under the original initialization
model; this is not a new repeated-subclass teardown design. Malformed MULTI_SZ,
oversized paths, BuildPath allocation failures, ignored CopyFile/RegSetValueEx/
AddPortEx failures and arbitrary external driver failures retain original
limitations. This S does not promise transactional printer installation or
repair every dormant fax path. Separate driver/UI deployment and real guest
fax operation are outside the admitted scope; ordinary printing acceptance
cannot substitute for them. S40 retains its separate multimedia/fax review.

## Delivery evidence

Local replay paths: build/t478-s25/{baseline,confirmed-current}, build.log,
package-final, verify-final-Full and publication.txt. Static image/provenance
receipt: artifacts/mvdm-audit/shard-1/fax-s25-results.json. Production build
uses the same x86 worker/WOW32 and x64 six-target commands as S24. Frozen
package ID is 14775894-6a66-4e25-bf87-741d89b73acc (version 0.0.478, protocol 67). Full passes 16/16 gates in 406217 ms. All ten images and their manifest were
published to this worktree assets/release and hash-verified against that
frozen package. DOS4GW/NESticle is owner-excluded and skipped, not passed.

No guest/media, O:/winnt or remote repository is changed. Earlier S23 startup
and S24 concurrent-writer diagnostic risks remain recorded, not claimed fixed.
