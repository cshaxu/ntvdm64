# T190 S3 COMMAND bootstrap object-closure 001

Date: 2026-08-12  
Packet: M0 T190 S3  
Disposition: narrow derivative plan closed; no build or runtime observation.

## Decision

T190 must derive from
`artifacts/build/current/t189-boot-file-predicate-derivative-r2`, whose engine
SHA-256 is `A008832FBE0E6094A1F91D196E14ED9527BDAAE99FDE1042EC507BFDBE0C8608`.
That is the only retained composition that includes T189's current boot-file
service and namespace-provider objects.  A T187-base derivative would silently
restore the stale-object error that T189 proved.

`tools/build/New-T190S3CommandBootstrapDerivative.ps1` defines the only permitted
T190 build closure:

| Object | Disposition | Reason |
| --- | --- | --- |
| `cpu/exception.o` | recompile | Existing default-off mechanical handoff flags are required for the established executable composition; no Bochs source change is involved. |
| `adapter/bx_ntvdm_adapter_runtime.obj` | recompile | T190 changes its `54:02` service call ABI. |
| `adapter/bx_ntvdm_cmd_comspec_bootstrap_service.obj` | recompile | T190 changes the complete bounded COMMAND state contract. |
| boot-file service and boot-namespace provider objects | retain | Their T189-built hashes are recorded and no source/header dependency changed. |
| all archives, devices, CLI, guest OpenNT and other adapter objects | retain/exclude | No changed source owns them. |

The tool copies current runtime/COMSPEC source and headers, pins the retained
base/object hashes, extracts only the already-approved link recipe, and rejects
any makefile with other than three object rules.  A successful later build is
not a runtime claim; it requires the separate S4 admission.
