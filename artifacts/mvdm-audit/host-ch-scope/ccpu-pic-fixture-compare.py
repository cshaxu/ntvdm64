import subprocess,os,pathlib,hashlib,json
p=pathlib.Path('build/t473-s10/bitwrap-probe'); exe=p/'probe-pic.exe'; results=[]
for label,ready in [('blocked',False),('ready',True)]:
 env=os.environ.copy();env.pop('PIC_READY_CONTROL',None)
 if ready:env['PIC_READY_CONTROL']='1'
 r=subprocess.run([str(exe.resolve())],capture_output=True,env=env); raw=r.stdout+r.stderr;(p/('pic-'+label+'-capture.txt')).write_bytes(raw)
 txt=raw.decode('utf-8',errors='replace');expected=('PIC intack vector=8 eoi=0 wow=1' if ready else 'PIC intack vector=-1 eoi=0 wow=0');mask=('00000000' if ready else 'ffffffff');expected_exit=(0 if ready else 1)
 assert r.returncode==expected_exit and expected in txt and f'PIC before request irr=00 imr=00 DelayIrqLine={mask}' in txt and f'PIC after request irr=01 imr=00 DelayIrqLine={mask}' in txt,(label,r.returncode,txt[-1000:]);results.append({'case':label,'exit_code':r.returncode,'sha256':hashlib.sha256(raw).hexdigest().upper(),'witness':expected})
print(json.dumps({'exe_sha256':hashlib.sha256(exe.read_bytes()).hexdigest().upper(),'results':results},indent=2))
