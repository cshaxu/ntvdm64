# NT4 EN-US BYOB Profile Contract

Status: R0.1 design contract, 2026-08-07. This defines an external local
metadata file for a future research runner. It does not add a runtime target,
ship guest files, identify an arbitrary image as authentic, or permit the
frozen self-authored instrumentation to become a DOS engine.

## Scope

The fixed first profile is named `nt4-en-us-command-smoke-v1`.

Its purpose is to lock one owner-supplied, internally consistent NT4 EN-US
file set before the research runner writes any guest memory. It supports only
the planned bounded `COMMAND.COM /C <owned command>` experiment. It is not a
general Windows NT installation profile and does not imply WOW, Win16,
redirector, DPMI, EMS, graphics, mouse, network, serial, printer, or physical
media support.

The profile document is local user metadata. It may live beside the BYOB
directory or at another user-selected location, but it is never accepted from
the repository, a release package, or an automatic downloader.

## Required First-Cut Roles

| Role | Required file name | Requirement | Reason |
| --- | --- | --- | --- |
| `ntio` | `NTIO.SYS` | required | Historical prepared-entry bootstrap image. |
| `ntdos` | `NTDOS.SYS` | required | Historical DOS kernel loaded by the original DEM transition. |
| `command` | `COMMAND.COM` | required | Original command processor for the bounded smoke command. |

The following are declared feature tiers, not implicit requirements:

| Feature tier | Candidate files | Admission condition |
| --- | --- | --- |
| `xms` | `HIMEM.SYS` and its required configuration inputs | Original startup trace reaches the XMS load/init path. |
| `dpmi` | `DOSX.EXE` | Selected smoke command needs protected-mode/DPMI behavior. |
| `startup-profile` | an owner-provided `_default.pif` configuration input | Required only when R3.C activates original `config()`; it is passed to the original PIF parser to prevent `%WINDIR%` discovery. It is not a guest DOS image and is never supplied by this project. |
| `nls` | `COUNTRY.SYS`, keyboard/NLS files and selected configuration | Trace reaches the corresponding original loader. |
| `redirector` | `REDIR.EXE`, host-side redirector inputs | Explicit opt-in after contained namespace support exists. |
| `utilities` | EDIT, DEBUG, EDLIN, KEYB and other commands/drivers | Independently selected test capability; never inferred from directory contents. |

An optional file may be recorded only when its feature tier is declared. Its
presence does not enable the feature. A later profile revision must promote
each trace-required optional role to `required` before execution reaches it.

When `startup-profile` is declared, role `pif-default` has the canonical file
name `_default.pif`. It is an owner-provided host configuration input, not a
claim that the file is an NT4 system binary or a redistributable project asset.
The future direct-CLI transport may provide only this validated role's
contained path to original `GetNextVDMCommand`; original `GetPIFData` remains
the parser. An absent `pif-default` role prohibits entering original
`config()`, because an empty PIF name would trigger forbidden `%WINDIR%`
discovery. The first active role must be a nonempty structural PIF whose
manifest identity has been selected for the default parser path; PIF extension
features that request UI, environment expansion, fullscreen, hotkeys, or fast
paste remain outside the first profile until separately admitted.

## Manifest Format

The UTF-8, LF-terminated metadata file has schema name
`ntdos64-byob-profile-v1`. JSON field order is not meaningful. It contains no
absolute path, file bytes, byte excerpt, embedded archive, source image,
registry data, host drive mapping, or automatic acquisition URL.

```json
{
  "schema": "ntdos64-byob-profile-v1",
  "profile": "nt4-en-us-command-smoke-v1",
  "architecture": "x86",
  "locale": "en-US",
  "compatibility_group": "owner-defined-opaque-id",
  "components": [
    {
      "role": "ntio",
      "file_name": "NTIO.SYS",
      "required": true,
      "bytes": 0,
      "sha256": "64 lowercase hexadecimal characters",
      "version": null
    }
  ],
  "features": [],
  "owner_note": null
}
```

`compatibility_group` is an owner-chosen opaque identifier shared by files
known to have been selected together. It is not a Microsoft version claim.
`version` is optional descriptive metadata and cannot replace the byte count
plus SHA-256 identity. `owner_note`, when present, is local free text and must
not contain an absolute path or guest-file content.

The runner receives the BYOB root separately on the command line. Every
component is opened only as `root + file_name`; no manifest field may alter
that root or request a parent path, alternate data stream, device path, URL,
or host-system directory.

## Validation Order

Validation is read-only and precedes allocation, image placement, CPU reset,
guest-memory write, or historical initialization.

1. Parse a bounded UTF-8 manifest and require the exact schema, profile,
   `x86` architecture, and `en-US` locale.
2. Reject duplicate roles, duplicate file names, unknown required roles,
   missing first-cut roles, invalid feature declarations, unsafe names, or
   malformed SHA-256/size values.
3. Canonically compare every required file name against the declared profile;
   accept no path separator, drive designator, dot segment, alternate stream,
   wildcard, or case-conflicting duplicate.
4. Open each component beneath the caller-selected root as a regular,
   nonempty, non-reparse file. Do not search `PATH`, the Windows directory,
   current directory, registry, removable drives, or network locations.
5. Hash the opened handle, compare the exact byte count and SHA-256, then
   close it. The validation pass retains only metadata and the result.
6. Require all selected components to use the same nonempty
   `compatibility_group`. This proves only owner-declared set consistency;
   it does not prove retail provenance or semantic compatibility.
7. Produce a redacted acceptance record and only then authorize a later
   bootstrap-planning operation. The record still does not authorize guest
   execution; Phase R1/R2 gates remain required.

The implementation must preflight all components before reporting success. A
failure must leave no guest allocation, guest-memory mutation, cache copy,
temporary guest file, host-root placeholder file, or host configuration
change.

## Rejection Results

The future runner must return a stable typed result without printing the
BYOB root or content. Suggested result names are:

| Result | Meaning |
| --- | --- |
| `profile_format_invalid` | Schema, type, encoding, field or size bounds are invalid. |
| `profile_target_mismatch` | Profile, architecture or locale differs from the fixed first cut. |
| `role_missing_or_duplicate` | A required role is missing, repeated, or unknown as required. |
| `feature_declaration_invalid` | An optional role conflicts with the declared feature tier. |
| `component_name_unsafe` | The component name could escape the selected root or name an unsupported object. |
| `component_open_rejected` | The component is absent, unreadable, a directory, reparse point, device, or empty file. |
| `component_identity_mismatch` | Byte count or SHA-256 differs from the manifest. |
| `compatibility_group_mismatch` | Required roles were not owner-locked as one selected set. |
| `manifest_output_unsafe` | A requested local report path overlaps the BYOB root or violates caller policy. |

For privacy, a normal diagnostic identifies only the profile name, role and
result code. It must not print a full source path, file bytes, byte offsets,
raw DOS state, or guest content-derived strings. An owner-enabled local
diagnostic may include the supplied manifest path, but not a discovered guest
path.

## Explicit Non-Compatibility Claims

Passing this manifest validation proves only that the files presented to the
next stage match an owner-recorded selection. It does not prove:

- that files are retail NT4 files, correctly licensed, or built from a
  particular source snapshot;
- that NTIO and NTDOS have matching undocumented layouts;
- that the selected optional feature is runnable;
- that a historical host API, BIOS device, BOP/SVC, filesystem, or command
  behavior has been restored; or
- that a program can run.

The first semantic evidence remains the original-chain trace required by the
ROI roadmap.

## Relationship To Existing Instrumentation

The old `nvtdm-runtime-manifest-v1` code under the source-only
`ntvdm-startup-reference` target is a frozen trace artifact. It is not this
format, has no profile/feature contract, and must not be extended or used as
the future runner's admission implementation. This contract is the sole
design input for a new, separately admitted research-runner validator.

## Verification Plan

The implementation task for this contract must supply isolated tests proving:

1. a complete synthetic metadata-only fixture is accepted only after all
   required file handles hash-match;
2. each rejection result above is deterministic;
3. a reparse point, `..` name, absolute path, ADS name, or duplicate
   case-insensitive name is rejected before a guest allocation;
4. an optional component cannot enable a feature without its declared tier;
   and
5. the acceptance/failure record contains no selected-root path or bytes.

Those tests are profile-validation tests, not guest execution tests, and may
use owned synthetic files only.

## Current Implementation Status

`src/runner/byob_profile.c` is the initial default-disabled implementation of this
contract. It has verified shuffled-field acceptance and typed rejection of
duplicate JSON keys, target mismatch, unsafe component names, and altered
component identity with owned temporary files. The complete verification plan
above remains the acceptance standard before any validator result may authorize
a monitor-session construction or guest-memory preparation.
