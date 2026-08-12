/*
 * Declaration-only bridge for the original system library's RTL time calls.
 * Field order and prototypes come from the retained OpenNT ntrtl.h input.
 * This file supplies no clock policy or implementation.
 */
#ifndef NTDOS64_OPENNT_SYSTEM_NT_TIME_COMPAT_H
#define NTDOS64_OPENNT_SYSTEM_NT_TIME_COMPAT_H

/* OpenNT public/sdk/inc/ntdef.h defines CSHORT as short. */
typedef short CSHORT;

typedef struct _TIME_FIELDS {
    CSHORT Year;
    CSHORT Month;
    CSHORT Day;
    CSHORT Hour;
    CSHORT Minute;
    CSHORT Second;
    CSHORT Milliseconds;
    CSHORT Weekday;
} TIME_FIELDS, *PTIME_FIELDS;

extern VOID NTAPI RtlTimeToTimeFields(
    PLARGE_INTEGER Time,
    PTIME_FIELDS TimeFields);

extern BOOLEAN NTAPI RtlTimeFieldsToTime(
    PTIME_FIELDS TimeFields,
    PLARGE_INTEGER Time);

#endif
