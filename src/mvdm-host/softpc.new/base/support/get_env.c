/*[
 *      Name:           get_env.c
 *
 *      Derived From:   FmIit.c
 *
 *      Author:         William Charnell
 *
 *      Created On:     5th May 1995
 *
 *      Sccs ID:        @(#)get_env.c	1.2 06/16/95
 *
 *      Purpose:        Provide easy interface to env vbls
 *
 *      Design document:
 *
 *      Test document:
 *
 *      (c) Copyright Insignia Solutions Ltd., 1993. All rights reserved
]*/
#include <insignia.h>
#include <host_def.h>
#include <host_inc.h>

#include  <config.h>
#include  <error.h>
#include  <debug.h>	/* for always_trace macros */

#include <get_env.h>



#ifdef NTVDM

#ifdef LOCAL
#undef LOCAL
#endif

#else
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif

#ifndef NTVDM
LOCAL void badGetenv IFN3(char *, name, char *, type_name, char *, value)
{
	printf("Bad getenv(\"%s\") %s value \"%s\"", name, type_name, value);
}


LOCAL void setupVerboseGetenv IFN0()
{
	SAVED IBOOL firstGetenv = TRUE;
	char *env;
	char *how = "Defaulting";

	if (firstGetenv)
	{
#ifdef	PROD
		verboseGetenv = FALSE;
#else	/* PROD */
#error DIVERGENCE MVDM-HOST-DIV-503: non-PROD alternative is not selected; see mirror register.
#endif	/* PROD */
		env = host_getenv("VERBOSE_GETENV");
		if (env)
		{
			if (strcmp(env, "FALSE") == 0)
				verboseGetenv = FALSE;
			else if (strcmp(env, "TRUE") == 0)
				verboseGetenv = TRUE;
			else
				badGetenv("VERBOSE_GETENV", "IBOOL", env);
			how = "Setting";
		}
#ifndef	PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif	/* PROD */
		firstGetenv = FALSE;
	}
}
#endif


GLOBAL IBOOL IBOOLgetenv IFN2(char *, name, IBOOL, default_value)
{
        char *env;
        IBOOL value;

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif

	value = default_value;
	env = host_getenv(name);
	if (env)
        {
#ifdef NTVDM
            value = _stricmp(env, "TRUE") == 0;
#else
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif

        }

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif

	return (value);
}


GLOBAL ISM32 ISM32getenv IFN2(char *, name, ISM32, default_value)
{
        char *env, *ptr;
        ISM32 value;

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif

	value = default_value;
	env = host_getenv(name);
	if (env)
	{
		value = strtol(env, &ptr, 0);
		if (ptr == env)	/* cannot do conversion */
                        value = default_value;
#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
        }

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
	return (value);
}


GLOBAL char *STRINGgetenv IFN2(char *, name, char *, default_value)
{
        char *env;
        char *value;

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif

	value = default_value;
	env = host_getenv(name);
	if (env)
	{
                value = env;
#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
        }

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
	return (value);
}
