/*[
 * Generated File: cpu4gen.h
 *
]*/


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
	void	(*SetQuickEventCount)	IPT1(IU32,	val);
	IU32	(*GetQuickEventCount)	IPT0();
	IU32	(*CalcQuickEventInstTime)	IPT1(IU32,	val);
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
	void	(*PurgeLostIretHookLine)	IPT1(IU16,	lineNum);
};

extern	struct	CpuVector	Cpu;

#ifdef	CCPU
IMPORT	void	c_cpu_simulate	IPT0();
#define	cpu_simulate	c_cpu_simulate
#else	/* CCPU */

#ifdef PROD
#line 187
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 189
IMPORT	void	cpu_simulate	IPT0();
#endif /*PROD*/
#line 191

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_interrupt	IPT2(CPU_INT_TYPE, intType, IU16, intNum);
#define	cpu_interrupt	c_cpu_interrupt
#else	/* CCPU */

#ifdef PROD
#line 200
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 202
IMPORT	void	cpu_interrupt	IPT2(CPU_INT_TYPE, intType, IU16, intNum);
#endif /*PROD*/
#line 204

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_clearHwInt	IPT0();
#define	cpu_clearHwInt	c_cpu_clearHwInt
#else	/* CCPU */

#ifdef PROD
#line 213
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 215
IMPORT	void	cpu_clearHwInt	IPT0();
#endif /*PROD*/
#line 217

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_EOA_hook	IPT0();
#define	cpu_EOA_hook	c_cpu_EOA_hook
#else	/* CCPU */

#ifdef PROD
#line 226
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 228
IMPORT	void	cpu_EOA_hook	IPT0();
#endif /*PROD*/
#line 230

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_terminate	IPT0();
#define	cpu_terminate	c_cpu_terminate
#else	/* CCPU */

#ifdef PROD
#line 239
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 241
IMPORT	void	cpu_terminate	IPT0();
#endif /*PROD*/
#line 243

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_init	IPT0();
#define	cpu_init	c_cpu_init
#else	/* CCPU */

#ifdef PROD
#line 252
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 254
IMPORT	void	cpu_init	IPT0();
#endif /*PROD*/
#line 256

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_q_ev_set_count	IPT1(IU32, val);
#define	host_q_ev_set_count	c_cpu_q_ev_set_count
#else	/* CCPU */

#ifdef PROD
#line 265
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 267
IMPORT	void	host_q_ev_set_count	IPT1(IU32, val);
#endif /*PROD*/
#line 269

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_cpu_q_ev_get_count	IPT0();
#define	host_q_ev_get_count	c_cpu_q_ev_get_count
#else	/* CCPU */

#ifdef PROD
#line 278
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 280
IMPORT	IU32	host_q_ev_get_count	IPT0();
#endif /*PROD*/
#line 282

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_cpu_calc_q_ev_inst_for_time	IPT1(IU32, val);
#define	host_calc_q_ev_inst_for_time	c_cpu_calc_q_ev_inst_for_time
#else	/* CCPU */

#ifdef PROD
#line 291
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 293
IMPORT	IU32	host_calc_q_ev_inst_for_time	IPT1(IU32, val);
#endif /*PROD*/
#line 295

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_init_ios_in	IPT4(IHP, InTables, IHP, OutTables, IUH, maxAdaptor, IU16, portMask);
#define	cpu_init_ios_in	c_cpu_init_ios_in
#else	/* CCPU */

#ifdef PROD
#line 304
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 306
IMPORT	void	cpu_init_ios_in	IPT4(IHP, InTables, IHP, OutTables, IUH, maxAdaptor, IU16, portMask);
#endif /*PROD*/
#line 308

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_define_inb	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_inb	c_cpu_define_inb
#else	/* CCPU */

#ifdef PROD
#line 317
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 319
IMPORT	void	ios_define_inb	IPT2(IUH, adaptor, IHP, func);
#endif /*PROD*/
#line 321

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_define_inw	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_inw	c_cpu_define_inw
#else	/* CCPU */

#ifdef PROD
#line 330
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 332
IMPORT	void	ios_define_inw	IPT2(IUH, adaptor, IHP, func);
#endif /*PROD*/
#line 334

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_define_ind	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_ind	c_cpu_define_ind
#else	/* CCPU */

#ifdef PROD
#line 343
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 345
IMPORT	void	ios_define_ind	IPT2(IUH, adaptor, IHP, func);
#endif /*PROD*/
#line 347

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_define_outb	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_outb	c_cpu_define_outb
#else	/* CCPU */

#ifdef PROD
#line 356
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 358
IMPORT	void	ios_define_outb	IPT2(IUH, adaptor, IHP, func);
#endif /*PROD*/
#line 360

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_define_outw	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_outw	c_cpu_define_outw
#else	/* CCPU */

#ifdef PROD
#line 369
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 371
IMPORT	void	ios_define_outw	IPT2(IUH, adaptor, IHP, func);
#endif /*PROD*/
#line 373

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_cpu_define_outd	IPT2(IUH, adaptor, IHP, func);
#define	ios_define_outd	c_cpu_define_outd
#else	/* CCPU */

#ifdef PROD
#line 382
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 384
IMPORT	void	ios_define_outd	IPT2(IUH, adaptor, IHP, func);
#endif /*PROD*/
#line 386

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setAL	IPT1(IU8, val);
#define	setAL(val)	c_setAL(val)
#else	/* CCPU */

#ifdef PROD
#line 395
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 397
IMPORT	void	setAL	IPT1(IU8, val);
#endif /*PROD*/
#line 399

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setAH	IPT1(IU8, val);
#define	setAH(val)	c_setAH(val)
#else	/* CCPU */

#ifdef PROD
#line 408
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 410
IMPORT	void	setAH	IPT1(IU8, val);
#endif /*PROD*/
#line 412

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setAX	IPT1(IU16, val);
#define	setAX(val)	c_setAX(val)
#else	/* CCPU */

#ifdef PROD
#line 421
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 423
IMPORT	void	setAX	IPT1(IU16, val);
#endif /*PROD*/
#line 425

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setEAX	IPT1(IU32, val);
#define	setEAX(val)	c_setEAX(val)
#else	/* CCPU */

#ifdef PROD
#line 434
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 436
IMPORT	void	setEAX	IPT1(IU32, val);
#endif /*PROD*/
#line 438

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setBL	IPT1(IU8, val);
#define	setBL(val)	c_setBL(val)
#else	/* CCPU */

#ifdef PROD
#line 447
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 449
IMPORT	void	setBL	IPT1(IU8, val);
#endif /*PROD*/
#line 451

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setBH	IPT1(IU8, val);
#define	setBH(val)	c_setBH(val)
#else	/* CCPU */

#ifdef PROD
#line 460
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 462
IMPORT	void	setBH	IPT1(IU8, val);
#endif /*PROD*/
#line 464

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setBX	IPT1(IU16, val);
#define	setBX(val)	c_setBX(val)
#else	/* CCPU */

#ifdef PROD
#line 473
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 475
IMPORT	void	setBX	IPT1(IU16, val);
#endif /*PROD*/
#line 477

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setEBX	IPT1(IU32, val);
#define	setEBX(val)	c_setEBX(val)
#else	/* CCPU */

#ifdef PROD
#line 486
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 488
IMPORT	void	setEBX	IPT1(IU32, val);
#endif /*PROD*/
#line 490

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCL	IPT1(IU8, val);
#define	setCL(val)	c_setCL(val)
#else	/* CCPU */

#ifdef PROD
#line 499
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 501
IMPORT	void	setCL	IPT1(IU8, val);
#endif /*PROD*/
#line 503

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCH	IPT1(IU8, val);
#define	setCH(val)	c_setCH(val)
#else	/* CCPU */

#ifdef PROD
#line 512
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 514
IMPORT	void	setCH	IPT1(IU8, val);
#endif /*PROD*/
#line 516

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCX	IPT1(IU16, val);
#define	setCX(val)	c_setCX(val)
#else	/* CCPU */

#ifdef PROD
#line 525
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 527
IMPORT	void	setCX	IPT1(IU16, val);
#endif /*PROD*/
#line 529

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setECX	IPT1(IU32, val);
#define	setECX(val)	c_setECX(val)
#else	/* CCPU */

#ifdef PROD
#line 538
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 540
IMPORT	void	setECX	IPT1(IU32, val);
#endif /*PROD*/
#line 542

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setDL	IPT1(IU8, val);
#define	setDL(val)	c_setDL(val)
#else	/* CCPU */

#ifdef PROD
#line 551
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 553
IMPORT	void	setDL	IPT1(IU8, val);
#endif /*PROD*/
#line 555

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setDH	IPT1(IU8, val);
#define	setDH(val)	c_setDH(val)
#else	/* CCPU */

#ifdef PROD
#line 564
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 566
IMPORT	void	setDH	IPT1(IU8, val);
#endif /*PROD*/
#line 568

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setDX	IPT1(IU16, val);
#define	setDX(val)	c_setDX(val)
#else	/* CCPU */

#ifdef PROD
#line 577
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 579
IMPORT	void	setDX	IPT1(IU16, val);
#endif /*PROD*/
#line 581

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setEDX	IPT1(IU32, val);
#define	setEDX(val)	c_setEDX(val)
#else	/* CCPU */

#ifdef PROD
#line 590
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 592
IMPORT	void	setEDX	IPT1(IU32, val);
#endif /*PROD*/
#line 594

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setSI	IPT1(IU16, val);
#define	setSI(val)	c_setSI(val)
#else	/* CCPU */

#ifdef PROD
#line 603
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 605
IMPORT	void	setSI	IPT1(IU16, val);
#endif /*PROD*/
#line 607

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setESI	IPT1(IU32, val);
#define	setESI(val)	c_setESI(val)
#else	/* CCPU */

#ifdef PROD
#line 616
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 618
IMPORT	void	setESI	IPT1(IU32, val);
#endif /*PROD*/
#line 620

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setDI	IPT1(IU16, val);
#define	setDI(val)	c_setDI(val)
#else	/* CCPU */

#ifdef PROD
#line 629
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 631
IMPORT	void	setDI	IPT1(IU16, val);
#endif /*PROD*/
#line 633

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setEDI	IPT1(IU32, val);
#define	setEDI(val)	c_setEDI(val)
#else	/* CCPU */

#ifdef PROD
#line 642
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 644
IMPORT	void	setEDI	IPT1(IU32, val);
#endif /*PROD*/
#line 646

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setSP	IPT1(IU16, val);
#define	setSP(val)	c_setSP(val)
#else	/* CCPU */

#ifdef PROD
#line 655
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 657
IMPORT	void	setSP	IPT1(IU16, val);
#endif /*PROD*/
#line 659

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setESP	IPT1(IU32, val);
#define	setESP(val)	c_setESP(val)
#else	/* CCPU */

#ifdef PROD
#line 668
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 670
IMPORT	void	setESP	IPT1(IU32, val);
#endif /*PROD*/
#line 672

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setBP	IPT1(IU16, val);
#define	setBP(val)	c_setBP(val)
#else	/* CCPU */

#ifdef PROD
#line 681
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 683
IMPORT	void	setBP	IPT1(IU16, val);
#endif /*PROD*/
#line 685

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setEBP	IPT1(IU32, val);
#define	setEBP(val)	c_setEBP(val)
#else	/* CCPU */

#ifdef PROD
#line 694
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 696
IMPORT	void	setEBP	IPT1(IU32, val);
#endif /*PROD*/
#line 698

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setIP	IPT1(IU16, val);
#define	setIP(val)	c_setIP(val)
#else	/* CCPU */

#ifdef PROD
#line 707
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 709
IMPORT	void	setIP	IPT1(IU16, val);
#endif /*PROD*/
#line 711

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setEIP	IPT1(IU32, val);
#define	setEIP(val)	c_setEIP(val)
#else	/* CCPU */

#ifdef PROD
#line 720
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 722
IMPORT	void	setEIP	IPT1(IU32, val);
#endif /*PROD*/
#line 724

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	c_setCS	IPT1(IU16, val);
#define	setCS(val)	c_setCS(val)
#else	/* CCPU */

#ifdef PROD
#line 733
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 735
IMPORT	IUH	setCS	IPT1(IU16, val);
#endif /*PROD*/
#line 737

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	c_setSS	IPT1(IU16, val);
#define	setSS(val)	c_setSS(val)
#else	/* CCPU */

#ifdef PROD
#line 746
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 748
IMPORT	IUH	setSS	IPT1(IU16, val);
#endif /*PROD*/
#line 750

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	c_setDS	IPT1(IU16, val);
#define	setDS(val)	c_setDS(val)
#else	/* CCPU */

#ifdef PROD
#line 759
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 761
IMPORT	IUH	setDS	IPT1(IU16, val);
#endif /*PROD*/
#line 763

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	c_setES	IPT1(IU16, val);
#define	setES(val)	c_setES(val)
#else	/* CCPU */

#ifdef PROD
#line 772
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 774
IMPORT	IUH	setES	IPT1(IU16, val);
#endif /*PROD*/
#line 776

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	c_setFS	IPT1(IU16, val);
#define	setFS(val)	c_setFS(val)
#else	/* CCPU */

#ifdef PROD
#line 785
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 787
IMPORT	IUH	setFS	IPT1(IU16, val);
#endif /*PROD*/
#line 789

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	c_setGS	IPT1(IU16, val);
#define	setGS(val)	c_setGS(val)
#else	/* CCPU */

#ifdef PROD
#line 798
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 800
IMPORT	IUH	setGS	IPT1(IU16, val);
#endif /*PROD*/
#line 802

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setEFLAGS	IPT1(IU32, val);
#define	setEFLAGS(val)	c_setEFLAGS(val)
#else	/* CCPU */

#ifdef PROD
#line 811
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 813
IMPORT	void	setEFLAGS	IPT1(IU32, val);
#endif /*PROD*/
#line 815

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setSTATUS	IPT1(IU16, val);
#define	setSTATUS(val)	c_setSTATUS(val)
#else	/* CCPU */

#ifdef PROD
#line 824
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 826
IMPORT	void	setSTATUS	IPT1(IU16, val);
#endif /*PROD*/
#line 828

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setIOPL	IPT1(IU8, val);
#define	setIOPL(val)	c_setIOPL(val)
#else	/* CCPU */

#ifdef PROD
#line 837
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 839
IMPORT	void	setIOPL	IPT1(IU8, val);
#endif /*PROD*/
#line 841

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setMSW	IPT1(IU16, val);
#define	setMSW(val)	c_setMSW(val)
#else	/* CCPU */

#ifdef PROD
#line 850
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 852
IMPORT	void	setMSW	IPT1(IU16, val);
#endif /*PROD*/
#line 854

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCR0	IPT1(IU32, val);
#define	setCR0(val)	c_setCR0(val)
#else	/* CCPU */

#ifdef PROD
#line 863
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 865
IMPORT	void	setCR0	IPT1(IU32, val);
#endif /*PROD*/
#line 867

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCR2	IPT1(IU32, val);
#define	setCR2(val)	c_setCR2(val)
#else	/* CCPU */

#ifdef PROD
#line 876
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 878
IMPORT	void	setCR2	IPT1(IU32, val);
#endif /*PROD*/
#line 880

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCR3	IPT1(IU32, val);
#define	setCR3(val)	c_setCR3(val)
#else	/* CCPU */

#ifdef PROD
#line 889
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 891
IMPORT	void	setCR3	IPT1(IU32, val);
#endif /*PROD*/
#line 893

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCF	IPT1(IBOOL, val);
#define	setCF(val)	c_setCF(val)
#else	/* CCPU */

#ifdef PROD
#line 902
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 904
IMPORT	void	setCF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 906

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setPF	IPT1(IBOOL, val);
#define	setPF(val)	c_setPF(val)
#else	/* CCPU */

#ifdef PROD
#line 915
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 917
IMPORT	void	setPF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 919

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setAF	IPT1(IBOOL, val);
#define	setAF(val)	c_setAF(val)
#else	/* CCPU */

#ifdef PROD
#line 928
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 930
IMPORT	void	setAF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 932

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setZF	IPT1(IBOOL, val);
#define	setZF(val)	c_setZF(val)
#else	/* CCPU */

#ifdef PROD
#line 941
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 943
IMPORT	void	setZF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 945

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setSF	IPT1(IBOOL, val);
#define	setSF(val)	c_setSF(val)
#else	/* CCPU */

#ifdef PROD
#line 954
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 956
IMPORT	void	setSF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 958

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setTF	IPT1(IBOOL, val);
#define	setTF(val)	c_setTF(val)
#else	/* CCPU */

#ifdef PROD
#line 967
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 969
IMPORT	void	setTF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 971

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setIF	IPT1(IBOOL, val);
#define	setIF(val)	c_setIF(val)
#else	/* CCPU */

#ifdef PROD
#line 980
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 982
IMPORT	void	setIF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 984

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setDF	IPT1(IBOOL, val);
#define	setDF(val)	c_setDF(val)
#else	/* CCPU */

#ifdef PROD
#line 993
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 995
IMPORT	void	setDF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 997

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setOF	IPT1(IBOOL, val);
#define	setOF(val)	c_setOF(val)
#else	/* CCPU */

#ifdef PROD
#line 1006
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1008
IMPORT	void	setOF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1010

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setNT	IPT1(IBOOL, val);
#define	setNT(val)	c_setNT(val)
#else	/* CCPU */

#ifdef PROD
#line 1019
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1021
IMPORT	void	setNT	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1023

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setRF	IPT1(IBOOL, val);
#define	setRF(val)	c_setRF(val)
#else	/* CCPU */

#ifdef PROD
#line 1032
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1034
IMPORT	void	setRF	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1036

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setVM	IPT1(IBOOL, val);
#define	setVM(val)	c_setVM(val)
#else	/* CCPU */

#ifdef PROD
#line 1045
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1047
IMPORT	void	setVM	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1049

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setAC	IPT1(IBOOL, val);
#define	setAC(val)	c_setAC(val)
#else	/* CCPU */

#ifdef PROD
#line 1058
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1060
IMPORT	void	setAC	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1062

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setPE	IPT1(IBOOL, val);
#define	setPE(val)	c_setPE(val)
#else	/* CCPU */

#ifdef PROD
#line 1071
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1073
IMPORT	void	setPE	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1075

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setMP	IPT1(IBOOL, val);
#define	setMP(val)	c_setMP(val)
#else	/* CCPU */

#ifdef PROD
#line 1084
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1086
IMPORT	void	setMP	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1088

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setEM	IPT1(IBOOL, val);
#define	setEM(val)	c_setEM(val)
#else	/* CCPU */

#ifdef PROD
#line 1097
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1099
IMPORT	void	setEM	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1101

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setTS	IPT1(IBOOL, val);
#define	setTS(val)	c_setTS(val)
#else	/* CCPU */

#ifdef PROD
#line 1110
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1112
IMPORT	void	setTS	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1114

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setPG	IPT1(IBOOL, val);
#define	setPG(val)	c_setPG(val)
#else	/* CCPU */

#ifdef PROD
#line 1123
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1125
IMPORT	void	setPG	IPT1(IBOOL, val);
#endif /*PROD*/
#line 1127

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setLDT_SELECTOR	IPT1(IU16, val);
#define	setLDT_SELECTOR(val)	c_setLDT_SELECTOR(val)
#else	/* CCPU */

#ifdef PROD
#line 1136
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1138
IMPORT	void	setLDT_SELECTOR	IPT1(IU16, val);
#endif /*PROD*/
#line 1140

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setTR_SELECTOR	IPT1(IU16, val);
#define	setTR_SELECTOR(val)	c_setTR_SELECTOR(val)
#else	/* CCPU */

#ifdef PROD
#line 1149
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1151
IMPORT	void	setTR_SELECTOR	IPT1(IU16, val);
#endif /*PROD*/
#line 1153

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getAL	IPT0();
#define	getAL()	c_getAL()
#else	/* CCPU */

#ifdef PROD
#line 1162
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1164
IMPORT	IU8	getAL	IPT0();
#endif /*PROD*/
#line 1166

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getAH	IPT0();
#define	getAH()	c_getAH()
#else	/* CCPU */

#ifdef PROD
#line 1175
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1177
IMPORT	IU8	getAH	IPT0();
#endif /*PROD*/
#line 1179

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getAX	IPT0();
#define	getAX()	c_getAX()
#else	/* CCPU */

#ifdef PROD
#line 1188
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1190
IMPORT	IU16	getAX	IPT0();
#endif /*PROD*/
#line 1192

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getEAX	IPT0();
#define	getEAX()	c_getEAX()
#else	/* CCPU */

#ifdef PROD
#line 1201
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1203
IMPORT	IU32	getEAX	IPT0();
#endif /*PROD*/
#line 1205

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getBL	IPT0();
#define	getBL()	c_getBL()
#else	/* CCPU */

#ifdef PROD
#line 1214
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1216
IMPORT	IU8	getBL	IPT0();
#endif /*PROD*/
#line 1218

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getBH	IPT0();
#define	getBH()	c_getBH()
#else	/* CCPU */

#ifdef PROD
#line 1227
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1229
IMPORT	IU8	getBH	IPT0();
#endif /*PROD*/
#line 1231

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getBX	IPT0();
#define	getBX()	c_getBX()
#else	/* CCPU */

#ifdef PROD
#line 1240
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1242
IMPORT	IU16	getBX	IPT0();
#endif /*PROD*/
#line 1244

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getEBX	IPT0();
#define	getEBX()	c_getEBX()
#else	/* CCPU */

#ifdef PROD
#line 1253
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1255
IMPORT	IU32	getEBX	IPT0();
#endif /*PROD*/
#line 1257

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getCL	IPT0();
#define	getCL()	c_getCL()
#else	/* CCPU */

#ifdef PROD
#line 1266
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1268
IMPORT	IU8	getCL	IPT0();
#endif /*PROD*/
#line 1270

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getCH	IPT0();
#define	getCH()	c_getCH()
#else	/* CCPU */

#ifdef PROD
#line 1279
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1281
IMPORT	IU8	getCH	IPT0();
#endif /*PROD*/
#line 1283

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getCX	IPT0();
#define	getCX()	c_getCX()
#else	/* CCPU */

#ifdef PROD
#line 1292
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1294
IMPORT	IU16	getCX	IPT0();
#endif /*PROD*/
#line 1296

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getECX	IPT0();
#define	getECX()	c_getECX()
#else	/* CCPU */

#ifdef PROD
#line 1305
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1307
IMPORT	IU32	getECX	IPT0();
#endif /*PROD*/
#line 1309

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getDL	IPT0();
#define	getDL()	c_getDL()
#else	/* CCPU */

#ifdef PROD
#line 1318
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1320
IMPORT	IU8	getDL	IPT0();
#endif /*PROD*/
#line 1322

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getDH	IPT0();
#define	getDH()	c_getDH()
#else	/* CCPU */

#ifdef PROD
#line 1331
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1333
IMPORT	IU8	getDH	IPT0();
#endif /*PROD*/
#line 1335

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getDX	IPT0();
#define	getDX()	c_getDX()
#else	/* CCPU */

#ifdef PROD
#line 1344
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1346
IMPORT	IU16	getDX	IPT0();
#endif /*PROD*/
#line 1348

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getEDX	IPT0();
#define	getEDX()	c_getEDX()
#else	/* CCPU */

#ifdef PROD
#line 1357
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1359
IMPORT	IU32	getEDX	IPT0();
#endif /*PROD*/
#line 1361

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getSI	IPT0();
#define	getSI()	c_getSI()
#else	/* CCPU */

#ifdef PROD
#line 1370
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1372
IMPORT	IU16	getSI	IPT0();
#endif /*PROD*/
#line 1374

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getESI	IPT0();
#define	getESI()	c_getESI()
#else	/* CCPU */

#ifdef PROD
#line 1383
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1385
IMPORT	IU32	getESI	IPT0();
#endif /*PROD*/
#line 1387

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getDI	IPT0();
#define	getDI()	c_getDI()
#else	/* CCPU */

#ifdef PROD
#line 1396
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1398
IMPORT	IU16	getDI	IPT0();
#endif /*PROD*/
#line 1400

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getEDI	IPT0();
#define	getEDI()	c_getEDI()
#else	/* CCPU */

#ifdef PROD
#line 1409
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1411
IMPORT	IU32	getEDI	IPT0();
#endif /*PROD*/
#line 1413

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getSP	IPT0();
#define	getSP()	c_getSP()
#else	/* CCPU */

#ifdef PROD
#line 1422
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1424
IMPORT	IU16	getSP	IPT0();
#endif /*PROD*/
#line 1426

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getESP	IPT0();
#define	getESP()	c_getESP()
#else	/* CCPU */

#ifdef PROD
#line 1435
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1437
IMPORT	IU32	getESP	IPT0();
#endif /*PROD*/
#line 1439

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getBP	IPT0();
#define	getBP()	c_getBP()
#else	/* CCPU */

#ifdef PROD
#line 1448
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1450
IMPORT	IU16	getBP	IPT0();
#endif /*PROD*/
#line 1452

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getEBP	IPT0();
#define	getEBP()	c_getEBP()
#else	/* CCPU */

#ifdef PROD
#line 1461
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1463
IMPORT	IU32	getEBP	IPT0();
#endif /*PROD*/
#line 1465

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getIP	IPT0();
#define	getIP()	c_getIP()
#else	/* CCPU */

#ifdef PROD
#line 1474
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1476
IMPORT	IU16	getIP	IPT0();
#endif /*PROD*/
#line 1478

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getEIP	IPT0();
#define	getEIP()	c_getEIP()
#else	/* CCPU */

#ifdef PROD
#line 1487
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1489
IMPORT	IU32	getEIP	IPT0();
#endif /*PROD*/
#line 1491

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getCS	IPT0();
#define	getCS()	c_getCS()
#else	/* CCPU */

#ifdef PROD
#line 1500
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1502
IMPORT	IU16	getCS	IPT0();
#endif /*PROD*/
#line 1504

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getSS	IPT0();
#define	getSS()	c_getSS()
#else	/* CCPU */

#ifdef PROD
#line 1513
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1515
IMPORT	IU16	getSS	IPT0();
#endif /*PROD*/
#line 1517

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getDS	IPT0();
#define	getDS()	c_getDS()
#else	/* CCPU */

#ifdef PROD
#line 1526
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1528
IMPORT	IU16	getDS	IPT0();
#endif /*PROD*/
#line 1530

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getES	IPT0();
#define	getES()	c_getES()
#else	/* CCPU */

#ifdef PROD
#line 1539
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1541
IMPORT	IU16	getES	IPT0();
#endif /*PROD*/
#line 1543

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getFS	IPT0();
#define	getFS()	c_getFS()
#else	/* CCPU */

#ifdef PROD
#line 1552
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1554
IMPORT	IU16	getFS	IPT0();
#endif /*PROD*/
#line 1556

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getGS	IPT0();
#define	getGS()	c_getGS()
#else	/* CCPU */

#ifdef PROD
#line 1565
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1567
IMPORT	IU16	getGS	IPT0();
#endif /*PROD*/
#line 1569

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getEFLAGS	IPT0();
#define	getEFLAGS()	c_getEFLAGS()
#else	/* CCPU */

#ifdef PROD
#line 1578
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1580
IMPORT	IU32	getEFLAGS	IPT0();
#endif /*PROD*/
#line 1582

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getSTATUS	IPT0();
#define	getSTATUS()	c_getSTATUS()
#else	/* CCPU */

#ifdef PROD
#line 1591
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1593
IMPORT	IU16	getSTATUS	IPT0();
#endif /*PROD*/
#line 1595

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU8	c_getIOPL	IPT0();
#define	getIOPL()	c_getIOPL()
#else	/* CCPU */

#ifdef PROD
#line 1604
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1606
IMPORT	IU8	getIOPL	IPT0();
#endif /*PROD*/
#line 1608

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getMSW	IPT0();
#define	getMSW()	c_getMSW()
#else	/* CCPU */

#ifdef PROD
#line 1617
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1619
IMPORT	IU16	getMSW	IPT0();
#endif /*PROD*/
#line 1621

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getCR0	IPT0();
#define	getCR0()	c_getCR0()
#else	/* CCPU */

#ifdef PROD
#line 1630
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1632
IMPORT	IU32	getCR0	IPT0();
#endif /*PROD*/
#line 1634

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getCR2	IPT0();
#define	getCR2()	c_getCR2()
#else	/* CCPU */

#ifdef PROD
#line 1643
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1645
IMPORT	IU32	getCR2	IPT0();
#endif /*PROD*/
#line 1647

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getCR3	IPT0();
#define	getCR3()	c_getCR3()
#else	/* CCPU */

#ifdef PROD
#line 1656
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1658
IMPORT	IU32	getCR3	IPT0();
#endif /*PROD*/
#line 1660

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getCF	IPT0();
#define	getCF()	c_getCF()
#else	/* CCPU */

#ifdef PROD
#line 1669
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1671
IMPORT	IBOOL	getCF	IPT0();
#endif /*PROD*/
#line 1673

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getPF	IPT0();
#define	getPF()	c_getPF()
#else	/* CCPU */

#ifdef PROD
#line 1682
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1684
IMPORT	IBOOL	getPF	IPT0();
#endif /*PROD*/
#line 1686

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getAF	IPT0();
#define	getAF()	c_getAF()
#else	/* CCPU */

#ifdef PROD
#line 1695
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1697
IMPORT	IBOOL	getAF	IPT0();
#endif /*PROD*/
#line 1699

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getZF	IPT0();
#define	getZF()	c_getZF()
#else	/* CCPU */

#ifdef PROD
#line 1708
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1710
IMPORT	IBOOL	getZF	IPT0();
#endif /*PROD*/
#line 1712

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getSF	IPT0();
#define	getSF()	c_getSF()
#else	/* CCPU */

#ifdef PROD
#line 1721
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1723
IMPORT	IBOOL	getSF	IPT0();
#endif /*PROD*/
#line 1725

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getTF	IPT0();
#define	getTF()	c_getTF()
#else	/* CCPU */

#ifdef PROD
#line 1734
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1736
IMPORT	IBOOL	getTF	IPT0();
#endif /*PROD*/
#line 1738

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getIF	IPT0();
#define	getIF()	c_getIF()
#else	/* CCPU */

#ifdef PROD
#line 1747
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1749
IMPORT	IBOOL	getIF	IPT0();
#endif /*PROD*/
#line 1751

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getDF	IPT0();
#define	getDF()	c_getDF()
#else	/* CCPU */

#ifdef PROD
#line 1760
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1762
IMPORT	IBOOL	getDF	IPT0();
#endif /*PROD*/
#line 1764

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getOF	IPT0();
#define	getOF()	c_getOF()
#else	/* CCPU */

#ifdef PROD
#line 1773
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1775
IMPORT	IBOOL	getOF	IPT0();
#endif /*PROD*/
#line 1777

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getNT	IPT0();
#define	getNT()	c_getNT()
#else	/* CCPU */

#ifdef PROD
#line 1786
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1788
IMPORT	IBOOL	getNT	IPT0();
#endif /*PROD*/
#line 1790

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getRF	IPT0();
#define	getRF()	c_getRF()
#else	/* CCPU */

#ifdef PROD
#line 1799
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1801
IMPORT	IBOOL	getRF	IPT0();
#endif /*PROD*/
#line 1803

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getVM	IPT0();
#define	getVM()	c_getVM()
#else	/* CCPU */

#ifdef PROD
#line 1812
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1814
IMPORT	IBOOL	getVM	IPT0();
#endif /*PROD*/
#line 1816

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getAC	IPT0();
#define	getAC()	c_getAC()
#else	/* CCPU */

#ifdef PROD
#line 1825
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1827
IMPORT	IBOOL	getAC	IPT0();
#endif /*PROD*/
#line 1829

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getPE	IPT0();
#define	getPE()	c_getPE()
#else	/* CCPU */

#ifdef PROD
#line 1838
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1840
IMPORT	IBOOL	getPE	IPT0();
#endif /*PROD*/
#line 1842

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getMP	IPT0();
#define	getMP()	c_getMP()
#else	/* CCPU */

#ifdef PROD
#line 1851
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1853
IMPORT	IBOOL	getMP	IPT0();
#endif /*PROD*/
#line 1855

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getEM	IPT0();
#define	getEM()	c_getEM()
#else	/* CCPU */

#ifdef PROD
#line 1864
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1866
IMPORT	IBOOL	getEM	IPT0();
#endif /*PROD*/
#line 1868

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getTS	IPT0();
#define	getTS()	c_getTS()
#else	/* CCPU */

#ifdef PROD
#line 1877
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1879
IMPORT	IBOOL	getTS	IPT0();
#endif /*PROD*/
#line 1881

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getET	IPT0();
#define	getET()	c_getET()
#else	/* CCPU */

#ifdef PROD
#line 1890
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1892
IMPORT	IBOOL	getET	IPT0();
#endif /*PROD*/
#line 1894

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getNE	IPT0();
#define	getNE()	c_getNE()
#else	/* CCPU */

#ifdef PROD
#line 1903
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1905
IMPORT	IBOOL	getNE	IPT0();
#endif /*PROD*/
#line 1907

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getWP	IPT0();
#define	getWP()	c_getWP()
#else	/* CCPU */

#ifdef PROD
#line 1916
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1918
IMPORT	IBOOL	getWP	IPT0();
#endif /*PROD*/
#line 1920

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_getPG	IPT0();
#define	getPG()	c_getPG()
#else	/* CCPU */

#ifdef PROD
#line 1929
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1931
IMPORT	IBOOL	getPG	IPT0();
#endif /*PROD*/
#line 1933

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getGDT_BASE	IPT0();
#define	getGDT_BASE()	c_getGDT_BASE()
#else	/* CCPU */

#ifdef PROD
#line 1942
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1944
IMPORT	IU32	getGDT_BASE	IPT0();
#endif /*PROD*/
#line 1946

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getGDT_LIMIT	IPT0();
#define	getGDT_LIMIT()	c_getGDT_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 1955
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1957
IMPORT	IU16	getGDT_LIMIT	IPT0();
#endif /*PROD*/
#line 1959

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getIDT_BASE	IPT0();
#define	getIDT_BASE()	c_getIDT_BASE()
#else	/* CCPU */

#ifdef PROD
#line 1968
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1970
IMPORT	IU32	getIDT_BASE	IPT0();
#endif /*PROD*/
#line 1972

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getIDT_LIMIT	IPT0();
#define	getIDT_LIMIT()	c_getIDT_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 1981
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1983
IMPORT	IU16	getIDT_LIMIT	IPT0();
#endif /*PROD*/
#line 1985

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getLDT_SELECTOR	IPT0();
#define	getLDT_SELECTOR()	c_getLDT_SELECTOR()
#else	/* CCPU */

#ifdef PROD
#line 1994
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 1996
IMPORT	IU16	getLDT_SELECTOR	IPT0();
#endif /*PROD*/
#line 1998

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getLDT_BASE	IPT0();
#define	getLDT_BASE()	c_getLDT_BASE()
#else	/* CCPU */

#ifdef PROD
#line 2007
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2009
IMPORT	IU32	getLDT_BASE	IPT0();
#endif /*PROD*/
#line 2011

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getLDT_LIMIT	IPT0();
#define	getLDT_LIMIT()	c_getLDT_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 2020
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2022
IMPORT	IU32	getLDT_LIMIT	IPT0();
#endif /*PROD*/
#line 2024

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getTR_SELECTOR	IPT0();
#define	getTR_SELECTOR()	c_getTR_SELECTOR()
#else	/* CCPU */

#ifdef PROD
#line 2033
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2035
IMPORT	IU16	getTR_SELECTOR	IPT0();
#endif /*PROD*/
#line 2037

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getTR_BASE	IPT0();
#define	getTR_BASE()	c_getTR_BASE()
#else	/* CCPU */

#ifdef PROD
#line 2046
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2048
IMPORT	IU32	getTR_BASE	IPT0();
#endif /*PROD*/
#line 2050

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getTR_LIMIT	IPT0();
#define	getTR_LIMIT()	c_getTR_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 2059
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2061
IMPORT	IU32	getTR_LIMIT	IPT0();
#endif /*PROD*/
#line 2063

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getTR_AR	IPT0();
#define	getTR_AR()	c_getTR_AR()
#else	/* CCPU */

#ifdef PROD
#line 2072
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2074
IMPORT	IU16	getTR_AR	IPT0();
#endif /*PROD*/
#line 2076

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	host_get_q_calib_val	IPT0();
#define	host_get_q_calib_val	host_get_q_calib_val
#else	/* CCPU */

#ifdef PROD
#line 2085
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2087
IMPORT	IUH	host_get_q_calib_val	IPT0();
#endif /*PROD*/
#line 2089

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	host_get_jump_restart	IPT0();
#define	host_get_jump_restart	host_get_jump_restart
#else	/* CCPU */

#ifdef PROD
#line 2098
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2100
IMPORT	IUH	host_get_jump_restart	IPT0();
#endif /*PROD*/
#line 2102

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	host_set_jump_restart	IPT1(IUH, initialVal);
#define	host_set_jump_restart(initialVal)	host_set_jump_restart(initialVal)
#else	/* CCPU */

#ifdef PROD
#line 2111
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2113
IMPORT	void	host_set_jump_restart	IPT1(IUH, initialVal);
#endif /*PROD*/
#line 2115

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	setEOIEnableAddr	IPT1(IU8 *, initialVal);
#define	setEOIEnableAddr(initialVal)	setEOIEnableAddr(initialVal)
#else	/* CCPU */

#ifdef PROD
#line 2124
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2126
IMPORT	void	setEOIEnableAddr	IPT1(IU8 *, initialVal);
#endif /*PROD*/
#line 2128

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	setAddProfileDataPtr	IPT1(IHP, initialVal);
#define	setAddProfileDataPtr(initialVal)	setAddProfileDataPtr(initialVal)
#else	/* CCPU */

#ifdef PROD
#line 2137
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2139
IMPORT	void	setAddProfileDataPtr	IPT1(IHP, initialVal);
#endif /*PROD*/
#line 2141

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	setMaxProfileDataAddr	IPT1(IHP, initialVal);
#define	setMaxProfileDataAddr(initialVal)	setMaxProfileDataAddr(initialVal)
#else	/* CCPU */

#ifdef PROD
#line 2150
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2152
IMPORT	void	setMaxProfileDataAddr	IPT1(IHP, initialVal);
#endif /*PROD*/
#line 2154

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IHP	getAddProfileDataAddr	IPT0();
#define	getAddProfileDataAddr()	getAddProfileDataAddr()
#else	/* CCPU */

#ifdef PROD
#line 2163
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2165
IMPORT	IHP	getAddProfileDataAddr	IPT0();
#endif /*PROD*/
#line 2167

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	PurgeLostIretHookLine	IPT1(IU16, lineNum);
#define	PurgeLostIretHookLine(lineNum)	PurgeLostIretHookLine(lineNum)
#else	/* CCPU */

#ifdef PROD
#line 2176
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2178
IMPORT	void	PurgeLostIretHookLine	IPT1(IU16, lineNum);
#endif /*PROD*/
#line 2180

#endif	/* CCPU */

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
	void	(*SetRegConstraint)	IPT2(IU32,	regId, IU8,	constraintType);
	void	(*GrowRecPool)	IPT0();
	void	(*BpiCompileBPI)	IPT1(char *,	instructions);
	void	(*TrashIntelRegisters)	IPT0();
	void	(*FmDeleteAllStructures)	IPT1(IU32,	newCR0);
	TypeConstraintBitMapRECptr	(*ConstraintsFromUnivEpcPtr)	IPT1(TypeEntryPointCacheRECptr,	univ);
	TypeConstraintBitMapRECptr	(*ConstraintsFromUnivHandle)	IPT1(IU16,	handle);
};

#ifdef	CCPU
IMPORT	IHP	c_getSadInfoTable	IPT0();
#define	getSadInfoTable()	c_getSadInfoTable()
#else	/* CCPU */

#ifdef PROD
#line 2251
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2253
IMPORT	IHP	getSadInfoTable	IPT0();
#endif /*PROD*/
#line 2255

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setGDT_BASE_LIMIT	IPT2(IU32, base, IU16, limit);
#define	setGDT_BASE_LIMIT(base, limit)	c_setGDT_BASE_LIMIT(base, limit)
#else	/* CCPU */

#ifdef PROD
#line 2264
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2266
IMPORT	IBOOL	setGDT_BASE_LIMIT	IPT2(IU32, base, IU16, limit);
#endif /*PROD*/
#line 2268

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setIDT_BASE_LIMIT	IPT2(IU32, base, IU16, limit);
#define	setIDT_BASE_LIMIT(base, limit)	c_setIDT_BASE_LIMIT(base, limit)
#else	/* CCPU */

#ifdef PROD
#line 2277
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2279
IMPORT	IBOOL	setIDT_BASE_LIMIT	IPT2(IU32, base, IU16, limit);
#endif /*PROD*/
#line 2281

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setLDT_BASE_LIMIT	IPT2(IU32, base, IU32, limit);
#define	setLDT_BASE_LIMIT(base, limit)	c_setLDT_BASE_LIMIT(base, limit)
#else	/* CCPU */

#ifdef PROD
#line 2290
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2292
IMPORT	IBOOL	setLDT_BASE_LIMIT	IPT2(IU32, base, IU32, limit);
#endif /*PROD*/
#line 2294

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setTR_BASE_LIMIT	IPT2(IU32, base, IU32, limit);
#define	setTR_BASE_LIMIT(base, limit)	c_setTR_BASE_LIMIT(base, limit)
#else	/* CCPU */

#ifdef PROD
#line 2303
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2305
IMPORT	IBOOL	setTR_BASE_LIMIT	IPT2(IU32, base, IU32, limit);
#endif /*PROD*/
#line 2307

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setTR_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setTR_BASE_LIMIT_AR(base, limit, ar)	c_setTR_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */

#ifdef PROD
#line 2316
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2318
IMPORT	IBOOL	setTR_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#endif /*PROD*/
#line 2320

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setCS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setCS_BASE_LIMIT_AR(base, limit, ar)	c_setCS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */

#ifdef PROD
#line 2329
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2331
IMPORT	IBOOL	setCS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#endif /*PROD*/
#line 2333

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setSS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setSS_BASE_LIMIT_AR(base, limit, ar)	c_setSS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */

#ifdef PROD
#line 2342
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2344
IMPORT	IBOOL	setSS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#endif /*PROD*/
#line 2346

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setDS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setDS_BASE_LIMIT_AR(base, limit, ar)	c_setDS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */

#ifdef PROD
#line 2355
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2357
IMPORT	IBOOL	setDS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#endif /*PROD*/
#line 2359

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setES_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setES_BASE_LIMIT_AR(base, limit, ar)	c_setES_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */

#ifdef PROD
#line 2368
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2370
IMPORT	IBOOL	setES_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#endif /*PROD*/
#line 2372

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setFS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setFS_BASE_LIMIT_AR(base, limit, ar)	c_setFS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */

#ifdef PROD
#line 2381
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2383
IMPORT	IBOOL	setFS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#endif /*PROD*/
#line 2385

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IBOOL	c_setGS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#define	setGS_BASE_LIMIT_AR(base, limit, ar)	c_setGS_BASE_LIMIT_AR(base, limit, ar)
#else	/* CCPU */

#ifdef PROD
#line 2394
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2396
IMPORT	IBOOL	setGS_BASE_LIMIT_AR	IPT3(IU32, base, IU32, limit, IU16, ar);
#endif /*PROD*/
#line 2398

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCS_SELECTOR	IPT1(IU16, val);
#define	setCS_SELECTOR(val)	c_setCS_SELECTOR(val)
#else	/* CCPU */

#ifdef PROD
#line 2407
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2409
IMPORT	void	setCS_SELECTOR	IPT1(IU16, val);
#endif /*PROD*/
#line 2411

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setSS_SELECTOR	IPT1(IU16, val);
#define	setSS_SELECTOR(val)	c_setSS_SELECTOR(val)
#else	/* CCPU */

#ifdef PROD
#line 2420
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2422
IMPORT	void	setSS_SELECTOR	IPT1(IU16, val);
#endif /*PROD*/
#line 2424

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setDS_SELECTOR	IPT1(IU16, val);
#define	setDS_SELECTOR(val)	c_setDS_SELECTOR(val)
#else	/* CCPU */

#ifdef PROD
#line 2433
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2435
IMPORT	void	setDS_SELECTOR	IPT1(IU16, val);
#endif /*PROD*/
#line 2437

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setES_SELECTOR	IPT1(IU16, val);
#define	setES_SELECTOR(val)	c_setES_SELECTOR(val)
#else	/* CCPU */

#ifdef PROD
#line 2446
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2448
IMPORT	void	setES_SELECTOR	IPT1(IU16, val);
#endif /*PROD*/
#line 2450

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setFS_SELECTOR	IPT1(IU16, val);
#define	setFS_SELECTOR(val)	c_setFS_SELECTOR(val)
#else	/* CCPU */

#ifdef PROD
#line 2459
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2461
IMPORT	void	setFS_SELECTOR	IPT1(IU16, val);
#endif /*PROD*/
#line 2463

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setGS_SELECTOR	IPT1(IU16, val);
#define	setGS_SELECTOR(val)	c_setGS_SELECTOR(val)
#else	/* CCPU */

#ifdef PROD
#line 2472
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2474
IMPORT	void	setGS_SELECTOR	IPT1(IU16, val);
#endif /*PROD*/
#line 2476

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getCS_SELECTOR	IPT0();
#define	getCS_SELECTOR()	c_getCS_SELECTOR()
#else	/* CCPU */

#ifdef PROD
#line 2485
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2487
IMPORT	IU16	getCS_SELECTOR	IPT0();
#endif /*PROD*/
#line 2489

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getSS_SELECTOR	IPT0();
#define	getSS_SELECTOR()	c_getSS_SELECTOR()
#else	/* CCPU */

#ifdef PROD
#line 2498
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2500
IMPORT	IU16	getSS_SELECTOR	IPT0();
#endif /*PROD*/
#line 2502

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getDS_SELECTOR	IPT0();
#define	getDS_SELECTOR()	c_getDS_SELECTOR()
#else	/* CCPU */

#ifdef PROD
#line 2511
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2513
IMPORT	IU16	getDS_SELECTOR	IPT0();
#endif /*PROD*/
#line 2515

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getES_SELECTOR	IPT0();
#define	getES_SELECTOR()	c_getES_SELECTOR()
#else	/* CCPU */

#ifdef PROD
#line 2524
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2526
IMPORT	IU16	getES_SELECTOR	IPT0();
#endif /*PROD*/
#line 2528

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getFS_SELECTOR	IPT0();
#define	getFS_SELECTOR()	c_getFS_SELECTOR()
#else	/* CCPU */

#ifdef PROD
#line 2537
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2539
IMPORT	IU16	getFS_SELECTOR	IPT0();
#endif /*PROD*/
#line 2541

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getGS_SELECTOR	IPT0();
#define	getGS_SELECTOR()	c_getGS_SELECTOR()
#else	/* CCPU */

#ifdef PROD
#line 2550
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2552
IMPORT	IU16	getGS_SELECTOR	IPT0();
#endif /*PROD*/
#line 2554

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getCS_BASE	IPT0();
#define	getCS_BASE()	c_getCS_BASE()
#else	/* CCPU */

#ifdef PROD
#line 2563
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2565
IMPORT	IU32	getCS_BASE	IPT0();
#endif /*PROD*/
#line 2567

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getSS_BASE	IPT0();
#define	getSS_BASE()	c_getSS_BASE()
#else	/* CCPU */

#ifdef PROD
#line 2576
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2578
IMPORT	IU32	getSS_BASE	IPT0();
#endif /*PROD*/
#line 2580

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getDS_BASE	IPT0();
#define	getDS_BASE()	c_getDS_BASE()
#else	/* CCPU */

#ifdef PROD
#line 2589
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2591
IMPORT	IU32	getDS_BASE	IPT0();
#endif /*PROD*/
#line 2593

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getES_BASE	IPT0();
#define	getES_BASE()	c_getES_BASE()
#else	/* CCPU */

#ifdef PROD
#line 2602
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2604
IMPORT	IU32	getES_BASE	IPT0();
#endif /*PROD*/
#line 2606

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getFS_BASE	IPT0();
#define	getFS_BASE()	c_getFS_BASE()
#else	/* CCPU */

#ifdef PROD
#line 2615
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2617
IMPORT	IU32	getFS_BASE	IPT0();
#endif /*PROD*/
#line 2619

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getGS_BASE	IPT0();
#define	getGS_BASE()	c_getGS_BASE()
#else	/* CCPU */

#ifdef PROD
#line 2628
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2630
IMPORT	IU32	getGS_BASE	IPT0();
#endif /*PROD*/
#line 2632

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getCS_LIMIT	IPT0();
#define	getCS_LIMIT()	c_getCS_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 2641
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2643
IMPORT	IU32	getCS_LIMIT	IPT0();
#endif /*PROD*/
#line 2645

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getSS_LIMIT	IPT0();
#define	getSS_LIMIT()	c_getSS_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 2654
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2656
IMPORT	IU32	getSS_LIMIT	IPT0();
#endif /*PROD*/
#line 2658

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getDS_LIMIT	IPT0();
#define	getDS_LIMIT()	c_getDS_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 2667
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2669
IMPORT	IU32	getDS_LIMIT	IPT0();
#endif /*PROD*/
#line 2671

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getES_LIMIT	IPT0();
#define	getES_LIMIT()	c_getES_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 2680
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2682
IMPORT	IU32	getES_LIMIT	IPT0();
#endif /*PROD*/
#line 2684

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getFS_LIMIT	IPT0();
#define	getFS_LIMIT()	c_getFS_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 2693
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2695
IMPORT	IU32	getFS_LIMIT	IPT0();
#endif /*PROD*/
#line 2697

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU32	c_getGS_LIMIT	IPT0();
#define	getGS_LIMIT()	c_getGS_LIMIT()
#else	/* CCPU */

#ifdef PROD
#line 2706
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2708
IMPORT	IU32	getGS_LIMIT	IPT0();
#endif /*PROD*/
#line 2710

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getCS_AR	IPT0();
#define	getCS_AR()	c_getCS_AR()
#else	/* CCPU */

#ifdef PROD
#line 2719
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2721
IMPORT	IU16	getCS_AR	IPT0();
#endif /*PROD*/
#line 2723

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getSS_AR	IPT0();
#define	getSS_AR()	c_getSS_AR()
#else	/* CCPU */

#ifdef PROD
#line 2732
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2734
IMPORT	IU16	getSS_AR	IPT0();
#endif /*PROD*/
#line 2736

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getDS_AR	IPT0();
#define	getDS_AR()	c_getDS_AR()
#else	/* CCPU */

#ifdef PROD
#line 2745
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2747
IMPORT	IU16	getDS_AR	IPT0();
#endif /*PROD*/
#line 2749

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getES_AR	IPT0();
#define	getES_AR()	c_getES_AR()
#else	/* CCPU */

#ifdef PROD
#line 2758
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2760
IMPORT	IU16	getES_AR	IPT0();
#endif /*PROD*/
#line 2762

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getFS_AR	IPT0();
#define	getFS_AR()	c_getFS_AR()
#else	/* CCPU */

#ifdef PROD
#line 2771
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2773
IMPORT	IU16	getFS_AR	IPT0();
#endif /*PROD*/
#line 2775

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IU16	c_getGS_AR	IPT0();
#define	getGS_AR()	c_getGS_AR()
#else	/* CCPU */

#ifdef PROD
#line 2784
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2786
IMPORT	IU16	getGS_AR	IPT0();
#endif /*PROD*/
#line 2788

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	IUH	c_getCPL	IPT0();
#define	getCPL()	c_getCPL()
#else	/* CCPU */

#ifdef PROD
#line 2797
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2799
IMPORT	IUH	getCPL	IPT0();
#endif /*PROD*/
#line 2801

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCPL	IPT1(IUH, prot);
#define	setCPL(prot)	c_setCPL(prot)
#else	/* CCPU */

#ifdef PROD
#line 2810
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2812
IMPORT	void	setCPL	IPT1(IUH, prot);
#endif /*PROD*/
#line 2814

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_getCpuState	IPT1(TypeCpuStateRECptr, state);
#define	getCpuState(state)	c_getCpuState(state)
#else	/* CCPU */

#ifdef PROD
#line 2823
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2825
IMPORT	void	getCpuState	IPT1(TypeCpuStateRECptr, state);
#endif /*PROD*/
#line 2827

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setCpuState	IPT1(TypeCpuStateRECptr, state);
#define	setCpuState(state)	c_setCpuState(state)
#else	/* CCPU */

#ifdef PROD
#line 2836
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2838
IMPORT	void	setCpuState	IPT1(TypeCpuStateRECptr, state);
#endif /*PROD*/
#line 2840

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_InitNanoCpu	IPT1(IU32, variety);
#define	initNanoCpu(variety)	c_InitNanoCpu(variety)
#else	/* CCPU */

#ifdef PROD
#line 2849
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2851
IMPORT	void	initNanoCpu	IPT1(IU32, variety);
#endif /*PROD*/
#line 2853

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_PrepareBlocksToCompile	IPT1(IU32, variety);
#define	prepareBlocksToCompile(variety)	c_PrepareBlocksToCompile(variety)
#else	/* CCPU */

#ifdef PROD
#line 2862
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2864
IMPORT	void	prepareBlocksToCompile	IPT1(IU32, variety);
#endif /*PROD*/
#line 2866

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_setRegConstraint	IPT2(IU32, regId, IU8, constraintType);
#define	setRegConstraint(regId, constraintType)	c_setRegConstraint(regId, constraintType)
#else	/* CCPU */

#ifdef PROD
#line 2875
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2877
IMPORT	void	setRegConstraint	IPT2(IU32, regId, IU8, constraintType);
#endif /*PROD*/
#line 2879

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_growRecPool	IPT0();
#define	growRecPool	c_growRecPool
#else	/* CCPU */

#ifdef PROD
#line 2888
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2890
IMPORT	void	growRecPool	IPT0();
#endif /*PROD*/
#line 2892

#endif	/* CCPU */

#ifdef	CCPU

#else	/* CCPU */

#ifdef PROD
#line 2900
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2902
IMPORT	void	BpiCompileBPI	IPT1(char *, instructions);
#endif /*PROD*/
#line 2904

#endif	/* CCPU */

#ifdef	CCPU
IMPORT	void	c_trashIntelRegisters	IPT0();
#define	trashIntelregisters	c_trashIntelRegisters
#else	/* CCPU */

#ifdef PROD
#line 2913
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2915
IMPORT	void	trashIntelregisters	IPT0();
#endif /*PROD*/
#line 2917

#endif	/* CCPU */

#ifdef	CCPU

#else	/* CCPU */

#ifdef PROD
#line 2925
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2927
IMPORT	void	FmDeleteAllStructures	IPT1(IU32, newCR0);
#endif /*PROD*/
#line 2929

#endif	/* CCPU */

#ifdef	CCPU

#else	/* CCPU */

#ifdef PROD
#line 2937
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2939
IMPORT	TypeConstraintBitMapRECptr	constraintsFromUnivEpcPtr	IPT1(TypeEntryPointCacheRECptr, univ);
#endif /*PROD*/
#line 2941

#endif	/* CCPU */

#ifdef	CCPU

#else	/* CCPU */

#ifdef PROD
#line 2949
#error Excluded unsupported non-CCPU PROD configuration
#else /* PROD */
#line 2951
IMPORT	TypeConstraintBitMapRECptr	constraintsFromUnivHandle	IPT1(IU16, handle);
#endif /*PROD*/
#line 2953

#endif	/* CCPU */

/*======================================== END ========================================*/

