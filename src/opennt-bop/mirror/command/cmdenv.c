
/*  cmdenv.c - Environment supporting functions for command.lib
 *
 *
 *  Modification History:
 *
 *  williamh 13-May-1993 Created
 */

/* OpenNT source: src/opennt/base/mvdm/dos/command/cmdenv.c.
 * Divergence: the admitted init-environment body is composed through the
 * narrow COMMAND environment shim rather than the historical CCPU/SAS/RTL
 * product include closure.  The original body retains all filtering, OEM
 * conversion, buffer-size and result rules. */
#define BX_NTVDM_COMMAND_ENV_ADMIT_INIT_ENV 1
#define BX_NTVDM_COMMAND_ENV_ADMIT_DYNAMIC 1
#define BX_NTVDM_COMMAND_ENV_ADMIT_XFORM 1
#define BX_NTVDM_COMMAND_ENV_ADMITTED_SLICE 1
#include "../../../opennt-host/capability/command_misc_shim.h"

#define VDM_ENV_INC_SIZE    512

CHAR windir[] = "windir";
extern BOOL fSeparateWow;

#if defined(BX_NTVDM_COMMAND_ENV_ADMIT_XFORM) || !defined(BX_NTVDM_COMMAND_ENV_ADMITTED_SLICE)

// Transform the given DOS environment to 32bits environment.
// WARNING!! The environment block we passed to 32bits must be in sort order.
//	     Therefore, we call RtlSetEnvironmentVariable to do the work
// The result string must be in ANSI character set.
BOOL	cmdXformEnvironment(PCHAR pEnv16, PANSI_STRING Env_A)
{
    UNICODE_STRING  Name_U, Value_U, Temp_U;
    STRING	    String;
    PWCHAR	    pwch, NewEnv, CurEnv, CurEnvCopy, pTmp;
    NTSTATUS	    Status;
    BOOL	    fFoundComSpec;
    USHORT	    NewEnvLen;

    if (pEnv16 == NULL)
	return FALSE;

    // flag true if we alread found comspec envirnment
    // !!!! Do we allow two or more comspec in environment????????
    fFoundComSpec = FALSE;

    /* DIVERGENCE: the original body snapshots the NT4 process environment.
     * The CLI owns this environment per VDM session instead, so the shim
     * materializes an equivalent private Unicode multisz.  The original
     * filtering, RtlSetEnvironmentVariable ordering, and OEM conversion
     * below remain unchanged. */
    CurEnv = bx_ntvdm_command_environment_snapshot();
    if (CurEnv == NULL)
	return FALSE;
    pwch = CurEnv;
    // figure how long the environment strings is
    while (*pwch != UNICODE_NULL || *(pwch + 1) != UNICODE_NULL)
	pwch++;

    // plus 2  to include the last two NULL chars
    CurEnvCopy = malloc((pwch - CurEnv + 2) * sizeof(WCHAR));
    if (!CurEnvCopy) {
	bx_ntvdm_command_environment_free_snapshot(CurEnv);
	return FALSE;
    }

    // make a copy of current process environment so we can walk through
    // it. The environment can be changed by any threads in the process
    // thus is not safe to walk through without a local copy
    RtlMoveMemory(CurEnvCopy, CurEnv, (pwch - CurEnv + 2) * sizeof(WCHAR));
    bx_ntvdm_command_environment_free_snapshot(CurEnv);

    // create a new environment block. We don't want to change
    // any currnt process environment variables, instead, we are
    // preparing a new one for the new process.
    Status = RtlCreateEnvironment(FALSE, (PVOID *)&NewEnv);
    if (!NT_SUCCESS(Status)) {
	free(CurEnvCopy);
	return FALSE;
    }
    NewEnvLen = 0;
    // now pick up environment we want from the current environment
    // and set it to the new environment block
    // the variables we want:
    // (1). comspec
    // (2). current directories settings

    pwch = CurEnvCopy;

    while (*pwch != UNICODE_NULL) {
	if (*pwch == L'=') {
	    // variable names started with L'=' are current directroy settings
	    pTmp = wcschr(pwch + 1, L'=');
	    if (pTmp) {
		if ((size_t)(pTmp - pwch) > USHRT_MAX / sizeof(WCHAR)) {
		    RtlDestroyEnvironment(NewEnv);
		    free(CurEnvCopy);
		    return FALSE;
		}
		Name_U.Buffer = pwch;
		Name_U.Length = (USHORT)((pTmp - pwch) * sizeof(WCHAR));
		RtlInitUnicodeString(&Value_U, pTmp + 1);
		Status = RtlSetEnvironmentVariable(&NewEnv, &Name_U, &Value_U);
		if (!NT_SUCCESS(Status)) {
		    RtlDestroyEnvironment(NewEnv);
		    free(CurEnvCopy);
		    return FALSE;
		}
		// <name> + <'='> + <value> + <'\0'>
		if (NewEnvLen > USHRT_MAX - Name_U.Length - Value_U.Length - 2 * sizeof(WCHAR)) {
		    RtlDestroyEnvironment(NewEnv);
		    free(CurEnvCopy);
		    return FALSE;
		}
		NewEnvLen += Name_U.Length + Value_U.Length + 2 * sizeof(WCHAR);
	    }
	}
	else if	(!fFoundComSpec) {
		fFoundComSpec = !_wcsnicmp(pwch, L"COMSPEC=", 8);
		if (fFoundComSpec) {
		    Name_U.Buffer = pwch;
		    Name_U.Length = 7 * sizeof(WCHAR);
		    RtlInitUnicodeString(&Value_U, pwch + 8);
		    Status = RtlSetEnvironmentVariable(&NewEnv,
						       &Name_U,
						       &Value_U
						       );
		    if (!NT_SUCCESS(Status)) {
			RtlDestroyEnvironment(NewEnv);
			free(CurEnvCopy);
			return FALSE;
		    }
		    if (NewEnvLen > USHRT_MAX - Name_U.Length - Value_U.Length - 2 * sizeof(WCHAR)) {
			RtlDestroyEnvironment(NewEnv);
			free(CurEnvCopy);
			return FALSE;
		    }
		    NewEnvLen += Name_U.Length + Value_U.Length + 2 * sizeof(WCHAR);
		}
	}
	pwch += wcslen(pwch) + 1;
    }
    // we are done with current process environment.
    free(CurEnvCopy);

    // now deal with 16bits settings passed from dos.
    // characters in 16bits environment are in OEM character set

    // 16bit comspec environment variable
    fFoundComSpec = FALSE;
    while (*pEnv16 != '\0') {
	RtlInitString(&String, pEnv16);
	// discard 16bits comspec
	if (!fFoundComSpec) {
	    fFoundComSpec = !_strnicmp(pEnv16, comspec, 8);
	    if (fFoundComSpec) {
		// ignore 16bits comspec environment
		pEnv16 += String.Length + 1;
		continue;
	    }
	}
	Status = RtlOemStringToUnicodeString(&Temp_U, &String, TRUE);
	if (!NT_SUCCESS(Status)) {
	    RtlDestroyEnvironment(NewEnv);
	    return FALSE;
	}
	pwch = wcschr(Temp_U.Buffer, L'=');
	if (pwch) {
	    if ((size_t)(pwch - Temp_U.Buffer) > USHRT_MAX / sizeof(WCHAR)) {
		RtlFreeUnicodeString(&Temp_U);
		RtlDestroyEnvironment(NewEnv);
		return FALSE;
	    }
	    Name_U.Buffer = Temp_U.Buffer;
	    Name_U.Length = (USHORT)((pwch - Temp_U.Buffer) * sizeof(WCHAR));
	    RtlInitUnicodeString(&Value_U, pwch + 1);
	    Status = RtlSetEnvironmentVariable( &NewEnv, &Name_U, &Value_U);
	    RtlFreeUnicodeString(&Temp_U);
	    if (!NT_SUCCESS(Status)) {
		RtlDestroyEnvironment(NewEnv);
		return FALSE;
	    }
	    if (NewEnvLen > USHRT_MAX - Name_U.Length - Value_U.Length - 2 * sizeof(WCHAR)) {
		RtlDestroyEnvironment(NewEnv);
		return FALSE;
	    }
	    NewEnvLen += Name_U.Length + Value_U.Length + 2 * sizeof(WCHAR);
	}
	pEnv16 += String.Length + 1;
    }
    // count the last terminated null char
    Temp_U.Length = NewEnvLen + sizeof(WCHAR);
    Temp_U.Buffer = NewEnv;
    Status = RtlUnicodeStringToAnsiString(Env_A, &Temp_U, TRUE);
    RtlDestroyEnvironment(NewEnv);	/* don't need it anymore */
    return(NT_SUCCESS(Status));
}

#endif /* BX_NTVDM_COMMAND_ENV_ADMIT_XFORM || !BX_NTVDM_COMMAND_ENV_ADMITTED_SLICE */




/* get ntvdm initial environment. This initial environment is necessary
 * for the first instance of command.com before it processing autoexec.bat
 * this function strips off an environment headed with "=" and
 * replace the comspec with 16bits comspec and upper case all environment vars.
 *
 * Entry: Client (ES:0) = buffer to receive the environment
 *	  Client (BX) = size in paragraphs of the given buffer
 *
 * Exit:  (BX) = 0 if nothing to copy
 *	  (BX)	<= the given size, function okay
 *	  (BX) > given size, (BX) has the required size
 */

#if defined(BX_NTVDM_COMMAND_ENV_ADMIT_INIT_ENV) || !defined(BX_NTVDM_COMMAND_ENV_ADMITTED_SLICE)
VOID cmdGetInitEnvironment(VOID)
{
    /* Defined initial state for modern analysis; original control flow assigns
     * this before use whenever the 16-bit environment is emitted. */
    CHAR *lpszzEnvBuffer = NULL, *lpszEnv;
    WORD cchEnvBuffer;
    CHAR *lpszzEnvStrings, * lpszz;
    WORD cchString;
    WORD cchRemain;
    WORD cchIncrement = MAX_PATH;
    BOOL fFoundComSpec = FALSE;
    BOOL fFoundWindir = FALSE;
    BOOL fVarIsWindir = FALSE;

    // if not during the initialization return nothing
    if (!IsFirstCall) {
	setBX(0);
	return;
    }
    if (cchInitEnvironment == 0) {
        //
        // If the PROMPT variable is not set, add it as $P$G. This is to
        // keep the command.com shell consistent with SCS cmd.exe(which
        // always does this) when we don't have a top level cmd shell.
        //
        {
           CHAR *pPromptStr = "PROMPT";
           char ach[2];

           if (!GetEnvironmentVariable(pPromptStr,ach,1)) {
                SetEnvironmentVariable(pPromptStr, "$P$G");
           }
        }

        cchRemain = 0;
	fFoundComSpec = FALSE;
	lpszEnv =
	lpszzEnvStrings = GetEnvironmentStrings();
	while (*lpszEnv) {
	    /* `cchString` is the original 16-bit COMMAND/DOS layout field. */
	    cchString = (WORD)(strlen(lpszEnv) + 1u);
	    cchVDMEnv32 += cchString;
	    lpszEnv += cchString;
	}
	lpszz = lpszzEnvStrings;

	if (lpszzVDMEnv32 != NULL)
	    free(lpszzVDMEnv32);
	lpszzVDMEnv32 = malloc(++cchVDMEnv32);
	if (lpszzVDMEnv32 == NULL) {
	    RcMessageBox(EG_MALLOC_FAILURE, NULL, NULL,
			 RMB_ICON_BANG | RMB_ABORT);
	    TerminateVDM();
        }

        RtlMoveMemory(lpszzVDMEnv32, lpszzEnvStrings, cchVDMEnv32);

	while (*lpszz != '\0') {
	    cchString = (WORD)(strlen(lpszz) + 1u);
	    if (*lpszz != '=') {

		if (!fFoundComSpec && !_strnicmp(lpszz, comspec, 8)){
		    fFoundComSpec = TRUE;
		    lpszz += cchString;
		    continue;
		}

		if (!fFoundWindir && !_strnicmp(lpszz, windir, 6)) {
		    fFoundWindir = TRUE;
		    if (fSeparateWow) {
			// starting a separate WOW box - flag this one so its
			// name won't be converted to uppercase later.
			fVarIsWindir = TRUE;
		    } else {
			// starting a DOS app, so remove "windir" to make sure
			// they don't think they are running under Windows.
			lpszz += cchString;
			continue;
		    }
		}

                if (cchRemain < cchString) {
                    if (cchIncrement < cchString)
                        cchIncrement = cchString;
		    lpszzEnvBuffer =
		    (CHAR *)realloc(lpszzInitEnvironment,
                                    cchInitEnvironment + cchRemain + cchIncrement
				    );
		    if (lpszzEnvBuffer == NULL) {
			if (lpszzInitEnvironment != NULL) {
			    free(lpszzInitEnvironment);
			    lpszzInitEnvironment = NULL;
			}
			cchInitEnvironment = 0;
			break;
		    }
		    lpszzInitEnvironment = lpszzEnvBuffer;
		    lpszzEnvBuffer += cchInitEnvironment;
                    cchRemain += cchIncrement;
		}
		// the environment strings from base is in ANSI and dos needs OEM
		AnsiToOemBuff(lpszz, lpszzEnvBuffer, cchString);
		// convert the name to upper case -- ONLY THE NAME, NOT VALUE.
		if (!fVarIsWindir && (lpszEnv = strchr(lpszzEnvBuffer, '=')) != NULL){
		    *lpszEnv = '\0';
		    _strupr(lpszzEnvBuffer);
		    *lpszEnv = '=';
		} else {
		    fVarIsWindir = FALSE;
		}
		cchRemain -= cchString;
		cchInitEnvironment += cchString ;
		lpszzEnvBuffer += cchString;
	    }
	    lpszz += cchString;
	}
	FreeEnvironmentStrings(lpszzEnvStrings);

	lpszzEnvBuffer = (CHAR *) realloc(lpszzInitEnvironment,
					  cchInitEnvironment + 1
					  );
	if (lpszzInitEnvironment != NULL ) {
	    lpszzInitEnvironment = lpszzEnvBuffer;
	    lpszzInitEnvironment[cchInitEnvironment++] = '\0';
	}
	else {
	    if (lpszzInitEnvironment != NULL) {
		free(lpszzInitEnvironment);
		lpszzInitEnvironment = NULL;
	    }
	    cchInitEnvironment = 0;
	}
    }
    lpszzEnvBuffer = (CHAR *) GetVDMAddr(getES(), 0);
    cchEnvBuffer =  (WORD)getBX() << 4;
    if (cchEnvBuffer < cchInitEnvironment + cbComSpec) {
        setBX((USHORT)((cchInitEnvironment + cbComSpec + 15) >> 4));
	return;
    }
    else {
	strncpy(lpszzEnvBuffer, lpszComSpec, cbComSpec);
	lpszzEnvBuffer += cbComSpec;
    }
    if (lpszzInitEnvironment != NULL) {
        setBX((USHORT)((cchInitEnvironment + cbComSpec + 15) >> 4));
	memcpy(lpszzEnvBuffer, lpszzInitEnvironment, cchInitEnvironment);
	free(lpszzInitEnvironment);
	lpszzInitEnvironment = NULL;
	cchInitEnvironment = 0;

    }
    else
	setBX(0);

    return;
}

#endif /* BX_NTVDM_COMMAND_ENV_ADMIT_INIT_ENV */



/** create a DOS environment for DOS.
    This is to get 32bits environment(comes with the dos executanle)
    and merge it with the environment settings in autoexec.nt so that
    COMMAND.COM gets the expected environment. We already created a
    double-null terminated string during autoexec.nt parsing. The string
    has mutltiple substring:
    "EnvName_1 NULL EnvValue_1 NULL[EnvName_n NULL EnvValue_n NULL] NULL"
    When name conflicts happened(a environment name was found in both
    16 bits and 32 bits), we do the merging based on the rules:
    get 16bits value, expands any environment variables in the string
    by using the current environment.

WARINING !!! The changes made by applications through directly manipulation
	     in command.com environment segment will be lost.

**/
#if defined(BX_NTVDM_COMMAND_ENV_ADMIT_DYNAMIC) || !defined(BX_NTVDM_COMMAND_ENV_ADMITTED_SLICE)
BOOL cmdCreateVDMEnvironment(
PVDMENVBLK  pVDMEnvBlk
)
{
PCHAR	p1, p2;
BOOL	fFoundComSpec;
BOOL	fFoundWindir;
BOOL	fVarIsWindir;
DWORD	Length;
PCHAR	lpszzVDMEnv, lpszzEnv;
CHAR	achBuffer[MAX_PATH + 1];

    pVDMEnvBlk->lpszzEnv = malloc(cchVDMEnv32 + cbComSpec + 1);
    if ((lpszzVDMEnv = pVDMEnvBlk->lpszzEnv) == NULL)
	return FALSE;

    pVDMEnvBlk->cchRemain = cchVDMEnv32 + cbComSpec + 1;
    pVDMEnvBlk->cchEnv = 0;

    // grab the 16bits comspec first
    if (cbComSpec && lpszComSpec && *lpszComSpec) {
	RtlCopyMemory(lpszzVDMEnv, lpszComSpec, cbComSpec);
	pVDMEnvBlk->cchEnv += cbComSpec;
	pVDMEnvBlk->cchRemain -= cbComSpec;
	lpszzVDMEnv += cbComSpec;
    }
    if (lpszzVDMEnv32) {

	// go through the given 32bits environmnet and take what we want:
	// everything except:
	// (1). variable name begin with '='
	// (2). compsec
	// (3). string without a '=' -- malformatted environment variable
	// (4). windir, so DOS apps don't think they're running under Windows
	// Note that strings pointed by lpszzVDMEnv32 are in ANSI character set


	fFoundComSpec = FALSE;
	fFoundWindir = FALSE;
	fVarIsWindir = FALSE;
	lpszzEnv = lpszzVDMEnv32;

	while (*lpszzEnv) {
	    /* DIVERGENCE: the original DWORD field carries bounded multisz byte
	     * counts.  Keep that ABI width explicit on x86/x64. */
	    Length = (DWORD)(strlen(lpszzEnv) + 1u);
	    if (*lpszzEnv != '=' &&
		(p1 = strchr(lpszzEnv, '=')) != NULL &&
		(fFoundComSpec || !(fFoundComSpec = _strnicmp(lpszzEnv,
							     comspec,
							     8
							    ) == 0)) ){
		if (!fFoundWindir) {
		    fFoundWindir = (_strnicmp(lpszzEnv,
							    windir,
							    6) == 0);
		    fVarIsWindir = fFoundWindir;
		}
		if (!fVarIsWindir || fSeparateWow) {
		    if (Length >= pVDMEnvBlk->cchRemain) {
			/* DIVERGENCE: the original fixed 512-byte increment assumes a
			 * single source variable never exceeds that increment.  Modern
			 * session input is length-bounded but may legitimately contain a
			 * larger PATH; retain the same realloc/ownership path while
			 * growing enough for this one source string plus its terminator. */
			DWORD Increment = VDM_ENV_INC_SIZE;
			if (Length >= pVDMEnvBlk->cchRemain + Increment)
			    Increment = Length - pVDMEnvBlk->cchRemain + 1u;
			lpszzVDMEnv = realloc(pVDMEnvBlk->lpszzEnv,
					      pVDMEnvBlk->cchEnv +
					      pVDMEnvBlk->cchRemain +
					      Increment
					     );
			if (lpszzVDMEnv == NULL){
			    free(pVDMEnvBlk->lpszzEnv);
			    return FALSE;
			}
			pVDMEnvBlk->cchRemain += Increment;
			pVDMEnvBlk->lpszzEnv = lpszzVDMEnv;
			lpszzVDMEnv += pVDMEnvBlk->cchEnv;
		    }
		    AnsiToOemBuff(lpszzEnv, lpszzVDMEnv, Length);
		    if (!fVarIsWindir) {
			*(lpszzVDMEnv + (DWORD)(p1 - lpszzEnv)) = '\0';
			_strupr(lpszzVDMEnv);
			*(lpszzVDMEnv + (DWORD)(p1 - lpszzEnv)) = '=';
		    } else {
			fVarIsWindir = FALSE;
		    }
		    pVDMEnvBlk->cchEnv += Length;
		    pVDMEnvBlk->cchRemain -= Length;
		    lpszzVDMEnv += Length;
                }
                else
                    fVarIsWindir = FALSE;
	    }
	    lpszzEnv += Length;
	}
    }
    *lpszzVDMEnv = '\0';
    pVDMEnvBlk->cchEnv++;
    pVDMEnvBlk->cchRemain--;

    if (lpszzcmdEnv16 != NULL) {
	lpszzEnv = lpszzcmdEnv16;

	while (*lpszzEnv) {
	    p1 = lpszzEnv + strlen(lpszzEnv) + 1;
	    p2 = NULL;
	    if (*p1) {
		p2 = achBuffer;
		// expand the strings pointed by p1
		Length = cmdExpandEnvironmentStrings(pVDMEnvBlk,
						     p1,
						     p2,
						     MAX_PATH + 1
						     );
		if (Length && Length > MAX_PATH) {
		    p2 =  (PCHAR) malloc(Length);
		    if (p2 == NULL) {
			free(pVDMEnvBlk->lpszzEnv);
			return FALSE;
		    }
		    cmdExpandEnvironmentStrings(pVDMEnvBlk,
						p1,
						p2,
						Length
					       );
		}
	    }
	    if (!cmdSetEnvironmentVariable(pVDMEnvBlk,
					   lpszzEnv,
					   p2
					   )){
		if (p2 && p2 != achBuffer)
		    free(p2);
		free(pVDMEnvBlk->lpszzEnv);
		return FALSE;
	    }
	    lpszzEnv = p1 + strlen(p1) + 1;
	}
    }
    lpszzVDMEnv = realloc(pVDMEnvBlk->lpszzEnv, pVDMEnvBlk->cchEnv);
    if (lpszzVDMEnv != NULL) {
	pVDMEnvBlk->lpszzEnv = lpszzVDMEnv;
	pVDMEnvBlk->cchRemain = 0;
    }
    return TRUE;
}


BOOL  cmdSetEnvironmentVariable(
PVDMENVBLK  pVDMEnvBlk,
PCHAR	lpszName,
PCHAR	lpszValue
)
{
    /* DIVERGENCE: initialize the historical search temporary so modern
     * flow analysis recognizes the source's short-circuit lookup ordering. */
    PCHAR   p, p1 = NULL, pEnd;
    DWORD   ExtraLength, Length, cchValue, cchOldValue;

    pVDMEnvBlk = (pVDMEnvBlk) ? pVDMEnvBlk : &cmdVDMEnvBlk;

    if (pVDMEnvBlk == NULL || lpszName == NULL)
	return FALSE;
    if (!(p = pVDMEnvBlk->lpszzEnv))
	return FALSE;
    pEnd = p + pVDMEnvBlk->cchEnv - 1;

	    /* DIVERGENCE: these are original DWORD environment-count fields, not
	     * host pointer arithmetic; retain their historical width explicitly. */
	    cchValue = (lpszValue) ? (DWORD)strlen(lpszValue) : 0u;

	    Length = (DWORD)strlen(lpszName);
    while (*p && ((p1 = strchr(p, '=')) == NULL ||
		  (DWORD)(p1 - p) != Length ||
		  _strnicmp(p, lpszName, Length)))
	p += strlen(p) + 1;

    if (*p) {
	// name was found in the base environment, replace it
	p1++;
		cchOldValue = (DWORD)strlen(p1);
	if (cchValue <= cchOldValue) {
	    if (!cchValue) {
		RtlMoveMemory(p,
			      p1 + cchOldValue + 1,
			      (DWORD)(pEnd - p) - cchOldValue
			     );
		pVDMEnvBlk->cchRemain += Length + cchOldValue + 2;
		pVDMEnvBlk->cchEnv -=  Length + cchOldValue + 2;
	    }
	    else {
		RtlCopyMemory(p1,
			      lpszValue,
			      cchValue
			     );
		if (cchValue != cchOldValue) {
		    RtlMoveMemory(p1 + cchValue,
				  p1 + cchOldValue,
				  (DWORD)(pEnd - p1) - cchOldValue + 1
				  );
		    pVDMEnvBlk->cchEnv -= cchOldValue - cchValue;
		    pVDMEnvBlk->cchRemain += cchOldValue - cchValue;
		}
	    }
	    return TRUE;
	}
	else {
	    // need more space for the new value
	    // we delete it from here and fall through
	    RtlMoveMemory(p,
			  p1 + cchOldValue + 1,
			  (DWORD)(pEnd - p1) - cchOldValue
			 );
	    pVDMEnvBlk->cchRemain += Length + 1 + cchOldValue + 1;
	    pVDMEnvBlk->cchEnv -= Length + 1 + cchOldValue + 1;
	}
    }
    if (cchValue) {
	ExtraLength = Length + 1 + cchValue + 1;
	if (pVDMEnvBlk->cchRemain  < ExtraLength) {
	    p = realloc(pVDMEnvBlk->lpszzEnv,
			pVDMEnvBlk->cchEnv + pVDMEnvBlk->cchRemain + ExtraLength
		       );
	    if (p == NULL)
		return FALSE;
	    pVDMEnvBlk->lpszzEnv = p;
	    pVDMEnvBlk->cchRemain += ExtraLength;
	}
	p = pVDMEnvBlk->lpszzEnv + pVDMEnvBlk->cchEnv - 1;
	RtlCopyMemory(p, lpszName, Length + 1);
	_strupr(p);
	p += Length;
	*p++ = '=';
	RtlCopyMemory(p, lpszValue, cchValue + 1);
	*(p + cchValue + 1) = '\0';
	pVDMEnvBlk->cchEnv += ExtraLength;
	pVDMEnvBlk->cchRemain -= ExtraLength;
	return TRUE;
    }
    return FALSE;

}


DWORD cmdExpandEnvironmentStrings(
PVDMENVBLK  pVDMEnvBlk,
PCHAR	lpszSrc,
PCHAR	lpszDst,
DWORD	cchDst
)
{


    DWORD   RequiredLength, RemainLength, Length;
    PCHAR   p1;

    RequiredLength = 0;
    RemainLength = (lpszDst) ? cchDst : 0;
    pVDMEnvBlk = (pVDMEnvBlk) ? pVDMEnvBlk : &cmdVDMEnvBlk;
    if (pVDMEnvBlk == NULL || lpszSrc == NULL)
	return 0;

    while(*lpszSrc) {
	if (*lpszSrc == '%') {
	    p1 = strchr(lpszSrc + 1, '%');
	    if (p1 != NULL) {
		if (p1 == lpszSrc + 1) {	// a "%%"
		    lpszSrc += 2;
		    continue;
		}
		*p1 = '\0';
		Length = cmdGetEnvironmentVariable(pVDMEnvBlk,
						   lpszSrc + 1,
						   lpszDst,
						   RemainLength
						  );
		*p1 = '%';
		lpszSrc = p1 + 1;
		if (Length) {
		    if (Length < RemainLength) {
			RemainLength -= Length;
			lpszDst += Length;
		    }
		    else {
			RemainLength = 0;
			Length --;
		    }
		    RequiredLength += Length;
		}
		continue;
	    }
	    else {
		 RequiredLength++;
		 if (RemainLength) {
		    *lpszDst++ = *lpszSrc;
		    RemainLength--;
		 }
		 lpszSrc++;
		 continue;
	    }
	}
	else {
	    RequiredLength++;
	    if (RemainLength) {
		*lpszDst++ = *lpszSrc;
		RemainLength--;
	    }
	    lpszSrc++;
	}
    }	// while(*lpszSrc)
    RequiredLength++;
    if (RemainLength)
	*lpszDst = '\0';
    return RequiredLength;
}

DWORD cmdGetEnvironmentVariable(
PVDMENVBLK pVDMEnvBlk,
PCHAR	lpszName,
PCHAR	lpszValue,
DWORD	cchValue
)
{

    DWORD   RequiredLength, Length;
    /* DIVERGENCE: p1 is assigned by the original short-circuit search before
     * use; initialize it for modern flow analysis without changing that flow. */
    PCHAR   p, p1 = NULL;

    pVDMEnvBlk = (pVDMEnvBlk) ? pVDMEnvBlk : &cmdVDMEnvBlk;
    if (pVDMEnvBlk == NULL || lpszName == NULL)
	return 0;

    RequiredLength = 0;
    /* DIVERGENCE: preserve OpenNT's DWORD count ABI on x86/x64 hosts. */
    Length = (DWORD)strlen(lpszName);

    // if the name is "windir", get its value from ntvdm process's environment
    // for DOS because we took it out of the environment block the application
    // will see.
    if (Length == 6 && !fSeparateWow && !_strnicmp(lpszName, windir, 6)) {
	return(GetEnvironmentVariableOem(lpszName, lpszValue, cchValue));
    }

    if ((p = pVDMEnvBlk->lpszzEnv) != NULL) {
       while (*p && ((p1 = strchr(p, '=')) == NULL ||
		     (DWORD)(p1 - p) != Length ||
		     _strnicmp(lpszName, p, Length)))
	    p += strlen(p) + 1;
       if (*p) {
	    RequiredLength = (DWORD)strlen(p1 + 1);
	    if (cchValue > RequiredLength && lpszValue)
		RtlCopyMemory(lpszValue, p1 + 1, RequiredLength + 1);
	    else
		RequiredLength++;
       }
    }
    return RequiredLength;
}

#endif /* BX_NTVDM_COMMAND_ENV_ADMIT_DYNAMIC || !BX_NTVDM_COMMAND_ENV_ADMITTED_SLICE */
