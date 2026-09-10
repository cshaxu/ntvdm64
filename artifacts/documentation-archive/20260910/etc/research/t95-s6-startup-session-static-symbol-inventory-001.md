# T95 S6 Startup-Session Static Symbol Inventory 001

## Purpose

This is a static, pre-link inventory of the narrow adapter startup-session
slice. It answers only whether the eight objects compiled under the MSVC/x86
island refer to each other and to a bounded set of platform facilities. It is
not a link, ABI-crossing, Bochs-composition, guest-write or runtime result.

## Inputs and procedure

The input is the immutable successful object root
`artifacts/analysis/t95-s6-startup-session-msvc-objects-003-20260811-001`.
The eight object SHA-256 values, exact input root and tool identity are in the
retained
`artifacts/analysis/t95-s6-startup-session-symbols-001-20260811-001/symbol-inventory.json`.

`C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.43.34808\bin\Hostx86\x86\dumpbin.exe`
ran once per declared object with both `/symbols` and `/directives`. Raw output
is retained beside the manifest. No input object was copied, changed, linked or
executed.

## Classification

| Edge family | Classification | Evidence and ownership |
| --- | --- | --- |
| Startup-session, CPU-state, guest-range/write and startup-plan functions | Required slice-internal | Undefined references from the startup-session and plan/write objects resolve to definitions among the five adapter objects. |
| BYOB profile, identity and image functions | Required slice-internal | Startup-session references `byob_profile_validate_file_select`, `byob_image_load_exact` and `byob_image_release`; definitions are in the three copied CLI objects. |
| `mem*`, string/wide-string formatting, security-cookie, range-check, stack and 64-bit arithmetic helpers | Required MSVC/UCRT | `/MT` emitted `LIBCMT` and `OLDNAMES` default directives. These are compiler/CRT ownership, not a Bochs or adapter-runtime edge. |
| `CreateFileW`, file information/size/read/close, process heap and multibyte conversion imports | Required Win32 platform | The BYOB objects import only these `Kernel32`-class file/heap APIs to validate and load a user-selected BYOB package. They remain adapter/CLI ownership. |
| `BCrypt*` SHA-256 imports | Required Win32 platform | `byob_identity.c` and `byob_image.c` explicitly use BCrypt to verify declared component hashes. A future link design must name `bcrypt.lib`; this inventory does not perform that link. |
| `uuid.lib` default directive | Default, unresolved necessity | The three BYOB objects carry this default-library directive, but their symbol output has no UUID import or unresolved UUID symbol. A future link-design study must account for or suppress it only with evidence; it is not permission to add functionality. |
| Bochs, OpenNT, SoftPC, DEM/BOP/CMD, broad adapter runtime, host-service, namespace/search/drive-policy | Absent / prohibited | No symbol in the eight raw `/symbols` outputs names any of those owners. This is the required negative result for the narrow slice, not proof that a future Bochs bridge is safe. |

## Conclusion and next gate

The object set has a bounded, ordinary same-island dependency direction:
adapter startup-session -> CLI BYOB verifier/loader -> UCRT + Win32
(`Kernel32` and `BCrypt`). It has no static object edge into Bochs or the wide
adapter service plane.

This makes a **separately admitted link-design study** lawful. It does not make
a link lawful yet: that study must define a non-product host fixture or
versioned C ABI direction, startup/CRT entry ownership, explicit platform
libraries, default-library treatment, and a negative assertion that no Bochs
C++ object or cross-island allocation crosses the boundary.
