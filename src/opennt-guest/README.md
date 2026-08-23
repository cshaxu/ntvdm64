# opennt-guest

Original OpenNT DOS and WOW16 guest source, guest artifacts and their
source-build inputs. Original prebuilt OpenNT artifacts remain the default
packaging inputs; source-built outputs are fallback/reproducibility inputs.

The retained original source layout is `dos-v86/` and `wow16/`.  These trees
were moved intact in T260 S4 with no content edit.  Historical documents may
refer to their former `src/opennt/base/mvdm/...` provenance paths; those are
source identities, not live product input paths.

This component is guest content, never an in-process host provider library.
It may communicate with host behavior only through the guest's original
instruction/BOP/interrupt contracts.

## Local intrusion register

Every imported guest-source modification is registered here with original path,
source identity, exception identifier, necessity and focused verification. The
expected normal count is zero. The future public surface is the declared guest
image/artifact manifest, not C/C++ host headers.
