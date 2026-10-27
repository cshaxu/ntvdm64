# DOS core guest build owner review

99 pending shard-2 DOS core / driver / redirector / shared-header records; no product, guest, other shard or main ledger change. Byte identity and original OpenNT presence are listed separately; current-only inputs are not thereby attributed to OpenNT. The original recipe establishes historical file role only, not live host selection, current guest-image lineage or feature correctness.

Candidate guest build source files: 42; pending: 57; frozen/current mismatches: 0.

DOS source chain uses msdos.lnk and makefile NTDOS link16, reloc and stripz; BIOS uses msbio_4.lnk and NTIO link16/reloc; keyboard variants require actual keyboard.lnk membership and explicit source rule, not filename similarity. REDIR uses explicit OBJS and MASM inference; HIMEM and XMM use their explicit rules. Direct include entries show an actual selected source include and are conditional on macro/profile settings; not all historical configurations are claimed. Binary objects, existing SYS/EXE, output/recipe and unmatched source remain pending.

No guest capability is excluded; original and current hash equality is not proof installed-image provenance. All 42 candidate file hashes match the OpenNT originals. The following original build input hashes also match their current mirrors byte for byte:

- `dos/v86/doskrnl/dos/makefile` `4bfc3320f1f519fd3ec2e851ffa28e7f633e3594d6e17f4fe3e5f7183da57d3e`
- `dos/v86/doskrnl/dos/msdos.lnk` `828ff652663f3999f8fa75311ed31398c8e0138f93cee86f38a6f36ca9a9d840`
- `dos/v86/doskrnl/bios/makefile` `03c54d48fdf24eece3b664a0a6c625dd649a5ef423173a80f9dfc36be025ddf8`
- `dos/v86/doskrnl/bios/msbio_4.lnk` `8efcecf5e4896d96ed3bf515c078b338d4a2676a24e4c60b7f894009e856d341`
- `dos/v86/dev/keyboard/makefile` `031df79e3dd9fbe2d39b3f808f83da74882778d21e53546e0f09a99df854ccd3`
- `dos/v86/dev/keyboard/keyboard.lnk` `e39a13af777e2bfe4419df80fe229f4972642361afcd04510929ce74bc6e0e41`
- `dos/v86/dev/himem/makefile` `4bcad509bad1cba600c444b0682fc3a8ae3c112fccd97ccba4b57cd17b6583c2`
- `dos/v86/lib/xmm/makefile` `2b0369a11d112c8c5d6c09d11a65a5f3ef0d17ce4d8df26136d1afbcb74b705f`
- `dos/v86/redir/makefile` `318569c93bb478aea0a11e078c82e2fb2d4f649d2e2cc2fb78dbfed781fde53f`
- `dos/v86/make.inc` `e9d2e142803b748ce09ad7c6f91ac062c1a2f02493fdc7350e63b63db7836a4f`
