# Guest data, image and historical build-input roles

The exact-delta checkpoint replays four independent source and package
observations. It classifies six roles as partial: five delivered guest images
or resources and one country data input whose collation bytes appear in the
delivered image. File identity does not prove which source built an image, that
the guest loads it, or that its behavior and failure paths work. MEM's pinned
original is OpenNT-4.5; the others also match the primary OpenNT reference.

Four exclusions apply only to two historical message skeletons and two
documentation/change-history files. DOS messages, locale, OEM identification
and kernel behavior remain in scope. The individual contracts and remaining
proof are in the [dispositions](parallel-guest-delivery-role-dispositions.csv)
and the [source/package reports](parallel-source-package-observations.md).
Byte retention is 100%; guest behavioral restoration is not quantified.
No guest was modified or rebuilt.
