# Candidate stack words for sampled native calls

The three-round sample was re-examined with the now section-verified WOW32
map and the recorded 32-bit win32u export table. The [analysis](stack-exports-analysis.json)
extracts a possible return address only when EBP points within the 128 stack
bytes captured at suspension. Across open and close it reports ten samples
pairing `NtUserGetProp2` with the nearest WOW32 symbol
`wow_window_words_acquire`, two pairing `NtUserInternalGetWindowText` with
`wow_window_client_capture_name`, and two pairing `NtUserGetProp2` with
`wow_window_words_project_observed`. Other pairs and the analyzer are saved
alongside this note. The analyzer uses nearest-symbol matching; it does not
recover formal function sizes or a complete stack.

Source inspection is consistent with these candidates: `acquire` queries
the native HWND property under a shared publication lock and takes a binding
reference; graph commit later validates properties under an exclusive lock.
Those checks participate in generation identity and retirement, and their
removal cannot be justified by a sample count. Optimized x86 code can omit a
frame pointer or place non-return data at EBP; the observed word is not a
proven call frame. The sample cannot establish call frequency, exclusive
cost, or causal latency. No runtime changes follow from this observation.

## Existing generation storage is not a drop-in property replacement

The page domain can return host storage for an already pinned exact guest
backing. That route also supports retained generations after native retirement
and may allocate a binding for a sampled window. In contrast,
`wow_window_words_acquire` first applies the original same-process check and
reads the current native property under `publication_lock`; it returns
`ERROR_NOT_FOUND` when the property is absent. During graph projection a
local native window may be observed without being enrolled, and this absence
is intentionally accepted without creating a new binding. Property validation
is repeated under the exclusive commit lock before prepared state is swapped.

Consequently using generation storage to replace that first property lookup
would not be a mechanical substitution: it must distinguish property-only,
unenrolled, exact current and retired/reused HWND generations without
allocating or enrolling during a read. The existing page-domain lookup also
requires an outstanding generation reference and execution-owner context,
whereas native acquisition's API is used outside this transaction. No
equivalence or timing benefit has been established for this substitution.
This is a bounded source analysis; it does not prove that a smaller safe
transaction-specific lookup is impossible.
