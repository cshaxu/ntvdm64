// Test-only compilation of the actual exception translation unit with observed
// CPU/host boundaries. No fixture body is a production provider.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import crypto from 'node:crypto';
const out=path.resolve(process.argv[2]);
assert(/^build\/M\d+-T\d+\/S\d+\/.+/.test(path.relative(process.cwd(),out).replaceAll('\\','/')));
fs.mkdirSync(out,{recursive:true});
const read=p=>fs.readFileSync(p,'utf8').replaceAll('\r','');
const file='src/mvdm/softpc.new/base/ccpu386/c_xcptn.c';
const current=read(file);
const original=read('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/ccpu386/c_xcptn.c');
const patch=fs.readFileSync('O:/repos.external/ntvdmx64/ntvdmpatch/patches/common/ccpu.patch');
assert.equal(crypto.createHash('sha256').update(patch).digest('hex'),'d3ea35d3f7c3bd4617f5f6edc90725d3a70f503e353677c251720d9e5c9929fc');
const patchText=patch.toString('utf8').replaceAll('\r','');
for(const [start,end] of [['252c259','312c320'],['312c320','373c382'],['373c382','437c447'],['437c447','482a494,502'],['583c603','641a663,674']]) {
 const hunk=patchText.slice(patchText.indexOf(start+'\n'),patchText.indexOf(end+'\n'));
 assert(hunk.length>0,'Pinned upstream hunk must remain available for provenance review');
}
const block=/if\(GET_PE\(\) && host_exint_hook\([^\n]+\) \{\n[\s\S]*?c_cpu_continue\(\);[^\n]*\n[\t ]*\}/g;
assert.equal([...current.matchAll(block)].length,5);
const releaseGuard=/^#ifdef PROD\n\/\* DIVERGENCE\(MVDM-HOST-DIV-271\):[\s\S]*?^#define check_exception_env\(\)\n#else\n(#define check_exception_env\(\)[\s\S]*?^}\n)#endif\n/m;
assert.match(current,releaseGuard,
 'Production must make the non-product CCPU_SHOW_EXCEPTIONS gate inert');
const before=current.replace(releaseGuard,'$1')
 .replace(/^[\t ]*\/\* DIVERGENCE: MVDM-HOST-DIV-268:.*\n/gm,'')
 .replace(block,s=>s.replace(') {',')').replace(/\n[\t ]*\}$/,''));
const normalize=s=>s.replace(/^#include <yoda.h>\n/m,'')
 .replace(/^\s*IMPORT char \*host_getenv.*\n/m,'');
assert.equal(normalize(before),normalize(original),'Only DIV-268 and the PROD-only DIV-271 release gate may differ from original exception logic');
const prelude=String.raw`
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#define GLOBAL
#define LOCAL static
#define IMPORT extern
#define SAVED static
#define VOID void
#define TRUE 1
#define FALSE 0
#define NTVDM 1
#define IPT1(t,a) (t a)
#define IPT2(t,a,u,b) (t a,u b)
#define IPT3(t,a,u,b,v,c) (t a,u b,v c)
#define IFN0() (void)
#define IFN1(t,a) (t a)
#define IFN2(t,a,u,b) (t a,u b)
#define IFN3(t,a,u,b,v,c) (t a,u b,v c)
typedef int BOOL,IBOOL,ISM32,IS32;
typedef uint16_t IU16;
typedef uint32_t IU32,IUH;
#define INTERNAL 0
#define EXTERNAL 1
#define CS_REG 0
#define CR_PFLA 2
static int pe,hook_result,hook_calls,deliveries,resets,operand,address,pop;
static int observed_vector,observed_error,observed_push,observed_priv;
static IU32 ip;
static jmp_buf checkpoint;
FILE *trace_file;
IBOOL took_absolute_toc;
#define SET_EIP(v) (ip=(v))
#define GET_EIP() ip
#define GET_PE() pe
#define GET_CS_SELECTOR() 0x1234
#define GET_SR_AR_X(r) 1
#define GET_IDT_LIMIT() 0xffff
#define GET_CR(r) 0xdead0000
#define SET_OPERAND_SIZE(v) (operand=(v))
#define SET_ADDRESS_SIZE(v) (address=(v))
#define SET_POP_DISP(v) (pop=(v))
#define strcasecmp _stricmp
static int expose_exception_environment;
static char *host_getenv(char *s) { (void)s;return expose_exception_environment?"1":NULL; }
static void force_yoda(void) { abort(); }
static void check_interface_active(int n) { (void)n; }
static void c_cpu_reset(void) { resets++; }
static void c_cpu_continue(void) { longjmp(checkpoint,1); }
BOOL host_exint_hook(IS32 n,IS32 e) {
 hook_calls++; observed_vector=n;observed_error=e;return hook_result;
}
static void do_intrupt(IU16 n,IBOOL priv,IBOOL push,IU16 e) {
 deliveries++;observed_vector=n;observed_error=e;observed_push=push;observed_priv=priv;
}
void DF(IU16);
`;
const harness=String.raw`
static int failures,cases;
#define CHECK(x) do { if(!(x)) { printf("FAIL case=%d line=%d: %s\n",cases,__LINE__,#x); failures++; } } while(0)
static void init(int mode,int handled) {
 pe=mode;hook_result=handled;hook_calls=deliveries=resets=0;
 doing_fault=doing_contributory=doing_page_fault=doing_double_fault=0;
 took_absolute_toc=0;EXT=EXTERNAL;ip=0x2222;CCPU_save_EIP=0xfffe;
 operand=address=pop=7;observed_push=observed_priv=-1;observed_error=observed_vector=-1;
}
int main(void) {
 int kind,mode,handled,expected,vector,error;trace_file=stderr;
 for(kind=0;kind<6;kind++) for(mode=0;mode<2;mode++) for(handled=0;handled<2;handled++) {
  init(mode,handled);cases++;expected=mode&&handled;
  if(!setjmp(checkpoint)) switch(kind) {
   case 0: GP(0x1237,0);break;
   case 1: GP_INT(0x181,0);break;
   case 2: DF(0);break;
   case 3: Int0();break;
   case 4: PF(7,0);break;
   case 5: Int6();break;
  }
  vector=kind<2?13:kind==2?8:kind==3?0:kind==4?14:6;
  error=kind==0?0x1235:kind==1?0x40b:kind==4?7:0;
  CHECK(ip==0xfffe);CHECK(resets==0);CHECK(hook_calls==mode);
  CHECK(deliveries==!expected);CHECK(observed_vector==vector);CHECK(observed_error==error);
  CHECK(took_absolute_toc==!expected);
  CHECK(doing_contributory==((kind<2||(kind==3&&mode))&&!expected));
  CHECK(doing_page_fault==(kind==4&&!expected));
  CHECK(doing_double_fault==(kind==2&&mode&&!expected));
  CHECK(doing_fault==(kind!=2 && !((kind==3||kind==4)&&expected)));
  if(!expected) {
   CHECK(operand==1&&address==1&&pop==0);CHECK(observed_priv==0);
   CHECK(observed_push==(kind<3||kind==4));
  } else { CHECK(operand==7&&address==7&&pop==7); }
 }
 // Nested fault escalation and triple-fault reset use the original bodies.
 for(kind=0;kind<3;kind++) {
  init(1,0);cases++;
  if(kind==0) doing_contributory=1;
  if(kind==1) doing_page_fault=1;
  if(kind==2) doing_double_fault=1;
  if(!setjmp(checkpoint)) { if(kind==1) PF(7,0);else GP(8,0); }
  if(kind<2) { CHECK(deliveries==1);CHECK(observed_vector==8);CHECK(observed_error==0);CHECK(doing_double_fault); }
  else { CHECK(resets==1);CHECK(deliveries==0);CHECK(!doing_double_fault&&!doing_page_fault&&!doing_contributory); }
 }
 printf("%s %d actual-source exception cases; failures=%d\n",failures?"FAIL":"PASS",cases,failures);
 return failures?1:0;
}
`;
const prodHarness=String.raw`
int main(void) {
 trace_file=NULL;expose_exception_environment=1;
 pe=0;hook_result=0;hook_calls=deliveries=resets=0;
 doing_fault=doing_contributory=doing_page_fault=doing_double_fault=0;
 took_absolute_toc=0;EXT=EXTERNAL;ip=0x2222;CCPU_save_EIP=0xfffe;
 operand=address=pop=7;observed_push=observed_priv=-1;observed_error=observed_vector=-1;
 if(!setjmp(checkpoint)) Int0();
 if(show_exceptions || trap_exceptions || deliveries!=1 || observed_vector!=0 ||
    observed_error!=0 || observed_priv!=0) {
  printf("FAIL PROD exception environment live=%d trap=%d deliveries=%d vector=%d error=%d priv=%d\\n",
   show_exceptions,trap_exceptions,deliveries,observed_vector,observed_error,observed_priv);return 1;
 }
 printf("PASS PROD CCPU_SHOW_EXCEPTIONS is inert without trace_file\\n");
 return 0;
}
`;
for(const [name,source] of [['exception-profile',current],['exception-before',before]])
 fs.writeFileSync(path.join(out,name+'.c'),prelude+source.replace(/^#include[^\n]*\n/gm,'')+harness);
fs.writeFileSync(path.join(out,'exception-prod-environment.c'),
 prelude+'\n#define PROD 1\n'+current.replace(/^#include[^\n]*\n/gm,'')+prodHarness);
console.log('PASS exact five-block source delta and pinned upstream patch hash');
