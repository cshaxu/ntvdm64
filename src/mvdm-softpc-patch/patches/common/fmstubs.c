#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>

#include "insignia.h"
#include "host_def.h"

#include "timestmp.h"
#include "nt_timer.h"
#include <intrin.h>

/* DIVERGENCE MVDM-SOFTPC-PATCH-002: preserve the patch's immediate debugger-
 * break contract with the one MSVC intrinsic that emits it on both x86/x64. */


#if defined(MVDM_SOFTPC_PATCH_EDL_FAST_BOP_ONLY)
/* DIVERGENCE(MVDM-SOFTPC-PATCH-004): this separately selected formal-link
 * object preserves only the reached EDL default contract. The full imported
 * patch body below contains unrelated empty placeholder hooks and is evidence
 * only; linking it would hide distinct unresolved SoftPC interfaces. */
VOID EDL_fast_bop(ULONG immed)
{
	/* Whatever this it... Better break on it and see if it gets used */
	__debugbreak();
}
#elif defined(MVDM_SOFTPC_PATCH_ACTIVITY_CHECK_ONLY)
/* DIVERGENCE(MVDM-SOFTPC-PATCH-006): select precisely the imported patch's
 * ActivityCheckAfterTimeSlice body for the original CPU40 idle/timeslice
 * caller cohort.  The full patch body contains unrelated placeholders, so
 * the formal product must not link it wholesale merely to obtain this
 * source-shaped empty callback. */
VOID ActivityCheckAfterTimeSlice(VOID)
{
}
#elif defined(MVDM_SOFTPC_PATCH_CCPU_VECTOR_DEFAULTS_ONLY)
/* DIVERGENCE(MVDM-SOFTPC-PATCH-005): the selected CCPU's generic SasVector
 * retains two MIPS/PPC vector tail slots even though no selected x86 CCPU
 * caller reaches them.  Retain the imported patch's immediate debug-break
 * failure direction, but use the generated vector's exact IUH width so the
 * x86 and x64 table call ABI is identical.  This object deliberately exports
 * only these original defaults and EDL_fast_bop; it does not activate the
 * patch's unrelated placeholder hooks. */
VOID EDL_fast_bop(ULONG immed)
{
	/* Whatever this it... Better break on it and see if it gets used */
	__debugbreak();
}

IU32 c_VirtualiseInstruction(IU32 eipInRom, IUH size,
                             IU32 linearAddrOrPort, IU32 dataIn)
{
	/* Hopefully this is never called... */
	__debugbreak();
	return 0;
}

IU8 *c_sas_touch(IU32 addr, IU32 length)
{
	/* Hopefully this is never called... */
	__debugbreak();
	return NULL;
}
#else

#if !defined(PROD) && defined(CPU_40_STYLE) 
void FmDebugBop()
{
#ifdef YODA
	force_yoda();
#else
	illegal_bop();
#endif
}

void	FmDebug(ULONG hook)
{
	__debugbreak();
}
#endif

VOID ActivityCheckAfterTimeSlice(VOID)
{
}

VOID ClearInstanceDataMarking(VOID)
{
	// No idea what this does, stub it...
}

VOID VdmSetPhysRecStructs(ULONG HostAddress, ULONG IntelAddress, ULONG Size)
{
	// TODO: Shoud set PhysicalPageREC.translation entries?
}

VOID EDL_fast_bop(ULONG immed)
{
	// Whatever this it... Better break on it and see if it gets used
	__debugbreak();
}

/* DIVERGENCE(MVDM-SOFTPC-PATCH-005): the imported placeholder must match the
 * generated CCPU vector's native-word size argument on both host widths. */
IU32 c_VirtualiseInstruction (IU32 eipInRom, IUH size, IU32 linearAddrOrPort, IU32 dataIn)
{
	// Hopefully this is never called...
	__debugbreak();
	return 0;
}

IU8 *c_sas_touch (IU32 addr, IU32 length)
{
	// Hopefully this is never called...
	__debugbreak();
	return NULL;
}

IUH host_q_timestamp_diff IPT2(QTIMESTAMP_PTR, tbegin, QTIMESTAMP_PTR, tend)
{
	LARGE_INTEGER DiffTime = *((PLARGE_INTEGER)tend);
	DiffTime.QuadPart -= ((PLARGE_INTEGER)tbegin)->QuadPart;
	return DiffTime.LowPart;
}

void host_q_write_timestamp IPT1(QTIMESTAMP_PTR, stamp)
{
	GetPerfCounterUsecs(NULL, (PLARGE_INTEGER)&stamp->data);
}

/* Not supported */
IUH host_get_q_calib_val	IFN0()
{
	return 0;
}

/* Stolen from video */
IUH host_get_jump_restart	IFN0()
{
	return 100;
}

#endif /* MVDM_SOFTPC_PATCH_EDL_FAST_BOP_ONLY */
