// Test-only source extraction and DOS fixtures; all outputs stay in build/.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
const out=path.resolve(process.argv[2]);
assert(/^build\/M\d+-T\d+\/S\d+\/.+/.test(path.relative(process.cwd(),out).replaceAll('\\','/')));
fs.mkdirSync(out,{recursive:true});
const read=p=>fs.readFileSync(p,'utf8').replaceAll('\r','');
const current=read('src/mvdm-host/softpc.new/base/ccpu386/c_main.c');
const original=read('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/ccpu386/c_main.c');
const macros=s=>s.slice(s.indexOf('#define UPDATE_INTEL_IP(x)'),s.indexOf('/* mark host format IP'));
assert.equal(macros(current),macros(original));
assert(!current.includes('CCPU_INSTRUCTION_DELTA'));
assert(current.includes('SET_EIP(GET_EIP() + DIFF_INST_BYTE(*q, p_start));'));
fs.writeFileSync(path.join(out,'ip-profile.c'),`#include <stdint.h>
#include <stdio.h>
typedef uint32_t IU32;
static IU32 ip,cs,operand;
static unsigned char bytes[8192], *p_start=bytes+4096;
#define USE16 0
#define WORD_MASK 0xffff
#define GET_EIP() ip
#define SET_EIP(x) ip=(x)
#define GET_CS_AR_X() cs
#define GET_OPERAND_SIZE() operand
#define DIFF_INST_BYTE(x,y) ((x)-(y))
${macros(current)}
int main(void) {
 const IU32 ips[]={0,0xffe,0xfffe,0xffff,0x10000,0xfffffffe};
 const int deltas[]={-4096,-4,0,2,4,15,4096};
 unsigned a,b,c; unsigned count=0;
 for(a=0;a<6;a++) for(b=0;b<7;b++) for(c=0;c<2;c++) {
  IU32 expected=(ips[a]+deltas[b]) & (c?0xffffffff:0xffff);
  ip=ips[a]; operand=c; UPDATE_INTEL_IP_USE_OP_SIZE(p_start+deltas[b]);
  if(ip!=expected) return 1;
  ip=ips[a]; cs=c; UPDATE_INTEL_IP(p_start+deltas[b]);
  expected=(ips[a]+deltas[b]) & ((!c&&ips[a]<0x10000)?0xffff:0xffffffff);
  if(ip!=expected) return 2;
  count+=2;
 }
 if((0xfffe+(4&0xffff))==((0xfffe+4)&0xffff)) return 3;
 printf("PASS %u original IP macro cases; old expression negative control fails as expected\\n",count);
 return 0;
}
`);
const cases=[];
function mz(name,entry,write) {
 const code=Buffer.alloc(65536,0xcc), header=Buffer.alloc(32);
 const emit=(at,...bytes)=>Buffer.from(bytes).copy(code,at);
 const pass=at=>emit(at,0xb8,0,0x4c,0xcd,0x21);
 write(emit,pass);
 const total=32+code.length;
 header.write('MZ');header.writeUInt16LE(total%512,2);header.writeUInt16LE(Math.ceil(total/512),4);
 header.writeUInt16LE(2,8);header.writeUInt16LE(0x100,10);header.writeUInt16LE(0xffff,12);
 header.writeUInt16LE(0x1000,14);header.writeUInt16LE(0xff0,16);header.writeUInt16LE(entry,20);
 header.writeUInt16LE(0x1c,24);
 fs.writeFileSync(path.join(out,name+'.EXE'),Buffer.concat([header,code]));cases.push(name);
}
// Near conditional jump ends exactly at 64 KiB; both outcomes must wrap.
for(const taken of [false,true]) mz(taken?'TAKEN16':'SKIP16',0xfff8,(e,p)=>{
 e(0xfff8,0x31,0xc0,0x90,0x90,0x0f,taken?0x84:0x85,0,2);p(0);p(0x200);
});
// Backward near branches and both 32-bit operand-size conditional outcomes.
mz('BACK16',0x200,(e,p)=>{e(0x200,0x31,0xc0,0x0f,0x84,0xfa,0xfe);p(0x100);});
for(const taken of [false,true]) mz(taken?'TAKEN32':'SKIP32',0x200,(e,p)=>{
 e(0x200,0x31,0xc0,0x66,0x0f,taken?0x84:0x85,0xf7,0,0,0);p(0x209);p(0x300);
});
// A five-byte immediate spans a guest page; CALL checks the pushed return IP.
mz('PAGE',0xffe,(e,p)=>{e(0xffe,0x66,0xb8,0x78,0x56,0x34,0x12);p(0x1004);});
mz('CALL16',0xfffd,(e,p)=>{e(0xfffd,0xe8,0,2);e(0x200,0x58,0x85,0xc0,0x75,5);p(0x205);e(0x20a,0xb8,1,0x4c,0xcd,0x21);});
mz('CALL32',0x200,(e,p)=>{e(0x200,0x66,0xe8,0xfa,0,0,0);e(0x300,0x66,0x58,0x66,0x3d,6,2,0,0,0x75,5);p(0x30a);e(0x30f,0xb8,1,0x4c,0xcd,0x21);});
mz('BACK32',0x200,(e,p)=>{e(0x200,0x31,0xc0,0x66,0x0f,0x84,0xf7,0xfe,0xff,0xff);p(0x100);});
// Normal scalar plus REP string execution, then a backward short loop.
mz('STRING',0x100,(e,p)=>{e(0x100,0x0e,0x1f,0x0e,0x07,0xfc,0xbe,0,4,0xbf,0,5,0xb9,16,0,0xf3,0xa4,0xb9,2,0,0xe2,0xfe);p(0x115);});
// Install and restore a private DOS vector; inspect the real guest trap frame.
for(const fault of [false,true]) mz(fault?'FAULT':'TRAP',0x100,(e)=>{
 const vector=fault?0:3, target=fault?0xfffc:0xffff, expected=fault?0xfffc:0;
 e(0x100,0x0e,0x1f,0xb8,vector,0x35,0xcd,0x21,0x89,0x1e,0,4,
   0x8c,0x06,2,4,0xba,0,2,0xb8,vector,0x25,0xcd,0x21,
   0xb8,1,0,0x31,0xc9,0x31,0xd2,0xe9,(target-0x121)&255,((target-0x121)>>8)&255);
 if(fault)e(target,0xf7,0xf1);else e(target,0xcc);
 // Pop saved IP, compare expected, preserve result in BP, restore vector.
 e(0x200,0x58,0xbd,1,0,0x3d,expected&255,expected>>8,0x75,2,0x31,0xed,
   0x2e,0x8b,0x16,0,4,0x2e,0x8e,0x1e,2,4,0xb8,vector,0x25,0xcd,0x21,
   0x89,0xe8,0xb4,0x4c,0xcd,0x21);
});
fs.writeFileSync(path.join(out,'guest-cases.json'),JSON.stringify(cases));
console.log('PASS original IP cohort identity; generated host and '+cases.length+' guest fixtures');
