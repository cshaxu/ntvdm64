# Win16 guest stub and network driver identities

The [stub review](shard-2/win16-kernel-stub-owner-review.md) compares a 546-byte DOS stub payload with installed WRITE and COMM images; it does not prove the complete stub binary was deployed or any guest execution. The [WFWNET review](shard-3/wfwnet-guest-input-role-review.md) matches the installed network driver to its original guest image; its optional IPX provider, operations and lifecycle remain untested. Both file roles are partial. Guest media remain immutable.
