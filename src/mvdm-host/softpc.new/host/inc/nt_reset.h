/*
** nt_reset.h
*/
#line 4
#ifdef X86GFX
#line 5
#error DIVERGENCE MVDM-HOST-DIV-512: X86GFX branch is not selected for these consumers; see mirror register.
#endif
#line 7

extern BOOL   VDMForWOW;
extern BOOL   fSeparateWow;
extern HANDLE MainThread;
extern ULONG  DosSessionId;
VOID TerminateVDM(VOID);
void host_applClose(void);
extern VOID enable_stream_io(VOID);

extern BOOL   StreamIoSwitchOn;
