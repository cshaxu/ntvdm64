# Shard 3: WOWEXEC icon and OLE server version input

The [read-only observer](wowexec-ole-input-observer.py) and
[observations](wowexec-ole-input-observations.json) verify both source
editions, catalog identities and the original WOWEXEC resource recipe.

| File | Proposed | Role and remaining boundary |
| --- | --- | --- |
| `wow16/test/shell/wowexec.ico` | excluded historical guest resource-input role | Original `wowexec.rc` names this icon; the original makefile compiles and attaches its resource to WOWEXEC.EXE. The retained mirror, ZIP and direct installed WOWEXEC.EXE have exactly the same SHA-256; this does not prove that these source bytes generated that binary or that task startup and shutdown are fully correct. |
| `wow16/ole/server/olever.h` | pending | Original version definitions include Windows 16/32 variants, while inspected `olesvr.rcv` contains its own version definitions and no direct inclusion of this header was found. Other or older resource consumers and variant provenance remain unresolved. |

Exclusion concerns the historical icon input, not the WOWEXEC binary or any
guest capability. Both source files retain 100% of the two original source
editions; behavior restoration percentage remains unmeasurable. No guest,
product, shared build tree, or process was modified.
