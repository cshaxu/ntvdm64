# M0 T453 S4 W04 package acceptance

Status: closed. S4 accepts the complete W04 boundary and publishes no new
product image because its only post-S2 changes are test and evidence closure.

## Inputs and release identity

The last product change is S2 delivery
`9bf66f914dceac0ffb75c004835e1c990ef05a3d`. S3
`260474f39b7114be394e560bc4878f682533a5cd` changes only the OEM fixture
generator, test adapters and evidence; it does not alter a production source
or a component input. Therefore S2's full cached ten-image build and release
manifest remain the valid coherent product package rather than a reason to
rebuild/publish unchanged binaries.

The release manifest's ten image hashes match both `assets/release` and the
configured deployment root. In particular, the selected `WOW32.DLL` is
`0145E96705E2E4DB3A63694601A072CEEFE9CB8B390B6F93BEFAB0853A2F7C1D`
at all three identities: S2 build, release asset and deployed system32.

## Repeated deployed DOS witness

The checked-in `Verify-T420S37OemGuest.ps1` ran against the current deployed
package with the source-built S2 observer and the retained `O37.COM` probe.
It uses its established private-desktop observer context, serial direct and
nested COMMAND routes, byte-level OEM 437 console checks and exact test-file
cleanup. Both routes passed.

| Route | Report SHA-256 | Console SHA-256 |
| --- | --- | --- |
| Direct | `95D9BD77EC88FFCA8094FF477166D2A9B5BA305696C2E217033342F5F6251CF4` | `27FA9FB27F44E182E1BC5A07EFF42DF8DB91B855CB60DB3AFD453481689AE8D0` |
| Nested | `FBD107F1B7D6A547AE837FC806BE553B4AA3628074766576A303724E17D582D6` | `27FA9FB27F44E182E1BC5A07EFF42DF8DB91B855CB60DB3AFD453481689AE8D0` |

The probe hash is
`DD643A9D94ACC08E8D1548FEE4B933E8F3747FCDAD20BF9466CAD75B14A87F78`.
For each route it created the same observed Unicode non-ASCII filename and
content hash `221A5AACE78DC038B582AEF468C2DB42B5D7AF23CA538BCDF4364CF683ABF4E3`,
then removed only its verified files/directories. The checks cover create,
rename, attributes, open/read, directory and FCB deletion, volume/media,
COMMAND drive-environment publication and nested cleanup.

## Whole W04 conclusion

S1 identifies the finite owner matrix; S2 repairs the single traced cleanup
read and supplies the coherent delivery; S3 proves the remaining non-ASCII,
font fallback and PIF consumer boundaries; S4 confirms the deployed package
repeats the real DOS route with no test residue. No guest was modified or
rebuilt. No generic filesystem/environment layer, alternate current-directory
owner, or extra OEM conversion state was introduced.
