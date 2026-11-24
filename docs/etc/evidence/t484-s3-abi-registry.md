# T484 S3 P115 public memory ABI and registry lifecycle

B0187 retains its compile-only x86 declaration contract: WOWGetVDMPointer
and WOW32ResolveMemory function-pointer shapes must agree under explicit /Gd,
without accidentally inheriting product /Gz. B0188 retains real production
registry-binding lifecycle with scoped shadow-registry service doubles:
populated shutdown, invalid retired handles, repeated shutdown and reopening.
Both C fixtures remain unchanged. Their distinct current contracts justify
retention; neither has a current suite caller and neither is guest acceptance.

Native configuration takes the actual original_wcall16 C profile for the ABI
fixture and changes only /Gz to /Gd. First attempt PID17648 fails because the
old runner selected the now-B-layer wow_user_thunk_scope profile, which no
longer includes the C-layer precomp.h tree. Selecting the actual C producer
profile resolves this stale test assumption; declarations are not copied.
The C fixture still makes selected prototype warnings errors after including
the product headers. Inherited header warnings are not claimed eliminated.

The registry target links the current obj_compat_wow_registry_binding_obj,
with generated object identity rather than the stale obj/compat path.
Remove Ninja command parsing, source substitution, manual CL/link commands,
caller-selected provider/compiler roots and their duplicate build setup.
No mirror, original guest, product source or asset change.

Final owned PID27792 exits0: public ABI compile marker and x86 object machine
0x14c, registry runtime marker, cached full host build, two hash/mtime-stable
outputs and both x64 rejection2 checks. Eleven raw host/22 published hashes
match the manifest. Existing publication verified, not republished. The first
failed attempt is retained, not counted as passing or as a production failure.

[Receipt](../../../artifacts/build-migration/t484/s3-abi-registry/result.json)
records attempts, provider/object identities and cache results. Raw:
build/t484-s3-abi-registry. Inventory218 complete/137 remaining of355.
Next B0189. S3 retrospective review, B0166/B0169 reconciliation and S4/S5
remain incomplete.
