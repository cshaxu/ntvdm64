/*
 * Modern ntddstor.h emits C++ enum-operator declarations unguarded by
 * __cplusplus. The historical NT4 disk sources are C translation units.
 * This declaration-only compatibility spelling has no disk I/O behavior.
 */
#ifndef APP_OPENNT_DISK_NTDDSTOR_COMPAT_H
#define APP_OPENNT_DISK_NTDDSTOR_COMPAT_H

#ifndef DEFINE_ENUM_FLAG_OPERATORS
#define DEFINE_ENUM_FLAG_OPERATORS(enum_type)
#endif

#endif
