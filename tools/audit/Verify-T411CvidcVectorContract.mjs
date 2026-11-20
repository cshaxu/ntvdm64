import fs from 'node:fs';
import path from 'node:path';

function requireText(root, relative, expected) {
  const source = fs.readFileSync(path.join(root, relative), 'utf8');
  if (!expected.test(source)) {
    throw new Error(`${relative}: missing selected-contract marker ${expected}`);
  }
  return source;
}

function listFromMetadata(source, name) {
  const match = source.match(new RegExp(`char\\s*\\*\\s*${name}\\[\\]\\s*=\\s*\\{([\\s\\S]*?)\\};`));
  if (!match) throw new Error(`c2cpusad.h: missing ${name}`);
  return [...match[1].matchAll(/"([A-Za-z0-9_]+)"/g)].map((entry) => entry[1]);
}

const root = path.resolve(process.argv[2] ?? '.');
const mapPath = process.argv[3] == null ? null : path.resolve(root, process.argv[3]);
const metadata = requireText(root,
  'src/mvdm-host/softpc.new/base/cvidc/c2cpusad.h', /CpuVectorNames/);
const publicSlots = listFromMetadata(metadata, 'CpuVectorNames');
const privateSlots = listFromMetadata(metadata, 'CpuPrivateVectorNames');
const generator = requireText(root, 'tools/build/GenerateCvidcCpuBinding.mjs',
  /CalcQuickEventInstTime:\s*'c_cpu_calc_q_ev_inst_for_time'/);
const binder = requireText(root,
  'src/compat/mvdm_cvidc_vector_binding.c',
  /Cpu\.Sas\s*=\s*&Sas[\s\S]*Sas\.Sas_overwrite_memory\s*=\s*c_sas_overwrite_memory[\s\S]*Cpu\.Video\s*=\s*\(IHP\)&Video/);
const qevnt = requireText(root,
  'src/mvdm-host/softpc.new/base/system/qevnt.c',
  /ULONG\s+qevJumpRestart\s*=\s*100\s*;/);
const cvidAccess = requireText(root,
  'src/mvdm-host/softpc.new/base/cvidc/accessfn.c',
  /Cpu\.CalcQuickEventInstTime/);
const ccpuHeader = requireText(root,
  'src/mvdm-host/softpc.new/base/ccpu386/cpu4gen.h',
  /#define\s+effective_addr\(seg, offset\)\s+c_effective_addr\(seg, offset\)/);
const evGlue = requireText(root, 'src/mvdm-host/softpc.new/base/cvidc/ev_glue.c',
  /setup_vga_globals[\s\S]*mvdm_cvidc_bind_vectors\(\)/);
const localfm = requireText(root, 'src/mvdm-host/softpc.new/base/ccpu386/localfm.c',
  /struct CpuVector Cpu\s*;/);
const main = requireText(root, 'src/mvdm-host/softpc.new/base/support/main.c',
  /config\([\s\S]*setup_vga_globals\(\);[\s\S]*host_init_screen\([\s\S]*InitialiseDosEmulation\([\s\S]*host_start_cpu\(/);
requireText(root, 'src/mvdm-host/softpc.new/base/cvidc/evidgen.h',
  /#ifndef CPU_40_STYLE\s+(?:#line\s+\d+\s+)?#define\s+getVideorplane/);
requireText(root, 'src/mvdm-host/softpc.new/base/inc/egacpu.h',
  /extern IS32 getVideodirty_low IPT0\(\)/);
if (/video_get_|video_set_|MVDM_CVIDC_VIDEO_BIND|boundVideo/.test(binder + generator)) {
  throw new Error('Autonomous accessor rebinding or typed wrapper generation returned');
}
if (!/extern\s+ULONG\s+qevJumpRestart\s*;[\s\S]*return\s+\(IUH\)qevJumpRestart\s*;[\s\S]*qevJumpRestart\s*=\s*\(ULONG\)value\s*;/.test(binder) ||
    /static\s+IUH\s+mvdm_cvidc_jump_restart/.test(binder)) {
  throw new Error('quick-event restart state must remain owned by original qevnt.c');
}

if (publicSlots.length !== 154 || privateSlots.length !== 55) {
  throw new Error(`unexpected original metadata count: public=${publicSlots.length}, private=${privateSlots.length}`);
}
if (!publicSlots.includes('CalcQuickEventInstTime') ||
    !generator.includes('ClearHwInt: \'0\'') ||
    !generator.includes("GetCpuState: '0'") ||
    !generator.includes("SetCpuState: '0'")) {
  throw new Error('selected C-VID provider/null-slot disposition drifted');
}
if (!binder.includes('mvdm_cvidc_bind_video_vector();') ||
    !cvidAccess.includes('result = (*(Cpu.CalcQuickEventInstTime))(val);') ||
    !ccpuHeader.includes('c_effective_addr(seg, offset)') ||
    !evGlue.includes('mvdm_cvidc_bind_vectors();')) {
  throw new Error('CCPU/C-VID profile-routing contract drifted');
}
if (!/(^|\n)IHP\s+Gdp\s*;/.test(evGlue) ||
    !/(^|\n)IHP\s+Gdp\s*;/.test(localfm) ||
    !/\bGdp\s*=\s*\(IHP\)host_malloc\(64 \* 1024\);/.test(evGlue)) {
  throw new Error('GDP must retain the original C declarations and C-VID allocation');
}
if (mapPath !== null) {
  const map = fs.readFileSync(mapPath, 'utf8');
  if (!/^.*_Gdp\s+\S+\s+(?:original-softpc-cvidc:ev_glue\.obj|<common>)\s*$/m.test(map) ||
      !/^.*_mvdm_cvidc_bind_vectors\s+\S+\s+f\s+original-softpc-cvidc:mvdm_cvidc_vector_binding\.obj\s*$/m.test(map) ||
      !/^.*_c_effective_addr\s+\S+\s+f\s+mvdm-softpc-effective-address:mvdm_softpc_effective_address\.obj\s*$/m.test(map)) {
    throw new Error('final map is missing a selected GDP, vector binder or effective-address provider');
  }
}

console.log(JSON.stringify({
  public_slots: publicSlots.length,
  private_slots: privateSlots.length,
  quick_event_slot: 'c_cpu_calc_q_ev_inst_for_time',
  ccpu_effective_address: 'direct-c_effective_addr',
  original_startup_binding_order: true,
  setup_bind: true,
  quick_event_restart_owner: 'system/qevnt.c::qevJumpRestart',
  gdp_initializer: 'cvidc/ev_glue.c',
  gdp_map_verified: mapPath !== null,
  unsupported_clear_hw_int: 'null'
}));
