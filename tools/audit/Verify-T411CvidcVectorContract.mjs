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
const metadata = requireText(root,
  'src/mvdm/softpc.new/base/cvidc/c2cpusad.h', /CpuVectorNames/);
const publicSlots = listFromMetadata(metadata, 'CpuVectorNames');
const privateSlots = listFromMetadata(metadata, 'CpuPrivateVectorNames');
const generator = requireText(root, 'tools/build/GenerateCvidcCpuBinding.mjs',
  /CalcQuickEventInstTime:\s*'c_cpu_calc_q_ev_inst_for_time'/);
const binder = requireText(root,
  'src/mvdm-overlay/softpc.new/base/cvidc/mvdm_cvidc_vector_binding.c',
  /Cpu\.Sas\s*=\s*&Sas[\s\S]*Sas\.Sas_overwrite_memory\s*=\s*c_sas_overwrite_memory[\s\S]*Cpu\.Video\s*=\s*\(IHP\)&Video/);
const cvidAccess = requireText(root,
  'src/mvdm/softpc.new/base/cvidc/accessfn.c',
  /Cpu\.CalcQuickEventInstTime/);
const ccpuHeader = requireText(root,
  'src/mvdm/softpc.new/base/ccpu386/cpu4gen.h',
  /#define\s+effective_addr\(seg, offset\)\s+c_effective_addr\(seg, offset\)/);
const xt = requireText(root, 'src/mvdm/softpc.new/base/support/xt.c',
  /#ifdef CPU_30_STYLE[\s\S]*Cpu\.EffectiveAddr/);
const evGlue = requireText(root, 'src/mvdm/softpc.new/base/cvidc/ev_glue.c',
  /setup_vga_globals[\s\S]*mvdm_cvidc_bind_vectors\(\)/);
const main = requireText(root, 'src/mvdm/softpc.new/base/support/main.c',
  /mvdm_cvidc_bind_video_vector\(\)[\s\S]*config\(/);
requireText(root, 'src/mvdm/softpc.new/base/cvidc/evidgen.h',
  /#ifndef CPU_40_STYLE\s+#define\s+getVideorplane/);
requireText(root, 'src/mvdm/softpc.new/base/inc/egacpu.h',
  /extern IS32 getVideodirty_low IPT0\(\)/);
if (/video_get_|video_set_|MVDM_CVIDC_VIDEO_BIND|boundVideo/.test(binder + generator)) {
  throw new Error('Autonomous accessor rebinding or typed wrapper generation returned');
}

if (publicSlots.length !== 154 || privateSlots.length !== 55) {
  throw new Error(`unexpected original metadata count: public=${publicSlots.length}, private=${privateSlots.length}`);
}
if (!publicSlots.includes('CalcQuickEventInstTime') ||
    !generator.includes('ClearHwInt: \'0\'')) {
  throw new Error('selected C-VID provider/null-slot disposition drifted');
}
if (!binder.includes('mvdm_cvidc_bind_video_vector();') ||
    !cvidAccess.includes('result = (*(Cpu.CalcQuickEventInstTime))(val);') ||
    !ccpuHeader.includes('c_effective_addr(seg, offset)') ||
    !xt.includes('#ifdef CPU_30_STYLE') || !evGlue.includes('mvdm_cvidc_bind_vectors();') ||
    !main.includes('mvdm_cvidc_bind_video_vector();')) {
  throw new Error('CCPU/C-VID profile-routing contract drifted');
}

console.log(JSON.stringify({
  public_slots: publicSlots.length,
  private_slots: privateSlots.length,
  quick_event_slot: 'c_cpu_calc_q_ev_inst_for_time',
  ccpu_effective_address: 'direct-c_effective_addr',
  cpu30_only_indirection: true,
  early_video_bind: true,
  setup_bind: true,
  unsupported_clear_hw_int: 'null'
}));
