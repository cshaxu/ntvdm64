# T198 S53: selector-15 AH=88 composition repair

## Root cause and repair

S52 proved selector `15` with AH=88 reaches the finite composition, but S53's
first run showed it declined.  The reusable provider already accepts that
contract; the composition's outer memory-provider guard admitted only selector
`12`, so selector `15` could not reach it.

The repair admits selector 12 or 15 to the existing provider.  The provider
retains the decisive condition: selector 15 is accepted only in real mode with
AH=88.  No other BIOS selector-15 subfunction is admitted.

## Clean witness

`artifacts/build/t198-s53-ah88-guard-repair-r3/run.log` records:

```text
bios15 observed=0 ... composition=1 outcome=1
next=50:0d terminal=1:8dc8:0669
```

The selector-15 boundary was accepted with a resume outcome, and the guest
advanced to the next original DEM boundary.  This does not activate cassette,
CMOS, devices, or a SoftPC BIOS handler.

## Next frontier

`50:0D` is `SVC_DEMGETBOOTDRIVE`, dispatched by OpenNT to `demGetBootDrive`.
Earlier source and runtime evidence identifies its contained fallback as the
profile boot drive; the original registry query is not admitted.  The next
task must audit the current finite composition path for that existing provider
before adding any new implementation.
