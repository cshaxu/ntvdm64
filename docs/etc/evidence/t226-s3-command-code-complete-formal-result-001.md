# M0 T226 S3 — COMMAND package code-complete formal result

## Scope

This result closes the local code-complete gate for the OpenNT COMMAND
`54:00..10` owner package. It does not claim native guest execution or a
complete historical NT host composition.

## Formal build and local result

The pinned MSVC x64 `/MT` Ninja graph was generated in
`build/M0-T226-S3/command-package-20260817b` with projected Bochs configuration
hash `23B0A094B911C8663587BB62AE6A6E1AD4E109023F2D1EA6C7A555D032EE2877`.

The existing whole-family fixture and fifteen package-local fixtures built and
returned zero. They cover bootstrap and COMSPEC materialization, launch and
CMDINFO, console/keyboard/current-directory behavior, initial environment,
child redirection and parent completion, lifecycle, source-proven no-op and
unavailable dispositions, plus the complete selector/service plane.

## Composition closure correction

The formal graph now includes the retained COMMAND launch-envelope object and
links composition fixtures with `bx-vdm`, `bx-mantle`, `bx-core`, `opennt-host`,
and `cli`. This corrects test build closure only; it adds no Bochs or guest
semantic change. Two legacy tests were updated to the current shared profile
ABI and strict MSVC comparison requirements.

## Code-complete disposition

T226 S1 binds the 17 COMMAND identities. T225 S20 source/current evidence and
this full local suite establish a provider or source-proven deferred,
unavailable, or no-op disposition for every row under Direct/Readonly. Overlay
remains ABI-only. No native trace selected any service work.