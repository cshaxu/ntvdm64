# M0 T199 S9 Whole XMS Package Disposition

All twelve XMS services remain deferred as one package. `bx-vdm` classifies every `52:00..0B` identity but may not synthesize A20, XMS memory, UMB, or firmware state.

The current minimum machine proves only one-MiB ordinary RAM and startup A20 enablement in `bx_ntvdm_minimal_machine.cc`. Its mechanical ABI only reads and writes checked ordinary RAM. It has no runtime A20 operation, extended-memory aperture, XMS handle table, UMB map, or INT15 notification contract. No XMS provider is therefore admitted.

| XMS group | Services | Core owner | Mantle capability required |
| --- | --- | --- | --- |
| A20 | `00` | Native Bochs A20/wrap state | Fixed copied set/query request and exact result; no selector vocabulary. |
| Extended memory | `01..05,0A,0B` | Native memory aperture/allocation semantics | Versioned allocation/handle/copy/query protocol with checked ranges and fixed result records. |
| UMB | `06..08` | Native memory map/reservation lifecycle | Reserve/request/release protocol after a real UMB layout exists. |
| INT15 notification | `09` | Native interrupt/firmware lifecycle | Concrete native firmware/PIC/keyboard lifecycle contract; no adapter callback. |

Future `bx-vdm` may translate copied XMS requests only to these typed mantle operations and restore source-derived register/error results. It may not own allocation policy, mutate Bochs state, call a Bochs API, or infer machine capacity. Each capability requires its own bx-core/bx-mantle package and boundary test.

No XMS facade is admitted yet: creating one now would add no capability and could turn deferred records into fake failures. Existing common ingress and `bx_ntvdm_xms_dpmi_plane_v1` are the complete safe boundary; `52:0C` remains non-callable. DPMI is audited next so shared machine boundaries are designed once.
