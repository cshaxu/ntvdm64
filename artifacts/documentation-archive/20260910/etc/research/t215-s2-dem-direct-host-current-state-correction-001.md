# T215 S2: DEM Direct-Host Current-State Correction

## Question

Does the DEM direct-host file/handle/FCB/search package selected by T215 S1
remain a future package, or has it already been completed in current bx-vdm?

## Evidence

The original complete source/ABI/failure map is already present in
`t202-s3-dem-file-handle-fcb-search-source-abi-map-001.md`.  It covers the
29-identity owner set, copied guest layouts, opaque token rule, selected-root
host API seams, profile outcomes, hard-error boundary and Redirector-only
pipe exclusion.

Current `bx_ntvdm_dem_package_session_v1_dispatch` confirms the later atomic
switch: once the whole provider is installed, its handle, namespace, FCB-path,
FCB direct/I/O and search partitions run before any boot/readonly fixture
fallback.  `50:47/48` are separately retained as the explicit Redirector
deferred result.

The T202 S4 record further supplies the required package evidence:

- r41 performs the atomic all-local-identity switch;
- r42 verifies the separate `47/48` result and complete current source
  closure;
- direct host, readonly and non-direct profile regressions are recorded for
  the local partitions;
- no host handle or pointer crosses the guest ABI.

## Conclusion

T215 S1's proposed DEM package was a stale-planning duplication.  The source
map and current source establish that this package is already recovered at its
admitted direct-host/disposition boundary.  It must not be re-admitted or
rewritten.  The queue candidate is removed.

The remaining DEM limitations are not hidden: historical INT 24 hard-error
delivery needs its own guest owner, `47/48` remain Redirector work, and actual
overlay/virtual filesystem backends are not implied by their source-shaped
unavailable outcomes.  None is a justification for a leaf patch.
