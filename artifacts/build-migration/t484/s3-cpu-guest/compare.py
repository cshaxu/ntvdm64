import hashlib,json,pathlib,subprocess,time,os
r=pathlib.Path(__file__).resolve().parents[2]
out=pathlib.Path(__file__).parent
start=time.time()
rows=[]
cases=[('Control','ccpu_control_probe.asm'),('SignFlags','ccpu_sign_flag_probe.asm'),('BitMemory','ccpu_bit_memory_probe.asm'),('BiosDispatch','bios_dispatch_probe.asm'),('DummyInt','bios_dispatch_probe.asm'),('SystemDevices','system_capability.asm'),('DiskReset','disks_capability.asm'),('VideoInvalid','bios_dispatch_probe.asm'),('SlaveBios','slave_bios_vector_probe.asm'),('RtcEntry','rtc_entry_probe.asm'),('RtcAlarm','rtc_alarm_probe.asm')]
matrix=[]
for name,source in cases:
    flags={'DummyInt':['-DCPU_PROBE_DUMMY_INT_ONLY=1'],'VideoInvalid':['-DCPU_PROBE_VIDEO_INVALID_ONLY=1']}.get(name,[])
    matrix.append((name,source,flags))
    if name not in ('BitMemory','SlaveBios','RtcEntry','RtcAlarm'):
        matrix.append((name+'-negative',source,flags+['-DCPU_PROBE_NEGATIVE_ORACLE=1']))
matrix += [('Transfer','ccpu_transfer_probe.asm',[]),('Transfer-negative','ccpu_transfer_probe.asm',['-DCPU_TRANSFER_NEGATIVE=1']),('Alu','ccpu_alu_probe.asm',[]),('CallReturn','ems_call_return_probe.asm',[]),('Mapping','ems_mapping_probe.asm',[])]
nasm=next(x.split('=',1)[1] for x in (r/'build/.ninja/fixtures-x86/CMakeCache.txt').read_text().splitlines() if x.startswith('NTVDM_NASM:FILEPATH='))
for name,source,flags in matrix:
    baseline=out/(name+'.reference.com')
    command=[nasm,'-f','bin',*flags,'-o',str(baseline),str(r/'test/observation'/source)]
    subprocess.run(command,check=True,capture_output=True)
    actual=r/'build/.ninja/fixtures-x86/guest-probes/cpu-integration'/(name+'.COM')
    same=baseline.read_bytes()==actual.read_bytes()
    rows.append(dict(name=name,source=source,flags=flags,bytes=actual.stat().st_size,sha256=hashlib.sha256(actual.read_bytes()).hexdigest(),identical=same))
result=dict(pid=os.getpid(),start=start,end=time.time(),cases=rows,passed=all(x['identical'] for x in rows))
(out/'comparison.json').write_text(json.dumps(result,indent=2)+'\n')
assert len(rows)==23 and result['passed']
print('PASS: 23/23 native probe outputs byte-identical to independent NASM invocations')
