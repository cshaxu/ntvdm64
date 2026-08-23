# opennt-guest

Original OpenNT DOS and WOW16 guest source, guest artifacts and their
source-build inputs. Original prebuilt OpenNT artifacts remain the default
packaging inputs; source-built outputs are fallback/reproducibility inputs.

This component is guest content, never an in-process host provider library.
It may communicate with host behavior only through the guest's original
instruction/BOP/interrupt contracts.

## Local intrusion register

Every imported guest-source modification is registered here with original path,
source identity, exception identifier, necessity and focused verification. The
expected normal count is zero. The future public surface is the declared guest
image/artifact manifest, not C/C++ host headers.
