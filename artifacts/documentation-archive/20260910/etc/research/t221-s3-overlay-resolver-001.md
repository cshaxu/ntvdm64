# T221 S3: Overlay Base Resolver

`bx_ntvdm_dem_overlay_resolver_v1` is the sole selector-blind conversion from
an admitted host namespace file to an Overlay file object.  It opens the host
file only with `GENERIC_READ`, copies bounded bytes and attributes into the
provider COW store, then returns only the provider-private backend token.

It handles the normal `OPEN_EXISTING`, create/open and truncate dispositions
without any host write.  A focused x64 `/MT` fixture creates a temporary host
file, opens it through the resolver, changes the Overlay object, and proves
the temporary host file remains `base`.

This does not yet alter a DEM BOP route: namespace open/create must first
adopt the backend token through `dem_file_session`; the handle family must
then dispatch this typed backend as one subfamily.  The resolver is therefore
an implementation prerequisite, not an Overlay closure claim.
