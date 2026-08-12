# T95 S7 BOP 54:01 Generic Transport Audit

## Question

Can the first reached command service use existing generic adapter mechanics,
or does it need a BOP-specific memory/CPU bridge?

## Evidence

`bx_ntvdm_adapter_runtime_v4_dispatch` already receives the copied exception,
CPU state and instruction window. Its generic
`bx_ntvdm_guest_gather_read_action_v1` requests one to four checked physical
ranges, with at most 256 total bytes. A 36-byte `CMDINFO` record is therefore
within the existing transport limit. The matching host-session queue and
completion functions bind the copied bytes to the original boundary and CPU
state before a result can be accepted.

`bx_ntvdm_adapter_runtime_v4_complete_guest_gather_read` already supports a
`bx_ntvdm_multi_write_transaction_v1`: several ordinary guest RAM ranges plus
a fixed-width CPU result/delta are preflighted and queued through the same
generic session path. Existing COMSPEC and mouse services demonstrate the
prepare/complete sequencing without adding a Bochs memory API.

The just-verified `bx_ntvdm_cmdinfo_v1` codec supplies the record decoder and
capacity plan. Together, these facts prove the required mechanics closure:

```text
54:01 observation + DS:DX
  -> one generic checked 36-byte gather
  -> copied CMDINFO decoder + immutable command envelope
  -> generic multi-write transaction + fixed CPU result
```

No new CPU instruction handling, Bochs device, Bochs memory access, BOP
memory protocol, raw pointer, or direct guest cast is required.

## Not Yet Admitted

The current envelope has only the selected bootstrap `C:\\COMMAND.COM`. It is
evidence for a copied value shape, not an executable CLI target. Returning it
from `CMDGETNEXTCMD` would ask COMMAND.COM to execute itself; it is not a
valid CLI session command.

Before a `54:01` service can resume, CLI must define one contained target
materialization: identity-validated target bytes, a selected canonical DOS
path, immutable normalized tail, and absence/error policy. The resulting
target must enter the same read-only namespace as an ordinary guest file.
Only then may an adapter service combine the target envelope, decoded CMDINFO,
and existing generic gather/multi-write mechanics. This is a capability
contract, not a special BOP workaround.

## Conclusion

Confidence is high that generic transport is sufficient. The next blocker is
not Bochs or adapter mechanics; it is the still-unselected CLI-to-contained-
guest target mapping. BOP `54:01` remains observed and pass-through until that
contract is admitted and tested.
