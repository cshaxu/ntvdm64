# Win16 extension source roles and alternative-owner boundaries

This S10 audit-only checkpoint compares against `edf710b2b`. The
[103 dispositions](wow16-extension-source-role-dispositions.csv) comprise
28 [main-session core roles](shard-0/wow16-core-guest-owner-review.md) and
29/28/18 extension roles in
[shard 1](shard-1/guest-extension-owner-review.md),
[shard 2](shard-2/guest-extension-owner-review.md), and
[shard 3](shard-3/guest-extension-owner-review.md).
The three extension totals sum to 75; the earlier progress estimate of 76
was off by one. The exact-path joined table is the authoritative count.

## Scope and independent review

All accepted files have exact frozen/selected-original identity and a named
original guest source/object/link, include, definition or resource consumer.
The main review checked COMMDLG's explicit five-object link response and
USA/LANG resource copy chain, SHELL's response generation, TOOLHELP's OBJS
and inline link16 response, and OLE client/server object and link rules.
OLE client's literal command is `link` from the historical 16-bit tools
path, not a fictitious `link16` spelling. Conditional locale copies are not
proof of current guest provenance. Ambiguous same-name includes and binaries
remain pending.

Excluded rises from 2,017 to 2,120; pending falls from 2,387 to 2,284.
Full stays 52 and partial stays 228. Only these exact 103 role transitions
are accepted. Excluded means the retained original guest build file is not
a modern host implementation input; guest capability, callbacks, ABI and
immutable-image execution remain separate obligations. No mirror-different
file is excluded, no runtime is changed, and no guest is rebuilt.

## A library consumer recovered from its actual macro context

The earlier library review left LIBH's consumer unresolved. The original
`wow16/commdlg/makefile:2` defines `WINLIB=..\\lib`, line 21 selects link16,
and the inline response at lines 143-153 consumes `$(WINLIB)\\libh` with
`libw` and `snocrtd`. The default `.lib` suffix and macro expansion identify
the actual `wow16/lib/libh.lib` input even though no literal `libh.lib`
appears. Current/original makefile SHA-256 is
`118C94655FAA068FD0027DE8BBFEF0BEBEC53A8D26B213268D80BC86981A5D36`.
This establishes an original consumer role, not frozen-library derivation,
successful linking or guest behavior. The 32 library ledger statuses remain
unchanged. SLIBCE, OLDNAMES, CDLLCEW and SNOCRTDW still lack such a precise
consumer in the reviewed recipes.

## Dormant code is not automatically a missing original WOW provider

Two concrete owner traces prevent incorrect blanket exclusions or recoveries:

The [16 input identities](wow16-alternative-owner-inputs.json) independently
match frozen/current and original source observations. Fifteen whole files
are identical to the original; `wow32.c` has other retained mirror changes,
but its exact `WOW32UnimplementedAPI` body matches the original. Neither
this local body comparison nor the original table implies full file recovery.

* `wow16/gdi/gdi16.h:13-45` explicitly supports both Win16 and Win32
  compilation; `meta.c` and `metasup.c` include it. These are not proven
  guest-only headers/bodies. The actual selected GDI response has five
  objects, while `gdi.asm:208-218` sends play/enumeration through WOW32;
  `wgtbl2.h` selects `WG32PlayMetaFile`, `WG32PlayMetaFileRecord` and
  `WG32EnumMetaFile`. Current Ninja compiles and links `wgmeta.obj`.
  GET/SETMETAFILEBITS are deliberately local guest APIs, not missing host
  functions. The earlier [T456 owner review](../../docs/etc/evidence/m0-t456-s3-metafile-owner-host-audit.md)
  proves the public host seam only, not all original guest callbacks.
  These dormant dual-purpose files and the selected owner remain pending;
  S15 must retain the actual per-ordinal guest conversion, enumeration,
  callback mutation, failure and temporary-allocation retirement checks.
* `wow16/toolhelp/makefile:115-119` explicitly retains CLASS1.C/CLASS2.ASM
  for reference and substitutes THTHUNKS.ASM. That selected file emits
  `ToolHelpThunk ClassFirst/ClassNext`, but `wow32/wthtbl2.h:15-16` maps
  both to `UNIMPLEMENTEDAPI`. `wow32.h:665-670` maps that name to
  `WOW32UnimplementedAPI`, whose original body at `wow32.c:823-839`
  returns FALSE (with a checked-build diagnostic). Thus a thunk's presence
  is not a working class-enumeration implementation. CLASS1/CLASS2 directly
  depend on the old guest USER heap/class layout; treating modern USER
  pointers as that layout would violate the approved host boundary. No
  replacement implementation or unsupported-function expansion is admitted
  by this finding. These reference files remain pending for the S10
  original-profile disposition, not falsely full or silently recovered.

## Reproduction

Run the existing checkpoint in an owned background process:

```text
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py build/t473-s10/wow16-core-roles/extension-checkpoint.json --base edf710b2b --prefix wow16-extension-source-role- --expected-count 103
```

It checks exact transitions, original/frozen identity, shard partition,
unchanged unrelated rows, full-leaf identity, live drift and recorded host
graph observations. This is not product acceptance, and S10 remains open.
Concurrent setup/video product candidates are preserved outside this delivery.

The [checkpoint result](wow16-extension-source-role-verification.json)
passed on 2026-10-27 (PID 33284, exit 0). The 16-input identity/function-body
review passed (PID 28084, exit 0). Documentation governance (PID 3236) and
relative links (PID 5744) passed with exit 0; `git diff --check` passed.
CURRENT is 15,059 bytes. No product image changed, so no runtime rebuild or
publication is claimed or required for this audit-only checkpoint.
