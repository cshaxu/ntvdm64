from pathlib import Path
p=Path('tests/mvdm-host/ccpu_bounded_execution_fixture.c');s=p.read_text();needle='        ica_hw_interrupt(ICA_MASTER, CPU_TIMER_INT, 1);';assert s.count(needle)==1
s=s.replace(needle,'''        if (GetEnvironmentVariableA("PIC_READY_CONTROL", NULL, 0)) { DelayIrqLine = 0; fprintf(stderr, "PIC ready-control enabled\\n"); }
        { half_word ir=0,mask=0; ica_outb(0x20,0x0a); ica_inb(0x20,&ir); ica_inb(0x21,&mask); fprintf(stderr,"PIC before request irr=%02x imr=%02x DelayIrqLine=%08lx\\n", ir, mask,(unsigned long)DelayIrqLine); }
'''+needle+'''
        { half_word ir=0,mask=0; ica_outb(0x20,0x0a); ica_inb(0x20,&ir); ica_inb(0x21,&mask); fprintf(stderr,"PIC after request irr=%02x imr=%02x DelayIrqLine=%08lx\\n", ir, mask,(unsigned long)DelayIrqLine); }
''')
s=s.replace('        interrupt_vector = ica_intack(&hook_address);', '        interrupt_vector = ica_intack(&hook_address);\n        fprintf(stderr, "PIC intack vector=%ld eoi=%u wow=%u\\n",(long)interrupt_vector, fixture_eoi_hook_calls, fixture_wow_idle_calls);')
s=s.replace('extern unsigned fixture_eoi_hook_calls;','extern unsigned long DelayIrqLine;\nextern unsigned fixture_eoi_hook_calls;')
Path('build/t473-s10/bitwrap-probe/pic-fixture.c').write_text(s)
