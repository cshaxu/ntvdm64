# T198 S10 P1 Provider Plane Witness 001

## Result

The new `bx_ntvdm_boot_namespace_plane_v1` is an adapter-owned whole-family
state owner.  It accepts common ingress/selection/plane facts, owns the
immutable namespace and pending read context, and emits only copied mantle
mechanical actions or typed results.

## Verification

The i686 MinGW closure at `artifacts/build/t198-s10-plane-r1/` compiled and
ran its focused fixture with exit 0.  It covers COMMAND `54:0c` copied write;
DEM `50:12` copied read followed by provider completion; DEM `50:16` copied
bulk write; and `50:09` remaining pass-through when no explicit DTA record is
supplied.  No legacy runtime object was linked.

The first run exposed a real plane bug: `50:16` was compared as decimal 16
instead of byte value `0x16` (decimal 22).  Global DEM classification therefore
correctly declined it.  The plane now uses `0x16u`; no provider or machine
semantics changed.

The action record's clear/validation helpers are now header-only fixed-layout
operations.  This lets i686 adapter code construct the record while mantle
remains the sole owner of `bx_mem` execution.

## Limit

This is P1 only.  Seek/close, explicit valid-DTA pathname first/next, malformed
action rejection, and an MSVC composed bridge are still S10 work.
