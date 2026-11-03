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
#line 39
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 41

#ifndef NTVDM
#line 43
#error DIVERGENCE MVDM-HOST-DIV-591: non-NTVDM branch is not selected
#endif
#line 81


GLOBAL IBOOL IBOOLgetenv IFN2(char *, name, IBOOL, default_value)
{
        char *env;
        IBOOL value;

#ifndef NTVDM
#line 89
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 92

	value = default_value;
	env = host_getenv(name);
	if (env)
        {
#ifdef NTVDM
            value = _stricmp(env, "TRUE") == 0;
#else
#line 100
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 110

        }

#ifndef NTVDM
#line 114
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 120

	return (value);
}


GLOBAL ISM32 ISM32getenv IFN2(char *, name, ISM32, default_value)
{
        char *env, *ptr;
        ISM32 value;

#ifndef NTVDM
#line 131
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 134

	value = default_value;
	env = host_getenv(name);
	if (env)
	{
		value = strtol(env, &ptr, 0);
		if (ptr == env)	/* cannot do conversion */
                        value = default_value;
#ifndef NTVDM
#line 143
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 145
        }

#ifndef NTVDM
#line 148
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 153
	return (value);
}


GLOBAL char *STRINGgetenv IFN2(char *, name, char *, default_value)
{
        char *env;
        char *value;

#ifndef NTVDM
#line 163
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 166

	value = default_value;
	env = host_getenv(name);
	if (env)
	{
                value = env;
#ifndef NTVDM
#line 173
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 175
        }

#ifndef NTVDM
#line 178
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 186
	return (value);
}
