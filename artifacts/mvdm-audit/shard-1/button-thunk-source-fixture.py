from pathlib import Path
import hashlib,re,subprocess,tempfile
root=Path(__file__).resolve().parents[3]
source=root/'src/mvdm/wow32/wmsgbm.c'
expected='D7EF837D592221481C988CCB2E164FAFF9D7A0B6349B59E1BBC3841323A98692'
raw=source.read_bytes()
if hashlib.sha256(raw).hexdigest().upper()!=expected: raise RuntimeError('source differs from frozen identity')
text=raw.decode('latin1')
def extract(name):
 m=re.search(r'(?:BOOL|VOID)\s+FASTCALL\s+'+name+r'\s*\([^)]*\)\s*\{',text)
 if not m: raise RuntimeError('missing '+name)
 i=m.end(); depth=1
 while depth and i<len(text):
  if text[i]=='{':depth+=1
  if text[i]=='}':depth-=1
  i+=1
 if depth:raise RuntimeError('unbalanced')
 return text[m.start():i]
prefix=r'''
#include <windows.h>
#include <stdio.h>
#define FASTCALL
#define LOGDEBUG(a,b) ((void)0)
#define WIN31_BM_CLICK (WM_USER+99)
typedef struct { struct { struct { WORD wMsg; } WndProc; } Parm16; UINT uMsg; } MSGPARAMEX, *LPMSGPARAMEX;
'''
suffix=r'''
int main(void) {
 MSGPARAMEX m; unsigned i; unsigned tests[]={WM_USER,WM_USER+1,WM_USER+2,WM_USER+3,WM_USER+4,WM_USER+5,WIN31_BM_CLICK,WM_USER+200};
 for(i=0;i<sizeof(tests)/sizeof(tests[0]);i++) {m.Parm16.WndProc.wMsg=(WORD)tests[i];m.uMsg=tests[i];
 if(!ThunkBMMsg16(&m))return 2; UnThunkBMMsg16(&m);
 printf("%04x -> %04x\n",tests[i],m.uMsg);
 if(m.uMsg != (i<5?BM_GETCHECK+i:(i==6?BM_CLICK:tests[i])))return 3;
 }
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='button-',dir=root/'build') as td:
 c=Path(td)/'fixture.c';exe=Path(td)/'fixture.exe';c.write_text(prefix+extract('ThunkBMMsg16')+extract('UnThunkBMMsg16')+suffix,encoding='utf-8')
 subprocess.run(['gcc','-std=c11','-O0',str(c),'-o',str(exe)],check=True)
 print('source_sha256='+expected)
 print(subprocess.run([str(exe)],capture_output=True,text=True,check=True).stdout,end='')
