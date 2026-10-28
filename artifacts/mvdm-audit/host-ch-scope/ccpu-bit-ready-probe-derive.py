import pathlib
base=pathlib.Path('tests/mvdm-host/ccpu_bounded_execution_fixture.c').read_text()
needle='    if (fixture_host_close_calls != 1u || !fixture_high_linear_page()) {'
assert base.count(needle)==1
extra='''    {
        unsigned round, i;
        const unsigned char seq[] = {0xbb,0x00,0x00,0xb9,0xff,0xff,0x0f,0xa3,0x0f,0x0f,0x92,0x06,0x00,0x80,0xd6,0xfe};
        for (round=0; round<2; ++round) {
            for(i=0; i<sizeof(seq); ++i) c_sas_store(0x80000+i, (i==7 && round) ? 0xbb : seq[i]);
            c_sas_store(0xfffe,0); c_sas_store(0xffff,0x80);
            c_sas_store(0x1ffe,0); c_sas_store(0x1fff,0);
            c_sas_store(0x8000,0xff);
            fputs("BIT before segment\\n", stderr); (void)c_setDS(0); (void)c_setCS(0x8000); c_setIP(0);
            fputs("BIT before simulation\\n", stderr); host_simulate(); fputs("BIT after simulation\\n", stderr);
            fprintf(stderr,"BIT16 op=%s near=%04x far=%04x carry=%u\\n",round?"BTC":"BT",(unsigned)c_sas_w_at(0xfffe),(unsigned)c_sas_w_at(0x1ffe),(unsigned)c_sas_hw_at(0x8000)); fflush(stderr);
        }
    }
'''
base=base.replace('extern unsigned fixture_eoi_hook_calls;', 'extern unsigned long DelayIrqLine;\nextern unsigned fixture_eoi_hook_calls;'); marker='        ica_hw_interrupt(ICA_MASTER, CPU_TIMER_INT, 1);'; assert base.count(marker)==1; base=base.replace(marker,'        /* Test-only: model the original SPCKBD-ready boundary. */\n        DelayIrqLine = 0;\n'+marker); marker2='        interrupt_vector = ica_intack(&hook_address);'; assert base.count(marker2)==1; base=base.replace(marker2,marker2+'\n        fprintf(stderr, "PIC16 vector=%ld eoi=%u wow=%u\\n",(long)interrupt_vector,fixture_eoi_hook_calls,fixture_wow_idle_calls);'); out=pathlib.Path('build/t473-s10/ccpu-bit-ready-probe');out.mkdir(parents=True,exist_ok=True);(out/'ccpu-fixture.c').write_text(base.replace(needle,extra+needle))
