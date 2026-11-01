/*[
 * Generated File: cpu4gen.h
 *
]*/

#ifndef _CPU4GEN_H_
#define _CPU4GEN_H_

#include <gdpvar.h>	/* For direct access getAX() etc. */

struct	CpuVector	{
#ifdef	CPU_PRIVATE
	struct	CpuPrivateVector	*Private;
#else	/* !CPU_PRIVATE */
	IHP	Private;
#endif	/* CPU_PRIVATE */
#ifdef	CPU_PRIVATE
	struct	SasVector	*Sas;
#else	/* !CPU_PRIVATE */
	IHP	Sas;
#endif	/* CPU_PRIVATE */
#ifdef	CPU_PRIVATE
	struct	VideoVector	*Video;
#else	/* !CPU_PRIVATE */
	IHP	Video;
#endif	/* CPU_PRIVATE */
	void	(*Simulate)	IPT0();
	void	(*Interrupt)	IPT2(CPU_INT_TYPE,	intType, IU16,	intNum);
	void	(*ClearHwInt)	IPT0();
	void	(*EndOfApplication)	IPT0();
	void	(*Terminate)	IPT0();
	void	(*Initialise)	IPT0();
	IU32	(*EffectiveAddr)	IPT2(IU16,	seg, IU32,	offset);
	void	(*SetQuickEventCount)	IPT1(IU32,	val);
	IU32	(*GetQuickEventCount)	IPT0();
	void	(*InitIOS)	IPT4(IHP,	InTables, IHP,	OutTables, IUH,	maxAdaptor, IU16,	portMask);
	void	(*DefineInb)	IPT2(IUH,	adaptor, IHP,	func);
	void	(*DefineInw)	IPT2(IUH,	adaptor, IHP,	func);
	void	(*DefineInd)	IPT2(IUH,	adaptor, IHP,	func);
	void	(*DefineOutb)	IPT2(IUH,	adaptor, IHP,	func);
	void	(*DefineOutw)	IPT2(IUH,	adaptor, IHP,	func);
	void	(*DefineOutd)	IPT2(IUH,	adaptor, IHP,	func);
	void	(*SetAL)	IPT1(IU8,	val);
	void	(*SetAH)	IPT1(IU8,	val);
	void	(*SetAX)	IPT1(IU16,	val);
	void	(*SetEAX)	IPT1(IU32,	val);
	void	(*SetBL)	IPT1(IU8,	val);
	void	(*SetBH)	IPT1(IU8,	val);
	void	(*SetBX)	IPT1(IU16,	val);
	void	(*SetEBX)	IPT1(IU32,	val);
	void	(*SetCL)	IPT1(IU8,	val);
	void	(*SetCH)	IPT1(IU8,	val);
	void	(*SetCX)	IPT1(IU16,	val);
	void	(*SetECX)	IPT1(IU32,	val);
	void	(*SetDL)	IPT1(IU8,	val);
	void	(*SetDH)	IPT1(IU8,	val);
	void	(*SetDX)	IPT1(IU16,	val);
	void	(*SetEDX)	IPT1(IU32,	val);
	void	(*SetSI)	IPT1(IU16,	val);
	void	(*SetESI)	IPT1(IU32,	val);
	void	(*SetDI)	IPT1(IU16,	val);
	void	(*SetEDI)	IPT1(IU32,	val);
	void	(*SetSP)	IPT1(IU16,	val);
	void	(*SetESP)	IPT1(IU32,	val);
	void	(*SetBP)	IPT1(IU16,	val);
	void	(*SetEBP)	IPT1(IU32,	val);
	void	(*SetIP)	IPT1(IU16,	val);
	void	(*SetEIP)	IPT1(IU32,	val);
	IUH	(*SetCS)	IPT1(IU16,	val);
	IUH	(*SetSS)	IPT1(IU16,	val);
	IUH	(*SetDS)	IPT1(IU16,	val);
	IUH	(*SetES)	IPT1(IU16,	val);
	IUH	(*SetFS)	IPT1(IU16,	val);
	IUH	(*SetGS)	IPT1(IU16,	val);
	void	(*SetEFLAGS)	IPT1(IU32,	val);
	void	(*SetSTATUS)	IPT1(IU16,	val);
	void	(*SetIOPL)	IPT1(IU8,	val);
	void	(*SetMSW)	IPT1(IU16,	val);
	void	(*SetCR0)	IPT1(IU32,	val);
	void	(*SetCR2)	IPT1(IU32,	val);
	void	(*SetCR3)	IPT1(IU32,	val);
	void	(*SetCF)	IPT1(IBOOL,	val);
	void	(*SetPF)	IPT1(IBOOL,	val);
	void	(*SetAF)	IPT1(IBOOL,	val);
	void	(*SetZF)	IPT1(IBOOL,	val);
	void	(*SetSF)	IPT1(IBOOL,	val);
	void	(*SetTF)	IPT1(IBOOL,	val);
	void	(*SetIF)	IPT1(IBOOL,	val);
	void	(*SetDF)	IPT1(IBOOL,	val);
	void	(*SetOF)	IPT1(IBOOL,	val);
	void	(*SetNT)	IPT1(IBOOL,	val);
	void	(*SetRF)	IPT1(IBOOL,	val);
	void	(*SetVM)	IPT1(IBOOL,	val);
	void	(*SetAC)	IPT1(IBOOL,	val);
	void	(*SetPE)	IPT1(IBOOL,	val);
	void	(*SetMP)	IPT1(IBOOL,	val);
	void	(*SetEM)	IPT1(IBOOL,	val);
	void	(*SetTS)	IPT1(IBOOL,	val);
	void	(*SetPG)	IPT1(IBOOL,	val);
	void	(*SetLDT_SELECTOR)	IPT1(IU16,	val);
	void	(*SetTR_SELECTOR)	IPT1(IU16,	val);
	IU8	(*GetAL)	IPT0();
	IU8	(*GetAH)	IPT0();
	IU16	(*GetAX)	IPT0();
	IU32	(*GetEAX)	IPT0();
	IU8	(*GetBL)	IPT0();
	IU8	(*GetBH)	IPT0();
	IU16	(*GetBX)	IPT0();
	IU32	(*GetEBX)	IPT0();
	IU8	(*GetCL)	IPT0();
	IU8	(*GetCH)	IPT0();
	IU16	(*GetCX)	IPT0();
	IU32	(*GetECX)	IPT0();
	IU8	(*GetDL)	IPT0();
	IU8	(*GetDH)	IPT0();
	IU16	(*GetDX)	IPT0();
	IU32	(*GetEDX)	IPT0();
	IU16	(*GetSI)	IPT0();
	IU32	(*GetESI)	IPT0();
	IU16	(*GetDI)	IPT0();
	IU32	(*GetEDI)	IPT0();
	IU16	(*GetSP)	IPT0();
	IU32	(*GetESP)	IPT0();
	IU16	(*GetBP)	IPT0();
	IU32	(*GetEBP)	IPT0();
	IU16	(*GetIP)	IPT0();
	IU32	(*GetEIP)	IPT0();
	IU16	(*GetCS)	IPT0();
	IU16	(*GetSS)	IPT0();
	IU16	(*GetDS)	IPT0();
	IU16	(*GetES)	IPT0();
	IU16	(*GetFS)	IPT0();
	IU16	(*GetGS)	IPT0();
	IU32	(*GetEFLAGS)	IPT0();
	IU16	(*GetSTATUS)	IPT0();
	IU8	(*GetIOPL)	IPT0();
	IU16	(*GetMSW)	IPT0();
	IU32	(*GetCR0)	IPT0();
	IU32	(*GetCR2)	IPT0();
	IU32	(*GetCR3)	IPT0();
	IBOOL	(*GetCF)	IPT0();
	IBOOL	(*GetPF)	IPT0();
	IBOOL	(*GetAF)	IPT0();
	IBOOL	(*GetZF)	IPT0();
	IBOOL	(*GetSF)	IPT0();
	IBOOL	(*GetTF)	IPT0();
	IBOOL	(*GetIF)	IPT0();
	IBOOL	(*GetDF)	IPT0();
	IBOOL	(*GetOF)	IPT0();
	IBOOL	(*GetNT)	IPT0();
	IBOOL	(*GetRF)	IPT0();
	IBOOL	(*GetVM)	IPT0();
	IBOOL	(*GetAC)	IPT0();
	IBOOL	(*GetPE)	IPT0();
	IBOOL	(*GetMP)	IPT0();
	IBOOL	(*GetEM)	IPT0();
	IBOOL	(*GetTS)	IPT0();
	IBOOL	(*GetET)	IPT0();
	IBOOL	(*GetNE)	IPT0();
	IBOOL	(*GetWP)	IPT0();
	IBOOL	(*GetPG)	IPT0();
	IU32	(*GetGDT_BASE)	IPT0();
	IU16	(*GetGDT_LIMIT)	IPT0();
	IU32	(*GetIDT_BASE)	IPT0();
	IU16	(*GetIDT_LIMIT)	IPT0();
	IU16	(*GetLDT_SELECTOR)	IPT0();
	IU32	(*GetLDT_BASE)	IPT0();
	IU32	(*GetLDT_LIMIT)	IPT0();
	IU16	(*GetTR_SELECTOR)	IPT0();
	IU32	(*GetTR_BASE)	IPT0();
	IU32	(*GetTR_LIMIT)	IPT0();
	IU16	(*GetTR_AR)	IPT0();
	IUH	(*GetJumpCalibrateVal)	IPT0();
	IUH	(*GetJumpInitialVal)	IPT0();
	void	(*SetJumpInitialVal)	IPT1(IUH,	initialVal);
	void	(*SetEOIEnable)	IPT1(IU8 *,	initialVal);
	void	(*SetAddProfileData)	IPT1(IHP,	initialVal);
	void	(*SetMaxProfileData)	IPT1(IHP,	initialVal);
	IHP	(*GetAddProfileDataAddr)	IPT0();
	void	(*PurgeLostIretHookLine)	IPT2(IU16,	lineNum, IU32,	depth);
	void	(*ActivityCheckAfterTimeSlice)	IPT0();
	IBOOL	(*CheckCsSelectorAndEipForCallFarPatching)	IPT3(IU16,	csSel, IU32,	eip, IU32 *,	pCsBase);
};

extern	struct	CpuVector	Cpu;

#ifdef	CCPU
IMPORT	void	c_cpu_simulate	IPT0();
#define	cpu_simulate	c_cpu_simulate
#else	/* CCPU */
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 193

#ifdef	CCPU
IMPORT	void	c_cpu_interrupt	IPT2(CPU_INT_TYPE, intType, IU16, intNum);
#define	cpu_interrupt	c_cpu_interrupt
#else	/* CCPU */
#line 198
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 200

#ifdef	CCPU
IMPORT	void	c_cpu_clearHwInt	IPT0();
#define	cpu_clearHwInt	c_cpu_clearHwInt
#else	/* CCPU */
#line 205
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 207

#ifdef	CCPU
IMPORT	void	c_cpu_EOA_hook	IPT0();
#define	cpu_EOA_hook	c_cpu_EOA_hook
#else	/* CCPU */
#line 212
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 214

#ifdef	CCPU
IMPORT	void	c_cpu_terminate	IPT0();
#define	cpu_terminate()	c_cpu_terminate()
#else	/* CCPU */
#line 219
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 221

#ifdef	CCPU
IMPORT	void	c_cpu_init	IPT0();
#define	cpu_init	c_cpu_init
#else	/* CCPU */
#line 226
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 228

#ifdef	CCPU
IMPORT	IU32	c_effective_addr	IPT2(IU16, seg, IU32, offset);
#define	effective_addr(seg, offset)	c_effective_addr(seg, offset)
#else	/* CCPU */
#line 233
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 235

#ifdef	CCPU
IMPORT	void	c_cpu_q_ev_set_count	IPT1(IU32, val);
#define	host_q_ev_set_count	c_cpu_q_ev_set_count
#else	/* CCPU */
#line 240
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 242

#ifdef	CCPU
IMPORT	IU32	c_cpu_q_ev_get_count	IPT0();
#define	host_q_ev_get_count	c_cpu_q_ev_get_count
#else	/* CCPU */
#line 247
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 249

#ifdef	CCPU
IMPORT	void	c_cpu_init_ios_in	IPT4(IHP, InTables, IHP, OutTables, IUH, maxAdaptor, IU16, portMask);
#define	cpu_init_ios_in	c_cpu_init_ios_in
#else	/* CCPU */
#line 254
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 256

#ifdef	CCPU
IMPORT	void	c_cpu_define_inb	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_inb	c_cpu_define_inb
#else	/* CCPU */
#line 261
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 263

#ifdef	CCPU
IMPORT	void	c_cpu_define_inw	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_inw	c_cpu_define_inw
#else	/* CCPU */
#line 268
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 270

#ifdef	CCPU
IMPORT	void	c_cpu_define_ind	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_ind	c_cpu_define_ind
#else	/* CCPU */
#line 275
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 277

#ifdef	CCPU
IMPORT	void	c_cpu_define_outb	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_outb	c_cpu_define_outb
#else	/* CCPU */
#line 282
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 284

#ifdef	CCPU
IMPORT	void	c_cpu_define_outw	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_outw	c_cpu_define_outw
#else	/* CCPU */
#line 289
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 291

#ifdef	CCPU
IMPORT	void	c_cpu_define_outd	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_outd	c_cpu_define_outd
#else	/* CCPU */
#line 296
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 298

#ifdef	CCPU
IMPORT	void	c_setAL	IPT1(IU8, val);
#define	setAL(val)	c_setAL(val)
#else	/* CCPU */
#line 303
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 305

#ifdef	CCPU
IMPORT	void	c_setAH	IPT1(IU8, val);
#define	setAH(val)	c_setAH(val)
#else	/* CCPU */
#line 310
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 312

#ifdef	CCPU
IMPORT	void	c_setAX	IPT1(IU16, val);
#define	setAX(val)	c_setAX(val)
#else	/* CCPU */
#line 317
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 319

#ifdef	CCPU
IMPORT	void	c_setEAX	IPT1(IU32, val);
#define	setEAX(val)	c_setEAX(val)
#else	/* CCPU */
#line 324
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 326

#ifdef	CCPU
IMPORT	void	c_setBL	IPT1(IU8, val);
#define	setBL(val)	c_setBL(val)
#else	/* CCPU */
#line 331
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 333

#ifdef	CCPU
IMPORT	void	c_setBH	IPT1(IU8, val);
#define	setBH(val)	c_setBH(val)
#else	/* CCPU */
#line 338
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 340

#ifdef	CCPU
IMPORT	void	c_setBX	IPT1(IU16, val);
#define	setBX(val)	c_setBX(val)
#else	/* CCPU */
#line 345
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 347

#ifdef	CCPU
IMPORT	void	c_setEBX	IPT1(IU32, val);
#define	setEBX(val)	c_setEBX(val)
#else	/* CCPU */
#line 352
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 354

#ifdef	CCPU
IMPORT	void	c_setCL	IPT1(IU8, val);
#define	setCL(val)	c_setCL(val)
#else	/* CCPU */
#line 359
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 361

#ifdef	CCPU
IMPORT	void	c_setCH	IPT1(IU8, val);
#define	setCH(val)	c_setCH(val)
#else	/* CCPU */
#line 366
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 368

#ifdef	CCPU
IMPORT	void	c_setCX	IPT1(IU16, val);
#define	setCX(val)	c_setCX(val)
#else	/* CCPU */
#line 373
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 375

#ifdef	CCPU
IMPORT	void	c_setECX	IPT1(IU32, val);
#define	setECX(val)	c_setECX(val)
#else	/* CCPU */
#line 380
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 382

#ifdef	CCPU
IMPORT	void	c_setDL	IPT1(IU8, val);
#define	setDL(val)	c_setDL(val)
#else	/* CCPU */
#line 387
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 389

#ifdef	CCPU
IMPORT	void	c_setDH	IPT1(IU8, val);
#define	setDH(val)	c_setDH(val)
#else	/* CCPU */
#line 394
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 396

#ifdef	CCPU
IMPORT	void	c_setDX	IPT1(IU16, val);
#define	setDX(val)	c_setDX(val)
#else	/* CCPU */
#line 401
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 403

#ifdef	CCPU
IMPORT	void	c_setEDX	IPT1(IU32, val);
#define	setEDX(val)	c_setEDX(val)
#else	/* CCPU */
#line 408
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 410

#ifdef	CCPU
IMPORT	void	c_setSI	IPT1(IU16, val);
#define	setSI(val)	c_setSI(val)
#else	/* CCPU */
#line 415
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 417

#ifdef	CCPU
IMPORT	void	c_setESI	IPT1(IU32, val);
#define	setESI(val)	c_setESI(val)
#else	/* CCPU */
#line 422
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 424

#ifdef	CCPU
IMPORT	void	c_setDI	IPT1(IU16, val);
#define	setDI(val)	c_setDI(val)
#else	/* CCPU */
#line 429
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 431

#ifdef	CCPU
IMPORT	void	c_setEDI	IPT1(IU32, val);
#define	setEDI(val)	c_setEDI(val)
#else	/* CCPU */
#line 436
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 438

#ifdef	CCPU
IMPORT	void	c_setSP	IPT1(IU16, val);
#define	setSP(val)	c_setSP(val)
#else	/* CCPU */
#line 443
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 445

#ifdef	CCPU
IMPORT	void	c_setESP	IPT1(IU32, val);
#define	setESP(val)	c_setESP(val)
#else	/* CCPU */
#line 450
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 452

#ifdef	CCPU
IMPORT	void	c_setBP	IPT1(IU16, val);
#define	setBP(val)	c_setBP(val)
#else	/* CCPU */
#line 457
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 459

#ifdef	CCPU
IMPORT	void	c_setEBP	IPT1(IU32, val);
#define	setEBP(val)	c_setEBP(val)
#else	/* CCPU */
#line 464
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 466

#ifdef	CCPU
IMPORT	void	c_setIP	IPT1(IU16, val);
#define	setIP(val)	c_setIP(val)
#else	/* CCPU */
#line 471
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 473

#ifdef	CCPU
IMPORT	void	c_setEIP	IPT1(IU32, val);
#define	setEIP(val)	c_setEIP(val)
#else	/* CCPU */
#line 478
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 480

#ifdef	CCPU
/* DIVERGENCE: c_reg.c defines the selected CCPU segment loaders as
 * ISM32(IU16).  Preserve the generated function shape with its actual
 * source return carrier so callers and the host selection table agree. */
IMPORT	ISM32	c_setCS	IPT1(IU16, val);
#define	setCS(val)	c_setCS(val)
#else	/* CCPU */
#line 488
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 490

#ifdef	CCPU
IMPORT	ISM32	c_setSS	IPT1(IU16, val);
#define	setSS(val)	c_setSS(val)
#else	/* CCPU */
#line 495
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 497

#ifdef	CCPU
IMPORT	ISM32	c_setDS	IPT1(IU16, val);
#define	setDS(val)	c_setDS(val)
#else	/* CCPU */
#line 502
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 504

#ifdef	CCPU
IMPORT	ISM32	c_setES	IPT1(IU16, val);
#define	setES(val)	c_setES(val)
#else	/* CCPU */
#line 509
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 511

#ifdef	CCPU
IMPORT	IUH	c_setFS	IPT1(IU16, val);
#define	setFS(val)	c_setFS(val)
#else	/* CCPU */
#line 516
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 518

#ifdef	CCPU
IMPORT	IUH	c_setGS	IPT1(IU16, val);
#define	setGS(val)	c_setGS(val)
#else	/* CCPU */
#line 523
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 525

#ifdef	CCPU
IMPORT	void	c_setEFLAGS	IPT1(IU32, val);
#define	setEFLAGS(val)	c_setEFLAGS(val)
#else	/* CCPU */
#line 530
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 532

#ifdef	CCPU
IMPORT	void	c_setSTATUS	IPT1(IU16, val);
#define	setSTATUS(val)	c_setSTATUS(val)
#else	/* CCPU */
#line 537
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 539

#ifdef	CCPU
IMPORT	void	c_setIOPL	IPT1(IU8, val);
#define	setIOPL(val)	c_setIOPL(val)
#else	/* CCPU */
#line 544
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 546

#ifdef	CCPU
IMPORT	void	c_setMSW	IPT1(IU16, val);
#define	setMSW(val)	c_setMSW(val)
#else	/* CCPU */
#line 551
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 553

#ifdef	CCPU
IMPORT	void	c_setCR0	IPT1(IU32, val);
#define	setCR0(val)	c_setCR0(val)
#else	/* CCPU */
#line 558
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 560

#ifdef	CCPU
IMPORT	void	c_setCR2	IPT1(IU32, val);
#define	setCR2(val)	c_setCR2(val)
#else	/* CCPU */
#line 565
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 567

#ifdef	CCPU
IMPORT	void	c_setCR3	IPT1(IU32, val);
#define	setCR3(val)	c_setCR3(val)
#else	/* CCPU */
#line 572
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 574

#ifdef	CCPU
IMPORT	void	c_setCF	IPT1(IBOOL, val);
#define	setCF(val)	c_setCF(val)
#else	/* CCPU */
#line 579
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 581

#ifdef	CCPU
IMPORT	void	c_setPF	IPT1(IBOOL, val);
#define	setPF(val)	c_setPF(val)
#else	/* CCPU */
#line 586
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 588

#ifdef	CCPU
IMPORT	void	c_setAF	IPT1(IBOOL, val);
#define	setAF(val)	c_setAF(val)
#else	/* CCPU */
#line 593
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 595

#ifdef	CCPU
IMPORT	void	c_setZF	IPT1(IBOOL, val);
#define	setZF(val)	c_setZF(val)
#else	/* CCPU */
#line 600
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 602

#ifdef	CCPU
IMPORT	void	c_setSF	IPT1(IBOOL, val);
#define	setSF(val)	c_setSF(val)
#else	/* CCPU */
#line 607
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 609

#ifdef	CCPU
IMPORT	void	c_setTF	IPT1(IBOOL, val);
#define	setTF(val)	c_setTF(val)
#else	/* CCPU */
#line 614
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 616

#ifdef	CCPU
IMPORT	void	c_setIF	IPT1(IBOOL, val);
#define	setIF(val)	c_setIF(val)
#else	/* CCPU */
#line 621
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 623

#ifdef	CCPU
IMPORT	void	c_setDF	IPT1(IBOOL, val);
#define	setDF(val)	c_setDF(val)
#else	/* CCPU */
#line 628
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 630

#ifdef	CCPU
IMPORT	void	c_setOF	IPT1(IBOOL, val);
#define	setOF(val)	c_setOF(val)
#else	/* CCPU */
#line 635
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 637

#ifdef	CCPU
IMPORT	void	c_setNT	IPT1(IBOOL, val);
#define	setNT(val)	c_setNT(val)
#else	/* CCPU */
#line 642
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 644

#ifdef	CCPU
IMPORT	void	c_setRF	IPT1(IBOOL, val);
#define	setRF(val)	c_setRF(val)
#else	/* CCPU */
#line 649
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 651

#ifdef	CCPU
IMPORT	void	c_setVM	IPT1(IBOOL, val);
#define	setVM(val)	c_setVM(val)
#else	/* CCPU */
#line 656
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 658

#ifdef	CCPU
IMPORT	void	c_setAC	IPT1(IBOOL, val);
#define	setAC(val)	c_setAC(val)
#else	/* CCPU */
#line 663
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 665

#ifdef	CCPU
IMPORT	void	c_setPE	IPT1(IBOOL, val);
#define	setPE(val)	c_setPE(val)
#else	/* CCPU */
#line 670
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 672

#ifdef	CCPU
IMPORT	void	c_setMP	IPT1(IBOOL, val);
#define	setMP(val)	c_setMP(val)
#else	/* CCPU */
#line 677
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 679

#ifdef	CCPU
IMPORT	void	c_setEM	IPT1(IBOOL, val);
#define	setEM(val)	c_setEM(val)
#else	/* CCPU */
#line 684
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 686

#ifdef	CCPU
IMPORT	void	c_setTS	IPT1(IBOOL, val);
#define	setTS(val)	c_setTS(val)
#else	/* CCPU */
#line 691
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 693

#ifdef	CCPU
IMPORT	void	c_setPG	IPT1(IBOOL, val);
#define	setPG(val)	c_setPG(val)
#else	/* CCPU */
#line 698
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 700

#ifdef	CCPU
IMPORT	void	c_setLDT_SELECTOR	IPT1(IU16, val);
#define	setLDT_SELECTOR(val)	c_setLDT_SELECTOR(val)
#else	/* CCPU */
#line 705
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 707

#ifdef	CCPU
IMPORT	void	c_setTR_SELECTOR	IPT1(IU16, val);
#define	setTR_SELECTOR(val)	c_setTR_SELECTOR(val)
#else	/* CCPU */
#line 712
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 714

#ifdef	CCPU
IMPORT	IU8	c_getAL	IPT0();
#define	getAL()	c_getAL()
#else	/* CCPU */
#line 719
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 721

#ifdef	CCPU
IMPORT	IU8	c_getAH	IPT0();
#define	getAH()	c_getAH()
#else	/* CCPU */
#line 726
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 728

#ifdef	CCPU
IMPORT	IU16	c_getAX	IPT0();
#define	getAX()	c_getAX()
#else	/* CCPU */
#line 733
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 735

#ifdef	CCPU
IMPORT	IU32	c_getEAX	IPT0();
#define	getEAX()	c_getEAX()
#else	/* CCPU */
#line 740
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 742

#ifdef	CCPU
IMPORT	IU8	c_getBL	IPT0();
#define	getBL()	c_getBL()
#else	/* CCPU */
#line 747
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 749

#ifdef	CCPU
IMPORT	IU8	c_getBH	IPT0();
#define	getBH()	c_getBH()
#else	/* CCPU */
#line 754
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 756

#ifdef	CCPU
IMPORT	IU16	c_getBX	IPT0();
#define	getBX()	c_getBX()
#else	/* CCPU */
#line 761
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 763

#ifdef	CCPU
IMPORT	IU32	c_getEBX	IPT0();
#define	getEBX()	c_getEBX()
#else	/* CCPU */
#line 768
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 770

#ifdef	CCPU
IMPORT	IU8	c_getCL	IPT0();
#define	getCL()	c_getCL()
#else	/* CCPU */
#line 775
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 777

#ifdef	CCPU
IMPORT	IU8	c_getCH	IPT0();
#define	getCH()	c_getCH()
#else	/* CCPU */
#line 782
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 784

#ifdef	CCPU
IMPORT	IU16	c_getCX	IPT0();
#define	getCX()	c_getCX()
#else	/* CCPU */
#line 789
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 791

#ifdef	CCPU
IMPORT	IU32	c_getECX	IPT0();
#define	getECX()	c_getECX()
#else	/* CCPU */
#line 796
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 798

#ifdef	CCPU
IMPORT	IU8	c_getDL	IPT0();
#define	getDL()	c_getDL()
#else	/* CCPU */
#line 803
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 805

#ifdef	CCPU
IMPORT	IU8	c_getDH	IPT0();
#define	getDH()	c_getDH()
#else	/* CCPU */
#line 810
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 812

#ifdef	CCPU
IMPORT	IU16	c_getDX	IPT0();
#define	getDX()	c_getDX()
#else	/* CCPU */
#line 817
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 819

#ifdef	CCPU
IMPORT	IU32	c_getEDX	IPT0();
#define	getEDX()	c_getEDX()
#else	/* CCPU */
#line 824
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 826

#ifdef	CCPU
IMPORT	IU16	c_getSI	IPT0();
#define	getSI()	c_getSI()
#else	/* CCPU */
#line 831
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 833

#ifdef	CCPU
IMPORT	IU32	c_getESI	IPT0();
#define	getESI()	c_getESI()
#else	/* CCPU */
#line 838
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 840

#ifdef	CCPU
IMPORT	IU16	c_getDI	IPT0();
#define	getDI()	c_getDI()
#else	/* CCPU */
#line 845
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 847

#ifdef	CCPU
IMPORT	IU32	c_getEDI	IPT0();
#define	getEDI()	c_getEDI()
#else	/* CCPU */
#line 852
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 854

#ifdef	CCPU
IMPORT	IU16	c_getSP	IPT0();
#define	getSP()	c_getSP()
#else	/* CCPU */
#line 859
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 861

#ifdef	CCPU
IMPORT	IU32	c_getESP	IPT0();
#define	getESP()	c_getESP()
#else	/* CCPU */
#line 866
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 868

#ifdef	CCPU
IMPORT	IU16	c_getBP	IPT0();
#define	getBP()	c_getBP()
#else	/* CCPU */
#line 873
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 875

#ifdef	CCPU
IMPORT	IU32	c_getEBP	IPT0();
#define	getEBP()	c_getEBP()
#else	/* CCPU */
#line 880
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 882

#ifdef	CCPU
IMPORT	IU16	c_getIP	IPT0();
#define	getIP()	c_getIP()
#else	/* CCPU */
#line 887
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 889

#ifdef	CCPU
IMPORT	IU32	c_getEIP	IPT0();
#define	getEIP()	c_getEIP()
#else	/* CCPU */
#line 894
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 896

#ifdef	CCPU
IMPORT	IU16	c_getCS	IPT0();
#define	getCS()	c_getCS()
#else	/* CCPU */
#line 901
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 903

#ifdef	CCPU
IMPORT	IU16	c_getSS	IPT0();
#define	getSS()	c_getSS()
#else	/* CCPU */
#line 908
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 910

#ifdef	CCPU
IMPORT	IU16	c_getDS	IPT0();
#define	getDS()	c_getDS()
#else	/* CCPU */
#line 915
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 917

#ifdef	CCPU
IMPORT	IU16	c_getES	IPT0();
#define	getES()	c_getES()
#else	/* CCPU */
#line 922
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 924

#ifdef	CCPU
IMPORT	IU16	c_getFS	IPT0();
#define	getFS()	c_getFS()
#else	/* CCPU */
#line 929
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 931

#ifdef	CCPU
IMPORT	IU16	c_getGS	IPT0();
#define	getGS()	c_getGS()
#else	/* CCPU */
#line 936
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 938

#ifdef	CCPU
IMPORT	IU32	c_getEFLAGS	IPT0();
#define	getEFLAGS()	c_getEFLAGS()
#else	/* CCPU */
#line 943
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 945

#ifdef	CCPU
IMPORT	IU16	c_getSTATUS	IPT0();
#define	getSTATUS()	c_getSTATUS()
#else	/* CCPU */
#line 950
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 952

#ifdef	CCPU
IMPORT	IU8	c_getIOPL	IPT0();
#define	getIOPL()	c_getIOPL()
#else	/* CCPU */
#line 957
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 959

#ifdef	CCPU
IMPORT	IU16	c_getMSW	IPT0();
#define	getMSW()	c_getMSW()
#else	/* CCPU */
#line 964
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 966

#ifdef	CCPU
IMPORT	IU32	c_getCR0	IPT0();
#define	getCR0()	c_getCR0()
#else	/* CCPU */
#line 971
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 973

#ifdef	CCPU
IMPORT	IU32	c_getCR2	IPT0();
#define	getCR2()	c_getCR2()
#else	/* CCPU */
#line 978
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 980

#ifdef	CCPU
IMPORT	IU32	c_getCR3	IPT0();
#define	getCR3()	c_getCR3()
#else	/* CCPU */
#line 985
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 987

#ifdef	CCPU
/* DIVERGENCE: c_reg.c defines all CCPU flag getters as ISM32.  Use that
 * original implementation carrier rather than the incompatible generated
 * IBOOL declarations. */
IMPORT	ISM32	c_getCF	IPT0();
#define	getCF()	c_getCF()
#else	/* CCPU */
#line 995
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 997

#ifdef	CCPU
IMPORT	ISM32	c_getPF	IPT0();
#define	getPF()	c_getPF()
#else	/* CCPU */
#line 1002
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1004

#ifdef	CCPU
IMPORT	ISM32	c_getAF	IPT0();
#define	getAF()	c_getAF()
#else	/* CCPU */
#line 1009
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1011

#ifdef	CCPU
IMPORT	ISM32	c_getZF	IPT0();
#define	getZF()	c_getZF()
#else	/* CCPU */
#line 1016
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1018

#ifdef	CCPU
IMPORT	ISM32	c_getSF	IPT0();
#define	getSF()	c_getSF()
#else	/* CCPU */
#line 1023
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1025

#ifdef	CCPU
IMPORT	ISM32	c_getTF	IPT0();
#define	getTF()	c_getTF()
#else	/* CCPU */
#line 1030
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1032

#ifdef	CCPU
IMPORT	ISM32	c_getIF	IPT0();
#define	getIF()	c_getIF()
#else	/* CCPU */
#line 1037
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1039

#ifdef	CCPU
IMPORT	ISM32	c_getDF	IPT0();
#define	getDF()	c_getDF()
#else	/* CCPU */
#line 1044
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1046

#ifdef	CCPU
IMPORT	ISM32	c_getOF	IPT0();
#define	getOF()	c_getOF()
#else	/* CCPU */
#line 1051
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1053

#ifdef	CCPU
IMPORT	IBOOL	c_getNT	IPT0();
#define	getNT()	c_getNT()
#else	/* CCPU */
#line 1058
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1060

#ifdef	CCPU
IMPORT	IBOOL	c_getRF	IPT0();
#define	getRF()	c_getRF()
#else	/* CCPU */
#line 1065
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1067

#ifdef	CCPU
IMPORT	IBOOL	c_getVM	IPT0();
#define	getVM()	c_getVM()
#else	/* CCPU */
#line 1072
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1074

#ifdef	CCPU
IMPORT	IBOOL	c_getAC	IPT0();
#define	getAC()	c_getAC()
#else	/* CCPU */
#line 1079
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1081

#ifdef	CCPU
IMPORT	IBOOL	c_getPE	IPT0();
#define	getPE()	c_getPE()
#else	/* CCPU */
#line 1086
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1088

#ifdef	CCPU
IMPORT	IBOOL	c_getMP	IPT0();
#define	getMP()	c_getMP()
#else	/* CCPU */
#line 1093
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1095

#ifdef	CCPU
IMPORT	IBOOL	c_getEM	IPT0();
#define	getEM()	c_getEM()
#else	/* CCPU */
#line 1100
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1102

#ifdef	CCPU
IMPORT	IBOOL	c_getTS	IPT0();
#define	getTS()	c_getTS()
#else	/* CCPU */
#line 1107
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1109

#ifdef	CCPU
IMPORT	IBOOL	c_getET	IPT0();
#define	getET()	c_getET()
#else	/* CCPU */
#line 1114
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1116

#ifdef	CCPU
IMPORT	IBOOL	c_getNE	IPT0();
#define	getNE()	c_getNE()
#else	/* CCPU */
#line 1121
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1123

#ifdef	CCPU
IMPORT	IBOOL	c_getWP	IPT0();
#define	getWP()	c_getWP()
#else	/* CCPU */
#line 1128
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1130

#ifdef	CCPU
IMPORT	IBOOL	c_getPG	IPT0();
#define	getPG()	c_getPG()
#else	/* CCPU */
#line 1135
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1137

#ifdef	CCPU
IMPORT	IU32	c_getGDT_BASE	IPT0();
#define	getGDT_BASE()	c_getGDT_BASE()
#else	/* CCPU */
#line 1142
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1144

#ifdef	CCPU
IMPORT	IU16	c_getGDT_LIMIT	IPT0();
#define	getGDT_LIMIT()	c_getGDT_LIMIT()
#else	/* CCPU */
#line 1149
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1151

#ifdef	CCPU
IMPORT	IU32	c_getIDT_BASE	IPT0();
#define	getIDT_BASE()	c_getIDT_BASE()
#else	/* CCPU */
#line 1156
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1158

#ifdef	CCPU
IMPORT	IU16	c_getIDT_LIMIT	IPT0();
#define	getIDT_LIMIT()	c_getIDT_LIMIT()
#else	/* CCPU */
#line 1163
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1165

#ifdef	CCPU
IMPORT	IU16	c_getLDT_SELECTOR	IPT0();
#define	getLDT_SELECTOR()	c_getLDT_SELECTOR()
#else	/* CCPU */
#line 1170
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1172

#ifdef	CCPU
IMPORT	IU32	c_getLDT_BASE	IPT0();
#define	getLDT_BASE()	c_getLDT_BASE()
#else	/* CCPU */
#line 1177
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1179

#ifdef	CCPU
IMPORT	IU32	c_getLDT_LIMIT	IPT0();
#define	getLDT_LIMIT()	c_getLDT_LIMIT()
#else	/* CCPU */
#line 1184
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1186

#ifdef	CCPU
IMPORT	IU16	c_getTR_SELECTOR	IPT0();
#define	getTR_SELECTOR()	c_getTR_SELECTOR()
#else	/* CCPU */
#line 1191
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1193

#ifdef	CCPU
IMPORT	IU32	c_getTR_BASE	IPT0();
#define	getTR_BASE()	c_getTR_BASE()
#else	/* CCPU */
#line 1198
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1200

#ifdef	CCPU
IMPORT	IU32	c_getTR_LIMIT	IPT0();
#define	getTR_LIMIT()	c_getTR_LIMIT()
#else	/* CCPU */
#line 1205
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1207

#ifdef	CCPU
IMPORT	IU16	c_getTR_AR	IPT0();
#define	getTR_AR()	c_getTR_AR()
#else	/* CCPU */
#line 1212
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1214

#ifdef	CCPU
IMPORT	IUH	host_get_q_calib_val	IPT0();
#define	host_get_q_calib_val	host_get_q_calib_val
#else	/* CCPU */
#line 1219
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1221

#ifdef	CCPU
IMPORT	IUH	host_get_jump_restart	IPT0();
#define	host_get_jump_restart	host_get_jump_restart
#else	/* CCPU */
#line 1226
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1228

#ifdef	CCPU
IMPORT	void	host_set_jump_restart	IPT1(IUH, initialVal);
#define	host_set_jump_restart(initialVal)	host_set_jump_restart(initialVal)
#else	/* CCPU */
#line 1233
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1235

#ifdef	CCPU
IMPORT	void	setEOIEnableAddr	IPT1(IU8 *, initialVal);
#define	setEOIEnableAddr(initialVal)	setEOIEnableAddr(initialVal)
#else	/* CCPU */
#line 1240
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1242

#ifdef	CCPU
IMPORT	void	setAddProfileDataPtr	IPT1(IHP, initialVal);
#define	setAddProfileDataPtr(initialVal)	setAddProfileDataPtr(initialVal)
#else	/* CCPU */
#line 1247
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1249

#ifdef	CCPU
IMPORT	void	setMaxProfileDataAddr	IPT1(IHP, initialVal);
#define	setMaxProfileDataAddr(initialVal)	setMaxProfileDataAddr(initialVal)
#else	/* CCPU */
#line 1254
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1256

#ifdef	CCPU
IMPORT	IHP	getAddProfileDataAddr	IPT0();
#define	getAddProfileDataAddr()	getAddProfileDataAddr()
#else	/* CCPU */
#line 1261
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1263

#ifdef	CCPU
IMPORT	void	PurgeLostIretHookLine	IPT2(IU16, lineNum, IU32, depth);
#define	PurgeLostIretHookLine(lineNum, depth)	PurgeLostIretHookLine(lineNum, depth)
#else	/* CCPU */
#line 1268
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1270

#ifdef	CCPU
IMPORT	void	ActivityCheckAfterTimeSlice	IPT0();
#define	ActivityCheckAfterTimeSlice()	ActivityCheckAfterTimeSlice()
#else	/* CCPU */
#line 1275
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1277

#ifdef	CCPU
IMPORT	IBOOL	CheckCsSelectorAndEipForCallFarPatching	IPT3(IU16, csSel, IU32, eip, IU32 *, pCsBase);
#define	CheckCsSelectorAndEipForCallFarPatching(csSel, eip, pCsBase)	CheckCsSelectorAndEipForCallFarPatching(csSel, eip, pCsBase)
#else	/* CCPU */
#line 1282
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1284

typedef struct CpuStateREC * TypeCpuStateRECptr;
typedef struct ConstraintBitMapREC * TypeConstraintBitMapRECptr;
typedef struct EntryPointCacheREC * TypeEntryPointCacheRECptr;

struct	CpuPrivateVector	{
	IHP	(*GetSadInfoTable)	IPT0();
	IBOOL	(*SetGDT_BASE_LIMIT)	IPT2(IU32,	base, IU16,	limit);
	IBOOL	(*SetIDT_BASE_LIMIT)	IPT2(IU32,	base, IU16,	limit);
	IBOOL	(*SetLDT_BASE_LIMIT)	IPT2(IU32,	base, IU32,	limit);
	IBOOL	(*SetTR_BASE_LIMIT)	IPT2(IU32,	base, IU32,	limit);
	IBOOL	(*SetTR_BASE_LIMIT_AR)	IPT3(IU32,	base, IU32,	limit, IU16,	ar);
	IBOOL	(*SetCS_BASE_LIMIT_AR)	IPT3(IU32,	base, IU32,	limit, IU16,	ar);
	IBOOL	(*SetSS_BASE_LIMIT_AR)	IPT3(IU32,	base, IU32,	limit, IU16,	ar);
	IBOOL	(*SetDS_BASE_LIMIT_AR)	IPT3(IU32,	base, IU32,	limit, IU16,	ar);
	IBOOL	(*SetES_BASE_LIMIT_AR)	IPT3(IU32,	base, IU32,	limit, IU16,	ar);
	IBOOL	(*SetFS_BASE_LIMIT_AR)	IPT3(IU32,	base, IU32,	limit, IU16,	ar);
	IBOOL	(*SetGS_BASE_LIMIT_AR)	IPT3(IU32,	base, IU32,	limit, IU16,	ar);
	void	(*SetCS_SELECTOR)	IPT1(IU16,	val);
	void	(*SetSS_SELECTOR)	IPT1(IU16,	val);
	void	(*SetDS_SELECTOR)	IPT1(IU16,	val);
	void	(*SetES_SELECTOR)	IPT1(IU16,	val);
	void	(*SetFS_SELECTOR)	IPT1(IU16,	val);
	void	(*SetGS_SELECTOR)	IPT1(IU16,	val);
	IU16	(*GetCS_SELECTOR)	IPT0();
	IU16	(*GetSS_SELECTOR)	IPT0();
	IU16	(*GetDS_SELECTOR)	IPT0();
	IU16	(*GetES_SELECTOR)	IPT0();
	IU16	(*GetFS_SELECTOR)	IPT0();
	IU16	(*GetGS_SELECTOR)	IPT0();
	IU32	(*GetCS_BASE)	IPT0();
	IU32	(*GetSS_BASE)	IPT0();
	IU32	(*GetDS_BASE)	IPT0();
	IU32	(*GetES_BASE)	IPT0();
	IU32	(*GetFS_BASE)	IPT0();
	IU32	(*GetGS_BASE)	IPT0();
	IU32	(*GetCS_LIMIT)	IPT0();
	IU32	(*GetSS_LIMIT)	IPT0();
	IU32	(*GetDS_LIMIT)	IPT0();
	IU32	(*GetES_LIMIT)	IPT0();
	IU32	(*GetFS_LIMIT)	IPT0();
	IU32	(*GetGS_LIMIT)	IPT0();
	IU16	(*GetCS_AR)	IPT0();
	IU16	(*GetSS_AR)	IPT0();
	IU16	(*GetDS_AR)	IPT0();
	IU16	(*GetES_AR)	IPT0();
	IU16	(*GetFS_AR)	IPT0();
	IU16	(*GetGS_AR)	IPT0();
	IUH	(*GetCPL)	IPT0();
	void	(*SetCPL)	IPT1(IUH,	prot);
	void	(*GetCpuState)	IPT1(TypeCpuStateRECptr,	state);
	void	(*SetCpuState)	IPT1(TypeCpuStateRECptr,	state);
	void	(*InitNanoCpu)	IPT1(IU32,	variety);
	void	(*PrepareBlocksToCompile)	IPT1(IU32,	variety);
	void	(*InitRdWrCacheAndCookies)	IPT1(IU32,	variety);
	void	(*ResetRdWrCacheAndCookies)	IPT1(IU32,	variety);
	void	(*SetRegConstraint)	IPT2(IU32,	regId, IU8,	constraintType);
	void	(*BpiCompileBPI)	IPT1(char *,	instructions);
	void	(*TrashIntelRegisters)	IPT0();
	void	(*FmDeleteAllStructures)	IPT1(IU32,	newCR0);
	void	(*SfForceVideoOff)	IPT0();
	void	(*SfRestoreVideoState)	IPT0();
	void	(*SfMarkPageAsParsed)	IPT1(IU32,	intelPpn);
	void	(*SfMarkPageAsNotParsed)	IPT1(IU32,	intelPpn);
	void	(*SfRemovePciMappings)	IPT0();
};

#ifdef	CCPU
IMPORT	IHP	c_getSadInfoTable	IPT0();
#define	getSadInfoTable()	c_getSadInfoTable()
#else	/* CCPU */
#line 1355
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1357

#ifdef	CCPU
IMPORT	IBOOL	c_setGDT_BASE_LIMIT	IPT2(IU32, base, IU16, limit);
#define	setGDT_BASE_LIMIT(base, limit)	c_setGDT_BASE_LIMIT(base, limit)
#else	/* CCPU */
#line 1362
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1364

#ifdef	CCPU
IMPORT	IBOOL	c_setIDT_BASE_LIMIT	IPT2(IU32, base, IU16, limit);
#define	setIDT_BASE_LIMIT(base, limit)	c_setIDT_BASE_LIMIT(base, limit)
#else	/* CCPU */
#line 1369
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1371

#ifdef	CCPU
IMPORT	IBOOL	c_setLDT_BASE_LIMIT	IPT2(IU32, base, IU32, limit);
#define	setLDT_BASE_LIMIT(base, limit)	c_setLDT_BASE_LIMIT(base, limit)
#else	/* CCPU */
#line 1376
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1378

#ifdef	CCPU
IMPORT	IBOOL	c_setTR_BASE_LIMIT	IPT2(IU32, base, IU32, limit);
#define	setTR_BASE_LIMIT(base, limit)	c_setTR_BASE_LIMIT(base, limit)
#else	/* CCPU */
#line 1383
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1385

#ifdef	CCPU
IMPORT	IBOOL	c_setTR_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setTR_BASE_LIMIT_AR(base, limit, ar)	c_setTR_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */
#line 1390
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1392

#ifdef	CCPU
IMPORT	IBOOL	c_setCS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setCS_BASE_LIMIT_AR(base, limit, ar)	c_setCS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */
#line 1397
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1399

#ifdef	CCPU
IMPORT	IBOOL	c_setSS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setSS_BASE_LIMIT_AR(base, limit, ar)	c_setSS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */
#line 1404
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1406

#ifdef	CCPU
IMPORT	IBOOL	c_setDS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setDS_BASE_LIMIT_AR(base, limit, ar)	c_setDS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */
#line 1411
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1413

#ifdef	CCPU
IMPORT	IBOOL	c_setES_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setES_BASE_LIMIT_AR(base, limit, ar)	c_setES_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */
#line 1418
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1420

#ifdef	CCPU
IMPORT	IBOOL	c_setFS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setFS_BASE_LIMIT_AR(base, limit, ar)	c_setFS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */
#line 1425
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1427

#ifdef	CCPU
IMPORT	IBOOL	c_setGS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setGS_BASE_LIMIT_AR(base, limit, ar)	c_setGS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */
#line 1432
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1434

#ifdef	CCPU
IMPORT	void	c_setCS_SELECTOR	IPT1(IU16, val);
#define	setCS_SELECTOR(val)	c_setCS_SELECTOR(val)
#else	/* CCPU */
#line 1439
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1441

#ifdef	CCPU
IMPORT	void	c_setSS_SELECTOR	IPT1(IU16, val);
#define	setSS_SELECTOR(val)	c_setSS_SELECTOR(val)
#else	/* CCPU */
#line 1446
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1448

#ifdef	CCPU
IMPORT	void	c_setDS_SELECTOR	IPT1(IU16, val);
#define	setDS_SELECTOR(val)	c_setDS_SELECTOR(val)
#else	/* CCPU */
#line 1453
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1455

#ifdef	CCPU
IMPORT	void	c_setES_SELECTOR	IPT1(IU16, val);
#define	setES_SELECTOR(val)	c_setES_SELECTOR(val)
#else	/* CCPU */
#line 1460
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1462

#ifdef	CCPU
IMPORT	void	c_setFS_SELECTOR	IPT1(IU16, val);
#define	setFS_SELECTOR(val)	c_setFS_SELECTOR(val)
#else	/* CCPU */
#line 1467
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1469

#ifdef	CCPU
IMPORT	void	c_setGS_SELECTOR	IPT1(IU16, val);
#define	setGS_SELECTOR(val)	c_setGS_SELECTOR(val)
#else	/* CCPU */
#line 1474
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1476

#ifdef	CCPU
IMPORT	IU16	c_getCS_SELECTOR	IPT0();
#define	getCS_SELECTOR()	c_getCS_SELECTOR()
#else	/* CCPU */
#line 1481
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1483

#ifdef	CCPU
IMPORT	IU16	c_getSS_SELECTOR	IPT0();
#define	getSS_SELECTOR()	c_getSS_SELECTOR()
#else	/* CCPU */
#line 1488
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1490

#ifdef	CCPU
IMPORT	IU16	c_getDS_SELECTOR	IPT0();
#define	getDS_SELECTOR()	c_getDS_SELECTOR()
#else	/* CCPU */
#line 1495
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1497

#ifdef	CCPU
IMPORT	IU16	c_getES_SELECTOR	IPT0();
#define	getES_SELECTOR()	c_getES_SELECTOR()
#else	/* CCPU */
#line 1502
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1504

#ifdef	CCPU
IMPORT	IU16	c_getFS_SELECTOR	IPT0();
#define	getFS_SELECTOR()	c_getFS_SELECTOR()
#else	/* CCPU */
#line 1509
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1511

#ifdef	CCPU
IMPORT	IU16	c_getGS_SELECTOR	IPT0();
#define	getGS_SELECTOR()	c_getGS_SELECTOR()
#else	/* CCPU */
#line 1516
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1518

#ifdef	CCPU
IMPORT	IU32	c_getCS_BASE	IPT0();
#define	getCS_BASE()	c_getCS_BASE()
#else	/* CCPU */
#line 1523
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1525

#ifdef	CCPU
IMPORT	IU32	c_getSS_BASE	IPT0();
#define	getSS_BASE()	c_getSS_BASE()
#else	/* CCPU */
#line 1530
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1532

#ifdef	CCPU
IMPORT	IU32	c_getDS_BASE	IPT0();
#define	getDS_BASE()	c_getDS_BASE()
#else	/* CCPU */
#line 1537
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1539

#ifdef	CCPU
IMPORT	IU32	c_getES_BASE	IPT0();
#define	getES_BASE()	c_getES_BASE()
#else	/* CCPU */
#line 1544
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1546

#ifdef	CCPU
IMPORT	IU32	c_getFS_BASE	IPT0();
#define	getFS_BASE()	c_getFS_BASE()
#else	/* CCPU */
#line 1551
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1553

#ifdef	CCPU
IMPORT	IU32	c_getGS_BASE	IPT0();
#define	getGS_BASE()	c_getGS_BASE()
#else	/* CCPU */
#line 1558
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1560

#ifdef	CCPU
IMPORT	IU32	c_getCS_LIMIT	IPT0();
#define	getCS_LIMIT()	c_getCS_LIMIT()
#else	/* CCPU */
#line 1565
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1567

#ifdef	CCPU
IMPORT	IU32	c_getSS_LIMIT	IPT0();
#define	getSS_LIMIT()	c_getSS_LIMIT()
#else	/* CCPU */
#line 1572
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1574

#ifdef	CCPU
IMPORT	IU32	c_getDS_LIMIT	IPT0();
#define	getDS_LIMIT()	c_getDS_LIMIT()
#else	/* CCPU */
#line 1579
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1581

#ifdef	CCPU
IMPORT	IU32	c_getES_LIMIT	IPT0();
#define	getES_LIMIT()	c_getES_LIMIT()
#else	/* CCPU */
#line 1586
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1588

#ifdef	CCPU
IMPORT	IU32	c_getFS_LIMIT	IPT0();
#define	getFS_LIMIT()	c_getFS_LIMIT()
#else	/* CCPU */
#line 1593
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1595

#ifdef	CCPU
IMPORT	IU32	c_getGS_LIMIT	IPT0();
#define	getGS_LIMIT()	c_getGS_LIMIT()
#else	/* CCPU */
#line 1600
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1602

#ifdef	CCPU
IMPORT	IU16	c_getCS_AR	IPT0();
#define	getCS_AR()	c_getCS_AR()
#else	/* CCPU */
#line 1607
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1609

#ifdef	CCPU
IMPORT	IU16	c_getSS_AR	IPT0();
#define	getSS_AR()	c_getSS_AR()
#else	/* CCPU */
#line 1614
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1616

#ifdef	CCPU
IMPORT	IU16	c_getDS_AR	IPT0();
#define	getDS_AR()	c_getDS_AR()
#else	/* CCPU */
#line 1621
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1623

#ifdef	CCPU
IMPORT	IU16	c_getES_AR	IPT0();
#define	getES_AR()	c_getES_AR()
#else	/* CCPU */
#line 1628
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1630

#ifdef	CCPU
IMPORT	IU16	c_getFS_AR	IPT0();
#define	getFS_AR()	c_getFS_AR()
#else	/* CCPU */
#line 1635
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1637

#ifdef	CCPU
IMPORT	IU16	c_getGS_AR	IPT0();
#define	getGS_AR()	c_getGS_AR()
#else	/* CCPU */
#line 1642
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1644

#ifdef	CCPU
IMPORT	IUH	c_getCPL	IPT0();
#define	getCPL()	c_getCPL()
#else	/* CCPU */
#line 1649
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1651

#ifdef	CCPU
IMPORT	void	c_setCPL	IPT1(IUH, prot);
#define	setCPL(prot)	c_setCPL(prot)
#else	/* CCPU */
#line 1656
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1658

#ifdef	CCPU
IMPORT	void	c_getCpuState	IPT1(TypeCpuStateRECptr, state);
#define	getCpuState(state)	c_getCpuState(state)
#else	/* CCPU */
#line 1663
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1665

#ifdef	CCPU
IMPORT	void	c_setCpuState	IPT1(TypeCpuStateRECptr, state);
#define	setCpuState(state)	c_setCpuState(state)
#else	/* CCPU */
#line 1670
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1672

#ifdef	CCPU
IMPORT	void	c_InitNanoCpu	IPT1(IU32, variety);
#define	initNanoCpu(variety)	c_InitNanoCpu(variety)
#else	/* CCPU */
#line 1677
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1679

#ifdef	CCPU
IMPORT	void	c_PrepareBlocksToCompile	IPT1(IU32, variety);
#define	prepareBlocksToCompile(variety)	c_PrepareBlocksToCompile(variety)
#else	/* CCPU */
#line 1684
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1686

#ifdef	CCPU
IMPORT	void	c_InitRdWrCacheAndCookies	IPT1(IU32, variety);
#define	initRdWrCacheAndCookies(variety)	c_InitRdWrCacheAndCookies(variety)
#else	/* CCPU */
#line 1691
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1693

#ifdef	CCPU
IMPORT	void	c_ResetRdWrCacheAndCookies	IPT1(IU32, variety);
#define	resetRdWrCacheAndCookies(variety)	c_ResetRdWrCacheAndCookies(variety)
#else	/* CCPU */
#line 1698
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1700

#ifdef	CCPU
IMPORT	void	c_setRegConstraint	IPT2(IU32, regId, IU8, constraintType);
#define	setRegConstraint(regId, constraintType)	c_setRegConstraint(regId, constraintType)
#else	/* CCPU */
#line 1705
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1707

#ifdef	CCPU

#else	/* CCPU */
#line 1711
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1713

#ifdef	CCPU
IMPORT	void	c_trashIntelRegisters	IPT0();
#define	trashIntelregisters	c_trashIntelRegisters
#else	/* CCPU */
#line 1718
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1720

#ifdef	CCPU

#else	/* CCPU */
#line 1724
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1726

#ifdef	CCPU

#else	/* CCPU */
#line 1730
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1732

#ifdef	CCPU

#else	/* CCPU */
#line 1736
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1738

#ifdef	CCPU

#else	/* CCPU */
#line 1742
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1744

#ifdef	CCPU

#else	/* CCPU */
#line 1748
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1750

#ifdef	CCPU

#else	/* CCPU */
#line 1754
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#ifdef PROD
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif	/* CCPU */
#line 1756

#endif	/* _CPU4GEN_H_ */
#line 1758
/*======================================== END ========================================*/
