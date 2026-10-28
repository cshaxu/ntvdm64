import pathlib
base=pathlib.Path('tests/mvdm-host/ccpu_bounded_execution_fixture.c').read_text()
needle='    if (fixture_host_close_calls != 1u || !fixture_high_linear_page()) {'
assert base.count(needle)==1
extra="""    {
        const char *v=getenv(\"BIT_CASE\"); unsigned i; int which=v ? atoi(v) : 0; const unsigned char *seq; unsigned len;
        const unsigned char neg16[]={0xbb,0,0,0xb9,0xff,0xff,0x0f,0xa3,0x0f,0x0f,0x92,0x06,0,0x80,0xd6,0xfe};
        const unsigned char neg32[]={0xbb,0,0,0x66,0xb9,0xff,0xff,0xff,0xff,0x66,0x0f,0xa3,0x0f,0x0f,0x92,0x06,0,0x80,0xd6,0xfe};
        const unsigned char pos16[]={0xbb,0xfe,0xff,0xb9,0x10,0,0x0f,0xa3,0x0f,0x0f,0x92,0x06,0,0x80,0xd6,0xfe};
        const unsigned char reg16[]={0xbb,0,0x80,0xb9,0xff,0xff,0x0f,0xa3,0xcb,0x0f,0x92,0x06,0,0x80,0xd6,0xfe};
        const unsigned char imm16[]={0xbb,0,0x20,0x0f,0xba,0x27,0xff,0x0f,0x92,0x06,0,0x80,0xd6,0xfe};
        switch(which){ case 0:seq=neg16;len=sizeof(neg16);break;case 1:seq=neg32;len=sizeof(neg32);break;case 2:seq=pos16;len=sizeof(pos16);break;case 3:seq=reg16;len=sizeof(reg16);break;case 4:seq=imm16;len=sizeof(imm16);break;case 5:seq=neg32;len=sizeof(neg32);break;case 6:seq=pos16;len=sizeof(pos16);break;default:return 4; }
        for(i=0;i<len;i++)c_sas_store(0x80000+i,((which==5 || which==6) && seq[i]==0xa3) ? 0xbb : seq[i]);
        c_sas_store(0xfffe,0);c_sas_store(0xffff,0x80);c_sas_store(0xfffc,0);c_sas_store(0xfffd,0); c_sas_store(0x1ffe,0);c_sas_store(0x1fff,0);c_sas_store(0x2000,0);c_sas_store(0x2001,0x80);c_sas_store(0,1);c_sas_store(1,0);c_sas_store(0x8000,0xff);c_sas_store(0x10000,0);c_sas_store(0x10001,0);c_sas_store(0xffffc,0);c_sas_store(0xffffd,0);c_sas_store(0xffffe,0);c_sas_store(0xfffff,0x80);
        (void)c_setDS(0);(void)c_setCS(0x8000);c_setIP(0);
        if (!setjmp(bit_fault_frame)) host_simulate();
        fprintf(stderr,\"BITCASE=%d fault=%d near=%04x near32=%02x%02x%02x%02x far=%04x zero=%04x plus64k=%04x alias=%02x%02x%02x%02x immediate=%04x carry=%u\\n\",which,bit_fault_vector,(unsigned)c_sas_w_at(0xfffe),(unsigned)c_sas_hw_at(0xffff),(unsigned)c_sas_hw_at(0xfffe),(unsigned)c_sas_hw_at(0xfffd),(unsigned)c_sas_hw_at(0xfffc),(unsigned)c_sas_w_at(0x1ffe),(unsigned)c_sas_w_at(0),(unsigned)c_sas_w_at(0x10000),(unsigned)c_sas_hw_at(0xfffff),(unsigned)c_sas_hw_at(0xffffe),(unsigned)c_sas_hw_at(0xffffd),(unsigned)c_sas_hw_at(0xffffc),(unsigned)c_sas_w_at(0x2000),(unsigned)c_sas_hw_at(0x8000));fflush(stderr);
        sas_term(); return 0;
    }
"""

base=base.replace('static half_word port_bytes[4];','static jmp_buf bit_fault_frame;\nstatic int bit_fault_vector=-1;\nBOOL host_exint_hook(IS32 number, IS32 error) { (void)error; bit_fault_vector=(int)number; longjmp(bit_fault_frame,1); return FALSE; }\nstatic half_word port_bytes[4];'); base=base.replace('extern unsigned fixture_eoi_hook_calls;', 'extern unsigned long DelayIrqLine;\nextern unsigned fixture_eoi_hook_calls;'); marker='        ica_hw_interrupt(ICA_MASTER, CPU_TIMER_INT, 1);'; assert base.count(marker)==1; base=base.replace(marker,'        /* Test-only: model the original SPCKBD-ready boundary. */\n        DelayIrqLine = 0;\n'+marker); marker2='        interrupt_vector = ica_intack(&hook_address);'; assert base.count(marker2)==1; base=base.replace(marker2,marker2+'\n        fprintf(stderr, "PIC16 vector=%ld eoi=%u wow=%u\\n",(long)interrupt_vector,fixture_eoi_hook_calls,fixture_wow_idle_calls);'); out=pathlib.Path('build/t473-s10/ccpu-bit-matrix-probe');out.mkdir(parents=True,exist_ok=True);(out/'ccpu-fixture.c').write_text(base.replace(needle,extra+needle))
