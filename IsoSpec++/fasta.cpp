/*
 *   Copyright (C) 2015-2020 Mateusz Łącki and Michał Startek.
 *
 *   This file is part of IsoSpec.
 *
 *   IsoSpec is free software: you can redistribute it and/or modify
 *   it under the terms of the Simplified ("2-clause") BSD licence.
 *
 *   IsoSpec is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *   You should have received a copy of the Simplified BSD Licence
 *   along with IsoSpec.  If not, see <https://opensource.org/licenses/BSD-2-Clause>.
 */


#include "element_tables.h"
#include "fasta.h"

namespace IsoSpec{

// We will work with C H N O S Se tuples */
const int aa_isotope_numbers[6] = {2, 2, 2, 3, 4, 6};

const double aa_elem_masses[19] = {
    elem_table_mass[9], elem_table_mass[10],   // Carbon
    elem_table_mass[0], elem_table_mass[1],    // Hydrogen
    elem_table_mass[11], elem_table_mass[12],  // Nitrogen
    elem_table_mass[13], elem_table_mass[14], elem_table_mass[15], // Oxygen
    elem_table_mass[29], elem_table_mass[30], elem_table_mass[31], elem_table_mass[32], // Sulfur
    elem_table_mass[85], elem_table_mass[86], elem_table_mass[87], elem_table_mass[88], elem_table_mass[89], elem_table_mass[90] // Selenium
};


const double aa_elem_nominal_masses[19] = {
    elem_table_massNo[9], elem_table_massNo[10],   // Carbon
    elem_table_massNo[0], elem_table_massNo[1],    // Hydrogen
    elem_table_massNo[11], elem_table_massNo[12],  // Nitrogen
    elem_table_massNo[13], elem_table_massNo[14], elem_table_massNo[15], // Oxygen
    elem_table_massNo[29], elem_table_massNo[30], elem_table_massNo[31], elem_table_massNo[32], // Sulfur
    elem_table_massNo[85], elem_table_massNo[86], elem_table_massNo[87], elem_table_massNo[88], elem_table_massNo[89], elem_table_massNo[90] // Selenium
};


const double aa_elem_probabilities[19] = {
    elem_table_probability[9], elem_table_probability[10],   // Carbon
    elem_table_probability[0], elem_table_probability[1],    // Hydrogen
    elem_table_probability[11], elem_table_probability[12],  // Nitrogen
    elem_table_probability[13], elem_table_probability[14], elem_table_probability[15], // Oxygen
    elem_table_probability[29], elem_table_probability[30], elem_table_probability[31], elem_table_probability[32], // Sulfur
    elem_table_probability[85], elem_table_probability[86], elem_table_probability[87], elem_table_probability[88], elem_table_probability[89], elem_table_probability[90] // Selenium
};


const int aa_symbol_to_elem_counts[256*6] = {
/* Code:   0  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   1  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   2  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   3  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   4  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   5  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   6  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   7  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   8  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:   9  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  10  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  11  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  12  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  13  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  14  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  15  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  16  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  17  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  18  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  19  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  20  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  21  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  22  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  23  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  24  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  25  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  26  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  27  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  28  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  29  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  30  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  31  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code:  32  ASCII char:   */       0, 0, 0, 0, 0, 0,
/* Code:  33  ASCII char: ! */       0, 0, 0, 0, 0, 0,
/* Code:  34  ASCII char: " */       0, 0, 0, 0, 0, 0,
/* Code:  35  ASCII char: # */       0, 0, 0, 0, 0, 0,
/* Code:  36  ASCII char: $ */       0, 0, 0, 0, 0, 0,
/* Code:  37  ASCII char: % */       0, 0, 0, 0, 0, 0,
/* Code:  38  ASCII char: & */       0, 0, 0, 0, 0, 0,
/* Code:  39  ASCII char: ' */       0, 0, 0, 0, 0, 0,
/* Code:  40  ASCII char: ( */       0, 0, 0, 0, 0, 0,
/* Code:  41  ASCII char: ) */       0, 0, 0, 0, 0, 0,
/* Code:  42  ASCII char: * */       0, 0, 0, 0, 0, 0,
/* Code:  43  ASCII char: + */       0, 0, 0, 0, 0, 0,
/* Code:  44  ASCII char: , */       0, 0, 0, 0, 0, 0,
/* Code:  45  ASCII char: - */       0, 0, 0, 0, 0, 0,
/* Code:  46  ASCII char: . */       0, 0, 0, 0, 0, 0,
/* Code:  47  ASCII char: / */       0, 0, 0, 0, 0, 0,
/* Code:  48  ASCII char: 0 */       0, 0, 0, 0, 0, 0,
/* Code:  49  ASCII char: 1 */       0, 0, 0, 0, 0, 0,
/* Code:  50  ASCII char: 2 */       0, 0, 0, 0, 0, 0,
/* Code:  51  ASCII char: 3 */       0, 0, 0, 0, 0, 0,
/* Code:  52  ASCII char: 4 */       0, 0, 0, 0, 0, 0,
/* Code:  53  ASCII char: 5 */       0, 0, 0, 0, 0, 0,
/* Code:  54  ASCII char: 6 */       0, 0, 0, 0, 0, 0,
/* Code:  55  ASCII char: 7 */       0, 0, 0, 0, 0, 0,
/* Code:  56  ASCII char: 8 */       0, 0, 0, 0, 0, 0,
/* Code:  57  ASCII char: 9 */       0, 0, 0, 0, 0, 0,
/* Code:  58  ASCII char: : */       0, 0, 0, 0, 0, 0,
/* Code:  59  ASCII char: ; */       0, 0, 0, 0, 0, 0,
/* Code:  60  ASCII char: < */       0, 0, 0, 0, 0, 0,
/* Code:  61  ASCII char: = */       0, 0, 0, 0, 0, 0,
/* Code:  62  ASCII char: > */       0, 0, 0, 0, 0, 0,
/* Code:  63  ASCII char: ? */       0, 0, 0, 0, 0, 0,
/* Code:  64  ASCII char: @ */       0, 0, 0, 0, 0, 0,
/* Code:  65  ASCII char: A */       3, 5, 1, 1, 0, 0,
/* Code:  66  ASCII char: B */       0, 0, 0, 0, 0, 0,
/* Code:  67  ASCII char: C */       3, 5, 1, 1, 1, 0,
/* Code:  68  ASCII char: D */       4, 5, 1, 3, 0, 0,
/* Code:  69  ASCII char: E */       5, 7, 1, 3, 0, 0,
/* Code:  70  ASCII char: F */       9, 9, 1, 1, 0, 0,
/* Code:  71  ASCII char: G */       2, 3, 1, 1, 0, 0,
/* Code:  72  ASCII char: H */       6, 7, 3, 1, 0, 0,
/* Code:  73  ASCII char: I */       6, 11, 1, 1, 0, 0,
/* Code:  74  ASCII char: J */       0, 0, 0, 0, 0, 0,
/* Code:  75  ASCII char: K */       6, 12, 2, 1, 0, 0,
/* Code:  76  ASCII char: L */       6, 11, 1, 1, 0, 0,
/* Code:  77  ASCII char: M */       5, 9, 1, 1, 1, 0,
/* Code:  78  ASCII char: N */       4, 6, 2, 2, 0, 0,
/* Code:  79  ASCII char: O */       12, 21, 3, 3, 0, 0,
/* Code:  80  ASCII char: P */       5, 7, 1, 1, 0, 0,
/* Code:  81  ASCII char: Q */       5, 8, 2, 2, 0, 0,
/* Code:  82  ASCII char: R */       6, 12, 4, 1, 0, 0,
/* Code:  83  ASCII char: S */       3, 5, 1, 2, 0, 0,
/* Code:  84  ASCII char: T */       4, 7, 1, 2, 0, 0,
/* Code:  85  ASCII char: U */       3, 5, 1, 1, 0, 1,
/* Code:  86  ASCII char: V */       5, 9, 1, 1, 0, 0,
/* Code:  87  ASCII char: W */       11, 10, 2, 1, 0, 0,
/* Code:  88  ASCII char: X */       0, 0, 0, 0, 0, 0,
/* Code:  89  ASCII char: Y */       9, 9, 1, 2, 0, 0,
/* Code:  90  ASCII char: Z */       0, 0, 0, 0, 0, 0,
/* Code:  91  ASCII char: [ */       0, 0, 0, 0, 0, 0,
/* Code:  92  ASCII char: \ */       0, 0, 0, 0, 0, 0,
/* Code:  93  ASCII char: ] */       0, 0, 0, 0, 0, 0,
/* Code:  94  ASCII char: ^ */       0, 0, 0, 0, 0, 0,
/* Code:  95  ASCII char: _ */       0, 0, 0, 0, 0, 0,
/* Code:  96  ASCII char: ` */       0, 0, 0, 0, 0, 0,
/* Code:  97  ASCII char: a */       3, 5, 1, 1, 0, 0,
/* Code:  98  ASCII char: b */       0, 0, 0, 0, 0, 0,
/* Code:  99  ASCII char: c */       3, 5, 1, 1, 1, 0,
/* Code: 100  ASCII char: d */       4, 5, 1, 3, 0, 0,
/* Code: 101  ASCII char: e */       5, 7, 1, 3, 0, 0,
/* Code: 102  ASCII char: f */       9, 9, 1, 1, 0, 0,
/* Code: 103  ASCII char: g */       2, 3, 1, 1, 0, 0,
/* Code: 104  ASCII char: h */       6, 7, 3, 1, 0, 0,
/* Code: 105  ASCII char: i */       6, 11, 1, 1, 0, 0,
/* Code: 106  ASCII char: j */       0, 0, 0, 0, 0, 0,
/* Code: 107  ASCII char: k */       6, 12, 2, 1, 0, 0,
/* Code: 108  ASCII char: l */       6, 11, 1, 1, 0, 0,
/* Code: 109  ASCII char: m */       5, 9, 1, 1, 1, 0,
/* Code: 110  ASCII char: n */       4, 6, 2, 2, 0, 0,
/* Code: 111  ASCII char: o */       12, 21, 3, 3, 0, 0,
/* Code: 112  ASCII char: p */       5, 7, 1, 1, 0, 0,
/* Code: 113  ASCII char: q */       5, 8, 2, 2, 0, 0,
/* Code: 114  ASCII char: r */       6, 12, 4, 1, 0, 0,
/* Code: 115  ASCII char: s */       3, 5, 1, 2, 0, 0,
/* Code: 116  ASCII char: t */       4, 7, 1, 2, 0, 0,
/* Code: 117  ASCII char: u */       3, 5, 1, 1, 0, 1,
/* Code: 118  ASCII char: v */       5, 9, 1, 1, 0, 0,
/* Code: 119  ASCII char: w */       11, 10, 2, 1, 0, 0,
/* Code: 120  ASCII char: x */       0, 0, 0, 0, 0, 0,
/* Code: 121  ASCII char: y */       9, 9, 1, 2, 0, 0,
/* Code: 122  ASCII char: z */       0, 0, 0, 0, 0, 0,
/* Code: 123  ASCII char: { */       0, 0, 0, 0, 0, 0,
/* Code: 124  ASCII char: | */       0, 0, 0, 0, 0, 0,
/* Code: 125  ASCII char: } */       0, 0, 0, 0, 0, 0,
/* Code: 126  ASCII char: ~ */       0, 0, 0, 0, 0, 0,
/* Code: 127  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 128  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 129  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 130  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 131  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 132  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 133  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 134  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 135  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 136  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 137  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 138  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 139  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 140  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 141  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 142  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 143  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 144  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 145  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 146  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 147  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 148  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 149  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 150  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 151  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 152  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 153  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 154  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 155  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 156  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 157  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 158  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 159  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 160  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 161  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 162  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 163  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 164  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 165  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 166  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 167  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 168  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 169  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 170  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 171  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 172  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 173  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 174  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 175  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 176  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 177  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 178  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 179  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 180  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 181  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 182  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 183  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 184  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 185  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 186  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 187  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 188  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 189  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 190  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 191  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 192  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 193  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 194  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 195  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 196  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 197  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 198  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 199  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 200  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 201  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 202  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 203  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 204  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 205  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 206  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 207  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 208  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 209  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 210  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 211  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 212  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 213  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 214  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 215  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 216  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 217  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 218  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 219  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 220  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 221  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 222  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 223  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 224  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 225  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 226  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 227  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 228  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 229  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 230  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 231  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 232  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 233  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 234  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 235  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 236  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 237  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 238  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 239  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 240  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 241  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 242  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 243  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 244  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 245  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 246  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 247  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 248  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 249  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 250  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 251  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 252  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 253  unprintable   */       0, 0, 0, 0, 0, 0,
/* Code: 254  unprintable   */       0, 0, 0, 0, 0, 0
};

} // namespace IsoSpec
