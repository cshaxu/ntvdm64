# M0 T301 S1 — C Type-Keyword Parser Correction

The source-call parser previously treated C type constructs such as `int(...)`
as direct function calls. T300 now excludes C declaration/type keywords before
forming a boundary call. The corrected first-degree boundary has 34,156 rows,
not 34,429; 273 prior rows were removed. T301's regenerated include, work,
declaration and prior-definition crosswalk ledgers are the sole current input.
Earlier P1/P2/P3 counts remain historical evidence of the procedure, not the
current audit cardinality.
