# S5 BOP Static Width Result

状态：2026-08-09。此为 default-disabled link-only 测量，不授权 direct `MS_bop_0` 成为 runtime
dispatch path。

## 对照

`ntdos64-opennt-historical-transport-v2-msbop-closure` 使用原始 CCPU SAS、原始 `MS_bop_0`、
原始 `DemDispatch` 和完整 DEM object group，但不链接 `BIOS[]` table。它只回答一个问题：完整
BIOS table 静态函数指针闭包在 no-SM0 链接中增加了多少宽度。

| diagnostic | 不同 undefined symbols | duplicate symbols |
| --- | ---: | ---: |
| `MS_bop_0 -> DemDispatch(14h)`，无 `BIOS[]` | 114 | 0 |
| `BIOS[50h] -> MS_bop_0 -> DemDispatch(14h)`，完整 `BIOS[]` | 282 | 0 |

故完整 BIOS table/provider 的静态增量为 **168** 个不同 unresolved symbols。这个数是 link
closure 差额，不代表 date trace 会动态调用其中任一项。

原始日志：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-v2-msbop-closure-no-sm0-20260809.log
  s5-v2-owner-closure-no-sm0-20260809.log
```

## 114 项的构成

按引用 object group 聚合：

| group | 不同 unresolved | 解释 |
| --- | ---: | --- |
| DEM | 66 | 原始 `apfnSVC[]` 的完整 72 项服务表及其 host/file/process 依赖。 |
| `nt_bop.c` | 14 | 同一 translation unit 内其他 BOP entry 的静态依赖。 |
| host idle lifecycle | 9 | `MS_bop_0` 中 `IDLE_disk()` 的静态引用及相关 lifecycle。service `14h` 按原始条件不动态执行该分支。 |
| Sim32 | 5 | 原始 SAS/VDM pointer route 的静态依赖。 |
| CCPU/XMS | 各 1 | CCPU address/SAS 与 XMS 静态边缘。 |

## 已否定的工具链假设

曾以 `/Gy` 为 `nt_bop.c` 生成 function sections，并仅在此 diagnostic 使用 `/OPT:REF`。结果仍为
同一 `114` 项 unresolved，说明当前 object/link 模型不能据此可靠剥离 `nt_bop.c` 的其他 BOP
entry。该开关已撤销，不改变保留的历史 source 编译布局。

对应失败测量日志：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-v2-msbop-closure-no-sm0-gy-20260809.log
```

## 结论

目前可严谨地说：`BIOS[]` 是 168 项静态宽度的来源，而 `MS_bop_0` 与 `DemDispatch` 本身仍携带
114 项完整 translation-unit/service-table 宽度。下一步不能以局部重写、fake BOP 或小型 SVC table
来“消掉”这些项；应继续以真实 NTIO prefix trace 判定哪个历史 owner 首次动态抵达。
