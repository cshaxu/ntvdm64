# T228 S1 P10 - source-built transient continuity correlation

## Question

Does the 300,000-tick COMMAND-domain observation represent original
source-built transient code, or merely an unclassified runtime buffer?

## Inputs

- P9 paired immutable observation: terminal state 0C41:8097 with a fixed
  fifteen-byte ordinary-RAM window:
  47 47 EB E9 5F 07 C3 8F 06 AA 91 33 DB 93 92.
- Source-built COMMAND.COM from
  build/M0-T225-S8/command-primary-source-build-r6/base/mvdm/dos/v86/cmd/command/COMMAND.COM.
- Retained source-built command.map, whose layout names TRANCODE from 02490h
  through 0AE41h and TRANSPACE through 0C5CFh.

## Procedure

The established COM correlation convention maps a real-mode runtime offset to
source-built file offset by subtracting 0100h. The P9 offset 8097h therefore
maps to COMMAND.COM file offset 7F97h. The immutable file was read at that
offset and compared byte-for-byte against P9's copied ordinary-RAM window.

## Observation

COMMAND.COM file offset 7F97h begins:

    47 47 EB E9 5F 07 C3 8F 06 AA 91 33 DB 93 92 F7 ...

The first fifteen bytes are identical to the paired P9 runtime window. The
offset lies inside the linked transient image range. Thus 0C41:8097 is not an
adapter-produced buffer and not an arbitrary resident execution location: it
is source-built COMMAND transient code still present at its initial image
placement.

## Interpretation

P9 and this correlation establish a stronger ordered boundary:

1. at 300,000 ticks, source-built COMMAND transient bytes are executable at
   their initial image placement;
2. by 1,000,000 ticks, execution has entered zero-filled 2073 code;
3. the later 5A5F zero transient destination is a later manifestation, not
   the first observed loss.

The candidate failure domain is therefore narrowed to the original transition
from the initial COMMAND transient image to a dynamically chosen allocation,
including EndInit initial move, guest NTDOS arena operations, and resident
LodCom preservation/relocation. It is not a BOP/provider capability gap and
not a generic source-built COMMAND staging error.

## Limitation and follow-up

This correlation does not prove which guest allocation/control operation first
created or selected 2073, and it does not authorize a direct image copy to
that segment. The next diagnostic, if admitted, must remain selector-blind,
default-off, state-copy-only, and must distinguish guest allocation/control
transfer stages without BOP decoding or guest-state mutation.