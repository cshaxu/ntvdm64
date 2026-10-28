# Released WOW shell, hook and subclass owners

The [observer](wow-shell-hook-image-observer.py) compares all three source
files with both original editions and binds their selected COFF bodies to
released WOW32 code: 22 shell, two subclass and five hook functions. The
[snapshot](wow-shell-hook-image-observations.json) records exact object, map
and DLL identities. Relocation words are masked during function comparisons;
table target, actual invocation and runtime ownership require separate proof.

The original shell route converts guest data and delegates to host shell or
registry services, with Win16 callback and current-directory boundaries
described in the [shell review](../shard-3/shell-thunk-role-review.md).
Original hook and subclass logic handles guest procedure identities at the
host USER boundary. Hooks, nested callbacks, task/window destruction, drop
data, registry failures, buffer bounds and teardown remain unverified. These
three roles merit partial classification after ledger reconciliation; this is
not a claim of complete Win16 shell or hook behavior.
