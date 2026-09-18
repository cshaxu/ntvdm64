// Test-only source extraction and DOS fixtures; all outputs stay in build/.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
const out=path.resolve(process.argv[2]);
assert(/^build\/M\d+-T\d+\/S\d+\/.+/.test(path.relative(process.cwd(),out).replaceAll('\\','/')));
fs.mkdirSync(out,{recursive:true});
const read=p=>fs.readFileSync(p,'utf8').replaceAll('\r','');
const current=read('src/mvdm/softpc.new/base/ccpu386/c_main.c');
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
const witnesses={};
function mz(name,entry,write,requiredWitnesses=[]) {
 assert(/^[A-Z0-9]{1,8}$/.test(name),'DOS fixture name must be 8.3-safe');
 const code=Buffer.alloc(65536,0xcc), header=Buffer.alloc(32);
 const emit=(at,...bytes)=>Buffer.from(bytes).copy(code,at);
 const marker=`CCPU-${name}-OK\r\n$`;
 /* A successful guest must leave a textual Console witness as well as a
  * zero DOS exit status.  The checked instruction layout often gives the
  * success branch only five bytes before its failure branch, so each branch
  * uses a three-byte near CALL to a shared, fixed success epilogue.  8000h
  * keeps every selected source address within the signed 16-bit CALL range. */
 const pass=at=>{const rel=(0x8000-(at+3))&0xffff;emit(at,0xe8,rel&255,rel>>8)};
 emit(0x8000,0x0e,0x1f,0xba,0,0x81,0xb4,9,0xcd,0x21,
      0xb8,0,0x4c,0xcd,0x21);
 emit(0x8100,...Buffer.from(marker,'ascii'));
 write(emit,pass);
 /* Some original fixture bodies predate the shared success helper and still
  * spell `mov ax,4c00h; int 21h` directly.  Route only that exact successful
  * DOS exit spelling through the same short witness trampoline; 4c01h
  * failure exits deliberately remain untouched. */
 for(let at=0;at<0x7ffb;at++) {
  if(code[at]===0xb8 && code[at+1]===0 && code[at+2]===0x4c &&
     code[at+3]===0xcd && code[at+4]===0x21) {
   pass(at); emit(at+3,0x90,0x90);
  }
 }
 const total=32+code.length;
 header.write('MZ');header.writeUInt16LE(total%512,2);header.writeUInt16LE(Math.ceil(total/512),4);
 header.writeUInt16LE(2,8);header.writeUInt16LE(0x100,10);header.writeUInt16LE(0xffff,12);
 header.writeUInt16LE(0x1000,14);header.writeUInt16LE(0xff0,16);header.writeUInt16LE(entry,20);
 header.writeUInt16LE(0x1c,24);
 fs.writeFileSync(path.join(out,name+'.EXE'),Buffer.concat([header,code]));
 cases.push(name);
 witnesses[name]=[`CCPU-${name}-OK`,...requiredWitnesses];
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
// Execute x87 in the real guest. FPU64 specifically checks FISTP m64 rounds
// the 32-bit 1.5 constant at 0410h to the 64-bit integer value 2.
mz('FPU64',0x100,(e)=>{e(0x100,0x0e,0x1f,0xd9,0x06,0x10,4,0xdf,0x3e,0,4,0x81,0x3e,0,4,2,0,0x75,0x1a,0x83,0x3e,2,4,0,0x75,0x13,0x83,0x3e,4,4,0,0x75,0x0c,0x83,0x3e,6,4,0,0x75,0x05,0xb8,0,0x4c,0xcd,0x21,0xb8,1,0x4c,0xcd,0x21,0xb8,2,0x4c,0xcd,0x21);e(0x410,0,0,0xc0,0x3f);});
// FISTP m32 exercises the same precision path without the M64I carrier.
mz('FPUI32',0x100,(e)=>{e(0x100,0x0e,0x1f,0xd9,0x06,0x10,4,0xdb,0x1e,0,4,0x81,0x3e,0,4,2,0,0x75,5,0xb8,0,0x4c,0xcd,0x21,0xb8,1,0x4c,0xcd,0x21);e(0x410,0,0,0xc0,0x3f);});
// Read the guest x87 control word before a precision-producing FISTP.
mz('FPUCW',0x100,(e)=>{e(0x100,0x0e,0x1f,0xd9,0x3e,0,4,0x81,0x3e,0,4,0x7f,3,0x75,5,0xb8,0,0x4c,0xcd,0x21,0xb8,1,0x4c,0xcd,0x21);});
// A masked precision exception remains guest-visible in the x87 status word;
// it must not escape as an unmasked host exception and kill the worker.
mz('FPUSW',0x100,(e)=>{e(0x100,0x0e,0x1f,0xd9,0x06,0x10,4,0xdf,0x3e,0,4,0xdd,0x3e,8,4,0xf7,0x06,8,4,0x20,0,0x74,5,0xb8,0,0x4c,0xcd,0x21,0xb8,1,0x4c,0xcd,0x21);e(0x410,0,0,0xc0,0x3f);});
// 32-bit ALU, shift and signed divide exercise CCPU opcode bodies beyond
// the COMMAND/MEM smoke path.  Each program uses DOS exit status as oracle.
mz('MUL32',0x100,(e)=>{e(0x100,0x0e,0x1f,0x66,0xb8,3,0,0,0,0x66,0x6b,0xc0,7,0x66,0xc1,0xe0,3,0x66,0x3d,0xa8,0,0,0,0x75,5,0xb8,0,0x4c,0xcd,0x21,0xb8,1,0x4c,0xcd,0x21);});
mz('DIV32',0x100,(e)=>{e(0x100,0x0e,0x1f,0x66,0xb8,0xeb,0xff,0xff,0xff,0x66,0x99,0x66,0xb9,7,0,0,0,0x66,0xf7,0xf9,0x66,0x3d,0xfd,0xff,0xff,0xff,0x75,5,0xb8,0,0x4c,0xcd,0x21,0xb8,1,0x4c,0xcd,0x21);});
// BSF is a selected 80386 two-byte scalar opcode.  BSWAP is deliberately
// excluded: the original CCPU dispatch enables it only under SPC486, while
// this product's CPU40 profile is CCPU386.
mz('BSF32',0x100,(e)=>{e(0x100,0x0e,0x1f,0x66,0xb8,0x10,0,0,0,0x66,0x0f,0xbc,0xc0,0x66,0x3d,4,0,0,0,0x75,5,0xb8,0,0x4c,0xcd,0x21,0xb8,1,0x4c,0xcd,0x21);});
// A memory RMW sequence verifies that the selected SAS write/read carrier
// and flag-producing instruction bodies operate on one guest word.
mz('RMW',0x100,(e)=>{e(0x100,0x0e,0x1f,0xc7,0x06,0,4,1,0,0xff,0x06,0,4,0x83,0x06,0,4,2,0x81,0x3e,0,4,4,0,0x75,5,0xb8,0,0x4c,0xcd,0x21,0xb8,1,0x4c,0xcd,0x21);});
// Real guest timer/IRQ progression without the retired V86-monitor HLT
// virtualization contract. The INT 1Ch handler proves actual IRQ delivery.
mz('TIMBUSY',0x100,(e,p)=>{
 e(0x100,0x0e,0x1f,0xb8,0x1c,0x35,0xcd,0x21,0x89,0x1e,0,4,
   0x8c,0x06,2,4,0xba,0,2,0xb8,0x1c,0x25,0xcd,0x21,
   0xc7,0x06,4,4,0,0,0xfb,0x90,0x83,0x3e,4,4,3,0x72,0xf8,
   0x8b,0x16,0,4,0xa1,2,4,0x8e,0xd8,0xb8,0x1c,0x25,0xcd,0x21,
   0x0e,0x1f);
 p(0x136);
 e(0x200,0x50,0x2e,0xff,0x06,4,4,0x58,0xcf);
});
// In the selected CPU40 profile DOS executes at CPL 3. HLT is therefore a
// deliberate negative control: the original CCPU must reject it before the
// post-HLT string, rather than falsely treating it as an idle primitive.
mz('HLTWAIT',0x100,(e)=>{
 e(0x100,0x0e,0x1f,0xba,0,3,0xb4,9,0xcd,0x21,0xfb,0x90,0xf4,
   0xba,0x20,3,0xb4,9,0xcd,0x21,0xb8,0,0x4c,0xcd,0x21);
 e(0x300,...Buffer.from('CCPU-HLTWAIT-BEFORE\\r\\n$','ascii'));
 e(0x320,...Buffer.from('CCPU-HLTWAIT-AFTER\\r\\n$','ascii'));
},['CCPU-HLTWAIT-BEFORE']);
// Install and restore a private DOS vector; inspect the real guest trap frame.
for(const fault of [false,true]) mz(fault?'FAULT':'TRAP',0x100,(e,p)=>{
 const vector=fault?0:3, target=fault?0xfffc:0xffff, expected=fault?0xfffc:0;
 e(0x100,0x0e,0x1f,0xb8,vector,0x35,0xcd,0x21,0x89,0x1e,0,4,
   0x8c,0x06,2,4,0xba,0,2,0xb8,vector,0x25,0xcd,0x21,
   0xb8,1,0,0x31,0xc9,0x31,0xd2,0xe9,(target-0x121)&255,((target-0x121)>>8)&255);
 if(fault)e(target,0xf7,0xf1);else e(target,0xcc);
 // Pop saved IP, compare expected, preserve result in BP, restore vector.
 e(0x200,0x58,0xbd,1,0,0x3d,expected&255,expected>>8,0x75,2,0x31,0xed,
   0x2e,0x8b,0x16,0,4,0x2e,0x8e,0x1e,2,4,0xb8,vector,0x25,0xcd,0x21,
   0x85,0xed,0x75,3);
 p(0x21e); e(0x221,0xb8,1,0x4c,0xcd,0x21);
});
fs.writeFileSync(path.join(out,'guest-cases.json'),JSON.stringify(cases));
fs.writeFileSync(path.join(out,'guest-witnesses.json'),JSON.stringify(witnesses));
console.log('PASS original IP cohort identity; generated host and '+cases.length+' guest fixtures');
