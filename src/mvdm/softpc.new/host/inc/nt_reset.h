/*
** nt_reset.h
*/
#ifdef X86GFX
/* DIVERGENCE MVDM-HOST-DIV-512: X86GFX branch is not selected for these consumers; see mirror register. */
#endif

extern BOOL   VDMForWOW;
extern BOOL   fSeparateWow;
extern HANDLE MainThread;
extern ULONG  DosSessionId;
VOID TerminateVDM(VOID);
void host_applClose(void);
extern VOID enable_stream_io(VOID);

extern BOOL   StreamIoSwitchOn;
