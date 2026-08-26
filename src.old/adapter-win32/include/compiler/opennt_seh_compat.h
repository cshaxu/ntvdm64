/*
 * Target-local spelling bridge for retained OpenNT C sources that use the
 * historical try/finally spellings.  The compiler supplies the underlying
 * __try forms through the source unit's normal native-NT header order; this
 * bridge must not pre-include OpenNT's crt/excpt.h because modern nt.h then
 * imports a second, incompatible exception declaration set.
 */
#ifndef APP_OPENNT_SEH_COMPAT_H
#define APP_OPENNT_SEH_COMPAT_H

#ifndef try
#define try __try
#endif
#ifndef finally
#define finally __finally
#endif
#ifndef except
#define except __except
#endif

#endif
