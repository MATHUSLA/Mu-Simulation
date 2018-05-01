## MATHUSLA MU Detector Encoding

The RPCs and Scintillators use different encoding schemes to represent subdetector components in _ROOT_ output files. Both encodings constitute total orders over a subset of the integers and can be used to split RPCs and Scintillators into layer-by-layer equivalence classes. This can be used for fast layer discrimination.

---

The RPCs are ordered by the following five digit code:
```
Example: 05086 ->   05   08    6             | RPC   (2 digit): 01-12
                    --   --    -             | Pad   (2 digit): 01-10
                   [RPC][Pad][Strip]         | Strip (1 digit):  1-8    
```

---

The Scintillators are ordered lexicographically by layer location. In the table below this location is refered to by the leftmost three symbols in the Scintillator Column.

| Scintillator | ID |
|:------------:|:--:|
| A11_C7       |  0 |
| A12_C6       |  1 |
| A13_C5       |  2 |
| A14_C4       |  3 |
| A15_C3       |  4 |
| A21_C3       |  5 |
| A22_C4       |  6 |
| A23_C5       |  7 |
| A24_C6       |  8 |
| A25_C7       |  9 |
| A31_B11      | 10 |
| A32_B10      | 11 |
| A33_B9       | 12 |
| A34_B8       | 13 |
| A35_B7       | 14 |
| A41_B8       | 15 |
| A42_B9       | 16 |
| A43_B10      | 17 |
| A44_B11      | 18 |
| A45_B12      | 19 |
| A51_C11      | 20 |
| A52_C10      | 21 |
| A53_C9       | 22 |
| A54_C8       | 23 |
| A61_C8       | 24 |
| A62_C9       | 25 |
| A63_C10      | 26 |
| A64_C11      | 27 |
| B11_C7       | 28 |
| B12_B7       | 29 |
| B13_C5       | 30 |
| B14_C4       | 31 |
| B15_C3       | 32 |
| B21_C3       | 33 |
| B22_C4       | 34 |
| B23_C5       | 35 |
| B24_C6       | 36 |
| B25_C7       | 37 |
| B31_B9       | 38 |
| B32_C9       | 39 |
| B33_B9       | 40 |
| B34_B8       | 41 |
| B35_B7       | 42 |
| B41_A10      | 43 |
| B42_A11      | 44 |
| B43_A11      | 45 |
| B44_B9       | 46 |
| B45_B12_1    | 47 |
| B46_B11      | 48 |
| B51_C9       | 49 |
| B52_C8       | 50 |
| B53_C7       | 51 |
| B54_A12      | 52 |
| B55_A10      | 53 |
| B61_B11      | 54 |
| B62_B11      | 55 |
| B63_C9       | 56 |
| B64_B11      | 57 |
| B65_B11      | 58 |
