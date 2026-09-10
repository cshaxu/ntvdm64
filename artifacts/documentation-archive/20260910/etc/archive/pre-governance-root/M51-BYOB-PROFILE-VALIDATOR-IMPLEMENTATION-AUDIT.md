# M51 BYOB Profile Validator Implementation Audit

Status: completed implementation audit, 2026-08-08. This covers only the
modern MinGW runner graph. It neither runs historical source nor loads a guest
into machine memory.

## Existing Implementation

| Concern | Current source | Finding |
| --- | --- | --- |
| Bounded JSON/schema parsing | `src/runner/byob_profile.c` | Local bounded parser limits manifest size to 64 KiB, nesting to 16, requires unique known fields, and validates UTF-8 before structural use. |
| Fixed NT4 EN-US profile rules | `src/runner/byob_profile.c` | Requires the three first-cut roles, canonical names, fixed schema/profile/architecture/locale, declared feature tiers, nonzero bytes, and lowercase SHA-256. |
| Rooted identity verification | `src/runner/byob_identity.c` | Opens the caller-selected root and each named regular non-reparse file, then checks size and SHA-256 through its opened handle. No search path or system-directory lookup is used. |
| Metadata selection | `src/byob_profile.[ch]` | Acceptance yields only selected role names, size, and hash. It does not retain component bytes. |
| Later transient byte read | `src/runner/byob_image.c` | A separately invoked, exact-handle loader returns owned bytes only after descriptor identity matches. It is not linked into the normal runner and must remain after validation/entry planning. |
| Test isolation | `CMakeLists.txt` | `byob-profile-test` is `EXCLUDE_FROM_ALL`, uses only modern sources and bcrypt, and has no historical object or core dependency. |

## Current Test Coverage

`tests/runner/byob_profile_test.c` covers shuffled valid fields, changed content hash,
target mismatch, unsafe name, duplicate top-level key, and descriptor-to-image
readback. This is useful admission evidence but not the full contract matrix.

The untested required cases are malformed UTF-8/oversize/depth limits;
missing and duplicate roles; case-folded duplicate names; every feature-tier
combination; empty compatibility group; absent, empty, directory, and reparse
components; and failed selections being zeroed. These need owned synthetic
fixtures only.

## Implementation Boundary

The next implementation task may expand only `byob-profile-test` and, where a
test exposes a contract breach, the existing validator/identity module. It
must not add a normal runner flag, machine constructor, guest-memory write,
historical source link, image cache, directory discovery, or any automatic
download.

The canonical validation order remains structural profile checks, preflighted
safe names, rooted identity checks, then typed metadata selection. The
existing `byob_image_load_exact` is deliberately outside that admission pass.

## Fixture Closure

M52 now supplies one owned fixture for each public acceptance/rejection family:
accepted shuffled input; format failure; target mismatch; missing role;
feature mismatch; unsafe name; directory open rejection; identity mismatch;
and empty compatibility group. It also checks that a failed selected result is
zeroed. M56 additionally covers the manifest-size limit plus missing and
zero-length component objects. The fixture does not yet enumerate every
subvariant such as reparse points, all optional-feature permutations, or every
parser depth boundary; those remain regression-expansion candidates rather
than admission gaps.
