# M0 T185 v5 runtime selection projection plan

## Packet purpose

Repair the v5 compatibility boundary exposed by T184.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Map declared v5 fields to runner/adapter consumers and identify omitted projections. | Complete: command placement and boot-file fields were omitted. |
| S2 | Implement and test the complete declared-field projection with no schema/provider expansion. | Complete: local v5 profile and adapter-install tests pass. |
| S3 | Recompose corrected current adapter/CLI objects into the retained engine without a Bochs rebuild. | Active in Status. |

## Invariants

- V5 remains exactly target then source-built QUIT; no new target or host namespace.
- The repair fills existing selection fields from already validated profile data.
- No guest process is launched in this packet.
