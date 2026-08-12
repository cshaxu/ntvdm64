# NT4-NT5 BOP Service-Byte Ordering

状态：2026-08-09，只读跨版本采证。目标语义固定为 NT4 EN-US；NT4.5 与 XP 仅为旁证，本文不修改任何
历史源码、overlay、CMake 或 runtime。

## 结论

NT4 与 NT4.5 的三类带尾随 service byte 的 BOP handler 都采取同一顺序：

```text
读取当前 CS:IP 的 service byte
  -> 调用原始 dispatcher
  -> IP = IP + 1
```

XP 对 `MS_bop_0`、`MS_bop_2`、`MS_bop_4` 改为先消费 service byte，随后才调用 dispatcher：

```text
读取当前 CS:IP 的 service byte
  -> IP = IP + 1
  -> 调用原始 dispatcher
```

这不是可忽略的写法差异。若 dispatcher、诊断或嵌套调用观察 `IP`，两条路径看到的状态不同。因此任何
NT4 reconstruction、trace fixture 或未来 ABI facade 都必须保留 **NT4 的 post-dispatch IP consumption**。
不得以 XP 的 `VdmMapFlat` 写法、或“先推进 IP 更自然”的判断倒灌 NT4。

## 固定证据

| handler | NT4 / NT4.5 | XP | NT4 处理要求 |
| --- | --- | --- | --- |
| `MS_bop_0`，DEM (`50h`) | `DemDispatch(DemCmd)` 后 `setIP(getIP()+1)`；NT4 `nt_bop.c:149-154` | `VdmMapFlat` 取字节后先 `setIP`，再 `DemDispatch`；XP `nt_bop.c:156-159` | service `11h`、`0Fh` 等进入 `DemDispatch` 时，IP 仍指向 service byte；只在原始 dispatch 返回后消费该 byte。 |
| `MS_bop_2`，XMS | `XMSDispatch(...)` 后 `setIP`；NT4 `nt_bop.c:282-288` | 先 `setIP`，再 `XMSDispatch`；XP `nt_bop.c:298-303` | NT4 XMS caller 同样使用 post-dispatch 规则。 |
| `MS_bop_4`，CMD | `CmdDispatch(Command)` 后 `setIP`；NT4 `nt_bop.c:315-321` | 先 `setIP`，再 `CmdDispatch`；XP `nt_bop.c:331-337` | `SVC_GETCONFIGSYS` 的 `CmdDispatch` 必须仍看见 service byte 地址。 |

NT4.5 对应 `nt_bop.c` 与 NT4 字节相同，SHA-256 均为
`378A34807645277090BF4B38556EA210502F79A898855B649B39D6C4CCC47F66`。XP 文件 SHA-256 为
`FAD3681177F9236B0CBB0C3CD291005DF0659AFEED3B44066FB834C16B65FA37`。

## 对当前研究边界的影响

1. 这是 historical-host 的职责，不是 CPU backend、generic transition hook 或 wrapper 的职责。
   monitor 只识别并移交 BOP selector；`MS_bop_*` 自己读取并消费其尾随 service byte。
2. 任何跨位宽 state record 若用于 NT4 `MS_bop_0`，其 call-in snapshot 必须把 IP 固定在 service byte。
   call-out snapshot 才能反映原始 owner 的 `IP + 1` 与 dispatcher 写入的寄存器/FLAGS。
3. 该证据不授权恢复完整 `BIOS[]` 表，也不授权把 XP `VdmMapFlat`、V86 context 或 host API 迁入 NT4。
4. `C4 C4 50 11` 的外层指令消费与本表的 service-byte 消费是两个层次：前者属于 CPU/monitor BOP
   decode，后者属于 `MS_bop_0`。两者不得合并成一个自写 DOS SVC decoder。

## 关联的 NT4 初始化证据

`sysinit1.asm` 在进入 CONFIG projection 前已经发出硬件/BIOS 类请求：

```text
sysinit1.asm:825   BOP 12h       ; 常规内存 KB
sysinit1.asm:1012  AH = 88h
sysinit1.asm:1017  BOP 15h       ; 扩展内存 KB
sysinit1.asm:1162-1163 DS:DX=config; CMDSVC SVC_GETCONFIGSYS
```

因此 `cmdGetConfigSys` 是首个已定位的 ambient-host 文件/配置 seam，却不是整个初始化的第一个外部
契约。其之前已经存在须由历史 BIOS/机器路径回答的内存服务。这个事实支持把下阶段工作分开：

- 以 trace/source audit 记录 `12h`、`15h` 所需的机器输入，不伪造 PC/AT 返回值；
- 独立设计受控、session-local 的 CONFIG projection，而不把它塞进 CPU 或 BIOS shim；
- 在完整 historical ownership 可组合前，两个方向都保持 bounded、fail-closed。
