# M0 T278 — component isolation and build-input audit

The physical re-root verifier established all of the following without
compiling or enabling a MVDM provider:

- all 1,689 selected original paths exist once at their frozen component
  destination and retain the selected upstream SHA-256;
- `opennt-mvdm-host` contains 1,491 selected paths, support 112, tools 28 and
  firmware 58; each root otherwise contains only its own README;
- firmware retains `softpc.new/...` package-relative paths rather than
  flattening the original package hierarchy; and
- 447 current project configuration files contain no reference to an old,
  moved `opennt-mvdm-host` path.

Historical OpenNT makefiles and `sources` files in the mirrors are source
evidence, not current project build inputs. Therefore this task did not edit a
build graph; source-package recovery tasks will add build bindings only after
separate admission.
