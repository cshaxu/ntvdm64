# S5 BYOB Profile Reproducibility Baseline

状态：2026-08-09。该记录只覆盖现代 runner 对用户自带文件的只读准入；它不加载 guest bytes，
不调用 SoftPC、BIOS、BOP、DEM 或 DOS。

## 固定的准入范围

`src/runner/byob_profile.c` 只接受下列固定 target identity：

```text
schema              ntdos64-byob-profile-v1
profile             nt4-en-us-command-smoke-v1
architecture        x86
locale              en-US
required roles      ntio, ntdos, command
```

每个 component 必须使用 canonical file name、非零 byte count 和小写 64 位 SHA-256；profile
还必须声明非空 compatibility group。可选 role 只能与显式 feature 一一对应（例如 `himem -> xms`、
`dosx -> dpmi`、`redir -> redirector`）。manifest、根目录和 guest files 都是调用者提供的本地路径，
验证只读取 metadata 和组件内容以重算 identity，不保存文件内容，也不写入 guest memory。

## 可重复的负向边界

`tests/runner/byob_profile_test.c` 覆盖并要求拒绝：

- hash/size identity 不匹配；
- target、locale、role 或 compatibility group 不匹配；
- 重复或缺失的 required role；
- 未声明 feature 的可选 role；
- 非 canonical 或不安全的文件名；
- profile/root/component 不能安全打开；
- 格式无效，或验证失败后 selection 未清零。

2026-08-09 在当前 MinGW runner graph 的复验：

```text
cmake -S . -B artifacts/build/current/default-mingw-verify
cmake --build artifacts/build/current/default-mingw-verify \
  --target byob-profile-test --parallel 8
artifacts/build/current/default-mingw-verify/byob-profile-test.exe

byob-profile-test: owned BYOB profile acceptance and rejection families verified
```

## 与历史 host 链的关系

这个 baseline 只解决“哪些本地文件可以被声明为本次 session 的 BYOB 组件”。它不授权：

- 将 `NTIO.SYS` 或 `NTDOS.SYS` 放入 SAS；
- 调用 `demLoadDos` 或直接调用任何 DEM/BOP service；
- 把 profile 当作 DOS namespace、媒体策略、CONFIG projection 或 host-drive policy；
- 以成功的 metadata admission 宣称 NTIO、NTDOS 或 COMMAND 已运行。

当真实历史 caller 到达 `demLoadDos` 时，已有的 default-disabled caller-first fixture 仍是资源读取
ownership 的唯一依据；见 `S5-ORIGINAL-DEMLOADDOS-CLOSURE.md`。profile 验证仅为该未来
contained-resource seam 提供不可变的身份前置条件。
