#line 2 "../src/gigatron.md"

/*
#   Copyright (c) 2021, LB3361
#
#    Redistribution and use in source and binary forms, with or
#    without modification, are permitted provided that the following
#    conditions are met:
#
#    1.  Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials
#       provided with the distribution.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
#    CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
#    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
#    BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
#    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
#    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#    POSSIBILITY OF SUCH DAMAGE.
*/


/*---- BEGIN HEADER --*/

#include "c.h"
#include <ctype.h>
#include <math.h>
#include <float.h>

#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)

static void address(Symbol, Symbol, long);
static void blkfetch(int, int, int, int) {}
static void blkloop(int, int, int, int, int, int[]) {}
static void blkstore(int, int, int, int) {}
static void defaddress(Symbol);
static void defconst(int, int, Value);
static void defstring(int, char *);
static void defsymbol(Symbol);
static void doarg(Node);
static void emit2(Node) {}
static void emit3(const char*, int, Node, int, Node*, short*);
static void emitfmt1(const char*, Node, int, Node*, short*);
static void export(Symbol);
static void clobber(Node);
static void preralloc(Node);
static Node pregen(Node);
static void function(Symbol, Symbol [], Symbol [], int);
static void global(Symbol);
static void import(Symbol);
static void local(Symbol);
static void progbeg(int, char **);
static void progend(void);
static void segment(int);
static void space(int);
static void target(Node);

/* string lists */
typedef struct slist *SList;
struct slist { SList prev; SList next; char s[1]; };
static struct slist lhead = { &lhead, &lhead, 0 };
static void lprint(const char *fmt, ...);

static int in_function = 0;
static struct slist xhead = { &xhead, &xhead, 0 };
static void xprint_init(void);
static void xprint(const char *fmt, ...);
static void xprint_finish(void);


/* Cost functions */
static int  if_zpconst(Node);
static int  if_zlconst(Node);
static int  if_zhconst(Node);
static int  if_zpglobal(Node);
static int  if_incr(Node,int,int);      /* cost hint */
static int  if_zoffset(Node,int,int);   /* cost hint */
static int  if_asgnreuse(Node,int,int); /* cost hint */
static int  if_rmw(Node,int);
static int  if_rmw_a(Node,int,int);
static int  if_rmw_incr(Node,int,int);
static int  if_not_asgn_tmp(Node,int);
static int  if_cv_from(Node,int,int);
static int  if_arg_reg_only(Node);
static int  if_arg_stk(Node,int);
static int  if_vac_contains(Node, Symbol);

#define mincpuf(c,f,cost) ((cpu<(c)&&!cpuflags.f)?LBURG_MAX:(cost))
#define mincpu(c,cost)    ((cpu<(c))?LBURG_MAX:(cost))
#define mincpu5(cost)     mincpu(5,cost)
#define mincpu6(cost)     mincpu(6,cost)
#define mincpu7(cost)     mincpu(7,cost)
#define ifcpu7(c1,c2)     ((cpu<7)?(c2):(c1))
#define if_spill()        ((spilling)?0:LBURG_MAX)

/* Registers */
static Symbol ireg[32], lreg[32], freg[32];
static Symbol iregw, lregw, fregw;

#define REGMASK_SAVED           0x000000ff
#define REGMASK_ARGS            0x0000ff00
#define REGMASK_MOREVARS        0x000fffff
#define REGMASK_TEMPS           0x00ffff00

/* Misc */
static int codenum = 0;         /* uni */
static unsigned rmask = 0;      /* masked registers */
static int cseg = 0;            /* segment */
static int cpu = 5;             /* cpu version */

static struct cpuflags_s {
  unsigned addhi : 1;           /* cpu as instruction addhi/addwi */
} cpuflags;

/* Register equivalences for the emitter state machine */
static int vac_clobbered;
static int xac_clobbered;
static Symbol vac_constval;
static Symbol vac_memval, lac_memval, fac_memval;
static unsigned vac_equiv, lac_equiv, fac_equiv;


/*---- END HEADER --*/
#line 140 "gigatron.c"
/*
generated at Fri Nov 21 12:27:56 2025
by $Id$
*/
static void _kids(NODEPTR_TYPE, int, NODEPTR_TYPE[]);
static void _label(NODEPTR_TYPE);
static int _rule(void*, int);

#define _stmt_NT 1
#define _reg_NT 2
#define _con0_NT 3
#define _con1_NT 4
#define _conB_NT 5
#define _zddr_NT 6
#define _conBs_NT 7
#define _conBn_NT 8
#define _conBm_NT 9
#define _conBa_NT 10
#define _con_NT 11
#define _addr_NT 12
#define _con8_NT 13
#define _conFF_NT 14
#define _conFF00_NT 15
#define _conXX00_NT 16
#define _lddr_NT 17
#define _asgn_NT 18
#define _ac_NT 19
#define _regx_NT 20
#define _ac0_NT 21
#define _eac0_NT 22
#define _eac_NT 23
#define _iarg_NT 24
#define _spill_NT 25
#define _vdst_NT 26
#define _asrc_NT 27
#define _lsrc_NT 28
#define _lac_NT 29
#define _larg_NT 30
#define _fac_NT 31
#define _farg_NT 32
#define _rmw_NT 33

static char *_ntname[] = {
	0,
	"stmt",
	"reg",
	"con0",
	"con1",
	"conB",
	"zddr",
	"conBs",
	"conBn",
	"conBm",
	"conBa",
	"con",
	"addr",
	"con8",
	"conFF",
	"conFF00",
	"conXX00",
	"lddr",
	"asgn",
	"ac",
	"regx",
	"ac0",
	"eac0",
	"eac",
	"iarg",
	"spill",
	"vdst",
	"asrc",
	"lsrc",
	"lac",
	"larg",
	"fac",
	"farg",
	"rmw",
	0
};

struct _state {
	short cost[34];
	struct {
		unsigned int _stmt:7;
		unsigned int _reg:6;
		unsigned int _con0:3;
		unsigned int _con1:3;
		unsigned int _conB:3;
		unsigned int _zddr:2;
		unsigned int _conBs:2;
		unsigned int _conBn:1;
		unsigned int _conBm:2;
		unsigned int _conBa:2;
		unsigned int _con:3;
		unsigned int _addr:2;
		unsigned int _con8:2;
		unsigned int _conFF:2;
		unsigned int _conFF00:1;
		unsigned int _conXX00:2;
		unsigned int _lddr:2;
		unsigned int _asgn:7;
		unsigned int _ac:7;
		unsigned int _regx:5;
		unsigned int _ac0:5;
		unsigned int _eac0:5;
		unsigned int _eac:7;
		unsigned int _iarg:3;
		unsigned int _spill:1;
		unsigned int _vdst:2;
		unsigned int _asrc:2;
		unsigned int _lsrc:1;
		unsigned int _lac:6;
		unsigned int _larg:2;
		unsigned int _fac:5;
		unsigned int _farg:2;
		unsigned int _rmw:2;
	} rule;
};

static short _nts_0[] = { 0 };
static short _nts_1[] = { _reg_NT, 0 };
static short _nts_2[] = { _zddr_NT, 0 };
static short _nts_3[] = { _addr_NT, 0 };
static short _nts_4[] = { _con_NT, 0 };
static short _nts_5[] = { _conB_NT, 0 };
static short _nts_6[] = { _asgn_NT, 0 };
static short _nts_7[] = { _ac_NT, 0 };
static short _nts_8[] = { _regx_NT, 0 };
static short _nts_9[] = { _eac0_NT, 0 };
static short _nts_10[] = { _ac0_NT, 0 };
static short _nts_11[] = { _eac_NT, 0 };
static short _nts_12[] = { _lddr_NT, 0 };
static short _nts_13[] = { _spill_NT, 0 };
static short _nts_14[] = { _ac_NT, _iarg_NT, 0 };
static short _nts_15[] = { _iarg_NT, _ac_NT, 0 };
static short _nts_16[] = { _iarg_NT, _con1_NT, _ac_NT, 0 };
static short _nts_17[] = { _ac_NT, _conB_NT, 0 };
static short _nts_18[] = { _ac_NT, _conBn_NT, 0 };
static short _nts_19[] = { _ac_NT, _con1_NT, 0 };
static short _nts_20[] = { _conB_NT, _ac_NT, 0 };
static short _nts_21[] = { _conBn_NT, _ac_NT, 0 };
static short _nts_22[] = { _conB_NT, _regx_NT, 0 };
static short _nts_23[] = { _conBn_NT, _regx_NT, 0 };
static short _nts_24[] = { _con_NT, _ac_NT, 0 };
static short _nts_25[] = { _ac_NT, _con_NT, 0 };
static short _nts_26[] = { _ac0_NT, _conB_NT, 0 };
static short _nts_27[] = { _eac_NT, _conB_NT, 0 };
static short _nts_28[] = { _eac_NT, _conBn_NT, 0 };
static short _nts_29[] = { _eac_NT, _con1_NT, 0 };
static short _nts_30[] = { _conB_NT, _eac_NT, 0 };
static short _nts_31[] = { _eac0_NT, _conB_NT, 0 };
static short _nts_32[] = { _eac_NT, _iarg_NT, 0 };
static short _nts_33[] = { _iarg_NT, _eac_NT, 0 };
static short _nts_34[] = { _iarg_NT, _con1_NT, _eac_NT, 0 };
static short _nts_35[] = { _zddr_NT, _ac_NT, 0 };
static short _nts_36[] = { _ac_NT, _con0_NT, 0 };
static short _nts_37[] = { _asrc_NT, 0 };
static short _nts_38[] = { _addr_NT, _asrc_NT, 0 };
static short _nts_39[] = { _ac_NT, _asrc_NT, 0 };
static short _nts_40[] = { _lddr_NT, _lsrc_NT, 0 };
static short _nts_41[] = { _lac_NT, 0 };
static short _nts_42[] = { _lac_NT, _larg_NT, 0 };
static short _nts_43[] = { _larg_NT, _lac_NT, 0 };
static short _nts_44[] = { _lac_NT, _reg_NT, 0 };
static short _nts_45[] = { _lac_NT, _conB_NT, 0 };
static short _nts_46[] = { _vdst_NT, _lac_NT, 0 };
static short _nts_47[] = { _vdst_NT, _reg_NT, 0 };
static short _nts_48[] = { _fac_NT, 0 };
static short _nts_49[] = { _fac_NT, _farg_NT, 0 };
static short _nts_50[] = { _farg_NT, _fac_NT, 0 };
static short _nts_51[] = { _vdst_NT, _fac_NT, 0 };
static short _nts_52[] = { _spill_NT, _reg_NT, 0 };
static short _nts_53[] = { _eac_NT, _con_NT, 0 };
static short _nts_54[] = { _conBm_NT, _eac_NT, 0 };
static short _nts_55[] = { _ac_NT, _conBs_NT, 0 };
static short _nts_56[] = { _rmw_NT, _conBs_NT, 0 };
static short _nts_57[] = { _rmw_NT, _conB_NT, 0 };
static short _nts_58[] = { _rmw_NT, _con_NT, 0 };
static short _nts_59[] = { _conBs_NT, 0 };
static short _nts_60[] = { _reg_NT, _con_NT, 0 };
static short _nts_61[] = { _reg_NT, _con_NT, _ac_NT, 0 };
static short _nts_62[] = { _lddr_NT, _ac_NT, 0 };
static short _nts_63[] = { _rmw_NT, _rmw_NT, _con1_NT, 0 };
static short _nts_64[] = { _rmw_NT, _rmw_NT, 0 };
static short _nts_65[] = { _rmw_NT, _rmw_NT, _ac_NT, 0 };
static short _nts_66[] = { _rmw_NT, _ac_NT, _rmw_NT, 0 };
static short _nts_67[] = { _rmw_NT, _rmw_NT, _conBa_NT, 0 };
static short _nts_68[] = { _reg_NT, _con8_NT, 0 };
static short _nts_69[] = { _ac_NT, _con8_NT, 0 };
static short _nts_70[] = { _ac_NT, _conFF_NT, 0 };
static short _nts_71[] = { _ac0_NT, _conFF_NT, 0 };
static short _nts_72[] = { _ac_NT, _conFF00_NT, 0 };
static short _nts_73[] = { _iarg_NT, _con8_NT, _ac_NT, 0 };
static short _nts_74[] = { _iarg_NT, _conFF00_NT, _ac_NT, 0 };
static short _nts_75[] = { _eac_NT, _con8_NT, _ac_NT, 0 };
static short _nts_76[] = { _ac_NT, _con8_NT, _eac_NT, 0 };
static short _nts_77[] = { _eac_NT, _conFF00_NT, _ac0_NT, 0 };
static short _nts_78[] = { _ac_NT, _conFF00_NT, _eac0_NT, 0 };
static short _nts_79[] = { _ac_NT, _con8_NT, _conB_NT, 0 };
static short _nts_80[] = { _ac_NT, _conFF00_NT, _conB_NT, 0 };
static short _nts_81[] = { _reg_NT, _conFF00_NT, _conB_NT, 0 };
static short _nts_82[] = { _ac0_NT, _conXX00_NT, 0 };
static short _nts_83[] = { _rmw_NT, _rmw_NT, _conFF00_NT, _ac_NT, 0 };
static short _nts_84[] = { _rmw_NT, _rmw_NT, _ac_NT, _conFF00_NT, 0 };
static short _nts_85[] = { _rmw_NT, _rmw_NT, _regx_NT, _conFF00_NT, 0 };
static short _nts_86[] = { _rmw_NT, _rmw_NT, _ac_NT, _con8_NT, 0 };
static short _nts_87[] = { _rmw_NT, _rmw_NT, _conFF00_NT, 0 };
static short _nts_88[] = { _rmw_NT, _rmw_NT, _conFF00_NT, _conB_NT, 0 };
static short _nts_89[] = { _rmw_NT, _rmw_NT, _conXX00_NT, 0 };

static short *_nts[] = {
	0,	/* 0 */
	_nts_0,	/* 1 */
	_nts_0,	/* 2 */
	_nts_0,	/* 3 */
	_nts_0,	/* 4 */
	_nts_0,	/* 5 */
	_nts_0,	/* 6 */
	_nts_0,	/* 7 */
	_nts_0,	/* 8 */
	_nts_1,	/* 9 */
	_nts_1,	/* 10 */
	_nts_1,	/* 11 */
	_nts_1,	/* 12 */
	_nts_1,	/* 13 */
	_nts_1,	/* 14 */
	_nts_1,	/* 15 */
	_nts_1,	/* 16 */
	_nts_0,	/* 17 */
	_nts_0,	/* 18 */
	_nts_0,	/* 19 */
	_nts_0,	/* 20 */
	_nts_0,	/* 21 */
	_nts_0,	/* 22 */
	_nts_0,	/* 23 */
	_nts_0,	/* 24 */
	_nts_0,	/* 25 */
	_nts_0,	/* 26 */
	_nts_0,	/* 27 */
	_nts_0,	/* 28 */
	_nts_0,	/* 29 */
	_nts_2,	/* 30 */
	_nts_0,	/* 31 */
	_nts_0,	/* 32 */
	_nts_0,	/* 33 */
	_nts_0,	/* 34 */
	_nts_0,	/* 35 */
	_nts_0,	/* 36 */
	_nts_0,	/* 37 */
	_nts_0,	/* 38 */
	_nts_0,	/* 39 */
	_nts_0,	/* 40 */
	_nts_0,	/* 41 */
	_nts_0,	/* 42 */
	_nts_3,	/* 43 */
	_nts_0,	/* 44 */
	_nts_0,	/* 45 */
	_nts_0,	/* 46 */
	_nts_0,	/* 47 */
	_nts_0,	/* 48 */
	_nts_0,	/* 49 */
	_nts_0,	/* 50 */
	_nts_0,	/* 51 */
	_nts_0,	/* 52 */
	_nts_0,	/* 53 */
	_nts_4,	/* 54 */
	_nts_2,	/* 55 */
	_nts_0,	/* 56 */
	_nts_5,	/* 57 */
	_nts_1,	/* 58 */
	_nts_6,	/* 59 */
	_nts_7,	/* 60 */
	_nts_1,	/* 61 */
	_nts_8,	/* 62 */
	_nts_7,	/* 63 */
	_nts_9,	/* 64 */
	_nts_9,	/* 65 */
	_nts_10,	/* 66 */
	_nts_11,	/* 67 */
	_nts_1,	/* 68 */
	_nts_12,	/* 69 */
	_nts_5,	/* 70 */
	_nts_4,	/* 71 */
	_nts_11,	/* 72 */
	_nts_11,	/* 73 */
	_nts_11,	/* 74 */
	_nts_11,	/* 75 */
	_nts_11,	/* 76 */
	_nts_2,	/* 77 */
	_nts_2,	/* 78 */
	_nts_2,	/* 79 */
	_nts_2,	/* 80 */
	_nts_2,	/* 81 */
	_nts_7,	/* 82 */
	_nts_7,	/* 83 */
	_nts_7,	/* 84 */
	_nts_7,	/* 85 */
	_nts_7,	/* 86 */
	_nts_8,	/* 87 */
	_nts_2,	/* 88 */
	_nts_2,	/* 89 */
	_nts_2,	/* 90 */
	_nts_0,	/* 91 */
	_nts_13,	/* 92 */
	_nts_13,	/* 93 */
	_nts_13,	/* 94 */
	_nts_14,	/* 95 */
	_nts_14,	/* 96 */
	_nts_14,	/* 97 */
	_nts_15,	/* 98 */
	_nts_15,	/* 99 */
	_nts_15,	/* 100 */
	_nts_16,	/* 101 */
	_nts_16,	/* 102 */
	_nts_16,	/* 103 */
	_nts_16,	/* 104 */
	_nts_17,	/* 105 */
	_nts_17,	/* 106 */
	_nts_17,	/* 107 */
	_nts_18,	/* 108 */
	_nts_18,	/* 109 */
	_nts_18,	/* 110 */
	_nts_14,	/* 111 */
	_nts_14,	/* 112 */
	_nts_14,	/* 113 */
	_nts_17,	/* 114 */
	_nts_17,	/* 115 */
	_nts_17,	/* 116 */
	_nts_18,	/* 117 */
	_nts_18,	/* 118 */
	_nts_18,	/* 119 */
	_nts_7,	/* 120 */
	_nts_8,	/* 121 */
	_nts_19,	/* 122 */
	_nts_19,	/* 123 */
	_nts_17,	/* 124 */
	_nts_17,	/* 125 */
	_nts_17,	/* 126 */
	_nts_17,	/* 127 */
	_nts_14,	/* 128 */
	_nts_14,	/* 129 */
	_nts_14,	/* 130 */
	_nts_14,	/* 131 */
	_nts_20,	/* 132 */
	_nts_21,	/* 133 */
	_nts_22,	/* 134 */
	_nts_23,	/* 135 */
	_nts_24,	/* 136 */
	_nts_14,	/* 137 */
	_nts_15,	/* 138 */
	_nts_20,	/* 139 */
	_nts_24,	/* 140 */
	_nts_14,	/* 141 */
	_nts_15,	/* 142 */
	_nts_14,	/* 143 */
	_nts_14,	/* 144 */
	_nts_14,	/* 145 */
	_nts_14,	/* 146 */
	_nts_25,	/* 147 */
	_nts_25,	/* 148 */
	_nts_25,	/* 149 */
	_nts_25,	/* 150 */
	_nts_7,	/* 151 */
	_nts_7,	/* 152 */
	_nts_14,	/* 153 */
	_nts_14,	/* 154 */
	_nts_15,	/* 155 */
	_nts_15,	/* 156 */
	_nts_17,	/* 157 */
	_nts_17,	/* 158 */
	_nts_26,	/* 159 */
	_nts_26,	/* 160 */
	_nts_14,	/* 161 */
	_nts_14,	/* 162 */
	_nts_15,	/* 163 */
	_nts_15,	/* 164 */
	_nts_17,	/* 165 */
	_nts_17,	/* 166 */
	_nts_26,	/* 167 */
	_nts_26,	/* 168 */
	_nts_14,	/* 169 */
	_nts_14,	/* 170 */
	_nts_15,	/* 171 */
	_nts_15,	/* 172 */
	_nts_17,	/* 173 */
	_nts_17,	/* 174 */
	_nts_26,	/* 175 */
	_nts_26,	/* 176 */
	_nts_27,	/* 177 */
	_nts_27,	/* 178 */
	_nts_27,	/* 179 */
	_nts_28,	/* 180 */
	_nts_28,	/* 181 */
	_nts_28,	/* 182 */
	_nts_27,	/* 183 */
	_nts_27,	/* 184 */
	_nts_27,	/* 185 */
	_nts_28,	/* 186 */
	_nts_28,	/* 187 */
	_nts_28,	/* 188 */
	_nts_29,	/* 189 */
	_nts_29,	/* 190 */
	_nts_27,	/* 191 */
	_nts_27,	/* 192 */
	_nts_30,	/* 193 */
	_nts_30,	/* 194 */
	_nts_27,	/* 195 */
	_nts_27,	/* 196 */
	_nts_31,	/* 197 */
	_nts_31,	/* 198 */
	_nts_27,	/* 199 */
	_nts_27,	/* 200 */
	_nts_31,	/* 201 */
	_nts_31,	/* 202 */
	_nts_27,	/* 203 */
	_nts_27,	/* 204 */
	_nts_31,	/* 205 */
	_nts_31,	/* 206 */
	_nts_32,	/* 207 */
	_nts_32,	/* 208 */
	_nts_32,	/* 209 */
	_nts_33,	/* 210 */
	_nts_33,	/* 211 */
	_nts_33,	/* 212 */
	_nts_34,	/* 213 */
	_nts_34,	/* 214 */
	_nts_34,	/* 215 */
	_nts_34,	/* 216 */
	_nts_32,	/* 217 */
	_nts_32,	/* 218 */
	_nts_32,	/* 219 */
	_nts_32,	/* 220 */
	_nts_32,	/* 221 */
	_nts_33,	/* 222 */
	_nts_33,	/* 223 */
	_nts_32,	/* 224 */
	_nts_32,	/* 225 */
	_nts_33,	/* 226 */
	_nts_33,	/* 227 */
	_nts_32,	/* 228 */
	_nts_32,	/* 229 */
	_nts_33,	/* 230 */
	_nts_33,	/* 231 */
	_nts_35,	/* 232 */
	_nts_15,	/* 233 */
	_nts_35,	/* 234 */
	_nts_15,	/* 235 */
	_nts_35,	/* 236 */
	_nts_15,	/* 237 */
	_nts_35,	/* 238 */
	_nts_15,	/* 239 */
	_nts_35,	/* 240 */
	_nts_15,	/* 241 */
	_nts_36,	/* 242 */
	_nts_17,	/* 243 */
	_nts_14,	/* 244 */
	_nts_15,	/* 245 */
	_nts_36,	/* 246 */
	_nts_17,	/* 247 */
	_nts_14,	/* 248 */
	_nts_15,	/* 249 */
	_nts_36,	/* 250 */
	_nts_17,	/* 251 */
	_nts_14,	/* 252 */
	_nts_15,	/* 253 */
	_nts_36,	/* 254 */
	_nts_17,	/* 255 */
	_nts_14,	/* 256 */
	_nts_15,	/* 257 */
	_nts_36,	/* 258 */
	_nts_36,	/* 259 */
	_nts_36,	/* 260 */
	_nts_36,	/* 261 */
	_nts_36,	/* 262 */
	_nts_36,	/* 263 */
	_nts_17,	/* 264 */
	_nts_17,	/* 265 */
	_nts_17,	/* 266 */
	_nts_17,	/* 267 */
	_nts_17,	/* 268 */
	_nts_17,	/* 269 */
	_nts_17,	/* 270 */
	_nts_17,	/* 271 */
	_nts_14,	/* 272 */
	_nts_14,	/* 273 */
	_nts_14,	/* 274 */
	_nts_14,	/* 275 */
	_nts_14,	/* 276 */
	_nts_14,	/* 277 */
	_nts_14,	/* 278 */
	_nts_14,	/* 279 */
	_nts_15,	/* 280 */
	_nts_15,	/* 281 */
	_nts_15,	/* 282 */
	_nts_15,	/* 283 */
	_nts_15,	/* 284 */
	_nts_15,	/* 285 */
	_nts_15,	/* 286 */
	_nts_15,	/* 287 */
	_nts_3,	/* 288 */
	_nts_12,	/* 289 */
	_nts_11,	/* 290 */
	_nts_3,	/* 291 */
	_nts_12,	/* 292 */
	_nts_11,	/* 293 */
	_nts_3,	/* 294 */
	_nts_37,	/* 295 */
	_nts_38,	/* 296 */
	_nts_39,	/* 297 */
	_nts_40,	/* 298 */
	_nts_41,	/* 299 */
	_nts_8,	/* 300 */
	_nts_11,	/* 301 */
	_nts_11,	/* 302 */
	_nts_41,	/* 303 */
	_nts_7,	/* 304 */
	_nts_7,	/* 305 */
	_nts_12,	/* 306 */
	_nts_12,	/* 307 */
	_nts_3,	/* 308 */
	_nts_3,	/* 309 */
	_nts_1,	/* 310 */
	_nts_7,	/* 311 */
	_nts_7,	/* 312 */
	_nts_12,	/* 313 */
	_nts_12,	/* 314 */
	_nts_3,	/* 315 */
	_nts_3,	/* 316 */
	_nts_42,	/* 317 */
	_nts_42,	/* 318 */
	_nts_43,	/* 319 */
	_nts_43,	/* 320 */
	_nts_42,	/* 321 */
	_nts_42,	/* 322 */
	_nts_42,	/* 323 */
	_nts_42,	/* 324 */
	_nts_43,	/* 325 */
	_nts_43,	/* 326 */
	_nts_42,	/* 327 */
	_nts_42,	/* 328 */
	_nts_42,	/* 329 */
	_nts_42,	/* 330 */
	_nts_44,	/* 331 */
	_nts_45,	/* 332 */
	_nts_44,	/* 333 */
	_nts_45,	/* 334 */
	_nts_44,	/* 335 */
	_nts_45,	/* 336 */
	_nts_44,	/* 337 */
	_nts_45,	/* 338 */
	_nts_41,	/* 339 */
	_nts_41,	/* 340 */
	_nts_42,	/* 341 */
	_nts_43,	/* 342 */
	_nts_42,	/* 343 */
	_nts_43,	/* 344 */
	_nts_42,	/* 345 */
	_nts_43,	/* 346 */
	_nts_41,	/* 347 */
	_nts_42,	/* 348 */
	_nts_43,	/* 349 */
	_nts_42,	/* 350 */
	_nts_43,	/* 351 */
	_nts_42,	/* 352 */
	_nts_43,	/* 353 */
	_nts_42,	/* 354 */
	_nts_42,	/* 355 */
	_nts_42,	/* 356 */
	_nts_42,	/* 357 */
	_nts_42,	/* 358 */
	_nts_42,	/* 359 */
	_nts_42,	/* 360 */
	_nts_42,	/* 361 */
	_nts_42,	/* 362 */
	_nts_42,	/* 363 */
	_nts_42,	/* 364 */
	_nts_42,	/* 365 */
	_nts_43,	/* 366 */
	_nts_43,	/* 367 */
	_nts_43,	/* 368 */
	_nts_43,	/* 369 */
	_nts_43,	/* 370 */
	_nts_43,	/* 371 */
	_nts_43,	/* 372 */
	_nts_43,	/* 373 */
	_nts_43,	/* 374 */
	_nts_43,	/* 375 */
	_nts_43,	/* 376 */
	_nts_43,	/* 377 */
	_nts_46,	/* 378 */
	_nts_47,	/* 379 */
	_nts_38,	/* 380 */
	_nts_39,	/* 381 */
	_nts_40,	/* 382 */
	_nts_46,	/* 383 */
	_nts_47,	/* 384 */
	_nts_38,	/* 385 */
	_nts_39,	/* 386 */
	_nts_40,	/* 387 */
	_nts_48,	/* 388 */
	_nts_8,	/* 389 */
	_nts_11,	/* 390 */
	_nts_48,	/* 391 */
	_nts_7,	/* 392 */
	_nts_12,	/* 393 */
	_nts_3,	/* 394 */
	_nts_1,	/* 395 */
	_nts_7,	/* 396 */
	_nts_12,	/* 397 */
	_nts_3,	/* 398 */
	_nts_49,	/* 399 */
	_nts_50,	/* 400 */
	_nts_49,	/* 401 */
	_nts_50,	/* 402 */
	_nts_49,	/* 403 */
	_nts_50,	/* 404 */
	_nts_49,	/* 405 */
	_nts_50,	/* 406 */
	_nts_48,	/* 407 */
	_nts_49,	/* 408 */
	_nts_49,	/* 409 */
	_nts_49,	/* 410 */
	_nts_49,	/* 411 */
	_nts_49,	/* 412 */
	_nts_49,	/* 413 */
	_nts_50,	/* 414 */
	_nts_50,	/* 415 */
	_nts_50,	/* 416 */
	_nts_50,	/* 417 */
	_nts_50,	/* 418 */
	_nts_50,	/* 419 */
	_nts_51,	/* 420 */
	_nts_47,	/* 421 */
	_nts_38,	/* 422 */
	_nts_39,	/* 423 */
	_nts_40,	/* 424 */
	_nts_3,	/* 425 */
	_nts_1,	/* 426 */
	_nts_7,	/* 427 */
	_nts_3,	/* 428 */
	_nts_1,	/* 429 */
	_nts_7,	/* 430 */
	_nts_3,	/* 431 */
	_nts_1,	/* 432 */
	_nts_7,	/* 433 */
	_nts_3,	/* 434 */
	_nts_1,	/* 435 */
	_nts_7,	/* 436 */
	_nts_3,	/* 437 */
	_nts_1,	/* 438 */
	_nts_7,	/* 439 */
	_nts_3,	/* 440 */
	_nts_1,	/* 441 */
	_nts_7,	/* 442 */
	_nts_3,	/* 443 */
	_nts_1,	/* 444 */
	_nts_7,	/* 445 */
	_nts_1,	/* 446 */
	_nts_1,	/* 447 */
	_nts_1,	/* 448 */
	_nts_1,	/* 449 */
	_nts_1,	/* 450 */
	_nts_1,	/* 451 */
	_nts_1,	/* 452 */
	_nts_1,	/* 453 */
	_nts_1,	/* 454 */
	_nts_1,	/* 455 */
	_nts_1,	/* 456 */
	_nts_1,	/* 457 */
	_nts_48,	/* 458 */
	_nts_41,	/* 459 */
	_nts_41,	/* 460 */
	_nts_7,	/* 461 */
	_nts_7,	/* 462 */
	_nts_7,	/* 463 */
	_nts_1,	/* 464 */
	_nts_1,	/* 465 */
	_nts_7,	/* 466 */
	_nts_7,	/* 467 */
	_nts_10,	/* 468 */
	_nts_10,	/* 469 */
	_nts_7,	/* 470 */
	_nts_7,	/* 471 */
	_nts_7,	/* 472 */
	_nts_10,	/* 473 */
	_nts_10,	/* 474 */
	_nts_10,	/* 475 */
	_nts_11,	/* 476 */
	_nts_11,	/* 477 */
	_nts_11,	/* 478 */
	_nts_11,	/* 479 */
	_nts_11,	/* 480 */
	_nts_1,	/* 481 */
	_nts_1,	/* 482 */
	_nts_1,	/* 483 */
	_nts_41,	/* 484 */
	_nts_41,	/* 485 */
	_nts_41,	/* 486 */
	_nts_41,	/* 487 */
	_nts_41,	/* 488 */
	_nts_48,	/* 489 */
	_nts_1,	/* 490 */
	_nts_1,	/* 491 */
	_nts_7,	/* 492 */
	_nts_7,	/* 493 */
	_nts_1,	/* 494 */
	_nts_1,	/* 495 */
	_nts_1,	/* 496 */
	_nts_1,	/* 497 */
	_nts_1,	/* 498 */
	_nts_10,	/* 499 */
	_nts_10,	/* 500 */
	_nts_7,	/* 501 */
	_nts_7,	/* 502 */
	_nts_9,	/* 503 */
	_nts_9,	/* 504 */
	_nts_11,	/* 505 */
	_nts_11,	/* 506 */
	_nts_7,	/* 507 */
	_nts_10,	/* 508 */
	_nts_7,	/* 509 */
	_nts_7,	/* 510 */
	_nts_7,	/* 511 */
	_nts_7,	/* 512 */
	_nts_10,	/* 513 */
	_nts_7,	/* 514 */
	_nts_7,	/* 515 */
	_nts_7,	/* 516 */
	_nts_48,	/* 517 */
	_nts_48,	/* 518 */
	_nts_7,	/* 519 */
	_nts_41,	/* 520 */
	_nts_48,	/* 521 */
	_nts_48,	/* 522 */
	_nts_10,	/* 523 */
	_nts_7,	/* 524 */
	_nts_41,	/* 525 */
	_nts_0,	/* 526 */
	_nts_3,	/* 527 */
	_nts_1,	/* 528 */
	_nts_7,	/* 529 */
	_nts_52,	/* 530 */
	_nts_52,	/* 531 */
	_nts_52,	/* 532 */
	_nts_52,	/* 533 */
	_nts_52,	/* 534 */
	_nts_52,	/* 535 */
	_nts_4,	/* 536 */
	_nts_5,	/* 537 */
	_nts_25,	/* 538 */
	_nts_25,	/* 539 */
	_nts_25,	/* 540 */
	_nts_25,	/* 541 */
	_nts_25,	/* 542 */
	_nts_25,	/* 543 */
	_nts_53,	/* 544 */
	_nts_53,	/* 545 */
	_nts_53,	/* 546 */
	_nts_53,	/* 547 */
	_nts_53,	/* 548 */
	_nts_53,	/* 549 */
	_nts_24,	/* 550 */
	_nts_24,	/* 551 */
	_nts_7,	/* 552 */
	_nts_7,	/* 553 */
	_nts_54,	/* 554 */
	_nts_54,	/* 555 */
	_nts_1,	/* 556 */
	_nts_11,	/* 557 */
	_nts_11,	/* 558 */
	_nts_41,	/* 559 */
	_nts_14,	/* 560 */
	_nts_14,	/* 561 */
	_nts_14,	/* 562 */
	_nts_14,	/* 563 */
	_nts_14,	/* 564 */
	_nts_25,	/* 565 */
	_nts_25,	/* 566 */
	_nts_25,	/* 567 */
	_nts_17,	/* 568 */
	_nts_17,	/* 569 */
	_nts_17,	/* 570 */
	_nts_55,	/* 571 */
	_nts_17,	/* 572 */
	_nts_1,	/* 573 */
	_nts_1,	/* 574 */
	_nts_1,	/* 575 */
	_nts_1,	/* 576 */
	_nts_1,	/* 577 */
	_nts_2,	/* 578 */
	_nts_2,	/* 579 */
	_nts_2,	/* 580 */
	_nts_2,	/* 581 */
	_nts_2,	/* 582 */
	_nts_5,	/* 583 */
	_nts_4,	/* 584 */
	_nts_7,	/* 585 */
	_nts_7,	/* 586 */
	_nts_7,	/* 587 */
	_nts_7,	/* 588 */
	_nts_7,	/* 589 */
	_nts_56,	/* 590 */
	_nts_57,	/* 591 */
	_nts_57,	/* 592 */
	_nts_57,	/* 593 */
	_nts_57,	/* 594 */
	_nts_58,	/* 595 */
	_nts_58,	/* 596 */
	_nts_58,	/* 597 */
	_nts_59,	/* 598 */
	_nts_5,	/* 599 */
	_nts_5,	/* 600 */
	_nts_5,	/* 601 */
	_nts_5,	/* 602 */
	_nts_4,	/* 603 */
	_nts_4,	/* 604 */
	_nts_4,	/* 605 */
	_nts_1,	/* 606 */
	_nts_1,	/* 607 */
	_nts_1,	/* 608 */
	_nts_60,	/* 609 */
	_nts_60,	/* 610 */
	_nts_60,	/* 611 */
	_nts_25,	/* 612 */
	_nts_25,	/* 613 */
	_nts_25,	/* 614 */
	_nts_53,	/* 615 */
	_nts_53,	/* 616 */
	_nts_53,	/* 617 */
	_nts_61,	/* 618 */
	_nts_61,	/* 619 */
	_nts_61,	/* 620 */
	_nts_12,	/* 621 */
	_nts_12,	/* 622 */
	_nts_12,	/* 623 */
	_nts_12,	/* 624 */
	_nts_12,	/* 625 */
	_nts_62,	/* 626 */
	_nts_62,	/* 627 */
	_nts_62,	/* 628 */
	_nts_1,	/* 629 */
	_nts_1,	/* 630 */
	_nts_1,	/* 631 */
	_nts_0,	/* 632 */
	_nts_2,	/* 633 */
	_nts_63,	/* 634 */
	_nts_63,	/* 635 */
	_nts_64,	/* 636 */
	_nts_64,	/* 637 */
	_nts_63,	/* 638 */
	_nts_63,	/* 639 */
	_nts_63,	/* 640 */
	_nts_65,	/* 641 */
	_nts_65,	/* 642 */
	_nts_65,	/* 643 */
	_nts_66,	/* 644 */
	_nts_66,	/* 645 */
	_nts_66,	/* 646 */
	_nts_65,	/* 647 */
	_nts_65,	/* 648 */
	_nts_65,	/* 649 */
	_nts_67,	/* 650 */
	_nts_67,	/* 651 */
	_nts_67,	/* 652 */
	_nts_67,	/* 653 */
	_nts_67,	/* 654 */
	_nts_67,	/* 655 */
	_nts_68,	/* 656 */
	_nts_69,	/* 657 */
	_nts_68,	/* 658 */
	_nts_69,	/* 659 */
	_nts_69,	/* 660 */
	_nts_68,	/* 661 */
	_nts_68,	/* 662 */
	_nts_69,	/* 663 */
	_nts_69,	/* 664 */
	_nts_69,	/* 665 */
	_nts_69,	/* 666 */
	_nts_69,	/* 667 */
	_nts_69,	/* 668 */
	_nts_69,	/* 669 */
	_nts_69,	/* 670 */
	_nts_69,	/* 671 */
	_nts_69,	/* 672 */
	_nts_70,	/* 673 */
	_nts_70,	/* 674 */
	_nts_71,	/* 675 */
	_nts_71,	/* 676 */
	_nts_72,	/* 677 */
	_nts_72,	/* 678 */
	_nts_72,	/* 679 */
	_nts_72,	/* 680 */
	_nts_73,	/* 681 */
	_nts_74,	/* 682 */
	_nts_75,	/* 683 */
	_nts_76,	/* 684 */
	_nts_77,	/* 685 */
	_nts_78,	/* 686 */
	_nts_79,	/* 687 */
	_nts_79,	/* 688 */
	_nts_80,	/* 689 */
	_nts_80,	/* 690 */
	_nts_81,	/* 691 */
	_nts_81,	/* 692 */
	_nts_82,	/* 693 */
	_nts_82,	/* 694 */
	_nts_83,	/* 695 */
	_nts_83,	/* 696 */
	_nts_83,	/* 697 */
	_nts_84,	/* 698 */
	_nts_84,	/* 699 */
	_nts_84,	/* 700 */
	_nts_85,	/* 701 */
	_nts_85,	/* 702 */
	_nts_85,	/* 703 */
	_nts_86,	/* 704 */
	_nts_86,	/* 705 */
	_nts_86,	/* 706 */
	_nts_87,	/* 707 */
	_nts_87,	/* 708 */
	_nts_87,	/* 709 */
	_nts_87,	/* 710 */
	_nts_87,	/* 711 */
	_nts_87,	/* 712 */
	_nts_88,	/* 713 */
	_nts_88,	/* 714 */
	_nts_88,	/* 715 */
	_nts_88,	/* 716 */
	_nts_88,	/* 717 */
	_nts_88,	/* 718 */
	_nts_64,	/* 719 */
	_nts_64,	/* 720 */
	_nts_64,	/* 721 */
	_nts_89,	/* 722 */
	_nts_89,	/* 723 */
	_nts_89,	/* 724 */
	_nts_89,	/* 725 */
	_nts_89,	/* 726 */
	_nts_89,	/* 727 */
};

static char *_templates[] = {
/* 0 */	0,
/* 1 */	"# read register\n",	/* reg: INDIRI1(VREGP) */
/* 2 */	"# read register\n",	/* reg: INDIRU1(VREGP) */
/* 3 */	"# read register\n",	/* reg: INDIRI2(VREGP) */
/* 4 */	"# read register\n",	/* reg: INDIRU2(VREGP) */
/* 5 */	"# read register\n",	/* reg: INDIRP2(VREGP) */
/* 6 */	"# read register\n",	/* reg: INDIRI4(VREGP) */
/* 7 */	"# read register\n",	/* reg: INDIRU4(VREGP) */
/* 8 */	"# read register\n",	/* reg: INDIRF5(VREGP) */
/* 9 */	"# write register\n",	/* stmt: ASGNI1(VREGP,reg) */
/* 10 */	"# write register\n",	/* stmt: ASGNU1(VREGP,reg) */
/* 11 */	"# write register\n",	/* stmt: ASGNI2(VREGP,reg) */
/* 12 */	"# write register\n",	/* stmt: ASGNU2(VREGP,reg) */
/* 13 */	"# write register\n",	/* stmt: ASGNP2(VREGP,reg) */
/* 14 */	"# write register\n",	/* stmt: ASGNI4(VREGP,reg) */
/* 15 */	"# write register\n",	/* stmt: ASGNU4(VREGP,reg) */
/* 16 */	"# write register\n",	/* stmt: ASGNF5(VREGP,reg) */
/* 17 */	"%a",	/* con0: CNSTI1 */
/* 18 */	"%a",	/* con0: CNSTU1 */
/* 19 */	"%a",	/* con0: CNSTI2 */
/* 20 */	"%a",	/* con0: CNSTU2 */
/* 21 */	"%a",	/* con0: CNSTP2 */
/* 22 */	"%a",	/* con1: CNSTI1 */
/* 23 */	"%a",	/* con1: CNSTU1 */
/* 24 */	"%a",	/* con1: CNSTI2 */
/* 25 */	"%a",	/* con1: CNSTU2 */
/* 26 */	"%a",	/* conB: CNSTI2 */
/* 27 */	"%a",	/* conB: CNSTU2 */
/* 28 */	"%a",	/* conB: CNSTP2 */
/* 29 */	"%a",	/* conB: CNSTU1 */
/* 30 */	"%0",	/* conB: zddr */
/* 31 */	"%a",	/* conBs: CNSTI1 */
/* 32 */	"%a",	/* conBs: CNSTI2 */
/* 33 */	"%a",	/* conBn: CNSTI2 */
/* 34 */	"%a",	/* conBm: CNSTI2 */
/* 35 */	"%a",	/* conBm: CNSTU2 */
/* 36 */	"%a",	/* conBa: CNSTI2 */
/* 37 */	"%a",	/* conBa: CNSTU2 */
/* 38 */	"%a",	/* con: CNSTI1 */
/* 39 */	"%a",	/* con: CNSTU1 */
/* 40 */	"%a",	/* con: CNSTI2 */
/* 41 */	"%a",	/* con: CNSTU2 */
/* 42 */	"%a",	/* con: CNSTP2 */
/* 43 */	"%0",	/* con: addr */
/* 44 */	"%a",	/* con8: CNSTU2 */
/* 45 */	"%a",	/* con8: CNSTI2 */
/* 46 */	"%a",	/* conFF: CNSTU2 */
/* 47 */	"%a",	/* conFF: CNSTI2 */
/* 48 */	"%a",	/* conFF00: CNSTU2 */
/* 49 */	"%a",	/* conXX00: CNSTU2 */
/* 50 */	"%a",	/* conXX00: CNSTI2 */
/* 51 */	"%a+%F",	/* lddr: ADDRLP2 */
/* 52 */	"%a+%F",	/* lddr: ADDRFP2 */
/* 53 */	"%a",	/* addr: ADDRGP2 */
/* 54 */	"%0",	/* addr: con */
/* 55 */	"%0",	/* addr: zddr */
/* 56 */	"%a",	/* zddr: ADDRGP2 */
/* 57 */	"%0",	/* zddr: conB */
/* 58 */	"",	/* stmt: reg */
/* 59 */	"%0",	/* stmt: asgn */
/* 60 */	"\t%0\n",	/* stmt: ac */
/* 61 */	"%{=%0}%0",	/* regx: reg */
/* 62 */	"%{=%0}%0",	/* reg: regx */
/* 63 */	"\t%{=vAC}%0%{?c==vAC::STW(%c);}\n",	/* reg: ac */
/* 64 */	"%{=%0}%0",	/* ac0: eac0 */
/* 65 */	"%{=%0}%0",	/* eac: eac0 */
/* 66 */	"%{=%0}%0",	/* ac: ac0 */
/* 67 */	"%{=%0}%0",	/* ac: eac */
/* 68 */	"%{=%0}%{?0=~vAC::LDW(%0);}",	/* eac: reg */
/* 69 */	"%{=%0}%{?0=~vAC::_SP(%0);}",	/* eac: lddr */
/* 70 */	"%{=%0}%{?0=~vAC::LDI(%0);}",	/* eac0: conB */
/* 71 */	"%{=%0}%{?0=~vAC::LDWI(%0);}",	/* eac: con */
/* 72 */	"%{?*0=~vAC::%0DEEK();}",	/* eac: INDIRI2(eac) */
/* 73 */	"%{?*0=~vAC::%0DEEK();}",	/* eac: INDIRU2(eac) */
/* 74 */	"%{?*0=~vAC::%0DEEK();}",	/* eac: INDIRP2(eac) */
/* 75 */	"%0PEEK();",	/* eac0: INDIRI1(eac) */
/* 76 */	"%0PEEK();",	/* eac0: INDIRU1(eac) */
/* 77 */	"%{?*0=~vAC::LDW(%0);}",	/* eac: INDIRI2(zddr) */
/* 78 */	"%{?*0=~vAC::LDW(%0);}",	/* eac: INDIRU2(zddr) */
/* 79 */	"%{?*0=~vAC::LDW(%0);}",	/* eac: INDIRP2(zddr) */
/* 80 */	"LD(%0);",	/* eac0: INDIRI1(zddr) */
/* 81 */	"LD(%0);",	/* eac0: INDIRU1(zddr) */
/* 82 */	"%{?*0=~vAC::%0DEEK();}",	/* ac: INDIRI2(ac) */
/* 83 */	"%{?*0=~vAC::%0DEEK();}",	/* ac: INDIRU2(ac) */
/* 84 */	"%{?*0=~vAC::%0DEEK();}",	/* ac: INDIRP2(ac) */
/* 85 */	"%0PEEK();",	/* ac0: INDIRI1(ac) */
/* 86 */	"%0PEEK();",	/* ac0: INDIRU1(ac) */
/* 87 */	"%0",	/* iarg: regx */
/* 88 */	"%0",	/* iarg: INDIRI2(zddr) */
/* 89 */	"%0",	/* iarg: INDIRU2(zddr) */
/* 90 */	"%0",	/* iarg: INDIRP2(zddr) */
/* 91 */	"%a+%F",	/* spill: ADDRLP2 */
/* 92 */	"T1|STW(T0);_LDLW(%0);STW(T1);LDW(T0);",	/* iarg: INDIRU2(spill) */
/* 93 */	"T1|STW(T0);_LDLW(%0);STW(T1);LDW(T0);",	/* iarg: INDIRI2(spill) */
/* 94 */	"T1|STW(T0);_LDLW(%0);STW(T1);LDW(T0);",	/* iarg: INDIRP2(spill) */
/* 95 */	"%0%[1b]ADDW(%1);",	/* ac: ADDI2(ac,iarg) */
/* 96 */	"%0%[1b]ADDW(%1);",	/* ac: ADDU2(ac,iarg) */
/* 97 */	"%0%[1b]ADDW(%1);",	/* ac: ADDP2(ac,iarg) */
/* 98 */	"%1%[0b]ADDW(%0);",	/* ac: ADDI2(iarg,ac) */
/* 99 */	"%1%[0b]ADDW(%0);",	/* ac: ADDU2(iarg,ac) */
/* 100 */	"%1%[0b]ADDW(%0);",	/* ac: ADDP2(iarg,ac) */
/* 101 */	"%2%[0b]ADDW(%0);ADDW(%0);",	/* ac: ADDI2(LSHI2(iarg,con1),ac) */
/* 102 */	"%2%[0b]ADDW(%0);ADDW(%0);",	/* ac: ADDU2(LSHU2(iarg,con1),ac) */
/* 103 */	"%2%[0b]ADDW(%0);ADDW(%0);",	/* ac: ADDP2(LSHI2(iarg,con1),ac) */
/* 104 */	"%2%[0b]ADDW(%0);ADDW(%0);",	/* ac: ADDP2(LSHU2(iarg,con1),ac) */
/* 105 */	"%0ADDI(%1);",	/* ac: ADDI2(ac,conB) */
/* 106 */	"%0ADDI(%1);",	/* ac: ADDU2(ac,conB) */
/* 107 */	"%0ADDI(%1);",	/* ac: ADDP2(ac,conB) */
/* 108 */	"%0SUBI(-v(%1));",	/* ac: ADDI2(ac,conBn) */
/* 109 */	"%0SUBI(-v(%1));",	/* ac: ADDU2(ac,conBn) */
/* 110 */	"%0SUBI(-v(%1));",	/* ac: ADDP2(ac,conBn) */
/* 111 */	"%0%[1b]SUBW(%1);",	/* ac: SUBI2(ac,iarg) */
/* 112 */	"%0%[1b]SUBW(%1);",	/* ac: SUBU2(ac,iarg) */
/* 113 */	"%0%[1b]SUBW(%1);",	/* ac: SUBP2(ac,iarg) */
/* 114 */	"%0SUBI(%1);",	/* ac: SUBI2(ac,conB) */
/* 115 */	"%0SUBI(%1);",	/* ac: SUBU2(ac,conB) */
/* 116 */	"%0SUBI(%1);",	/* ac: SUBP2(ac,conB) */
/* 117 */	"%0ADDI(-v(%1));",	/* ac: SUBI2(ac,conBn) */
/* 118 */	"%0ADDI(-v(%1));",	/* ac: SUBU2(ac,conBn) */
/* 119 */	"%0ADDI(-v(%1));",	/* ac: SUBP2(ac,conBn) */
/* 120 */	"%0STW(T3);LDI(0);SUBW(T3);",	/* ac: NEGI2(ac) */
/* 121 */	"LDI(0);SUBW(%0);",	/* ac: NEGI2(regx) */
/* 122 */	"%0LSLW();",	/* ac: LSHI2(ac,con1) */
/* 123 */	"%0LSLW();",	/* ac: LSHU2(ac,con1) */
/* 124 */	"%0_SHLI(%1);",	/* ac: LSHI2(ac,conB) */
/* 125 */	"%0_SHLI(%1);",	/* ac: LSHU2(ac,conB) */
/* 126 */	"%0_SHRIS(%1);",	/* ac: RSHI2(ac,conB) */
/* 127 */	"%0_SHRIU(%1);",	/* ac: RSHU2(ac,conB) */
/* 128 */	"%0%[1b]_SHL(%1);",	/* ac: LSHI2(ac,iarg) */
/* 129 */	"%0%[1b]_SHRS(%1);",	/* ac: RSHI2(ac,iarg) */
/* 130 */	"%0%[1b]_SHL(%1);",	/* ac: LSHU2(ac,iarg) */
/* 131 */	"%0%[1b]_SHRU(%1);",	/* ac: RSHU2(ac,iarg) */
/* 132 */	"%1%{mul0}",	/* ac: MULI2(conB,ac) */
/* 133 */	"%1%{mul0}",	/* ac: MULI2(conBn,ac) */
/* 134 */	"%{mul0%1}",	/* ac: MULI2(conB,regx) */
/* 135 */	"%{mul0%1}",	/* ac: MULI2(conBn,regx) */
/* 136 */	"%1_MULI(%0);",	/* ac: MULI2(con,ac) */
/* 137 */	"%0%[1b]_MUL(%1);",	/* ac: MULI2(ac,iarg) */
/* 138 */	"%1%[0b]_MUL(%0);",	/* ac: MULI2(iarg,ac) */
/* 139 */	"%1%{mul0}",	/* ac: MULU2(conB,ac) */
/* 140 */	"%1_MULI(%0);",	/* ac: MULU2(con,ac) */
/* 141 */	"%0%[1b]_MUL(%1);",	/* ac: MULU2(ac,iarg) */
/* 142 */	"%1%[0b]_MUL(%0);",	/* ac: MULU2(iarg,ac) */
/* 143 */	"%0%[1b]_DIVS(%1);",	/* ac: DIVI2(ac,iarg) */
/* 144 */	"%0%[1b]_DIVU(%1);",	/* ac: DIVU2(ac,iarg) */
/* 145 */	"%0%[1b]_MODS(%1);",	/* ac: MODI2(ac,iarg) */
/* 146 */	"%0%[1b]_MODU(%1);",	/* ac: MODU2(ac,iarg) */
/* 147 */	"%0%[1b]_DIVIS(%1);",	/* ac: DIVI2(ac,con) */
/* 148 */	"%0%[1b]_DIVIU(%1);",	/* ac: DIVU2(ac,con) */
/* 149 */	"%0%[1b]_MODIS(%1);",	/* ac: MODI2(ac,con) */
/* 150 */	"%0%[1b]_MODIU(%1);",	/* ac: MODU2(ac,con) */
/* 151 */	"%0STW(T3);_LDI(-1);XORW(T3);",	/* ac: BCOMI2(ac) */
/* 152 */	"%0STW(T3);_LDI(-1);XORW(T3);",	/* ac: BCOMU2(ac) */
/* 153 */	"%0%[1b]ANDW(%1);",	/* ac: BANDI2(ac,iarg) */
/* 154 */	"%0%[1b]ANDW(%1);",	/* ac: BANDU2(ac,iarg) */
/* 155 */	"%1%[0b]ANDW(%0);",	/* ac: BANDI2(iarg,ac) */
/* 156 */	"%1%[0b]ANDW(%0);",	/* ac: BANDU2(iarg,ac) */
/* 157 */	"%0ANDI(%1);",	/* ac: BANDI2(ac,conB) */
/* 158 */	"%0ANDI(%1);",	/* ac: BANDU2(ac,conB) */
/* 159 */	"%0ANDI(%1);",	/* ac0: BANDI2(ac0,conB) */
/* 160 */	"%0ANDI(%1);",	/* ac0: BANDU2(ac0,conB) */
/* 161 */	"%0%[1b]ORW(%1);",	/* ac: BORI2(ac,iarg) */
/* 162 */	"%0%[1b]ORW(%1);",	/* ac: BORU2(ac,iarg) */
/* 163 */	"%1%[0b]ORW(%0);",	/* ac: BORI2(iarg,ac) */
/* 164 */	"%1%[0b]ORW(%0);",	/* ac: BORU2(iarg,ac) */
/* 165 */	"%0ORI(%1);",	/* ac: BORI2(ac,conB) */
/* 166 */	"%0ORI(%1);",	/* ac: BORU2(ac,conB) */
/* 167 */	"%0ORI(%1);",	/* ac0: BORI2(ac0,conB) */
/* 168 */	"%0ORI(%1);",	/* ac0: BORU2(ac0,conB) */
/* 169 */	"%0%[1b]XORW(%1);",	/* ac: BXORI2(ac,iarg) */
/* 170 */	"%0%[1b]XORW(%1);",	/* ac: BXORU2(ac,iarg) */
/* 171 */	"%1%[0b]XORW(%0);",	/* ac: BXORI2(iarg,ac) */
/* 172 */	"%1%[0b]XORW(%0);",	/* ac: BXORU2(iarg,ac) */
/* 173 */	"%0XORI(%1);",	/* ac: BXORI2(ac,conB) */
/* 174 */	"%0XORI(%1);",	/* ac: BXORU2(ac,conB) */
/* 175 */	"%0XORI(%1);",	/* ac0: BXORI2(ac0,conB) */
/* 176 */	"%0XORI(%1);",	/* ac0: BXORU2(ac0,conB) */
/* 177 */	"%0ADDI(%1);",	/* eac: ADDI2(eac,conB) */
/* 178 */	"%0ADDI(%1);",	/* eac: ADDU2(eac,conB) */
/* 179 */	"%0ADDI(%1);",	/* eac: ADDP2(eac,conB) */
/* 180 */	"%0SUBI(-v(%1));",	/* eac: ADDI2(eac,conBn) */
/* 181 */	"%0SUBI(-v(%1));",	/* eac: ADDU2(eac,conBn) */
/* 182 */	"%0SUBI(-v(%1));",	/* eac: ADDP2(eac,conBn) */
/* 183 */	"%0SUBI(%1);",	/* eac: SUBI2(eac,conB) */
/* 184 */	"%0SUBI(%1);",	/* eac: SUBU2(eac,conB) */
/* 185 */	"%0SUBI(%1);",	/* eac: SUBP2(eac,conB) */
/* 186 */	"%0ADDI(-v(%1));",	/* eac: SUBI2(eac,conBn) */
/* 187 */	"%0ADDI(-v(%1));",	/* eac: SUBU2(eac,conBn) */
/* 188 */	"%0ADDI(-v(%1));",	/* eac: SUBP2(eac,conBn) */
/* 189 */	"%0LSLW();",	/* eac: LSHI2(eac,con1) */
/* 190 */	"%0LSLW();",	/* eac: LSHU2(eac,con1) */
/* 191 */	"%0_SHLI(%1);",	/* eac: LSHI2(eac,conB) */
/* 192 */	"%0_SHLI(%1);",	/* eac: LSHU2(eac,conB) */
/* 193 */	"%1%{mul0}",	/* eac: MULI2(conB,eac) */
/* 194 */	"%1%{mul0}",	/* eac: MULI2(conB,eac) */
/* 195 */	"%0ANDI(%1);",	/* eac: BANDI2(eac,conB) */
/* 196 */	"%0ANDI(%1);",	/* eac: BANDU2(eac,conB) */
/* 197 */	"%0ANDI(%1);",	/* eac0: BANDI2(eac0,conB) */
/* 198 */	"%0ANDI(%1);",	/* eac0: BANDU2(eac0,conB) */
/* 199 */	"%0ORI(%1);",	/* eac: BORI2(eac,conB) */
/* 200 */	"%0ORI(%1);",	/* eac: BORU2(eac,conB) */
/* 201 */	"%0ORI(%1);",	/* eac0: BORI2(eac0,conB) */
/* 202 */	"%0ORI(%1);",	/* eac0: BORU2(eac0,conB) */
/* 203 */	"%0XORI(%1);",	/* eac: BXORI2(eac,conB) */
/* 204 */	"%0XORI(%1);",	/* eac: BXORU2(eac,conB) */
/* 205 */	"%0XORI(%1);",	/* eac0: BXORI2(eac0,conB) */
/* 206 */	"%0XORI(%1);",	/* eac0: BXORU2(eac0,conB) */
/* 207 */	"%0%[1b]ADDW(%1);",	/* eac: ADDI2(eac,iarg) */
/* 208 */	"%0%[1b]ADDW(%1);",	/* eac: ADDU2(eac,iarg) */
/* 209 */	"%0%[1b]ADDW(%1);",	/* eac: ADDP2(eac,iarg) */
/* 210 */	"%1%[0b]ADDW(%0);",	/* eac: ADDI2(iarg,eac) */
/* 211 */	"%1%[0b]ADDW(%0);",	/* eac: ADDU2(iarg,eac) */
/* 212 */	"%1%[0b]ADDW(%0);",	/* eac: ADDP2(iarg,eac) */
/* 213 */	"%2%[0b]ADDW(%0);ADDW(%0);",	/* eac: ADDI2(LSHI2(iarg,con1),eac) */
/* 214 */	"%2%[0b]ADDW(%0);ADDW(%0);",	/* eac: ADDU2(LSHU2(iarg,con1),eac) */
/* 215 */	"%2%[0b]ADDW(%0);ADDW(%0);",	/* eac: ADDP2(LSHI2(iarg,con1),eac) */
/* 216 */	"%2%[0b]ADDW(%0);ADDW(%0);",	/* eac: ADDP2(LSHU2(iarg,con1),eac) */
/* 217 */	"%0%[1b]SUBW(%1);",	/* eac: SUBI2(eac,iarg) */
/* 218 */	"%0%[1b]SUBW(%1);",	/* eac: SUBU2(eac,iarg) */
/* 219 */	"%0%[1b]SUBW(%1);",	/* eac: SUBP2(eac,iarg) */
/* 220 */	"%0%[1b]ANDW(%1);",	/* eac: BANDI2(eac,iarg) */
/* 221 */	"%0%[1b]ANDW(%1);",	/* eac: BANDU2(eac,iarg) */
/* 222 */	"%1%[0b]ANDW(%0);",	/* eac: BANDI2(iarg,eac) */
/* 223 */	"%1%[0b]ANDW(%0);",	/* eac: BANDU2(iarg,eac) */
/* 224 */	"%0%[1b]ORW(%1);",	/* eac: BORI2(eac,iarg) */
/* 225 */	"%0%[1b]ORW(%1);",	/* eac: BORU2(eac,iarg) */
/* 226 */	"%1%[0b]ORW(%0);",	/* eac: BORI2(iarg,eac) */
/* 227 */	"%1%[0b]ORW(%0);",	/* eac: BORU2(iarg,eac) */
/* 228 */	"%0%[1b]XORW(%1);",	/* eac: BXORI2(eac,iarg) */
/* 229 */	"%0%[1b]XORW(%1);",	/* eac: BXORU2(eac,iarg) */
/* 230 */	"%1%[0b]XORW(%0);",	/* eac: BXORI2(iarg,eac) */
/* 231 */	"%1%[0b]XORW(%0);",	/* eac: BXORU2(iarg,eac) */
/* 232 */	"\t%{=vAC}%1%{?0==vAC::STW(%0);}\n",	/* asgn: ASGNP2(zddr,ac) */
/* 233 */	"\t%{=vAC}%1%[0b]DOKE(%0);\n",	/* asgn: ASGNP2(iarg,ac) */
/* 234 */	"\t%{=vAC}%1%{?0==vAC::STW(%0);}\n",	/* asgn: ASGNI2(zddr,ac) */
/* 235 */	"\t%{=vAC}%1%[0b]DOKE(%0);\n",	/* asgn: ASGNI2(iarg,ac) */
/* 236 */	"\t%{=vAC}%1%{?0==vAC::STW(%0);}\n",	/* asgn: ASGNU2(zddr,ac) */
/* 237 */	"\t%{=vAC}%1%[0b]DOKE(%0);\n",	/* asgn: ASGNU2(iarg,ac) */
/* 238 */	"\t%1%{?0==vAC::ST(%0);}\n",	/* asgn: ASGNI1(zddr,ac) */
/* 239 */	"\t%1%[0b]POKE(%0);\n",	/* asgn: ASGNI1(iarg,ac) */
/* 240 */	"\t%1%{?0==vAC::ST(%0);}\n",	/* asgn: ASGNU1(zddr,ac) */
/* 241 */	"\t%1%[0b]POKE(%0);\n",	/* asgn: ASGNU1(iarg,ac) */
/* 242 */	"\t%0_BEQ(%a);\n",	/* stmt: EQI2(ac,con0) */
/* 243 */	"\t%0XORI(%1);_BEQ(%a)%{!A};\n",	/* stmt: EQI2(ac,conB) */
/* 244 */	"\t%0%[1b]XORW(%1);_BEQ(%a)%{!A};\n",	/* stmt: EQI2(ac,iarg) */
/* 245 */	"\t%1%[0b]XORW(%0);_BEQ(%a)%{!A};\n",	/* stmt: EQI2(iarg,ac) */
/* 246 */	"\t%0_BNE(%a);\n",	/* stmt: NEI2(ac,con0) */
/* 247 */	"\t%0XORI(%1);_BNE(%a)%{!A};\n",	/* stmt: NEI2(ac,conB) */
/* 248 */	"\t%0%[1b]XORW(%1);_BNE(%a)%{!A};\n",	/* stmt: NEI2(ac,iarg) */
/* 249 */	"\t%1%[0b]XORW(%0);_BNE(%a)%{!A};\n",	/* stmt: NEI2(iarg,ac) */
/* 250 */	"\t%0_BEQ(%a);\n",	/* stmt: EQU2(ac,con0) */
/* 251 */	"\t%0XORI(%1);_BEQ(%a)%{!A};\n",	/* stmt: EQU2(ac,conB) */
/* 252 */	"\t%0%[1b]XORW(%1);_BEQ(%a)%{!A};\n",	/* stmt: EQU2(ac,iarg) */
/* 253 */	"\t%1%[0b]XORW(%0);_BEQ(%a)%{!A};\n",	/* stmt: EQU2(iarg,ac) */
/* 254 */	"\t%0_BNE(%a);\n",	/* stmt: NEU2(ac,con0) */
/* 255 */	"\t%0XORI(%1);_BNE(%a)%{!A};\n",	/* stmt: NEU2(ac,conB) */
/* 256 */	"\t%0%[1b]XORW(%1);_BNE(%a)%{!A};\n",	/* stmt: NEU2(ac,iarg) */
/* 257 */	"\t%1%[0b]XORW(%0);_BNE(%a)%{!A};\n",	/* stmt: NEU2(iarg,ac) */
/* 258 */	"\t%0_BLT(%a);\n",	/* stmt: LTI2(ac,con0) */
/* 259 */	"\t%0_BLE(%a);\n",	/* stmt: LEI2(ac,con0) */
/* 260 */	"\t%0_BGT(%a);\n",	/* stmt: GTI2(ac,con0) */
/* 261 */	"\t%0_BGE(%a);\n",	/* stmt: GEI2(ac,con0) */
/* 262 */	"\t%0_BNE(%a);\n",	/* stmt: GTU2(ac,con0) */
/* 263 */	"\t%0_BEQ(%a);\n",	/* stmt: LEU2(ac,con0) */
/* 264 */	"\t%0_CMPIS(%1);_BLT(%a)%{!A};\n",	/* stmt: LTI2(ac,conB) */
/* 265 */	"\t%0_CMPIS(%1);_BLE(%a)%{!A};\n",	/* stmt: LEI2(ac,conB) */
/* 266 */	"\t%0_CMPIS(%1);_BGT(%a)%{!A};\n",	/* stmt: GTI2(ac,conB) */
/* 267 */	"\t%0_CMPIS(%1);_BGE(%a)%{!A};\n",	/* stmt: GEI2(ac,conB) */
/* 268 */	"\t%0_CMPIU(%1);_BLT(%a)%{!A};\n",	/* stmt: LTU2(ac,conB) */
/* 269 */	"\t%0_CMPIU(%1);_BLE(%a)%{!A};\n",	/* stmt: LEU2(ac,conB) */
/* 270 */	"\t%0_CMPIU(%1);_BGT(%a)%{!A};\n",	/* stmt: GTU2(ac,conB) */
/* 271 */	"\t%0_CMPIU(%1);_BGE(%a)%{!A};\n",	/* stmt: GEU2(ac,conB) */
/* 272 */	"\t%0%[1b]_CMPWS(%1);_BLT(%a)%{!A};\n",	/* stmt: LTI2(ac,iarg) */
/* 273 */	"\t%0%[1b]_CMPWS(%1);_BLE(%a)%{!A};\n",	/* stmt: LEI2(ac,iarg) */
/* 274 */	"\t%0%[1b]_CMPWS(%1);_BGT(%a)%{!A};\n",	/* stmt: GTI2(ac,iarg) */
/* 275 */	"\t%0%[1b]_CMPWS(%1);_BGE(%a)%{!A};\n",	/* stmt: GEI2(ac,iarg) */
/* 276 */	"\t%0%[1b]_CMPWU(%1);_BLT(%a)%{!A};\n",	/* stmt: LTU2(ac,iarg) */
/* 277 */	"\t%0%[1b]_CMPWU(%1);_BLE(%a)%{!A};\n",	/* stmt: LEU2(ac,iarg) */
/* 278 */	"\t%0%[1b]_CMPWU(%1);_BGT(%a)%{!A};\n",	/* stmt: GTU2(ac,iarg) */
/* 279 */	"\t%0%[1b]_CMPWU(%1);_BGE(%a)%{!A};\n",	/* stmt: GEU2(ac,iarg) */
/* 280 */	"\t%1%[0b]_CMPWS(%0);_BGT(%a)%{!A};\n",	/* stmt: LTI2(iarg,ac) */
/* 281 */	"\t%1%[0b]_CMPWS(%0);_BGE(%a)%{!A};\n",	/* stmt: LEI2(iarg,ac) */
/* 282 */	"\t%1%[0b]_CMPWS(%0);_BLT(%a)%{!A};\n",	/* stmt: GTI2(iarg,ac) */
/* 283 */	"\t%1%[0b]_CMPWS(%0);_BLE(%a)%{!A};\n",	/* stmt: GEI2(iarg,ac) */
/* 284 */	"\t%1%[0b]_CMPWU(%0);_BGT(%a)%{!A};\n",	/* stmt: LTU2(iarg,ac) */
/* 285 */	"\t%1%[0b]_CMPWU(%0);_BGE(%a)%{!A};\n",	/* stmt: LEU2(iarg,ac) */
/* 286 */	"\t%1%[0b]_CMPWU(%0);_BLT(%a)%{!A};\n",	/* stmt: GTU2(iarg,ac) */
/* 287 */	"\t%1%[0b]_CMPWU(%0);_BLE(%a)%{!A};\n",	/* stmt: GEU2(iarg,ac) */
/* 288 */	"%0",	/* vdst: addr */
/* 289 */	"[SP,%0]",	/* vdst: lddr */
/* 290 */	"[vAC]|%0",	/* vdst: eac */
/* 291 */	"%0",	/* asrc: addr */
/* 292 */	"[SP,%0]",	/* asrc: lddr */
/* 293 */	"[vAC]|%0",	/* asrc: eac */
/* 294 */	"%0",	/* lsrc: addr */
/* 295 */	"\t_SP(%c)%{!A};STW(T2);%[0b]_MOVM(%0,[T2],%a,%b)%{!A};\n",	/* asgn: ARGB(INDIRB(asrc)) */
/* 296 */	"\t%[1b]_MOVM(%1,%0,%a,%b)%{!A};\n",	/* asgn: ASGNB(addr,INDIRB(asrc)) */
/* 297 */	"\t%0STW(T2);%[1b]_MOVM(%1,[T2],%a,%b)%{!A};\n",	/* asgn: ASGNB(ac,INDIRB(asrc)) */
/* 298 */	"\t_MOVM(%1,[SP,%0],%a,%b)%{!A};\n",	/* asgn: ASGNB(lddr,INDIRB(lsrc)) */
/* 299 */	"\t%0\n",	/* stmt: lac */
/* 300 */	"LDI(%0)%{!A};",	/* larg: regx */
/* 301 */	"%0",	/* larg: INDIRI4(eac) */
/* 302 */	"%0",	/* larg: INDIRU4(eac) */
/* 303 */	"\t%{=LAC}%0%{?c==LAC::_MOVL(LAC,%c);}%{!5}\n",	/* reg: lac */
/* 304 */	"\t%0_MOVL([vAC],%c)%{!A};\n",	/* reg: INDIRI4(ac) */
/* 305 */	"\t%0_MOVL([vAC],%c)%{!A};\n",	/* reg: INDIRU4(ac) */
/* 306 */	"\t_MOVL([SP,%0],%c)%{!A};\n",	/* reg: INDIRI4(lddr) */
/* 307 */	"\t_MOVL([SP,%0],%c)%{!A};\n",	/* reg: INDIRU4(lddr) */
/* 308 */	"\t_MOVL(%0,%c)%{!A};\n",	/* reg: INDIRI4(addr) */
/* 309 */	"\t_MOVL(%0,%c)%{!A};\n",	/* reg: INDIRU4(addr) */
/* 310 */	"%{=%0}%{?0=~LAC::_MOVL(%0,LAC);}%{!5}",	/* lac: reg */
/* 311 */	"%{?*0=~LAC::%0_MOVL([vAC],LAC)%{!A};}",	/* lac: INDIRI4(ac) */
/* 312 */	"%{?*0=~LAC::%0_MOVL([vAC],LAC)%{!A};}",	/* lac: INDIRU4(ac) */
/* 313 */	"%{?*0=~LAC::_MOVL([SP,%0],LAC)%{!A};}",	/* lac: INDIRU4(lddr) */
/* 314 */	"%{?*0=~LAC::_MOVL([SP,%0],LAC)%{!A};}",	/* lac: INDIRU4(lddr) */
/* 315 */	"%{?*0=~LAC::_MOVL(%0,LAC)%{!A};}",	/* lac: INDIRI4(addr) */
/* 316 */	"%{?*0=~LAC::_MOVL(%0,LAC)%{!A};}",	/* lac: INDIRU4(addr) */
/* 317 */	"%0%1_LADD()%{!5};",	/* lac: ADDI4(lac,larg) */
/* 318 */	"%0%1_LADD()%{!5};",	/* lac: ADDU4(lac,larg) */
/* 319 */	"%1%0_LADD()%{!5};",	/* lac: ADDI4(larg,lac) */
/* 320 */	"%1%0_LADD()%{!5};",	/* lac: ADDU4(larg,lac) */
/* 321 */	"%0%1_LSUB()%{!5};",	/* lac: SUBI4(lac,larg) */
/* 322 */	"%0%1_LSUB()%{!5};",	/* lac: SUBU4(lac,larg) */
/* 323 */	"%0%1_LMUL()%{!A};",	/* lac: MULI4(lac,larg) */
/* 324 */	"%0%1_LMUL()%{!A};",	/* lac: MULU4(lac,larg) */
/* 325 */	"%1%0_LMUL()%{!A};",	/* lac: MULI4(larg,lac) */
/* 326 */	"%1%0_LMUL()%{!A};",	/* lac: MULU4(larg,lac) */
/* 327 */	"%0%1_LDIVS()%{!A};",	/* lac: DIVI4(lac,larg) */
/* 328 */	"%0%1_LDIVU()%{!A};",	/* lac: DIVU4(lac,larg) */
/* 329 */	"%0%1_LMODS()%{!A};",	/* lac: MODI4(lac,larg) */
/* 330 */	"%0%1_LMODU()%{!A};",	/* lac: MODU4(lac,larg) */
/* 331 */	"%0LDW(%1);_LSHL()%{!A};",	/* lac: LSHI4(lac,reg) */
/* 332 */	"%0LDI(%1);_LSHL()%{!A};",	/* lac: LSHI4(lac,conB) */
/* 333 */	"%0LDW(%1);_LSHL()%{!A};",	/* lac: LSHU4(lac,reg) */
/* 334 */	"%0LDI(%1);_LSHL()%{!A};",	/* lac: LSHU4(lac,conB) */
/* 335 */	"%0LDW(%1);_LSHRS()%{!A};",	/* lac: RSHI4(lac,reg) */
/* 336 */	"%0LDI(%1);_LSHRS()%{!A};",	/* lac: RSHI4(lac,conB) */
/* 337 */	"%0LDW(%1);_LSHRU()%{!A};",	/* lac: RSHU4(lac,reg) */
/* 338 */	"%0LDI(%1);_LSHRU()%{!A};",	/* lac: RSHU4(lac,conB) */
/* 339 */	"%0_LNEG()%{!5};",	/* lac: NEGI4(lac) */
/* 340 */	"%0_LCOM()%{!A};",	/* lac: BCOMU4(lac) */
/* 341 */	"%0%1_LAND()%{!A};",	/* lac: BANDU4(lac,larg) */
/* 342 */	"%1%0_LAND()%{!A};",	/* lac: BANDU4(larg,lac) */
/* 343 */	"%0%1_LOR()%{!A};",	/* lac: BORU4(lac,larg) */
/* 344 */	"%1%0_LOR()%{!A};",	/* lac: BORU4(larg,lac) */
/* 345 */	"%0%1_LXOR()%{!A};",	/* lac: BXORU4(lac,larg) */
/* 346 */	"%1%0_LXOR()%{!A};",	/* lac: BXORU4(larg,lac) */
/* 347 */	"%0_LCOM()%{!A};",	/* lac: BCOMI4(lac) */
/* 348 */	"%0%1_LAND()%{!A};",	/* lac: BANDI4(lac,larg) */
/* 349 */	"%1%0_LAND()%{!A};",	/* lac: BANDI4(larg,lac) */
/* 350 */	"%0%1_LOR()%{!A};",	/* lac: BORI4(lac,larg) */
/* 351 */	"%1%0_LOR()%{!A};",	/* lac: BORI4(larg,lac) */
/* 352 */	"%0%1_LXOR()%{!A};",	/* lac: BXORI4(lac,larg) */
/* 353 */	"%1%0_LXOR()%{!A};",	/* lac: BXORI4(larg,lac) */
/* 354 */	"\t%0%1_LCMPS();_BLT(%a)%{!A};\n",	/* stmt: LTI4(lac,larg) */
/* 355 */	"\t%0%1_LCMPS();_BLE(%a)%{!A};\n",	/* stmt: LEI4(lac,larg) */
/* 356 */	"\t%0%1_LCMPS();_BGT(%a)%{!A};\n",	/* stmt: GTI4(lac,larg) */
/* 357 */	"\t%0%1_LCMPS();_BGE(%a)%{!A};\n",	/* stmt: GEI4(lac,larg) */
/* 358 */	"\t%0%1_LCMPU();_BLT(%a)%{!A};\n",	/* stmt: LTU4(lac,larg) */
/* 359 */	"\t%0%1_LCMPU();_BLE(%a)%{!A};\n",	/* stmt: LEU4(lac,larg) */
/* 360 */	"\t%0%1_LCMPU();_BGT(%a)%{!A};\n",	/* stmt: GTU4(lac,larg) */
/* 361 */	"\t%0%1_LCMPU();_BGE(%a)%{!A};\n",	/* stmt: GEU4(lac,larg) */
/* 362 */	"\t%0%1_LCMPX();_BNE(%a)%{!A};\n",	/* stmt: NEI4(lac,larg) */
/* 363 */	"\t%0%1_LCMPX();_BEQ(%a)%{!A};\n",	/* stmt: EQI4(lac,larg) */
/* 364 */	"\t%0%1_LCMPX();_BNE(%a)%{!A};\n",	/* stmt: NEU4(lac,larg) */
/* 365 */	"\t%0%1_LCMPX();_BEQ(%a)%{!A};\n",	/* stmt: EQU4(lac,larg) */
/* 366 */	"\t%1%0_LCMPS();_BGT(%a)%{!A};\n",	/* stmt: LTI4(larg,lac) */
/* 367 */	"\t%1%0_LCMPS();_BGE(%a)%{!A};\n",	/* stmt: LEI4(larg,lac) */
/* 368 */	"\t%1%0_LCMPS();_BLT(%a)%{!A};\n",	/* stmt: GTI4(larg,lac) */
/* 369 */	"\t%1%0_LCMPS();_BLE(%a)%{!A};\n",	/* stmt: GEI4(larg,lac) */
/* 370 */	"\t%1%0_LCMPU();_BGT(%a)%{!A};\n",	/* stmt: LTU4(larg,lac) */
/* 371 */	"\t%1%0_LCMPU();_BGE(%a)%{!A};\n",	/* stmt: LEU4(larg,lac) */
/* 372 */	"\t%1%0_LCMPU();_BLT(%a)%{!A};\n",	/* stmt: GTU4(larg,lac) */
/* 373 */	"\t%1%0_LCMPU();_BLE(%a)%{!A};\n",	/* stmt: GEU4(larg,lac) */
/* 374 */	"\t%1%0_LCMPX();_BNE(%a)%{!A};\n",	/* stmt: NEI4(larg,lac) */
/* 375 */	"\t%1%0_LCMPX();_BEQ(%a)%{!A};\n",	/* stmt: EQI4(larg,lac) */
/* 376 */	"\t%1%0_LCMPX();_BNE(%a)%{!A};\n",	/* stmt: NEU4(larg,lac) */
/* 377 */	"\t%1%0_LCMPX();_BEQ(%a)%{!A};\n",	/* stmt: EQU4(larg,lac) */
/* 378 */	"\t%{=LAC}%1%[0b]_MOVL(LAC,%0)%{!A};\n",	/* asgn: ASGNI4(vdst,lac) */
/* 379 */	"\t%[0b]_MOVL(%1,%0)%{!A};\n",	/* asgn: ASGNI4(vdst,reg) */
/* 380 */	"\t%[1b]_MOVL(%1,%0)%{!A};\n",	/* asgn: ASGNI4(addr,INDIRI4(asrc)) */
/* 381 */	"\t%0STW(T2);%[1b]_MOVL(%1,[T2])%{!A};\n",	/* asgn: ASGNI4(ac,INDIRI4(asrc)) */
/* 382 */	"\t_MOVL(%1,[SP,%0])%{!A};\n",	/* asgn: ASGNI4(lddr,INDIRI4(lsrc)) */
/* 383 */	"\t%{=LAC}%1%[0b]_MOVL(LAC,%0)%{!A};\n",	/* asgn: ASGNU4(vdst,lac) */
/* 384 */	"\t%[0b]_MOVL(%1,%0)%{!A};\n",	/* asgn: ASGNU4(vdst,reg) */
/* 385 */	"\t%[1b]_MOVL(%1,%0)%{!A};\n",	/* asgn: ASGNU4(addr,INDIRU4(asrc)) */
/* 386 */	"\t%0STW(T2);%[1b]_MOVL(%1,[T2])%{!A};\n",	/* asgn: ASGNU4(ac,INDIRU4(asrc)) */
/* 387 */	"\t_MOVL(%1,[SP,%0])%{!A};\n",	/* asgn: ASGNU4(lddr,INDIRU4(lsrc)) */
/* 388 */	"\t%0\n",	/* stmt: fac */
/* 389 */	"LDI(%0)%{!A};",	/* farg: regx */
/* 390 */	"%0",	/* farg: INDIRF5(eac) */
/* 391 */	"\t%{=FAC}%0%{?c==FAC::_MOVF(FAC,%c);}%{!A}\n",	/* reg: fac */
/* 392 */	"\t%0_MOVF([vAC],%c)%{!A};\n",	/* reg: INDIRF5(ac) */
/* 393 */	"\t_MOVF([SP,%0],%c)%{!A};\n",	/* reg: INDIRF5(lddr) */
/* 394 */	"\t_MOVF(%0,%c)%{!A%c};\n",	/* reg: INDIRF5(addr) */
/* 395 */	"%{=%0}%{?0=~FAC::_MOVF(%0,FAC)%{!A};}",	/* fac: reg */
/* 396 */	"%{?*0=~FAC::%0_MOVF([vAC],FAC)%{!A};}",	/* fac: INDIRF5(ac) */
/* 397 */	"%{?*0=~FAC::_MOVF([SP,%0],FAC)%{!A};}",	/* fac: INDIRF5(lddr) */
/* 398 */	"%{?*0=~FAC::_MOVF(%0,FAC)%{!A};}",	/* fac: INDIRF5(addr) */
/* 399 */	"%0%1_FADD()%{!A};",	/* fac: ADDF5(fac,farg) */
/* 400 */	"%1%0_FADD()%{!A};",	/* fac: ADDF5(farg,fac) */
/* 401 */	"%0%1_FSUB()%{!A};",	/* fac: SUBF5(fac,farg) */
/* 402 */	"%1%0_FSUBR()%{!A};",	/* fac: SUBF5(farg,fac) */
/* 403 */	"%0%1_FMUL()%{!A};",	/* fac: MULF5(fac,farg) */
/* 404 */	"%1%0_FMUL()%{!A};",	/* fac: MULF5(farg,fac) */
/* 405 */	"%0%1_FDIV()%{!A};",	/* fac: DIVF5(fac,farg) */
/* 406 */	"%1%0_FDIVR()%{!A};",	/* fac: DIVF5(farg,fac) */
/* 407 */	"%0_FNEG()%{!A};",	/* fac: NEGF5(fac) */
/* 408 */	"\t%0%1_FCMP();_BEQ(%a)%{!A};\n",	/* stmt: EQF5(fac,farg) */
/* 409 */	"\t%0%1_FCMP();_BNE(%a)%{!A};\n",	/* stmt: NEF5(fac,farg) */
/* 410 */	"\t%0%1_FCMP();_BLT(%a)%{!A};\n",	/* stmt: LTF5(fac,farg) */
/* 411 */	"\t%0%1_FCMP();_BLE(%a)%{!A};\n",	/* stmt: LEF5(fac,farg) */
/* 412 */	"\t%0%1_FCMP();_BGT(%a)%{!A};\n",	/* stmt: GTF5(fac,farg) */
/* 413 */	"\t%0%1_FCMP();_BGE(%a)%{!A};\n",	/* stmt: GEF5(fac,farg) */
/* 414 */	"\t%1%0_FCMP();_BEQ(%a)%{!A};\n",	/* stmt: EQF5(farg,fac) */
/* 415 */	"\t%1%0_FCMP();_BNE(%a)%{!A};\n",	/* stmt: NEF5(farg,fac) */
/* 416 */	"\t%1%0_FCMP();_BGT(%a)%{!A};\n",	/* stmt: LTF5(farg,fac) */
/* 417 */	"\t%1%0_FCMP();_BGE(%a)%{!A};\n",	/* stmt: LEF5(farg,fac) */
/* 418 */	"\t%1%0_FCMP();_BLT(%a)%{!A};\n",	/* stmt: GTF5(farg,fac) */
/* 419 */	"\t%1%0_FCMP();_BLE(%a)%{!A};\n",	/* stmt: GEF5(farg,fac) */
/* 420 */	"\t%{=FAC}%1%[0b]_MOVF(FAC,%0)%{!A};\n",	/* asgn: ASGNF5(vdst,fac) */
/* 421 */	"\t%[0b]_MOVF(%1,%0)%{!A};\n",	/* asgn: ASGNF5(vdst,reg) */
/* 422 */	"\t%[1b]_MOVF(%1,%0)%{!A};\n",	/* asgn: ASGNF5(addr,INDIRF5(asrc)) */
/* 423 */	"\t%0STW(T2);%[1b]_MOVF(%1,[T2])%{!A};\n",	/* asgn: ASGNF5(ac,INDIRF5(asrc)) */
/* 424 */	"\t_MOVF(%1,[SP,%0])%{!A};\n",	/* asgn: ASGNF5(lddr,INDIRF5(lsrc)) */
/* 425 */	"CALLI(%0)%{!ALF};",	/* fac: CALLF5(addr) */
/* 426 */	"CALL(%0)%{!ALF};",	/* fac: CALLF5(reg) */
/* 427 */	"%0CALL(vAC)%{!ALF};",	/* fac: CALLF5(ac) */
/* 428 */	"CALLI(%0)%{!ALF};",	/* lac: CALLI4(addr) */
/* 429 */	"CALL(%0)%{!ALF};",	/* lac: CALLI4(reg) */
/* 430 */	"%0CALL(vAC)%{!ALF};",	/* lac: CALLI4(ac) */
/* 431 */	"CALLI(%0)%{!ALF};",	/* lac: CALLU4(addr) */
/* 432 */	"CALL(%0)%{!ALF};",	/* lac: CALLU4(reg) */
/* 433 */	"%0CALL(vAC)%{!ALF};",	/* lac: CALLU4(ac) */
/* 434 */	"CALLI(%0)%{!ALF};",	/* ac: CALLI2(addr) */
/* 435 */	"CALL(%0)%{!ALF};",	/* ac: CALLI2(reg) */
/* 436 */	"%0CALL(vAC)%{!ALF};",	/* ac: CALLI2(ac) */
/* 437 */	"CALLI(%0)%{!ALF};",	/* ac: CALLU2(addr) */
/* 438 */	"CALL(%0)%{!ALF};",	/* ac: CALLU2(reg) */
/* 439 */	"%0CALL(vAC)%{!ALF};",	/* ac: CALLU2(ac) */
/* 440 */	"CALLI(%0)%{!ALF};",	/* ac: CALLP2(addr) */
/* 441 */	"CALL(%0)%{!ALF};",	/* ac: CALLP2(reg) */
/* 442 */	"%0CALL(vAC)%{!ALF};",	/* ac: CALLP2(ac) */
/* 443 */	"\tCALLI(%0)%{!ALF};\n",	/* stmt: CALLV(addr) */
/* 444 */	"\tCALL(%0)%{!ALF};\n",	/* stmt: CALLV(reg) */
/* 445 */	"\t%0CALL(vAC)%{!ALF};\n",	/* stmt: CALLV(ac) */
/* 446 */	"\t_MOVF(%0,[SP,%c])%{!A};\n",	/* stmt: ARGF5(reg) */
/* 447 */	"\t_MOVL(%0,[SP,%c])%{!A};\n",	/* stmt: ARGI4(reg) */
/* 448 */	"\t_MOVL(%0,[SP,%c])%{!A};\n",	/* stmt: ARGU4(reg) */
/* 449 */	"\t_STLW(%c,src=%0)%{!6};\n",	/* stmt: ARGI2(reg) */
/* 450 */	"\t_STLW(%c,src=%0)%{!6};\n",	/* stmt: ARGU2(reg) */
/* 451 */	"\t_STLW(%c,src=%0)%{!6};\n",	/* stmt: ARGP2(reg) */
/* 452 */	"# arg\n",	/* stmt: ARGF5(reg) */
/* 453 */	"# arg\n",	/* stmt: ARGI4(reg) */
/* 454 */	"# arg\n",	/* stmt: ARGU4(reg) */
/* 455 */	"# arg\n",	/* stmt: ARGI2(reg) */
/* 456 */	"# arg\n",	/* stmt: ARGU2(reg) */
/* 457 */	"# arg\n",	/* stmt: ARGP2(reg) */
/* 458 */	"\t%0\n",	/* stmt: RETF5(fac) */
/* 459 */	"\t%0\n",	/* stmt: RETI4(lac) */
/* 460 */	"\t%0\n",	/* stmt: RETU4(lac) */
/* 461 */	"\t%0\n",	/* stmt: RETI2(ac) */
/* 462 */	"\t%0\n",	/* stmt: RETU2(ac) */
/* 463 */	"\t%0\n",	/* stmt: RETP2(ac) */
/* 464 */	"LD(%0);",	/* eac0: LOADI1(reg) */
/* 465 */	"LD(%0);",	/* eac0: LOADU1(reg) */
/* 466 */	"%{=%0}%0",	/* ac: LOADI1(ac) */
/* 467 */	"%{=%0}%0",	/* ac: LOADU1(ac) */
/* 468 */	"%{=%0}%0",	/* ac0: LOADI1(ac0) */
/* 469 */	"%{=%0}%0",	/* ac0: LOADU1(ac0) */
/* 470 */	"%{=%0}%0",	/* ac: LOADI2(ac) */
/* 471 */	"%{=%0}%0",	/* ac: LOADU2(ac) */
/* 472 */	"%{=%0}%0",	/* ac: LOADP2(ac) */
/* 473 */	"%{=%0}%0",	/* ac0: LOADI2(ac0) */
/* 474 */	"%{=%0}%0",	/* ac0: LOADU2(ac0) */
/* 475 */	"%{=%0}%0",	/* ac0: LOADP2(ac0) */
/* 476 */	"%{=%0}%0",	/* eac: LOADI1(eac) */
/* 477 */	"%{=%0}%0",	/* eac: LOADU1(eac) */
/* 478 */	"%{=%0}%0",	/* eac: LOADI2(eac) */
/* 479 */	"%{=%0}%0",	/* eac: LOADU2(eac) */
/* 480 */	"%{=%0}%0",	/* eac: LOADP2(eac) */
/* 481 */	"%{=%0}%{?0=~vAC::LDW(%0);}",	/* eac: LOADI2(reg) */
/* 482 */	"%{=%0}%{?0=~vAC::LDW(%0);}",	/* eac: LOADU2(reg) */
/* 483 */	"%{=%0}%{?0=~vAC::LDW(%0);}",	/* eac: LOADP2(reg) */
/* 484 */	"%0LDW(LAC);",	/* eac: LOADI2(lac) */
/* 485 */	"%0LDW(LAC);",	/* eac: LOADU2(lac) */
/* 486 */	"%0LDW(LAC);",	/* eac: LOADP2(lac) */
/* 487 */	"%{=%0}%0",	/* lac: LOADI4(lac) */
/* 488 */	"%{=%0}%0",	/* lac: LOADU4(lac) */
/* 489 */	"%{=%0}%0",	/* fac: LOADF5(fac) */
/* 490 */	"\t%{?0=~vAC::LD(%0);}{?c==vAC::ST(%c);}%{!A}\n",	/* reg: LOADI1(reg) */
/* 491 */	"\t%{?0=~vAC::LD(%0);}{?c==vAC::ST(%c);}%{!A}\n",	/* reg: LOADU1(reg) */
/* 492 */	"\t%0%{?c==vAC::ST(%c);}\n",	/* reg: LOADI1(ac) */
/* 493 */	"\t%0%{?c==vAC::ST(%c);}\n",	/* reg: LOADU1(ac) */
/* 494 */	"\t_MOVL(%0,%c)%{!5};\n",	/* reg: LOADI4(reg) */
/* 495 */	"\t_MOVL(%0,%c)%{!5};\n",	/* reg: LOADU4(reg) */
/* 496 */	"\t_MOVF(%0,%c)%{!5};\n",	/* reg: LOADF5(reg) */
/* 497 */	"LD(%0);XORI(128);SUBI(128);",	/* eac: CVII2(reg) */
/* 498 */	"LD(%0);",	/* eac0: CVUI2(reg) */
/* 499 */	"%0XORI(128);SUBI(128);",	/* ac: CVII2(ac0) */
/* 500 */	"%0",	/* ac0: CVUI2(ac0) */
/* 501 */	"%0LD(vACL);XORI(128);SUBI(128);",	/* ac: CVII2(ac) */
/* 502 */	"%0LD(vACL);",	/* ac0: CVUI2(ac) */
/* 503 */	"%0XORI(128);SUBI(128);",	/* eac: CVII2(eac0) */
/* 504 */	"%0",	/* eac0: CVUI2(eac0) */
/* 505 */	"%0LD(vACL);XORI(128);SUBI(128);",	/* eac: CVII2(eac) */
/* 506 */	"%0LD(vACL);",	/* eac0: CVUI2(eac) */
/* 507 */	"%0_STLU(LAC);",	/* lac: CVIU4(ac) */
/* 508 */	"%0_STLU(LAC);",	/* lac: CVII4(ac0) */
/* 509 */	"%0_STLS(LAC);",	/* lac: CVII4(ac) */
/* 510 */	"%0_STLU(LAC);",	/* lac: CVUU4(ac) */
/* 511 */	"%0_STLU(LAC);",	/* lac: CVUI4(ac) */
/* 512 */	"\t%0_STLU(%c)%{!5};\n",	/* reg: CVIU4(ac) */
/* 513 */	"\t%0_STLU(%c)%{!5};\n",	/* reg: CVII4(ac0) */
/* 514 */	"\t%0_STLS(%c)%{!A};\n",	/* reg: CVII4(ac) */
/* 515 */	"\t%0_STLU(%c)%{!5};\n",	/* reg: CVUU4(ac) */
/* 516 */	"\t%0_STLU(%c)%{!5};\n",	/* reg: CVUI4(ac) */
/* 517 */	"%0_FTOU();LDW(LAC)%{!L};",	/* ac: CVFU2(fac) */
/* 518 */	"%0_FTOU();",	/* lac: CVFU4(fac) */
/* 519 */	"%0_STLU(LAC);_FCVU();",	/* fac: CVUF5(ac) */
/* 520 */	"%0_FCVU();",	/* fac: CVUF5(lac) */
/* 521 */	"%0_FTOI();LDW(LAC)%{!L};",	/* ac: CVFI2(fac) */
/* 522 */	"%0_FTOI();",	/* lac: CVFI4(fac) */
/* 523 */	"%0_STLU(LAC);_FCVU();",	/* fac: CVIF5(ac0) */
/* 524 */	"%0_STLS(LAC);_FCVI();",	/* fac: CVIF5(ac) */
/* 525 */	"%0_FCVI();",	/* fac: CVIF5(lac) */
/* 526 */	"\tlabel(%a)%{!ALF};\n",	/* stmt: LABELV */
/* 527 */	"\t_BRA(%0);\n",	/* stmt: JUMPV(addr) */
/* 528 */	"\tCALL(%0)%{!ALF};\n",	/* stmt: JUMPV(reg) */
/* 529 */	"\t%0CALL(vAC)%{!ALF};\n",	/* stmt: JUMPV(ac) */
/* 530 */	"\tSTW(T0);_STLW(%0,src=%1);LDW(T0)  #genspill\n",	/* asgn: ASGNI2(spill,reg) */
/* 531 */	"\tSTW(T0);_STLW(%0,src=%1);LDW(T0)  #genspill\n",	/* asgn: ASGNU2(spill,reg) */
/* 532 */	"\tSTW(T0);_STLW(%0,src=%1);LDW(T0)  #genspill\n",	/* asgn: ASGNP2(spill,reg) */
/* 533 */	"\tSTW(T0);_MOVL(%1,[SP,%0]);LDW(T0) #genspill\n",	/* asgn: ASGNI4(spill,reg) */
/* 534 */	"\tSTW(T0);_MOVL(%1,[SP,%0]);LDW(T0) #genspill\n",	/* asgn: ASGNU4(spill,reg) */
/* 535 */	"\tSTW(T0);_MOVF(%1,[SP,%0]);LDW(T0) #genspill\n",	/* asgn: ASGNF5(spill,reg) */
/* 536 */	"\t%{?0=~vAC:STW(%c):MOVIW(%0,%c)};\n",	/* regx: con */
/* 537 */	"\t%{?0=~vAC:STW(%c):MOVQW(%0,%c)};\n",	/* regx: conB */
/* 538 */	"%0ADDWI(%1);",	/* ac: ADDI2(ac,con) */
/* 539 */	"%0ADDWI(%1);",	/* ac: ADDU2(ac,con) */
/* 540 */	"%0ADDWI(%1);",	/* ac: ADDP2(ac,con) */
/* 541 */	"%0ADDWI(-v(%1));",	/* ac: SUBI2(ac,con) */
/* 542 */	"%0ADDWI(-v(%1));",	/* ac: SUBU2(ac,con) */
/* 543 */	"%0ADDWI(-v(%1));",	/* ac: SUBP2(ac,con) */
/* 544 */	"%0ADDWI(%1);",	/* eac: ADDI2(eac,con) */
/* 545 */	"%0ADDWI(%1);",	/* eac: ADDU2(eac,con) */
/* 546 */	"%0ADDWI(%1);",	/* eac: ADDP2(eac,con) */
/* 547 */	"%0ADDWI(-v(%1));",	/* eac: SUBI2(eac,con) */
/* 548 */	"%0ADDWI(-v(%1));",	/* eac: SUBU2(eac,con) */
/* 549 */	"%0ADDWI(-v(%1));",	/* eac: SUBP2(eac,con) */
/* 550 */	"%1_MULI(%0);",	/* ac: MULI2(con,ac) */
/* 551 */	"%1_MULI(%0);",	/* ac: MULU2(con,ac) */
/* 552 */	"%0LDSB(vACL);",	/* ac: CVII2(ac) */
/* 553 */	"%0NEGV(vAC);",	/* ac: NEGI2(ac) */
/* 554 */	"%1_MULI(%0);",	/* eac: MULI2(conBm,eac) */
/* 555 */	"%1_MULI(%0);",	/* eac: MULU2(conBm,eac) */
/* 556 */	"LDSB(%0);",	/* eac: CVII2(reg) */
/* 557 */	"%0LDSB(vACL);",	/* eac: CVII2(eac) */
/* 558 */	"%0NEGV(vAC);",	/* eac: NEGI2(eac) */
/* 559 */	"%0NEGVL(LAC);",	/* lac: NEGI4(lac) */
/* 560 */	"\t%{=%1}%0%[1b]DOKEA(%1);\n",	/* asgn: ASGNP2(ac,iarg) */
/* 561 */	"\t%{=%1}%0%[1b]DOKEA(%1);\n",	/* asgn: ASGNI2(ac,iarg) */
/* 562 */	"\t%{=%1}%0%[1b]DOKEA(%1);\n",	/* asgn: ASGNU2(ac,iarg) */
/* 563 */	"\t%{=%1}%0%[1b]POKEA(%1);\n",	/* asgn: ASGNI1(ac,iarg) */
/* 564 */	"\t%{=%1}%0%[1b]POKEA(%1);\n",	/* asgn: ASGNU1(ac,iarg) */
/* 565 */	"\t%{=%1}%0%[1b]DOKEI(%1);\n",	/* asgn: ASGNP2(ac,con) */
/* 566 */	"\t%{=%1}%0%[1b]DOKEI(%1);\n",	/* asgn: ASGNI2(ac,con) */
/* 567 */	"\t%{=%1}%0%[1b]DOKEI(%1);\n",	/* asgn: ASGNU2(ac,con) */
/* 568 */	"\t%{=%1}%0%[1b]DOKEQ(%1);\n",	/* asgn: ASGNP2(ac,conB) */
/* 569 */	"\t%{=%1}%0%[1b]DOKEQ(%1);\n",	/* asgn: ASGNI2(ac,conB) */
/* 570 */	"\t%{=%1}%0%[1b]DOKEQ(%1);\n",	/* asgn: ASGNU2(ac,conB) */
/* 571 */	"\t%0%[1b]POKEQ(%1);\n",	/* asgn: ASGNI1(ac,conBs) */
/* 572 */	"\t%0%[1b]POKEQ(%1);\n",	/* asgn: ASGNU1(ac,conB) */
/* 573 */	"%{?*0=~vAC::%{?0=~vAC:DEEK():DEEKV(%0)};}",	/* eac: INDIRI2(reg) */
/* 574 */	"%{?*0=~vAC::%{?0=~vAC:DEEK():DEEKV(%0)};}",	/* eac: INDIRU2(reg) */
/* 575 */	"%{?*0=~vAC::%{?0=~vAC:DEEK():DEEKV(%0)};}",	/* eac: INDIRP2(reg) */
/* 576 */	"%{?0=~vAC:PEEK():PEEKV(%0)};",	/* eac0: INDIRI1(reg) */
/* 577 */	"%{?0=~vAC:PEEK():PEEKV(%0)};",	/* eac0: INDIRU1(reg) */
/* 578 */	"DEEKV(%0);",	/* eac: INDIRI2(INDIRP2(zddr)) */
/* 579 */	"DEEKV(%0);",	/* eac: INDIRU2(INDIRP2(zddr)) */
/* 580 */	"DEEKV(%0);",	/* eac: INDIRP2(INDIRP2(zddr)) */
/* 581 */	"PEEKV(%0);",	/* eac0: INDIRI1(INDIRP2(zddr)) */
/* 582 */	"PEEKV(%0);",	/* eac0: INDIRU1(INDIRP2(zddr)) */
/* 583 */	"\t%{=%0}%{?c==vAC:LDI(%0):MOVQW(%0,%c)};\n",	/* reg: conB */
/* 584 */	"\t%{=%0}%{?c==vAC:LDWI(%0):MOVIW(%0,%c)};\n",	/* reg: con */
/* 585 */	"%{?*0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%0%{?c==vAC:DEEK():DEEKA(%c)};}\n",	/* reg: INDIRI2(ac) */
/* 586 */	"%{?*0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%0%{?c==vAC:DEEK():DEEKA(%c)};}\n",	/* reg: INDIRU2(ac) */
/* 587 */	"%{?*0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%0%{?c==vAC:DEEK():DEEKA(%c)};}\n",	/* reg: INDIRP2(ac) */
/* 588 */	"\t%0%{?c==vAC:PEEK():PEEKA(%c)};\n",	/* reg: INDIRI1(ac) */
/* 589 */	"\t%0%{?c==vAC:PEEK():PEEKA(%c)};\n",	/* reg: INDIRU1(ac) */
/* 590 */	"\t%{?1=~vAC:ST(%0):MOVQB(%1,%0)};\n",	/* asgn: ASGNI1(rmw,conBs) */
/* 591 */	"\t%{?1=~vAC:ST(%0):MOVQB(%1,%0)};\n",	/* asgn: ASGNU1(rmw,conB) */
/* 592 */	"\t%{=%1}%{?1=~vAC:STW(%0):MOVQW(%1,%0)};\n",	/* asgn: ASGNI2(rmw,conB) */
/* 593 */	"\t%{=%1}%{?1=~vAC:STW(%0):MOVQW(%1,%0)};\n",	/* asgn: ASGNU2(rmw,conB) */
/* 594 */	"\t%{=%1}%{?1=~vAC:STW(%0):MOVQW(%1,%0)};\n",	/* asgn: ASGNP2(rmw,conB) */
/* 595 */	"\t%{=%1}%{?1=~vAC:STW(%0):MOVIW(%1,%0)};\n",	/* asgn: ASGNI2(rmw,con) */
/* 596 */	"\t%{=%1}%{?1=~vAC:STW(%0):MOVIW(%1,%0)};\n",	/* asgn: ASGNU2(rmw,con) */
/* 597 */	"\t%{=%1}%{?1=~vAC:STW(%0):MOVIW(%1,%0)};\n",	/* asgn: ASGNP2(rmw,con) */
/* 598 */	"\t%{?0=~vAC:ST(%c):MOVQB(%0,%c)};\n",	/* regx: LOADI1(conBs) */
/* 599 */	"\t%{?0=~vAC:ST(%c):MOVQB(%0,%c)};\n",	/* regx: LOADU1(conB) */
/* 600 */	"\t%{=%0}%{?0=~vAC:STW(%c):MOVQW(%0,%c)};\n",	/* regx: LOADI2(conB) */
/* 601 */	"\t%{=%0}%{?0=~vAC:STW(%c):MOVQW(%0,%c)};\n",	/* regx: LOADU2(conB) */
/* 602 */	"\t%{=%0}%{?0=~vAC:STW(%c):MOVQW(%0,%c)};\n",	/* regx: LOADP2(conB) */
/* 603 */	"\t%{=%0}%{?0=~vAC:STW(%c):MOVIW(%0,%c)};\n",	/* regx: LOADI2(con) */
/* 604 */	"\t%{=%0}%{?0=~vAC:STW(%c):MOVIW(%0,%c)};\n",	/* regx: LOADU2(con) */
/* 605 */	"\t%{=%0}%{?0=~vAC:STW(%c):MOVIW(%0,%c)};\n",	/* regx: LOADP2(con) */
/* 606 */	"%{=%0}%{?0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%{?c==vAC:LDW(%0):MOVW(%0,%c)};}\n",	/* reg: LOADI2(reg) */
/* 607 */	"%{=%0}%{?0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%{?c==vAC:LDW(%0):MOVW(%0,%c)};}\n",	/* reg: LOADU2(reg) */
/* 608 */	"%{=%0}%{?0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%{?c==vAC:LDW(%0):MOVW(%0,%c)};}\n",	/* reg: LOADP2(reg) */
/* 609 */	"LDXW(%0,%1);",	/* eac: INDIRI2(ADDP2(reg,con)) */
/* 610 */	"LDXW(%0,%1);",	/* eac: INDIRU2(ADDP2(reg,con)) */
/* 611 */	"LDXW(%0,%1);",	/* eac: INDIRP2(ADDP2(reg,con)) */
/* 612 */	"%0LDXW(vAC,%1);",	/* ac: INDIRI2(ADDP2(ac,con)) */
/* 613 */	"%0LDXW(vAC,%1);",	/* ac: INDIRU2(ADDP2(ac,con)) */
/* 614 */	"%0LDXW(vAC,%1);",	/* ac: INDIRP2(ADDP2(ac,con)) */
/* 615 */	"%0LDXW(vAC,%1);",	/* eac: INDIRI2(ADDP2(eac,con)) */
/* 616 */	"%0LDXW(vAC,%1);",	/* eac: INDIRU2(ADDP2(eac,con)) */
/* 617 */	"%0LDXW(vAC,%1);",	/* eac: INDIRP2(ADDP2(eac,con)) */
/* 618 */	"\t%2STXW(%0,%1);\n",	/* asgn: ASGNI2(ADDP2(reg,con),ac) */
/* 619 */	"\t%2STXW(%0,%1);\n",	/* asgn: ASGNU2(ADDP2(reg,con),ac) */
/* 620 */	"\t%2STXW(%0,%1);\n",	/* asgn: ASGNP2(ADDP2(reg,con),ac) */
/* 621 */	"%{?*0=~vAC::_LDLW(%0);}",	/* eac: INDIRI1(lddr) */
/* 622 */	"%{?*0=~vAC::_LDLW(%0);}",	/* eac: INDIRU1(lddr) */
/* 623 */	"%{?*0=~vAC::_LDLW(%0);}",	/* eac: INDIRI2(lddr) */
/* 624 */	"%{?*0=~vAC::_LDLW(%0);}",	/* eac: INDIRU2(lddr) */
/* 625 */	"%{?*0=~vAC::_LDLW(%0);}",	/* eac: INDIRP2(lddr) */
/* 626 */	"\t%{=vAC}%1_STLW(%0);\n",	/* asgn: ASGNI2(lddr,ac) */
/* 627 */	"\t%{=vAC}%1_STLW(%0);\n",	/* asgn: ASGNU2(lddr,ac) */
/* 628 */	"\t%{=vAC}%1_STLW(%0);\n",	/* asgn: ASGNP2(lddr,ac) */
/* 629 */	"\t%{?0=~vAC::LDW(%0);%{!A}}_STLW(%c);\n",	/* stmt: ARGI2(reg) */
/* 630 */	"\t%{?0=~vAC::LDW(%0);%{!A}}_STLW(%c);\n",	/* stmt: ARGU2(reg) */
/* 631 */	"\t%{?0=~vAC::LDW(%0);%{!A}}_STLW(%c);\n",	/* stmt: ARGP2(reg) */
/* 632 */	"%a",	/* rmw: VREGP */
/* 633 */	"%0",	/* rmw: zddr */
/* 634 */	"\tINC(%0);\n",	/* asgn: ASGNU1(rmw,LOADU1(ADDI2(CVUI2(INDIRU1(rmw)),con1))) */
/* 635 */	"\tINC(%0);\n",	/* asgn: ASGNI1(rmw,LOADI1(ADDI2(CVII2(INDIRI1(rmw)),con1))) */
/* 636 */	"\tNEGV(%0);\n",	/* asgn: ASGNI2(rmw,NEGI2(INDIRI2(rmw))) */
/* 637 */	"\tNEGVL(%0);\n",	/* asgn: ASGNI4(rmw,NEGI4(INDIRI4(rmw))) */
/* 638 */	"\tINCV(%0);\n",	/* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),con1)) */
/* 639 */	"\tINCV(%0);\n",	/* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),con1)) */
/* 640 */	"\tINCV(%0);\n",	/* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),con1)) */
/* 641 */	"\t%2ADDV(%0);\n",	/* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),ac)) */
/* 642 */	"\t%2ADDV(%0);\n",	/* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),ac)) */
/* 643 */	"\t%2ADDV(%0);\n",	/* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),ac)) */
/* 644 */	"\t%1ADDV(%0);\n",	/* asgn: ASGNI2(rmw,ADDI2(ac,INDIRI2(rmw))) */
/* 645 */	"\t%1ADDV(%0);\n",	/* asgn: ASGNU2(rmw,ADDU2(ac,INDIRU2(rmw))) */
/* 646 */	"\t%1ADDV(%0);\n",	/* asgn: ASGNP2(rmw,ADDP2(ac,INDIRP2(rmw))) */
/* 647 */	"\t%2SUBV(%0);\n",	/* asgn: ASGNI2(rmw,SUBI2(INDIRI2(rmw),ac)) */
/* 648 */	"\t%2SUBV(%0);\n",	/* asgn: ASGNU2(rmw,SUBU2(INDIRU2(rmw),ac)) */
/* 649 */	"\t%2SUBV(%0);\n",	/* asgn: ASGNP2(rmw,SUBP2(INDIRP2(rmw),ac)) */
/* 650 */	"\t%{?2=~vAC:ADDV(%0):ADDSV(%2,%0)};\n",	/* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),conBa)) */
/* 651 */	"\t%{?2=~vAC:ADDV(%0):ADDSV(%2,%0)};\n",	/* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),conBa)) */
/* 652 */	"\t%{?2=~vAC:ADDV(%0):ADDSV(%2,%0)};\n",	/* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),conBa)) */
/* 653 */	"\t%{?2=~vAC:SUBV(%0):ADDSV(-%2,%0)};\n",	/* asgn: ASGNP2(rmw,SUBP2(INDIRP2(rmw),conBa)) */
/* 654 */	"\t%{?2=~vAC:SUBV(%0):ADDSV(-%2,%0)};\n",	/* asgn: ASGNU2(rmw,SUBU2(INDIRU2(rmw),conBa)) */
/* 655 */	"\t%{?2=~vAC:SUBV(%0):ADDSV(-%2,%0)};\n",	/* asgn: ASGNI2(rmw,SUBI2(INDIRI2(rmw),conBa)) */
/* 656 */	"LD(v(%0)+1);",	/* ac0: RSHU2(reg,con8) */
/* 657 */	"%0LD(vACH);",	/* ac0: RSHU2(ac,con8) */
/* 658 */	"\tLD(v(%0)+1);STW(%c);%{!A}\n",	/* reg: RSHU2(reg,con8) */
/* 659 */	"%0LD(vACH);XORI(128);SUBI(128);",	/* ac: RSHI2(ac,con8) */
/* 660 */	"%0LDSB(vACH);",	/* ac: RSHI2(ac,con8) */
/* 661 */	"\tLD(v(%0)+1);XORI(128);SUBI(128);STW(%c);%{!A}\n",	/* reg: RSHI2(reg,con8) */
/* 662 */	"\tLDSB(v(%0)+1);STW(%c);%{!A}\n",	/* reg: RSHI2(reg,con8) */
/* 663 */	"%0ST(vACH);ORI(255);XORI(255);",	/* ac: LSHI2(ac,con8) */
/* 664 */	"%0ST(vACH);ORI(255);XORI(255);",	/* ac: LSHU2(ac,con8) */
/* 665 */	"%0ST(vACH);MOVQB(0,vAC);",	/* ac: LSHI2(ac,con8) */
/* 666 */	"%0ST(vACH);MOVQB(0,vAC);",	/* ac: LSHU2(ac,con8) */
/* 667 */	"\t%0ST(v(%c)+1);LDI(0);ST(%c);%{!A}\n",	/* reg: LSHI2(ac,con8) */
/* 668 */	"\t%0ST(v(%c)+1);MOVQB(0,%c);\n",	/* reg: LSHI2(ac,con8) */
/* 669 */	"\t%0ST(v(%c)+1);LDI(0);ST(%c);%{!A}\n",	/* reg: LSHU2(ac,con8) */
/* 670 */	"\t%0ST(v(%c)+1);MOVQB(0,%c);\n",	/* reg: LSHU2(ac,con8) */
/* 671 */	"\t%0ST(v(%c)+1);LDI(0);ST(%c);%{!A}\n",	/* reg: LSHU2(CVUI2(LOADU1(ac)),con8) */
/* 672 */	"\t%0ST(v(%c)+1);MOVQB(0,%c);\n",	/* reg: LSHU2(CVUI2(LOADU1(ac)),con8) */
/* 673 */	"%0LD(vACL);",	/* ac0: BANDI2(ac,conFF) */
/* 674 */	"%0LD(vACL);",	/* ac0: BANDU2(ac,conFF) */
/* 675 */	"\t%0STW(%c);\n",	/* reg: BANDI2(ac0,conFF) */
/* 676 */	"\t%0STW(%c);\n",	/* reg: BANDU2(ac0,conFF) */
/* 677 */	"%0ORI(255);XORI(255);",	/* ac: BANDU2(ac,conFF00) */
/* 678 */	"%0ORI(255);XORI(255);",	/* ac: BANDI2(ac,conFF00) */
/* 679 */	"%0MOVQB(0,vAC);",	/* ac: BANDU2(ac,conFF00) */
/* 680 */	"%0MOVQB(0,vAC);",	/* ac: BANDI2(ac,conFF00) */
/* 681 */	"\t%2ST(%c);LD(%0);ST(v(%c)+1);%{!A}\n",	/* regx: ADDU2(LSHU2(CVUI2(LOADU1(iarg)),con8),CVUI2(LOADU1(ac))) */
/* 682 */	"\t%2ST(%c);LD(v(%0)+1);ST(v(%c)+1);%{!A}\n",	/* regx: ADDU2(BANDU2(iarg,conFF00),CVUI2(LOADU1(ac))) */
/* 683 */	"\t%2ST(%c);%0ST(v(%c)+1);%{!A}\n",	/* regx: ADDU2(LSHU2(CVUI2(LOADU1(eac)),con8),CVUI2(LOADU1(ac))) */
/* 684 */	"\t%0ST(v(%c)+1);%2ST(%c);%{!A}\n",	/* regx: ADDU2(LSHU2(CVUI2(LOADU1(ac)),con8),CVUI2(LOADU1(eac))) */
/* 685 */	"\t%2ST(%c);%0LD(vACH);ST(v(%c)+1);%{!A}\n",	/* regx: ADDU2(BANDU2(eac,conFF00),ac0) */
/* 686 */	"\t%0LD(vACH);ST(v(%c)+1);%2ST(%c);%{!A}\n",	/* regx: ADDU2(BANDU2(ac,conFF00),eac0) */
/* 687 */	"\t%0ST(v(%c)+1);LDI(%2);ST(%c);%{!A}\n",	/* regx: ADDU2(LSHU2(ac,con8),conB) */
/* 688 */	"\t%0ST(v(%c)+1);MOVQB(%2,%c);\n",	/* regx: ADDU2(LSHU2(ac,con8),conB) */
/* 689 */	"\t%0LD(vACH);ST(v(%c)+1);LDI(%2);ST(%c);%{!A}\n",	/* regx: ADDU2(BANDU2(ac,conFF00),conB) */
/* 690 */	"\t%0LD(vACH);ST(v(%c)+1);MOVQB(%2,%c);\n",	/* regx: ADDU2(BANDU2(ac,conFF00),conB) */
/* 691 */	"\tLD(v(%0)+1);ST(v(%c)+1);LDI(%2);ST(%c);%{!A}\n",	/* regx: ADDU2(BANDU2(reg,conFF00),conB) */
/* 692 */	"\tLD(v(%0)+1);ST(v(%c)+1);MOVQB(%2,%c);%{!A}\n",	/* regx: ADDU2(BANDU2(reg,conFF00),conB) */
/* 693 */	"\t%0ST(%c);LDI(hi(%1));ST(v(%c)+1);%{!A}\n",	/* regx: ADDU2(ac0,conXX00) */
/* 694 */	"%0ADDHI(hi(%1));",	/* ac: ADDU2(ac0,conXX00) */
/* 695 */	"\t%3ST(%0);\n",	/* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
/* 696 */	"\t%3ST(%0);\n",	/* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
/* 697 */	"\t%3ST(%0);\n",	/* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
/* 698 */	"\t%2LD(vACH);ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),BANDU2(ac,conFF00))) */
/* 699 */	"\t%2LD(vACH);ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),BANDU2(ac,conFF00))) */
/* 700 */	"\t%2LD(vACH);ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),BANDU2(ac,conFF00))) */
/* 701 */	"\tLD(v(%2)+1);ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),BANDU2(regx,conFF00))) */
/* 702 */	"\tLD(v(%2)+1);ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),BANDU2(regx,conFF00))) */
/* 703 */	"\tLD(v(%2)+1);ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),BANDU2(regx,conFF00))) */
/* 704 */	"\t%2ST(v(%0)+1);\n",	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
/* 705 */	"\t%2ST(v(%0)+1);\n",	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
/* 706 */	"\t%2ST(v(%0)+1);\n",	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
/* 707 */	"\tLDI(0);ST(%0);%{!A}\n",	/* asgn: ASGNU2(rmw,BANDU2(INDIRU2(rmw),conFF00)) */
/* 708 */	"\tLDI(0);ST(%0);%{!A}\n",	/* asgn: ASGNI2(rmw,BANDU2(INDIRI2(rmw),conFF00)) */
/* 709 */	"\tLDI(0);ST(%0);%{!A}\n",	/* asgn: ASGNP2(rmw,BANDU2(INDIRP2(rmw),conFF00)) */
/* 710 */	"\tMOVQB(0,%0);\n",	/* asgn: ASGNU2(rmw,BANDU2(INDIRU2(rmw),conFF00)) */
/* 711 */	"\tMOVQB(0,%0);\n",	/* asgn: ASGNI2(rmw,BANDU2(INDIRI2(rmw),conFF00)) */
/* 712 */	"\tMOVQB(0,%0);\n",	/* asgn: ASGNP2(rmw,BANDU2(INDIRP2(rmw),conFF00)) */
/* 713 */	"\tLDI(%3);ST(%0);\n",	/* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),conB)) */
/* 714 */	"\tLDI(%3);ST(%0);\n",	/* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),conB)) */
/* 715 */	"\tLDI(%3);ST(%0);\n",	/* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),conB)) */
/* 716 */	"\tMOVQB(%3,%0);\n",	/* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),conB)) */
/* 717 */	"\tMOVQB(%3,%0);\n",	/* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),conB)) */
/* 718 */	"\tMOVQB(%3,%0);\n",	/* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),conB)) */
/* 719 */	"\tMOVQB(0,v(%0)+1);\n",	/* asgn: ASGNU2(rmw,CVUI2(LOADU1(INDIRU2(rmw)))) */
/* 720 */	"\tMOVQB(0,v(%0)+1);\n",	/* asgn: ASGNI2(rmw,CVUI2(LOADU1(INDIRI2(rmw)))) */
/* 721 */	"\tMOVQB(0,v(%0)+1);\n",	/* asgn: ASGNP2(rmw,CVUI2(LOADU1(INDIRP2(rmw)))) */
/* 722 */	"\tLDI(hi(%2));ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),conXX00)) */
/* 723 */	"\tLDI(hi(%2));ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),conXX00)) */
/* 724 */	"\tLDI(hi(%2));ST(v(%0)+1);%{!A}\n",	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),conXX00)) */
/* 725 */	"\tMOVQB(hi(%2),v(%0)+1);\n",	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),conXX00)) */
/* 726 */	"\tMOVQB(hi(%2),v(%0)+1);\n",	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),conXX00)) */
/* 727 */	"\tMOVQB(hi(%2),v(%0)+1);\n",	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),conXX00)) */
};

static char _isinstruction[] = {
/* 0 */	0,
/* 1 */	1,	/* # read register\n */
/* 2 */	1,	/* # read register\n */
/* 3 */	1,	/* # read register\n */
/* 4 */	1,	/* # read register\n */
/* 5 */	1,	/* # read register\n */
/* 6 */	1,	/* # read register\n */
/* 7 */	1,	/* # read register\n */
/* 8 */	1,	/* # read register\n */
/* 9 */	1,	/* # write register\n */
/* 10 */	1,	/* # write register\n */
/* 11 */	1,	/* # write register\n */
/* 12 */	1,	/* # write register\n */
/* 13 */	1,	/* # write register\n */
/* 14 */	1,	/* # write register\n */
/* 15 */	1,	/* # write register\n */
/* 16 */	1,	/* # write register\n */
/* 17 */	0,	/* %a */
/* 18 */	0,	/* %a */
/* 19 */	0,	/* %a */
/* 20 */	0,	/* %a */
/* 21 */	0,	/* %a */
/* 22 */	0,	/* %a */
/* 23 */	0,	/* %a */
/* 24 */	0,	/* %a */
/* 25 */	0,	/* %a */
/* 26 */	0,	/* %a */
/* 27 */	0,	/* %a */
/* 28 */	0,	/* %a */
/* 29 */	0,	/* %a */
/* 30 */	0,	/* %0 */
/* 31 */	0,	/* %a */
/* 32 */	0,	/* %a */
/* 33 */	0,	/* %a */
/* 34 */	0,	/* %a */
/* 35 */	0,	/* %a */
/* 36 */	0,	/* %a */
/* 37 */	0,	/* %a */
/* 38 */	0,	/* %a */
/* 39 */	0,	/* %a */
/* 40 */	0,	/* %a */
/* 41 */	0,	/* %a */
/* 42 */	0,	/* %a */
/* 43 */	0,	/* %0 */
/* 44 */	0,	/* %a */
/* 45 */	0,	/* %a */
/* 46 */	0,	/* %a */
/* 47 */	0,	/* %a */
/* 48 */	0,	/* %a */
/* 49 */	0,	/* %a */
/* 50 */	0,	/* %a */
/* 51 */	0,	/* %a+%F */
/* 52 */	0,	/* %a+%F */
/* 53 */	0,	/* %a */
/* 54 */	0,	/* %0 */
/* 55 */	0,	/* %0 */
/* 56 */	0,	/* %a */
/* 57 */	0,	/* %0 */
/* 58 */	0,	/*  */
/* 59 */	0,	/* %0 */
/* 60 */	1,	/* \t%0\n */
/* 61 */	0,	/* %{=%0}%0 */
/* 62 */	0,	/* %{=%0}%0 */
/* 63 */	1,	/* \t%{=vAC}%0%{?c==vAC::STW(%c);}\n */
/* 64 */	0,	/* %{=%0}%0 */
/* 65 */	0,	/* %{=%0}%0 */
/* 66 */	0,	/* %{=%0}%0 */
/* 67 */	0,	/* %{=%0}%0 */
/* 68 */	0,	/* %{=%0}%{?0=~vAC::LDW(%0);} */
/* 69 */	0,	/* %{=%0}%{?0=~vAC::_SP(%0);} */
/* 70 */	0,	/* %{=%0}%{?0=~vAC::LDI(%0);} */
/* 71 */	0,	/* %{=%0}%{?0=~vAC::LDWI(%0);} */
/* 72 */	0,	/* %{?*0=~vAC::%0DEEK();} */
/* 73 */	0,	/* %{?*0=~vAC::%0DEEK();} */
/* 74 */	0,	/* %{?*0=~vAC::%0DEEK();} */
/* 75 */	0,	/* %0PEEK(); */
/* 76 */	0,	/* %0PEEK(); */
/* 77 */	0,	/* %{?*0=~vAC::LDW(%0);} */
/* 78 */	0,	/* %{?*0=~vAC::LDW(%0);} */
/* 79 */	0,	/* %{?*0=~vAC::LDW(%0);} */
/* 80 */	0,	/* LD(%0); */
/* 81 */	0,	/* LD(%0); */
/* 82 */	0,	/* %{?*0=~vAC::%0DEEK();} */
/* 83 */	0,	/* %{?*0=~vAC::%0DEEK();} */
/* 84 */	0,	/* %{?*0=~vAC::%0DEEK();} */
/* 85 */	0,	/* %0PEEK(); */
/* 86 */	0,	/* %0PEEK(); */
/* 87 */	0,	/* %0 */
/* 88 */	0,	/* %0 */
/* 89 */	0,	/* %0 */
/* 90 */	0,	/* %0 */
/* 91 */	0,	/* %a+%F */
/* 92 */	0,	/* T1|STW(T0);_LDLW(%0);STW(T1);LDW(T0); */
/* 93 */	0,	/* T1|STW(T0);_LDLW(%0);STW(T1);LDW(T0); */
/* 94 */	0,	/* T1|STW(T0);_LDLW(%0);STW(T1);LDW(T0); */
/* 95 */	0,	/* %0%[1b]ADDW(%1); */
/* 96 */	0,	/* %0%[1b]ADDW(%1); */
/* 97 */	0,	/* %0%[1b]ADDW(%1); */
/* 98 */	0,	/* %1%[0b]ADDW(%0); */
/* 99 */	0,	/* %1%[0b]ADDW(%0); */
/* 100 */	0,	/* %1%[0b]ADDW(%0); */
/* 101 */	0,	/* %2%[0b]ADDW(%0);ADDW(%0); */
/* 102 */	0,	/* %2%[0b]ADDW(%0);ADDW(%0); */
/* 103 */	0,	/* %2%[0b]ADDW(%0);ADDW(%0); */
/* 104 */	0,	/* %2%[0b]ADDW(%0);ADDW(%0); */
/* 105 */	0,	/* %0ADDI(%1); */
/* 106 */	0,	/* %0ADDI(%1); */
/* 107 */	0,	/* %0ADDI(%1); */
/* 108 */	0,	/* %0SUBI(-v(%1)); */
/* 109 */	0,	/* %0SUBI(-v(%1)); */
/* 110 */	0,	/* %0SUBI(-v(%1)); */
/* 111 */	0,	/* %0%[1b]SUBW(%1); */
/* 112 */	0,	/* %0%[1b]SUBW(%1); */
/* 113 */	0,	/* %0%[1b]SUBW(%1); */
/* 114 */	0,	/* %0SUBI(%1); */
/* 115 */	0,	/* %0SUBI(%1); */
/* 116 */	0,	/* %0SUBI(%1); */
/* 117 */	0,	/* %0ADDI(-v(%1)); */
/* 118 */	0,	/* %0ADDI(-v(%1)); */
/* 119 */	0,	/* %0ADDI(-v(%1)); */
/* 120 */	0,	/* %0STW(T3);LDI(0);SUBW(T3); */
/* 121 */	0,	/* LDI(0);SUBW(%0); */
/* 122 */	0,	/* %0LSLW(); */
/* 123 */	0,	/* %0LSLW(); */
/* 124 */	0,	/* %0_SHLI(%1); */
/* 125 */	0,	/* %0_SHLI(%1); */
/* 126 */	0,	/* %0_SHRIS(%1); */
/* 127 */	0,	/* %0_SHRIU(%1); */
/* 128 */	0,	/* %0%[1b]_SHL(%1); */
/* 129 */	0,	/* %0%[1b]_SHRS(%1); */
/* 130 */	0,	/* %0%[1b]_SHL(%1); */
/* 131 */	0,	/* %0%[1b]_SHRU(%1); */
/* 132 */	0,	/* %1%{mul0} */
/* 133 */	0,	/* %1%{mul0} */
/* 134 */	0,	/* %{mul0%1} */
/* 135 */	0,	/* %{mul0%1} */
/* 136 */	0,	/* %1_MULI(%0); */
/* 137 */	0,	/* %0%[1b]_MUL(%1); */
/* 138 */	0,	/* %1%[0b]_MUL(%0); */
/* 139 */	0,	/* %1%{mul0} */
/* 140 */	0,	/* %1_MULI(%0); */
/* 141 */	0,	/* %0%[1b]_MUL(%1); */
/* 142 */	0,	/* %1%[0b]_MUL(%0); */
/* 143 */	0,	/* %0%[1b]_DIVS(%1); */
/* 144 */	0,	/* %0%[1b]_DIVU(%1); */
/* 145 */	0,	/* %0%[1b]_MODS(%1); */
/* 146 */	0,	/* %0%[1b]_MODU(%1); */
/* 147 */	0,	/* %0%[1b]_DIVIS(%1); */
/* 148 */	0,	/* %0%[1b]_DIVIU(%1); */
/* 149 */	0,	/* %0%[1b]_MODIS(%1); */
/* 150 */	0,	/* %0%[1b]_MODIU(%1); */
/* 151 */	0,	/* %0STW(T3);_LDI(-1);XORW(T3); */
/* 152 */	0,	/* %0STW(T3);_LDI(-1);XORW(T3); */
/* 153 */	0,	/* %0%[1b]ANDW(%1); */
/* 154 */	0,	/* %0%[1b]ANDW(%1); */
/* 155 */	0,	/* %1%[0b]ANDW(%0); */
/* 156 */	0,	/* %1%[0b]ANDW(%0); */
/* 157 */	0,	/* %0ANDI(%1); */
/* 158 */	0,	/* %0ANDI(%1); */
/* 159 */	0,	/* %0ANDI(%1); */
/* 160 */	0,	/* %0ANDI(%1); */
/* 161 */	0,	/* %0%[1b]ORW(%1); */
/* 162 */	0,	/* %0%[1b]ORW(%1); */
/* 163 */	0,	/* %1%[0b]ORW(%0); */
/* 164 */	0,	/* %1%[0b]ORW(%0); */
/* 165 */	0,	/* %0ORI(%1); */
/* 166 */	0,	/* %0ORI(%1); */
/* 167 */	0,	/* %0ORI(%1); */
/* 168 */	0,	/* %0ORI(%1); */
/* 169 */	0,	/* %0%[1b]XORW(%1); */
/* 170 */	0,	/* %0%[1b]XORW(%1); */
/* 171 */	0,	/* %1%[0b]XORW(%0); */
/* 172 */	0,	/* %1%[0b]XORW(%0); */
/* 173 */	0,	/* %0XORI(%1); */
/* 174 */	0,	/* %0XORI(%1); */
/* 175 */	0,	/* %0XORI(%1); */
/* 176 */	0,	/* %0XORI(%1); */
/* 177 */	0,	/* %0ADDI(%1); */
/* 178 */	0,	/* %0ADDI(%1); */
/* 179 */	0,	/* %0ADDI(%1); */
/* 180 */	0,	/* %0SUBI(-v(%1)); */
/* 181 */	0,	/* %0SUBI(-v(%1)); */
/* 182 */	0,	/* %0SUBI(-v(%1)); */
/* 183 */	0,	/* %0SUBI(%1); */
/* 184 */	0,	/* %0SUBI(%1); */
/* 185 */	0,	/* %0SUBI(%1); */
/* 186 */	0,	/* %0ADDI(-v(%1)); */
/* 187 */	0,	/* %0ADDI(-v(%1)); */
/* 188 */	0,	/* %0ADDI(-v(%1)); */
/* 189 */	0,	/* %0LSLW(); */
/* 190 */	0,	/* %0LSLW(); */
/* 191 */	0,	/* %0_SHLI(%1); */
/* 192 */	0,	/* %0_SHLI(%1); */
/* 193 */	0,	/* %1%{mul0} */
/* 194 */	0,	/* %1%{mul0} */
/* 195 */	0,	/* %0ANDI(%1); */
/* 196 */	0,	/* %0ANDI(%1); */
/* 197 */	0,	/* %0ANDI(%1); */
/* 198 */	0,	/* %0ANDI(%1); */
/* 199 */	0,	/* %0ORI(%1); */
/* 200 */	0,	/* %0ORI(%1); */
/* 201 */	0,	/* %0ORI(%1); */
/* 202 */	0,	/* %0ORI(%1); */
/* 203 */	0,	/* %0XORI(%1); */
/* 204 */	0,	/* %0XORI(%1); */
/* 205 */	0,	/* %0XORI(%1); */
/* 206 */	0,	/* %0XORI(%1); */
/* 207 */	0,	/* %0%[1b]ADDW(%1); */
/* 208 */	0,	/* %0%[1b]ADDW(%1); */
/* 209 */	0,	/* %0%[1b]ADDW(%1); */
/* 210 */	0,	/* %1%[0b]ADDW(%0); */
/* 211 */	0,	/* %1%[0b]ADDW(%0); */
/* 212 */	0,	/* %1%[0b]ADDW(%0); */
/* 213 */	0,	/* %2%[0b]ADDW(%0);ADDW(%0); */
/* 214 */	0,	/* %2%[0b]ADDW(%0);ADDW(%0); */
/* 215 */	0,	/* %2%[0b]ADDW(%0);ADDW(%0); */
/* 216 */	0,	/* %2%[0b]ADDW(%0);ADDW(%0); */
/* 217 */	0,	/* %0%[1b]SUBW(%1); */
/* 218 */	0,	/* %0%[1b]SUBW(%1); */
/* 219 */	0,	/* %0%[1b]SUBW(%1); */
/* 220 */	0,	/* %0%[1b]ANDW(%1); */
/* 221 */	0,	/* %0%[1b]ANDW(%1); */
/* 222 */	0,	/* %1%[0b]ANDW(%0); */
/* 223 */	0,	/* %1%[0b]ANDW(%0); */
/* 224 */	0,	/* %0%[1b]ORW(%1); */
/* 225 */	0,	/* %0%[1b]ORW(%1); */
/* 226 */	0,	/* %1%[0b]ORW(%0); */
/* 227 */	0,	/* %1%[0b]ORW(%0); */
/* 228 */	0,	/* %0%[1b]XORW(%1); */
/* 229 */	0,	/* %0%[1b]XORW(%1); */
/* 230 */	0,	/* %1%[0b]XORW(%0); */
/* 231 */	0,	/* %1%[0b]XORW(%0); */
/* 232 */	1,	/* \t%{=vAC}%1%{?0==vAC::STW(%0);}\n */
/* 233 */	1,	/* \t%{=vAC}%1%[0b]DOKE(%0);\n */
/* 234 */	1,	/* \t%{=vAC}%1%{?0==vAC::STW(%0);}\n */
/* 235 */	1,	/* \t%{=vAC}%1%[0b]DOKE(%0);\n */
/* 236 */	1,	/* \t%{=vAC}%1%{?0==vAC::STW(%0);}\n */
/* 237 */	1,	/* \t%{=vAC}%1%[0b]DOKE(%0);\n */
/* 238 */	1,	/* \t%1%{?0==vAC::ST(%0);}\n */
/* 239 */	1,	/* \t%1%[0b]POKE(%0);\n */
/* 240 */	1,	/* \t%1%{?0==vAC::ST(%0);}\n */
/* 241 */	1,	/* \t%1%[0b]POKE(%0);\n */
/* 242 */	1,	/* \t%0_BEQ(%a);\n */
/* 243 */	1,	/* \t%0XORI(%1);_BEQ(%a)%{!A};\n */
/* 244 */	1,	/* \t%0%[1b]XORW(%1);_BEQ(%a)%{!A};\n */
/* 245 */	1,	/* \t%1%[0b]XORW(%0);_BEQ(%a)%{!A};\n */
/* 246 */	1,	/* \t%0_BNE(%a);\n */
/* 247 */	1,	/* \t%0XORI(%1);_BNE(%a)%{!A};\n */
/* 248 */	1,	/* \t%0%[1b]XORW(%1);_BNE(%a)%{!A};\n */
/* 249 */	1,	/* \t%1%[0b]XORW(%0);_BNE(%a)%{!A};\n */
/* 250 */	1,	/* \t%0_BEQ(%a);\n */
/* 251 */	1,	/* \t%0XORI(%1);_BEQ(%a)%{!A};\n */
/* 252 */	1,	/* \t%0%[1b]XORW(%1);_BEQ(%a)%{!A};\n */
/* 253 */	1,	/* \t%1%[0b]XORW(%0);_BEQ(%a)%{!A};\n */
/* 254 */	1,	/* \t%0_BNE(%a);\n */
/* 255 */	1,	/* \t%0XORI(%1);_BNE(%a)%{!A};\n */
/* 256 */	1,	/* \t%0%[1b]XORW(%1);_BNE(%a)%{!A};\n */
/* 257 */	1,	/* \t%1%[0b]XORW(%0);_BNE(%a)%{!A};\n */
/* 258 */	1,	/* \t%0_BLT(%a);\n */
/* 259 */	1,	/* \t%0_BLE(%a);\n */
/* 260 */	1,	/* \t%0_BGT(%a);\n */
/* 261 */	1,	/* \t%0_BGE(%a);\n */
/* 262 */	1,	/* \t%0_BNE(%a);\n */
/* 263 */	1,	/* \t%0_BEQ(%a);\n */
/* 264 */	1,	/* \t%0_CMPIS(%1);_BLT(%a)%{!A};\n */
/* 265 */	1,	/* \t%0_CMPIS(%1);_BLE(%a)%{!A};\n */
/* 266 */	1,	/* \t%0_CMPIS(%1);_BGT(%a)%{!A};\n */
/* 267 */	1,	/* \t%0_CMPIS(%1);_BGE(%a)%{!A};\n */
/* 268 */	1,	/* \t%0_CMPIU(%1);_BLT(%a)%{!A};\n */
/* 269 */	1,	/* \t%0_CMPIU(%1);_BLE(%a)%{!A};\n */
/* 270 */	1,	/* \t%0_CMPIU(%1);_BGT(%a)%{!A};\n */
/* 271 */	1,	/* \t%0_CMPIU(%1);_BGE(%a)%{!A};\n */
/* 272 */	1,	/* \t%0%[1b]_CMPWS(%1);_BLT(%a)%{!A};\n */
/* 273 */	1,	/* \t%0%[1b]_CMPWS(%1);_BLE(%a)%{!A};\n */
/* 274 */	1,	/* \t%0%[1b]_CMPWS(%1);_BGT(%a)%{!A};\n */
/* 275 */	1,	/* \t%0%[1b]_CMPWS(%1);_BGE(%a)%{!A};\n */
/* 276 */	1,	/* \t%0%[1b]_CMPWU(%1);_BLT(%a)%{!A};\n */
/* 277 */	1,	/* \t%0%[1b]_CMPWU(%1);_BLE(%a)%{!A};\n */
/* 278 */	1,	/* \t%0%[1b]_CMPWU(%1);_BGT(%a)%{!A};\n */
/* 279 */	1,	/* \t%0%[1b]_CMPWU(%1);_BGE(%a)%{!A};\n */
/* 280 */	1,	/* \t%1%[0b]_CMPWS(%0);_BGT(%a)%{!A};\n */
/* 281 */	1,	/* \t%1%[0b]_CMPWS(%0);_BGE(%a)%{!A};\n */
/* 282 */	1,	/* \t%1%[0b]_CMPWS(%0);_BLT(%a)%{!A};\n */
/* 283 */	1,	/* \t%1%[0b]_CMPWS(%0);_BLE(%a)%{!A};\n */
/* 284 */	1,	/* \t%1%[0b]_CMPWU(%0);_BGT(%a)%{!A};\n */
/* 285 */	1,	/* \t%1%[0b]_CMPWU(%0);_BGE(%a)%{!A};\n */
/* 286 */	1,	/* \t%1%[0b]_CMPWU(%0);_BLT(%a)%{!A};\n */
/* 287 */	1,	/* \t%1%[0b]_CMPWU(%0);_BLE(%a)%{!A};\n */
/* 288 */	0,	/* %0 */
/* 289 */	0,	/* [SP,%0] */
/* 290 */	0,	/* [vAC]|%0 */
/* 291 */	0,	/* %0 */
/* 292 */	0,	/* [SP,%0] */
/* 293 */	0,	/* [vAC]|%0 */
/* 294 */	0,	/* %0 */
/* 295 */	1,	/* \t_SP(%c)%{!A};STW(T2);%[0b]_MOVM(%0,[T2],%a,%b)%{!A};\n */
/* 296 */	1,	/* \t%[1b]_MOVM(%1,%0,%a,%b)%{!A};\n */
/* 297 */	1,	/* \t%0STW(T2);%[1b]_MOVM(%1,[T2],%a,%b)%{!A};\n */
/* 298 */	1,	/* \t_MOVM(%1,[SP,%0],%a,%b)%{!A};\n */
/* 299 */	1,	/* \t%0\n */
/* 300 */	0,	/* LDI(%0)%{!A}; */
/* 301 */	0,	/* %0 */
/* 302 */	0,	/* %0 */
/* 303 */	1,	/* \t%{=LAC}%0%{?c==LAC::_MOVL(LAC,%c);}%{!5}\n */
/* 304 */	1,	/* \t%0_MOVL([vAC],%c)%{!A};\n */
/* 305 */	1,	/* \t%0_MOVL([vAC],%c)%{!A};\n */
/* 306 */	1,	/* \t_MOVL([SP,%0],%c)%{!A};\n */
/* 307 */	1,	/* \t_MOVL([SP,%0],%c)%{!A};\n */
/* 308 */	1,	/* \t_MOVL(%0,%c)%{!A};\n */
/* 309 */	1,	/* \t_MOVL(%0,%c)%{!A};\n */
/* 310 */	0,	/* %{=%0}%{?0=~LAC::_MOVL(%0,LAC);}%{!5} */
/* 311 */	0,	/* %{?*0=~LAC::%0_MOVL([vAC],LAC)%{!A};} */
/* 312 */	0,	/* %{?*0=~LAC::%0_MOVL([vAC],LAC)%{!A};} */
/* 313 */	0,	/* %{?*0=~LAC::_MOVL([SP,%0],LAC)%{!A};} */
/* 314 */	0,	/* %{?*0=~LAC::_MOVL([SP,%0],LAC)%{!A};} */
/* 315 */	0,	/* %{?*0=~LAC::_MOVL(%0,LAC)%{!A};} */
/* 316 */	0,	/* %{?*0=~LAC::_MOVL(%0,LAC)%{!A};} */
/* 317 */	0,	/* %0%1_LADD()%{!5}; */
/* 318 */	0,	/* %0%1_LADD()%{!5}; */
/* 319 */	0,	/* %1%0_LADD()%{!5}; */
/* 320 */	0,	/* %1%0_LADD()%{!5}; */
/* 321 */	0,	/* %0%1_LSUB()%{!5}; */
/* 322 */	0,	/* %0%1_LSUB()%{!5}; */
/* 323 */	0,	/* %0%1_LMUL()%{!A}; */
/* 324 */	0,	/* %0%1_LMUL()%{!A}; */
/* 325 */	0,	/* %1%0_LMUL()%{!A}; */
/* 326 */	0,	/* %1%0_LMUL()%{!A}; */
/* 327 */	0,	/* %0%1_LDIVS()%{!A}; */
/* 328 */	0,	/* %0%1_LDIVU()%{!A}; */
/* 329 */	0,	/* %0%1_LMODS()%{!A}; */
/* 330 */	0,	/* %0%1_LMODU()%{!A}; */
/* 331 */	0,	/* %0LDW(%1);_LSHL()%{!A}; */
/* 332 */	0,	/* %0LDI(%1);_LSHL()%{!A}; */
/* 333 */	0,	/* %0LDW(%1);_LSHL()%{!A}; */
/* 334 */	0,	/* %0LDI(%1);_LSHL()%{!A}; */
/* 335 */	0,	/* %0LDW(%1);_LSHRS()%{!A}; */
/* 336 */	0,	/* %0LDI(%1);_LSHRS()%{!A}; */
/* 337 */	0,	/* %0LDW(%1);_LSHRU()%{!A}; */
/* 338 */	0,	/* %0LDI(%1);_LSHRU()%{!A}; */
/* 339 */	0,	/* %0_LNEG()%{!5}; */
/* 340 */	0,	/* %0_LCOM()%{!A}; */
/* 341 */	0,	/* %0%1_LAND()%{!A}; */
/* 342 */	0,	/* %1%0_LAND()%{!A}; */
/* 343 */	0,	/* %0%1_LOR()%{!A}; */
/* 344 */	0,	/* %1%0_LOR()%{!A}; */
/* 345 */	0,	/* %0%1_LXOR()%{!A}; */
/* 346 */	0,	/* %1%0_LXOR()%{!A}; */
/* 347 */	0,	/* %0_LCOM()%{!A}; */
/* 348 */	0,	/* %0%1_LAND()%{!A}; */
/* 349 */	0,	/* %1%0_LAND()%{!A}; */
/* 350 */	0,	/* %0%1_LOR()%{!A}; */
/* 351 */	0,	/* %1%0_LOR()%{!A}; */
/* 352 */	0,	/* %0%1_LXOR()%{!A}; */
/* 353 */	0,	/* %1%0_LXOR()%{!A}; */
/* 354 */	1,	/* \t%0%1_LCMPS();_BLT(%a)%{!A};\n */
/* 355 */	1,	/* \t%0%1_LCMPS();_BLE(%a)%{!A};\n */
/* 356 */	1,	/* \t%0%1_LCMPS();_BGT(%a)%{!A};\n */
/* 357 */	1,	/* \t%0%1_LCMPS();_BGE(%a)%{!A};\n */
/* 358 */	1,	/* \t%0%1_LCMPU();_BLT(%a)%{!A};\n */
/* 359 */	1,	/* \t%0%1_LCMPU();_BLE(%a)%{!A};\n */
/* 360 */	1,	/* \t%0%1_LCMPU();_BGT(%a)%{!A};\n */
/* 361 */	1,	/* \t%0%1_LCMPU();_BGE(%a)%{!A};\n */
/* 362 */	1,	/* \t%0%1_LCMPX();_BNE(%a)%{!A};\n */
/* 363 */	1,	/* \t%0%1_LCMPX();_BEQ(%a)%{!A};\n */
/* 364 */	1,	/* \t%0%1_LCMPX();_BNE(%a)%{!A};\n */
/* 365 */	1,	/* \t%0%1_LCMPX();_BEQ(%a)%{!A};\n */
/* 366 */	1,	/* \t%1%0_LCMPS();_BGT(%a)%{!A};\n */
/* 367 */	1,	/* \t%1%0_LCMPS();_BGE(%a)%{!A};\n */
/* 368 */	1,	/* \t%1%0_LCMPS();_BLT(%a)%{!A};\n */
/* 369 */	1,	/* \t%1%0_LCMPS();_BLE(%a)%{!A};\n */
/* 370 */	1,	/* \t%1%0_LCMPU();_BGT(%a)%{!A};\n */
/* 371 */	1,	/* \t%1%0_LCMPU();_BGE(%a)%{!A};\n */
/* 372 */	1,	/* \t%1%0_LCMPU();_BLT(%a)%{!A};\n */
/* 373 */	1,	/* \t%1%0_LCMPU();_BLE(%a)%{!A};\n */
/* 374 */	1,	/* \t%1%0_LCMPX();_BNE(%a)%{!A};\n */
/* 375 */	1,	/* \t%1%0_LCMPX();_BEQ(%a)%{!A};\n */
/* 376 */	1,	/* \t%1%0_LCMPX();_BNE(%a)%{!A};\n */
/* 377 */	1,	/* \t%1%0_LCMPX();_BEQ(%a)%{!A};\n */
/* 378 */	1,	/* \t%{=LAC}%1%[0b]_MOVL(LAC,%0)%{!A};\n */
/* 379 */	1,	/* \t%[0b]_MOVL(%1,%0)%{!A};\n */
/* 380 */	1,	/* \t%[1b]_MOVL(%1,%0)%{!A};\n */
/* 381 */	1,	/* \t%0STW(T2);%[1b]_MOVL(%1,[T2])%{!A};\n */
/* 382 */	1,	/* \t_MOVL(%1,[SP,%0])%{!A};\n */
/* 383 */	1,	/* \t%{=LAC}%1%[0b]_MOVL(LAC,%0)%{!A};\n */
/* 384 */	1,	/* \t%[0b]_MOVL(%1,%0)%{!A};\n */
/* 385 */	1,	/* \t%[1b]_MOVL(%1,%0)%{!A};\n */
/* 386 */	1,	/* \t%0STW(T2);%[1b]_MOVL(%1,[T2])%{!A};\n */
/* 387 */	1,	/* \t_MOVL(%1,[SP,%0])%{!A};\n */
/* 388 */	1,	/* \t%0\n */
/* 389 */	0,	/* LDI(%0)%{!A}; */
/* 390 */	0,	/* %0 */
/* 391 */	1,	/* \t%{=FAC}%0%{?c==FAC::_MOVF(FAC,%c);}%{!A}\n */
/* 392 */	1,	/* \t%0_MOVF([vAC],%c)%{!A};\n */
/* 393 */	1,	/* \t_MOVF([SP,%0],%c)%{!A};\n */
/* 394 */	1,	/* \t_MOVF(%0,%c)%{!A%c};\n */
/* 395 */	0,	/* %{=%0}%{?0=~FAC::_MOVF(%0,FAC)%{!A};} */
/* 396 */	0,	/* %{?*0=~FAC::%0_MOVF([vAC],FAC)%{!A};} */
/* 397 */	0,	/* %{?*0=~FAC::_MOVF([SP,%0],FAC)%{!A};} */
/* 398 */	0,	/* %{?*0=~FAC::_MOVF(%0,FAC)%{!A};} */
/* 399 */	0,	/* %0%1_FADD()%{!A}; */
/* 400 */	0,	/* %1%0_FADD()%{!A}; */
/* 401 */	0,	/* %0%1_FSUB()%{!A}; */
/* 402 */	0,	/* %1%0_FSUBR()%{!A}; */
/* 403 */	0,	/* %0%1_FMUL()%{!A}; */
/* 404 */	0,	/* %1%0_FMUL()%{!A}; */
/* 405 */	0,	/* %0%1_FDIV()%{!A}; */
/* 406 */	0,	/* %1%0_FDIVR()%{!A}; */
/* 407 */	0,	/* %0_FNEG()%{!A}; */
/* 408 */	1,	/* \t%0%1_FCMP();_BEQ(%a)%{!A};\n */
/* 409 */	1,	/* \t%0%1_FCMP();_BNE(%a)%{!A};\n */
/* 410 */	1,	/* \t%0%1_FCMP();_BLT(%a)%{!A};\n */
/* 411 */	1,	/* \t%0%1_FCMP();_BLE(%a)%{!A};\n */
/* 412 */	1,	/* \t%0%1_FCMP();_BGT(%a)%{!A};\n */
/* 413 */	1,	/* \t%0%1_FCMP();_BGE(%a)%{!A};\n */
/* 414 */	1,	/* \t%1%0_FCMP();_BEQ(%a)%{!A};\n */
/* 415 */	1,	/* \t%1%0_FCMP();_BNE(%a)%{!A};\n */
/* 416 */	1,	/* \t%1%0_FCMP();_BGT(%a)%{!A};\n */
/* 417 */	1,	/* \t%1%0_FCMP();_BGE(%a)%{!A};\n */
/* 418 */	1,	/* \t%1%0_FCMP();_BLT(%a)%{!A};\n */
/* 419 */	1,	/* \t%1%0_FCMP();_BLE(%a)%{!A};\n */
/* 420 */	1,	/* \t%{=FAC}%1%[0b]_MOVF(FAC,%0)%{!A};\n */
/* 421 */	1,	/* \t%[0b]_MOVF(%1,%0)%{!A};\n */
/* 422 */	1,	/* \t%[1b]_MOVF(%1,%0)%{!A};\n */
/* 423 */	1,	/* \t%0STW(T2);%[1b]_MOVF(%1,[T2])%{!A};\n */
/* 424 */	1,	/* \t_MOVF(%1,[SP,%0])%{!A};\n */
/* 425 */	0,	/* CALLI(%0)%{!ALF}; */
/* 426 */	0,	/* CALL(%0)%{!ALF}; */
/* 427 */	0,	/* %0CALL(vAC)%{!ALF}; */
/* 428 */	0,	/* CALLI(%0)%{!ALF}; */
/* 429 */	0,	/* CALL(%0)%{!ALF}; */
/* 430 */	0,	/* %0CALL(vAC)%{!ALF}; */
/* 431 */	0,	/* CALLI(%0)%{!ALF}; */
/* 432 */	0,	/* CALL(%0)%{!ALF}; */
/* 433 */	0,	/* %0CALL(vAC)%{!ALF}; */
/* 434 */	0,	/* CALLI(%0)%{!ALF}; */
/* 435 */	0,	/* CALL(%0)%{!ALF}; */
/* 436 */	0,	/* %0CALL(vAC)%{!ALF}; */
/* 437 */	0,	/* CALLI(%0)%{!ALF}; */
/* 438 */	0,	/* CALL(%0)%{!ALF}; */
/* 439 */	0,	/* %0CALL(vAC)%{!ALF}; */
/* 440 */	0,	/* CALLI(%0)%{!ALF}; */
/* 441 */	0,	/* CALL(%0)%{!ALF}; */
/* 442 */	0,	/* %0CALL(vAC)%{!ALF}; */
/* 443 */	1,	/* \tCALLI(%0)%{!ALF};\n */
/* 444 */	1,	/* \tCALL(%0)%{!ALF};\n */
/* 445 */	1,	/* \t%0CALL(vAC)%{!ALF};\n */
/* 446 */	1,	/* \t_MOVF(%0,[SP,%c])%{!A};\n */
/* 447 */	1,	/* \t_MOVL(%0,[SP,%c])%{!A};\n */
/* 448 */	1,	/* \t_MOVL(%0,[SP,%c])%{!A};\n */
/* 449 */	1,	/* \t_STLW(%c,src=%0)%{!6};\n */
/* 450 */	1,	/* \t_STLW(%c,src=%0)%{!6};\n */
/* 451 */	1,	/* \t_STLW(%c,src=%0)%{!6};\n */
/* 452 */	1,	/* # arg\n */
/* 453 */	1,	/* # arg\n */
/* 454 */	1,	/* # arg\n */
/* 455 */	1,	/* # arg\n */
/* 456 */	1,	/* # arg\n */
/* 457 */	1,	/* # arg\n */
/* 458 */	1,	/* \t%0\n */
/* 459 */	1,	/* \t%0\n */
/* 460 */	1,	/* \t%0\n */
/* 461 */	1,	/* \t%0\n */
/* 462 */	1,	/* \t%0\n */
/* 463 */	1,	/* \t%0\n */
/* 464 */	0,	/* LD(%0); */
/* 465 */	0,	/* LD(%0); */
/* 466 */	0,	/* %{=%0}%0 */
/* 467 */	0,	/* %{=%0}%0 */
/* 468 */	0,	/* %{=%0}%0 */
/* 469 */	0,	/* %{=%0}%0 */
/* 470 */	0,	/* %{=%0}%0 */
/* 471 */	0,	/* %{=%0}%0 */
/* 472 */	0,	/* %{=%0}%0 */
/* 473 */	0,	/* %{=%0}%0 */
/* 474 */	0,	/* %{=%0}%0 */
/* 475 */	0,	/* %{=%0}%0 */
/* 476 */	0,	/* %{=%0}%0 */
/* 477 */	0,	/* %{=%0}%0 */
/* 478 */	0,	/* %{=%0}%0 */
/* 479 */	0,	/* %{=%0}%0 */
/* 480 */	0,	/* %{=%0}%0 */
/* 481 */	0,	/* %{=%0}%{?0=~vAC::LDW(%0);} */
/* 482 */	0,	/* %{=%0}%{?0=~vAC::LDW(%0);} */
/* 483 */	0,	/* %{=%0}%{?0=~vAC::LDW(%0);} */
/* 484 */	0,	/* %0LDW(LAC); */
/* 485 */	0,	/* %0LDW(LAC); */
/* 486 */	0,	/* %0LDW(LAC); */
/* 487 */	0,	/* %{=%0}%0 */
/* 488 */	0,	/* %{=%0}%0 */
/* 489 */	0,	/* %{=%0}%0 */
/* 490 */	1,	/* \t%{?0=~vAC::LD(%0);}{?c==vAC::ST(%c);}%{!A}\n */
/* 491 */	1,	/* \t%{?0=~vAC::LD(%0);}{?c==vAC::ST(%c);}%{!A}\n */
/* 492 */	1,	/* \t%0%{?c==vAC::ST(%c);}\n */
/* 493 */	1,	/* \t%0%{?c==vAC::ST(%c);}\n */
/* 494 */	1,	/* \t_MOVL(%0,%c)%{!5};\n */
/* 495 */	1,	/* \t_MOVL(%0,%c)%{!5};\n */
/* 496 */	1,	/* \t_MOVF(%0,%c)%{!5};\n */
/* 497 */	0,	/* LD(%0);XORI(128);SUBI(128); */
/* 498 */	0,	/* LD(%0); */
/* 499 */	0,	/* %0XORI(128);SUBI(128); */
/* 500 */	0,	/* %0 */
/* 501 */	0,	/* %0LD(vACL);XORI(128);SUBI(128); */
/* 502 */	0,	/* %0LD(vACL); */
/* 503 */	0,	/* %0XORI(128);SUBI(128); */
/* 504 */	0,	/* %0 */
/* 505 */	0,	/* %0LD(vACL);XORI(128);SUBI(128); */
/* 506 */	0,	/* %0LD(vACL); */
/* 507 */	0,	/* %0_STLU(LAC); */
/* 508 */	0,	/* %0_STLU(LAC); */
/* 509 */	0,	/* %0_STLS(LAC); */
/* 510 */	0,	/* %0_STLU(LAC); */
/* 511 */	0,	/* %0_STLU(LAC); */
/* 512 */	1,	/* \t%0_STLU(%c)%{!5};\n */
/* 513 */	1,	/* \t%0_STLU(%c)%{!5};\n */
/* 514 */	1,	/* \t%0_STLS(%c)%{!A};\n */
/* 515 */	1,	/* \t%0_STLU(%c)%{!5};\n */
/* 516 */	1,	/* \t%0_STLU(%c)%{!5};\n */
/* 517 */	0,	/* %0_FTOU();LDW(LAC)%{!L}; */
/* 518 */	0,	/* %0_FTOU(); */
/* 519 */	0,	/* %0_STLU(LAC);_FCVU(); */
/* 520 */	0,	/* %0_FCVU(); */
/* 521 */	0,	/* %0_FTOI();LDW(LAC)%{!L}; */
/* 522 */	0,	/* %0_FTOI(); */
/* 523 */	0,	/* %0_STLU(LAC);_FCVU(); */
/* 524 */	0,	/* %0_STLS(LAC);_FCVI(); */
/* 525 */	0,	/* %0_FCVI(); */
/* 526 */	1,	/* \tlabel(%a)%{!ALF};\n */
/* 527 */	1,	/* \t_BRA(%0);\n */
/* 528 */	1,	/* \tCALL(%0)%{!ALF};\n */
/* 529 */	1,	/* \t%0CALL(vAC)%{!ALF};\n */
/* 530 */	1,	/* \tSTW(T0);_STLW(%0,src=%1);LDW(T0)  #genspill\n */
/* 531 */	1,	/* \tSTW(T0);_STLW(%0,src=%1);LDW(T0)  #genspill\n */
/* 532 */	1,	/* \tSTW(T0);_STLW(%0,src=%1);LDW(T0)  #genspill\n */
/* 533 */	1,	/* \tSTW(T0);_MOVL(%1,[SP,%0]);LDW(T0) #genspill\n */
/* 534 */	1,	/* \tSTW(T0);_MOVL(%1,[SP,%0]);LDW(T0) #genspill\n */
/* 535 */	1,	/* \tSTW(T0);_MOVF(%1,[SP,%0]);LDW(T0) #genspill\n */
/* 536 */	1,	/* \t%{?0=~vAC:STW(%c):MOVIW(%0,%c)};\n */
/* 537 */	1,	/* \t%{?0=~vAC:STW(%c):MOVQW(%0,%c)};\n */
/* 538 */	0,	/* %0ADDWI(%1); */
/* 539 */	0,	/* %0ADDWI(%1); */
/* 540 */	0,	/* %0ADDWI(%1); */
/* 541 */	0,	/* %0ADDWI(-v(%1)); */
/* 542 */	0,	/* %0ADDWI(-v(%1)); */
/* 543 */	0,	/* %0ADDWI(-v(%1)); */
/* 544 */	0,	/* %0ADDWI(%1); */
/* 545 */	0,	/* %0ADDWI(%1); */
/* 546 */	0,	/* %0ADDWI(%1); */
/* 547 */	0,	/* %0ADDWI(-v(%1)); */
/* 548 */	0,	/* %0ADDWI(-v(%1)); */
/* 549 */	0,	/* %0ADDWI(-v(%1)); */
/* 550 */	0,	/* %1_MULI(%0); */
/* 551 */	0,	/* %1_MULI(%0); */
/* 552 */	0,	/* %0LDSB(vACL); */
/* 553 */	0,	/* %0NEGV(vAC); */
/* 554 */	0,	/* %1_MULI(%0); */
/* 555 */	0,	/* %1_MULI(%0); */
/* 556 */	0,	/* LDSB(%0); */
/* 557 */	0,	/* %0LDSB(vACL); */
/* 558 */	0,	/* %0NEGV(vAC); */
/* 559 */	0,	/* %0NEGVL(LAC); */
/* 560 */	1,	/* \t%{=%1}%0%[1b]DOKEA(%1);\n */
/* 561 */	1,	/* \t%{=%1}%0%[1b]DOKEA(%1);\n */
/* 562 */	1,	/* \t%{=%1}%0%[1b]DOKEA(%1);\n */
/* 563 */	1,	/* \t%{=%1}%0%[1b]POKEA(%1);\n */
/* 564 */	1,	/* \t%{=%1}%0%[1b]POKEA(%1);\n */
/* 565 */	1,	/* \t%{=%1}%0%[1b]DOKEI(%1);\n */
/* 566 */	1,	/* \t%{=%1}%0%[1b]DOKEI(%1);\n */
/* 567 */	1,	/* \t%{=%1}%0%[1b]DOKEI(%1);\n */
/* 568 */	1,	/* \t%{=%1}%0%[1b]DOKEQ(%1);\n */
/* 569 */	1,	/* \t%{=%1}%0%[1b]DOKEQ(%1);\n */
/* 570 */	1,	/* \t%{=%1}%0%[1b]DOKEQ(%1);\n */
/* 571 */	1,	/* \t%0%[1b]POKEQ(%1);\n */
/* 572 */	1,	/* \t%0%[1b]POKEQ(%1);\n */
/* 573 */	0,	/* %{?*0=~vAC::%{?0=~vAC:DEEK():DEEKV(%0)};} */
/* 574 */	0,	/* %{?*0=~vAC::%{?0=~vAC:DEEK():DEEKV(%0)};} */
/* 575 */	0,	/* %{?*0=~vAC::%{?0=~vAC:DEEK():DEEKV(%0)};} */
/* 576 */	0,	/* %{?0=~vAC:PEEK():PEEKV(%0)}; */
/* 577 */	0,	/* %{?0=~vAC:PEEK():PEEKV(%0)}; */
/* 578 */	0,	/* DEEKV(%0); */
/* 579 */	0,	/* DEEKV(%0); */
/* 580 */	0,	/* DEEKV(%0); */
/* 581 */	0,	/* PEEKV(%0); */
/* 582 */	0,	/* PEEKV(%0); */
/* 583 */	1,	/* \t%{=%0}%{?c==vAC:LDI(%0):MOVQW(%0,%c)};\n */
/* 584 */	1,	/* \t%{=%0}%{?c==vAC:LDWI(%0):MOVIW(%0,%c)};\n */
/* 585 */	1,	/* %{?*0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%0%{?c==vAC:DEEK():DEEKA(%c)};}\n */
/* 586 */	1,	/* %{?*0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%0%{?c==vAC:DEEK():DEEKA(%c)};}\n */
/* 587 */	1,	/* %{?*0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%0%{?c==vAC:DEEK():DEEKA(%c)};}\n */
/* 588 */	1,	/* \t%0%{?c==vAC:PEEK():PEEKA(%c)};\n */
/* 589 */	1,	/* \t%0%{?c==vAC:PEEK():PEEKA(%c)};\n */
/* 590 */	1,	/* \t%{?1=~vAC:ST(%0):MOVQB(%1,%0)};\n */
/* 591 */	1,	/* \t%{?1=~vAC:ST(%0):MOVQB(%1,%0)};\n */
/* 592 */	1,	/* \t%{=%1}%{?1=~vAC:STW(%0):MOVQW(%1,%0)};\n */
/* 593 */	1,	/* \t%{=%1}%{?1=~vAC:STW(%0):MOVQW(%1,%0)};\n */
/* 594 */	1,	/* \t%{=%1}%{?1=~vAC:STW(%0):MOVQW(%1,%0)};\n */
/* 595 */	1,	/* \t%{=%1}%{?1=~vAC:STW(%0):MOVIW(%1,%0)};\n */
/* 596 */	1,	/* \t%{=%1}%{?1=~vAC:STW(%0):MOVIW(%1,%0)};\n */
/* 597 */	1,	/* \t%{=%1}%{?1=~vAC:STW(%0):MOVIW(%1,%0)};\n */
/* 598 */	1,	/* \t%{?0=~vAC:ST(%c):MOVQB(%0,%c)};\n */
/* 599 */	1,	/* \t%{?0=~vAC:ST(%c):MOVQB(%0,%c)};\n */
/* 600 */	1,	/* \t%{=%0}%{?0=~vAC:STW(%c):MOVQW(%0,%c)};\n */
/* 601 */	1,	/* \t%{=%0}%{?0=~vAC:STW(%c):MOVQW(%0,%c)};\n */
/* 602 */	1,	/* \t%{=%0}%{?0=~vAC:STW(%c):MOVQW(%0,%c)};\n */
/* 603 */	1,	/* \t%{=%0}%{?0=~vAC:STW(%c):MOVIW(%0,%c)};\n */
/* 604 */	1,	/* \t%{=%0}%{?0=~vAC:STW(%c):MOVIW(%0,%c)};\n */
/* 605 */	1,	/* \t%{=%0}%{?0=~vAC:STW(%c):MOVIW(%0,%c)};\n */
/* 606 */	1,	/* %{=%0}%{?0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%{?c==vAC:LDW(%0):MOVW(%0,%c)};}\n */
/* 607 */	1,	/* %{=%0}%{?0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%{?c==vAC:LDW(%0):MOVW(%0,%c)};}\n */
/* 608 */	1,	/* %{=%0}%{?0=~vAC:%{?c==vAC::\tSTW(%c);}:\t%{?c==vAC:LDW(%0):MOVW(%0,%c)};}\n */
/* 609 */	0,	/* LDXW(%0,%1); */
/* 610 */	0,	/* LDXW(%0,%1); */
/* 611 */	0,	/* LDXW(%0,%1); */
/* 612 */	0,	/* %0LDXW(vAC,%1); */
/* 613 */	0,	/* %0LDXW(vAC,%1); */
/* 614 */	0,	/* %0LDXW(vAC,%1); */
/* 615 */	0,	/* %0LDXW(vAC,%1); */
/* 616 */	0,	/* %0LDXW(vAC,%1); */
/* 617 */	0,	/* %0LDXW(vAC,%1); */
/* 618 */	1,	/* \t%2STXW(%0,%1);\n */
/* 619 */	1,	/* \t%2STXW(%0,%1);\n */
/* 620 */	1,	/* \t%2STXW(%0,%1);\n */
/* 621 */	0,	/* %{?*0=~vAC::_LDLW(%0);} */
/* 622 */	0,	/* %{?*0=~vAC::_LDLW(%0);} */
/* 623 */	0,	/* %{?*0=~vAC::_LDLW(%0);} */
/* 624 */	0,	/* %{?*0=~vAC::_LDLW(%0);} */
/* 625 */	0,	/* %{?*0=~vAC::_LDLW(%0);} */
/* 626 */	1,	/* \t%{=vAC}%1_STLW(%0);\n */
/* 627 */	1,	/* \t%{=vAC}%1_STLW(%0);\n */
/* 628 */	1,	/* \t%{=vAC}%1_STLW(%0);\n */
/* 629 */	1,	/* \t%{?0=~vAC::LDW(%0);%{!A}}_STLW(%c);\n */
/* 630 */	1,	/* \t%{?0=~vAC::LDW(%0);%{!A}}_STLW(%c);\n */
/* 631 */	1,	/* \t%{?0=~vAC::LDW(%0);%{!A}}_STLW(%c);\n */
/* 632 */	0,	/* %a */
/* 633 */	0,	/* %0 */
/* 634 */	1,	/* \tINC(%0);\n */
/* 635 */	1,	/* \tINC(%0);\n */
/* 636 */	1,	/* \tNEGV(%0);\n */
/* 637 */	1,	/* \tNEGVL(%0);\n */
/* 638 */	1,	/* \tINCV(%0);\n */
/* 639 */	1,	/* \tINCV(%0);\n */
/* 640 */	1,	/* \tINCV(%0);\n */
/* 641 */	1,	/* \t%2ADDV(%0);\n */
/* 642 */	1,	/* \t%2ADDV(%0);\n */
/* 643 */	1,	/* \t%2ADDV(%0);\n */
/* 644 */	1,	/* \t%1ADDV(%0);\n */
/* 645 */	1,	/* \t%1ADDV(%0);\n */
/* 646 */	1,	/* \t%1ADDV(%0);\n */
/* 647 */	1,	/* \t%2SUBV(%0);\n */
/* 648 */	1,	/* \t%2SUBV(%0);\n */
/* 649 */	1,	/* \t%2SUBV(%0);\n */
/* 650 */	1,	/* \t%{?2=~vAC:ADDV(%0):ADDSV(%2,%0)};\n */
/* 651 */	1,	/* \t%{?2=~vAC:ADDV(%0):ADDSV(%2,%0)};\n */
/* 652 */	1,	/* \t%{?2=~vAC:ADDV(%0):ADDSV(%2,%0)};\n */
/* 653 */	1,	/* \t%{?2=~vAC:SUBV(%0):ADDSV(-%2,%0)};\n */
/* 654 */	1,	/* \t%{?2=~vAC:SUBV(%0):ADDSV(-%2,%0)};\n */
/* 655 */	1,	/* \t%{?2=~vAC:SUBV(%0):ADDSV(-%2,%0)};\n */
/* 656 */	0,	/* LD(v(%0)+1); */
/* 657 */	0,	/* %0LD(vACH); */
/* 658 */	1,	/* \tLD(v(%0)+1);STW(%c);%{!A}\n */
/* 659 */	0,	/* %0LD(vACH);XORI(128);SUBI(128); */
/* 660 */	0,	/* %0LDSB(vACH); */
/* 661 */	1,	/* \tLD(v(%0)+1);XORI(128);SUBI(128);STW(%c);%{!A}\n */
/* 662 */	1,	/* \tLDSB(v(%0)+1);STW(%c);%{!A}\n */
/* 663 */	0,	/* %0ST(vACH);ORI(255);XORI(255); */
/* 664 */	0,	/* %0ST(vACH);ORI(255);XORI(255); */
/* 665 */	0,	/* %0ST(vACH);MOVQB(0,vAC); */
/* 666 */	0,	/* %0ST(vACH);MOVQB(0,vAC); */
/* 667 */	1,	/* \t%0ST(v(%c)+1);LDI(0);ST(%c);%{!A}\n */
/* 668 */	1,	/* \t%0ST(v(%c)+1);MOVQB(0,%c);\n */
/* 669 */	1,	/* \t%0ST(v(%c)+1);LDI(0);ST(%c);%{!A}\n */
/* 670 */	1,	/* \t%0ST(v(%c)+1);MOVQB(0,%c);\n */
/* 671 */	1,	/* \t%0ST(v(%c)+1);LDI(0);ST(%c);%{!A}\n */
/* 672 */	1,	/* \t%0ST(v(%c)+1);MOVQB(0,%c);\n */
/* 673 */	0,	/* %0LD(vACL); */
/* 674 */	0,	/* %0LD(vACL); */
/* 675 */	1,	/* \t%0STW(%c);\n */
/* 676 */	1,	/* \t%0STW(%c);\n */
/* 677 */	0,	/* %0ORI(255);XORI(255); */
/* 678 */	0,	/* %0ORI(255);XORI(255); */
/* 679 */	0,	/* %0MOVQB(0,vAC); */
/* 680 */	0,	/* %0MOVQB(0,vAC); */
/* 681 */	1,	/* \t%2ST(%c);LD(%0);ST(v(%c)+1);%{!A}\n */
/* 682 */	1,	/* \t%2ST(%c);LD(v(%0)+1);ST(v(%c)+1);%{!A}\n */
/* 683 */	1,	/* \t%2ST(%c);%0ST(v(%c)+1);%{!A}\n */
/* 684 */	1,	/* \t%0ST(v(%c)+1);%2ST(%c);%{!A}\n */
/* 685 */	1,	/* \t%2ST(%c);%0LD(vACH);ST(v(%c)+1);%{!A}\n */
/* 686 */	1,	/* \t%0LD(vACH);ST(v(%c)+1);%2ST(%c);%{!A}\n */
/* 687 */	1,	/* \t%0ST(v(%c)+1);LDI(%2);ST(%c);%{!A}\n */
/* 688 */	1,	/* \t%0ST(v(%c)+1);MOVQB(%2,%c);\n */
/* 689 */	1,	/* \t%0LD(vACH);ST(v(%c)+1);LDI(%2);ST(%c);%{!A}\n */
/* 690 */	1,	/* \t%0LD(vACH);ST(v(%c)+1);MOVQB(%2,%c);\n */
/* 691 */	1,	/* \tLD(v(%0)+1);ST(v(%c)+1);LDI(%2);ST(%c);%{!A}\n */
/* 692 */	1,	/* \tLD(v(%0)+1);ST(v(%c)+1);MOVQB(%2,%c);%{!A}\n */
/* 693 */	1,	/* \t%0ST(%c);LDI(hi(%1));ST(v(%c)+1);%{!A}\n */
/* 694 */	0,	/* %0ADDHI(hi(%1)); */
/* 695 */	1,	/* \t%3ST(%0);\n */
/* 696 */	1,	/* \t%3ST(%0);\n */
/* 697 */	1,	/* \t%3ST(%0);\n */
/* 698 */	1,	/* \t%2LD(vACH);ST(v(%0)+1);%{!A}\n */
/* 699 */	1,	/* \t%2LD(vACH);ST(v(%0)+1);%{!A}\n */
/* 700 */	1,	/* \t%2LD(vACH);ST(v(%0)+1);%{!A}\n */
/* 701 */	1,	/* \tLD(v(%2)+1);ST(v(%0)+1);%{!A}\n */
/* 702 */	1,	/* \tLD(v(%2)+1);ST(v(%0)+1);%{!A}\n */
/* 703 */	1,	/* \tLD(v(%2)+1);ST(v(%0)+1);%{!A}\n */
/* 704 */	1,	/* \t%2ST(v(%0)+1);\n */
/* 705 */	1,	/* \t%2ST(v(%0)+1);\n */
/* 706 */	1,	/* \t%2ST(v(%0)+1);\n */
/* 707 */	1,	/* \tLDI(0);ST(%0);%{!A}\n */
/* 708 */	1,	/* \tLDI(0);ST(%0);%{!A}\n */
/* 709 */	1,	/* \tLDI(0);ST(%0);%{!A}\n */
/* 710 */	1,	/* \tMOVQB(0,%0);\n */
/* 711 */	1,	/* \tMOVQB(0,%0);\n */
/* 712 */	1,	/* \tMOVQB(0,%0);\n */
/* 713 */	1,	/* \tLDI(%3);ST(%0);\n */
/* 714 */	1,	/* \tLDI(%3);ST(%0);\n */
/* 715 */	1,	/* \tLDI(%3);ST(%0);\n */
/* 716 */	1,	/* \tMOVQB(%3,%0);\n */
/* 717 */	1,	/* \tMOVQB(%3,%0);\n */
/* 718 */	1,	/* \tMOVQB(%3,%0);\n */
/* 719 */	1,	/* \tMOVQB(0,v(%0)+1);\n */
/* 720 */	1,	/* \tMOVQB(0,v(%0)+1);\n */
/* 721 */	1,	/* \tMOVQB(0,v(%0)+1);\n */
/* 722 */	1,	/* \tLDI(hi(%2));ST(v(%0)+1);%{!A}\n */
/* 723 */	1,	/* \tLDI(hi(%2));ST(v(%0)+1);%{!A}\n */
/* 724 */	1,	/* \tLDI(hi(%2));ST(v(%0)+1);%{!A}\n */
/* 725 */	1,	/* \tMOVQB(hi(%2),v(%0)+1);\n */
/* 726 */	1,	/* \tMOVQB(hi(%2),v(%0)+1);\n */
/* 727 */	1,	/* \tMOVQB(hi(%2),v(%0)+1);\n */
};

static char *_string[] = {
/* 0 */	0,
/* 1 */	"reg: INDIRI1(VREGP)",
/* 2 */	"reg: INDIRU1(VREGP)",
/* 3 */	"reg: INDIRI2(VREGP)",
/* 4 */	"reg: INDIRU2(VREGP)",
/* 5 */	"reg: INDIRP2(VREGP)",
/* 6 */	"reg: INDIRI4(VREGP)",
/* 7 */	"reg: INDIRU4(VREGP)",
/* 8 */	"reg: INDIRF5(VREGP)",
/* 9 */	"stmt: ASGNI1(VREGP,reg)",
/* 10 */	"stmt: ASGNU1(VREGP,reg)",
/* 11 */	"stmt: ASGNI2(VREGP,reg)",
/* 12 */	"stmt: ASGNU2(VREGP,reg)",
/* 13 */	"stmt: ASGNP2(VREGP,reg)",
/* 14 */	"stmt: ASGNI4(VREGP,reg)",
/* 15 */	"stmt: ASGNU4(VREGP,reg)",
/* 16 */	"stmt: ASGNF5(VREGP,reg)",
/* 17 */	"con0: CNSTI1",
/* 18 */	"con0: CNSTU1",
/* 19 */	"con0: CNSTI2",
/* 20 */	"con0: CNSTU2",
/* 21 */	"con0: CNSTP2",
/* 22 */	"con1: CNSTI1",
/* 23 */	"con1: CNSTU1",
/* 24 */	"con1: CNSTI2",
/* 25 */	"con1: CNSTU2",
/* 26 */	"conB: CNSTI2",
/* 27 */	"conB: CNSTU2",
/* 28 */	"conB: CNSTP2",
/* 29 */	"conB: CNSTU1",
/* 30 */	"conB: zddr",
/* 31 */	"conBs: CNSTI1",
/* 32 */	"conBs: CNSTI2",
/* 33 */	"conBn: CNSTI2",
/* 34 */	"conBm: CNSTI2",
/* 35 */	"conBm: CNSTU2",
/* 36 */	"conBa: CNSTI2",
/* 37 */	"conBa: CNSTU2",
/* 38 */	"con: CNSTI1",
/* 39 */	"con: CNSTU1",
/* 40 */	"con: CNSTI2",
/* 41 */	"con: CNSTU2",
/* 42 */	"con: CNSTP2",
/* 43 */	"con: addr",
/* 44 */	"con8: CNSTU2",
/* 45 */	"con8: CNSTI2",
/* 46 */	"conFF: CNSTU2",
/* 47 */	"conFF: CNSTI2",
/* 48 */	"conFF00: CNSTU2",
/* 49 */	"conXX00: CNSTU2",
/* 50 */	"conXX00: CNSTI2",
/* 51 */	"lddr: ADDRLP2",
/* 52 */	"lddr: ADDRFP2",
/* 53 */	"addr: ADDRGP2",
/* 54 */	"addr: con",
/* 55 */	"addr: zddr",
/* 56 */	"zddr: ADDRGP2",
/* 57 */	"zddr: conB",
/* 58 */	"stmt: reg",
/* 59 */	"stmt: asgn",
/* 60 */	"stmt: ac",
/* 61 */	"regx: reg",
/* 62 */	"reg: regx",
/* 63 */	"reg: ac",
/* 64 */	"ac0: eac0",
/* 65 */	"eac: eac0",
/* 66 */	"ac: ac0",
/* 67 */	"ac: eac",
/* 68 */	"eac: reg",
/* 69 */	"eac: lddr",
/* 70 */	"eac0: conB",
/* 71 */	"eac: con",
/* 72 */	"eac: INDIRI2(eac)",
/* 73 */	"eac: INDIRU2(eac)",
/* 74 */	"eac: INDIRP2(eac)",
/* 75 */	"eac0: INDIRI1(eac)",
/* 76 */	"eac0: INDIRU1(eac)",
/* 77 */	"eac: INDIRI2(zddr)",
/* 78 */	"eac: INDIRU2(zddr)",
/* 79 */	"eac: INDIRP2(zddr)",
/* 80 */	"eac0: INDIRI1(zddr)",
/* 81 */	"eac0: INDIRU1(zddr)",
/* 82 */	"ac: INDIRI2(ac)",
/* 83 */	"ac: INDIRU2(ac)",
/* 84 */	"ac: INDIRP2(ac)",
/* 85 */	"ac0: INDIRI1(ac)",
/* 86 */	"ac0: INDIRU1(ac)",
/* 87 */	"iarg: regx",
/* 88 */	"iarg: INDIRI2(zddr)",
/* 89 */	"iarg: INDIRU2(zddr)",
/* 90 */	"iarg: INDIRP2(zddr)",
/* 91 */	"spill: ADDRLP2",
/* 92 */	"iarg: INDIRU2(spill)",
/* 93 */	"iarg: INDIRI2(spill)",
/* 94 */	"iarg: INDIRP2(spill)",
/* 95 */	"ac: ADDI2(ac,iarg)",
/* 96 */	"ac: ADDU2(ac,iarg)",
/* 97 */	"ac: ADDP2(ac,iarg)",
/* 98 */	"ac: ADDI2(iarg,ac)",
/* 99 */	"ac: ADDU2(iarg,ac)",
/* 100 */	"ac: ADDP2(iarg,ac)",
/* 101 */	"ac: ADDI2(LSHI2(iarg,con1),ac)",
/* 102 */	"ac: ADDU2(LSHU2(iarg,con1),ac)",
/* 103 */	"ac: ADDP2(LSHI2(iarg,con1),ac)",
/* 104 */	"ac: ADDP2(LSHU2(iarg,con1),ac)",
/* 105 */	"ac: ADDI2(ac,conB)",
/* 106 */	"ac: ADDU2(ac,conB)",
/* 107 */	"ac: ADDP2(ac,conB)",
/* 108 */	"ac: ADDI2(ac,conBn)",
/* 109 */	"ac: ADDU2(ac,conBn)",
/* 110 */	"ac: ADDP2(ac,conBn)",
/* 111 */	"ac: SUBI2(ac,iarg)",
/* 112 */	"ac: SUBU2(ac,iarg)",
/* 113 */	"ac: SUBP2(ac,iarg)",
/* 114 */	"ac: SUBI2(ac,conB)",
/* 115 */	"ac: SUBU2(ac,conB)",
/* 116 */	"ac: SUBP2(ac,conB)",
/* 117 */	"ac: SUBI2(ac,conBn)",
/* 118 */	"ac: SUBU2(ac,conBn)",
/* 119 */	"ac: SUBP2(ac,conBn)",
/* 120 */	"ac: NEGI2(ac)",
/* 121 */	"ac: NEGI2(regx)",
/* 122 */	"ac: LSHI2(ac,con1)",
/* 123 */	"ac: LSHU2(ac,con1)",
/* 124 */	"ac: LSHI2(ac,conB)",
/* 125 */	"ac: LSHU2(ac,conB)",
/* 126 */	"ac: RSHI2(ac,conB)",
/* 127 */	"ac: RSHU2(ac,conB)",
/* 128 */	"ac: LSHI2(ac,iarg)",
/* 129 */	"ac: RSHI2(ac,iarg)",
/* 130 */	"ac: LSHU2(ac,iarg)",
/* 131 */	"ac: RSHU2(ac,iarg)",
/* 132 */	"ac: MULI2(conB,ac)",
/* 133 */	"ac: MULI2(conBn,ac)",
/* 134 */	"ac: MULI2(conB,regx)",
/* 135 */	"ac: MULI2(conBn,regx)",
/* 136 */	"ac: MULI2(con,ac)",
/* 137 */	"ac: MULI2(ac,iarg)",
/* 138 */	"ac: MULI2(iarg,ac)",
/* 139 */	"ac: MULU2(conB,ac)",
/* 140 */	"ac: MULU2(con,ac)",
/* 141 */	"ac: MULU2(ac,iarg)",
/* 142 */	"ac: MULU2(iarg,ac)",
/* 143 */	"ac: DIVI2(ac,iarg)",
/* 144 */	"ac: DIVU2(ac,iarg)",
/* 145 */	"ac: MODI2(ac,iarg)",
/* 146 */	"ac: MODU2(ac,iarg)",
/* 147 */	"ac: DIVI2(ac,con)",
/* 148 */	"ac: DIVU2(ac,con)",
/* 149 */	"ac: MODI2(ac,con)",
/* 150 */	"ac: MODU2(ac,con)",
/* 151 */	"ac: BCOMI2(ac)",
/* 152 */	"ac: BCOMU2(ac)",
/* 153 */	"ac: BANDI2(ac,iarg)",
/* 154 */	"ac: BANDU2(ac,iarg)",
/* 155 */	"ac: BANDI2(iarg,ac)",
/* 156 */	"ac: BANDU2(iarg,ac)",
/* 157 */	"ac: BANDI2(ac,conB)",
/* 158 */	"ac: BANDU2(ac,conB)",
/* 159 */	"ac0: BANDI2(ac0,conB)",
/* 160 */	"ac0: BANDU2(ac0,conB)",
/* 161 */	"ac: BORI2(ac,iarg)",
/* 162 */	"ac: BORU2(ac,iarg)",
/* 163 */	"ac: BORI2(iarg,ac)",
/* 164 */	"ac: BORU2(iarg,ac)",
/* 165 */	"ac: BORI2(ac,conB)",
/* 166 */	"ac: BORU2(ac,conB)",
/* 167 */	"ac0: BORI2(ac0,conB)",
/* 168 */	"ac0: BORU2(ac0,conB)",
/* 169 */	"ac: BXORI2(ac,iarg)",
/* 170 */	"ac: BXORU2(ac,iarg)",
/* 171 */	"ac: BXORI2(iarg,ac)",
/* 172 */	"ac: BXORU2(iarg,ac)",
/* 173 */	"ac: BXORI2(ac,conB)",
/* 174 */	"ac: BXORU2(ac,conB)",
/* 175 */	"ac0: BXORI2(ac0,conB)",
/* 176 */	"ac0: BXORU2(ac0,conB)",
/* 177 */	"eac: ADDI2(eac,conB)",
/* 178 */	"eac: ADDU2(eac,conB)",
/* 179 */	"eac: ADDP2(eac,conB)",
/* 180 */	"eac: ADDI2(eac,conBn)",
/* 181 */	"eac: ADDU2(eac,conBn)",
/* 182 */	"eac: ADDP2(eac,conBn)",
/* 183 */	"eac: SUBI2(eac,conB)",
/* 184 */	"eac: SUBU2(eac,conB)",
/* 185 */	"eac: SUBP2(eac,conB)",
/* 186 */	"eac: SUBI2(eac,conBn)",
/* 187 */	"eac: SUBU2(eac,conBn)",
/* 188 */	"eac: SUBP2(eac,conBn)",
/* 189 */	"eac: LSHI2(eac,con1)",
/* 190 */	"eac: LSHU2(eac,con1)",
/* 191 */	"eac: LSHI2(eac,conB)",
/* 192 */	"eac: LSHU2(eac,conB)",
/* 193 */	"eac: MULI2(conB,eac)",
/* 194 */	"eac: MULI2(conB,eac)",
/* 195 */	"eac: BANDI2(eac,conB)",
/* 196 */	"eac: BANDU2(eac,conB)",
/* 197 */	"eac0: BANDI2(eac0,conB)",
/* 198 */	"eac0: BANDU2(eac0,conB)",
/* 199 */	"eac: BORI2(eac,conB)",
/* 200 */	"eac: BORU2(eac,conB)",
/* 201 */	"eac0: BORI2(eac0,conB)",
/* 202 */	"eac0: BORU2(eac0,conB)",
/* 203 */	"eac: BXORI2(eac,conB)",
/* 204 */	"eac: BXORU2(eac,conB)",
/* 205 */	"eac0: BXORI2(eac0,conB)",
/* 206 */	"eac0: BXORU2(eac0,conB)",
/* 207 */	"eac: ADDI2(eac,iarg)",
/* 208 */	"eac: ADDU2(eac,iarg)",
/* 209 */	"eac: ADDP2(eac,iarg)",
/* 210 */	"eac: ADDI2(iarg,eac)",
/* 211 */	"eac: ADDU2(iarg,eac)",
/* 212 */	"eac: ADDP2(iarg,eac)",
/* 213 */	"eac: ADDI2(LSHI2(iarg,con1),eac)",
/* 214 */	"eac: ADDU2(LSHU2(iarg,con1),eac)",
/* 215 */	"eac: ADDP2(LSHI2(iarg,con1),eac)",
/* 216 */	"eac: ADDP2(LSHU2(iarg,con1),eac)",
/* 217 */	"eac: SUBI2(eac,iarg)",
/* 218 */	"eac: SUBU2(eac,iarg)",
/* 219 */	"eac: SUBP2(eac,iarg)",
/* 220 */	"eac: BANDI2(eac,iarg)",
/* 221 */	"eac: BANDU2(eac,iarg)",
/* 222 */	"eac: BANDI2(iarg,eac)",
/* 223 */	"eac: BANDU2(iarg,eac)",
/* 224 */	"eac: BORI2(eac,iarg)",
/* 225 */	"eac: BORU2(eac,iarg)",
/* 226 */	"eac: BORI2(iarg,eac)",
/* 227 */	"eac: BORU2(iarg,eac)",
/* 228 */	"eac: BXORI2(eac,iarg)",
/* 229 */	"eac: BXORU2(eac,iarg)",
/* 230 */	"eac: BXORI2(iarg,eac)",
/* 231 */	"eac: BXORU2(iarg,eac)",
/* 232 */	"asgn: ASGNP2(zddr,ac)",
/* 233 */	"asgn: ASGNP2(iarg,ac)",
/* 234 */	"asgn: ASGNI2(zddr,ac)",
/* 235 */	"asgn: ASGNI2(iarg,ac)",
/* 236 */	"asgn: ASGNU2(zddr,ac)",
/* 237 */	"asgn: ASGNU2(iarg,ac)",
/* 238 */	"asgn: ASGNI1(zddr,ac)",
/* 239 */	"asgn: ASGNI1(iarg,ac)",
/* 240 */	"asgn: ASGNU1(zddr,ac)",
/* 241 */	"asgn: ASGNU1(iarg,ac)",
/* 242 */	"stmt: EQI2(ac,con0)",
/* 243 */	"stmt: EQI2(ac,conB)",
/* 244 */	"stmt: EQI2(ac,iarg)",
/* 245 */	"stmt: EQI2(iarg,ac)",
/* 246 */	"stmt: NEI2(ac,con0)",
/* 247 */	"stmt: NEI2(ac,conB)",
/* 248 */	"stmt: NEI2(ac,iarg)",
/* 249 */	"stmt: NEI2(iarg,ac)",
/* 250 */	"stmt: EQU2(ac,con0)",
/* 251 */	"stmt: EQU2(ac,conB)",
/* 252 */	"stmt: EQU2(ac,iarg)",
/* 253 */	"stmt: EQU2(iarg,ac)",
/* 254 */	"stmt: NEU2(ac,con0)",
/* 255 */	"stmt: NEU2(ac,conB)",
/* 256 */	"stmt: NEU2(ac,iarg)",
/* 257 */	"stmt: NEU2(iarg,ac)",
/* 258 */	"stmt: LTI2(ac,con0)",
/* 259 */	"stmt: LEI2(ac,con0)",
/* 260 */	"stmt: GTI2(ac,con0)",
/* 261 */	"stmt: GEI2(ac,con0)",
/* 262 */	"stmt: GTU2(ac,con0)",
/* 263 */	"stmt: LEU2(ac,con0)",
/* 264 */	"stmt: LTI2(ac,conB)",
/* 265 */	"stmt: LEI2(ac,conB)",
/* 266 */	"stmt: GTI2(ac,conB)",
/* 267 */	"stmt: GEI2(ac,conB)",
/* 268 */	"stmt: LTU2(ac,conB)",
/* 269 */	"stmt: LEU2(ac,conB)",
/* 270 */	"stmt: GTU2(ac,conB)",
/* 271 */	"stmt: GEU2(ac,conB)",
/* 272 */	"stmt: LTI2(ac,iarg)",
/* 273 */	"stmt: LEI2(ac,iarg)",
/* 274 */	"stmt: GTI2(ac,iarg)",
/* 275 */	"stmt: GEI2(ac,iarg)",
/* 276 */	"stmt: LTU2(ac,iarg)",
/* 277 */	"stmt: LEU2(ac,iarg)",
/* 278 */	"stmt: GTU2(ac,iarg)",
/* 279 */	"stmt: GEU2(ac,iarg)",
/* 280 */	"stmt: LTI2(iarg,ac)",
/* 281 */	"stmt: LEI2(iarg,ac)",
/* 282 */	"stmt: GTI2(iarg,ac)",
/* 283 */	"stmt: GEI2(iarg,ac)",
/* 284 */	"stmt: LTU2(iarg,ac)",
/* 285 */	"stmt: LEU2(iarg,ac)",
/* 286 */	"stmt: GTU2(iarg,ac)",
/* 287 */	"stmt: GEU2(iarg,ac)",
/* 288 */	"vdst: addr",
/* 289 */	"vdst: lddr",
/* 290 */	"vdst: eac",
/* 291 */	"asrc: addr",
/* 292 */	"asrc: lddr",
/* 293 */	"asrc: eac",
/* 294 */	"lsrc: addr",
/* 295 */	"asgn: ARGB(INDIRB(asrc))",
/* 296 */	"asgn: ASGNB(addr,INDIRB(asrc))",
/* 297 */	"asgn: ASGNB(ac,INDIRB(asrc))",
/* 298 */	"asgn: ASGNB(lddr,INDIRB(lsrc))",
/* 299 */	"stmt: lac",
/* 300 */	"larg: regx",
/* 301 */	"larg: INDIRI4(eac)",
/* 302 */	"larg: INDIRU4(eac)",
/* 303 */	"reg: lac",
/* 304 */	"reg: INDIRI4(ac)",
/* 305 */	"reg: INDIRU4(ac)",
/* 306 */	"reg: INDIRI4(lddr)",
/* 307 */	"reg: INDIRU4(lddr)",
/* 308 */	"reg: INDIRI4(addr)",
/* 309 */	"reg: INDIRU4(addr)",
/* 310 */	"lac: reg",
/* 311 */	"lac: INDIRI4(ac)",
/* 312 */	"lac: INDIRU4(ac)",
/* 313 */	"lac: INDIRU4(lddr)",
/* 314 */	"lac: INDIRU4(lddr)",
/* 315 */	"lac: INDIRI4(addr)",
/* 316 */	"lac: INDIRU4(addr)",
/* 317 */	"lac: ADDI4(lac,larg)",
/* 318 */	"lac: ADDU4(lac,larg)",
/* 319 */	"lac: ADDI4(larg,lac)",
/* 320 */	"lac: ADDU4(larg,lac)",
/* 321 */	"lac: SUBI4(lac,larg)",
/* 322 */	"lac: SUBU4(lac,larg)",
/* 323 */	"lac: MULI4(lac,larg)",
/* 324 */	"lac: MULU4(lac,larg)",
/* 325 */	"lac: MULI4(larg,lac)",
/* 326 */	"lac: MULU4(larg,lac)",
/* 327 */	"lac: DIVI4(lac,larg)",
/* 328 */	"lac: DIVU4(lac,larg)",
/* 329 */	"lac: MODI4(lac,larg)",
/* 330 */	"lac: MODU4(lac,larg)",
/* 331 */	"lac: LSHI4(lac,reg)",
/* 332 */	"lac: LSHI4(lac,conB)",
/* 333 */	"lac: LSHU4(lac,reg)",
/* 334 */	"lac: LSHU4(lac,conB)",
/* 335 */	"lac: RSHI4(lac,reg)",
/* 336 */	"lac: RSHI4(lac,conB)",
/* 337 */	"lac: RSHU4(lac,reg)",
/* 338 */	"lac: RSHU4(lac,conB)",
/* 339 */	"lac: NEGI4(lac)",
/* 340 */	"lac: BCOMU4(lac)",
/* 341 */	"lac: BANDU4(lac,larg)",
/* 342 */	"lac: BANDU4(larg,lac)",
/* 343 */	"lac: BORU4(lac,larg)",
/* 344 */	"lac: BORU4(larg,lac)",
/* 345 */	"lac: BXORU4(lac,larg)",
/* 346 */	"lac: BXORU4(larg,lac)",
/* 347 */	"lac: BCOMI4(lac)",
/* 348 */	"lac: BANDI4(lac,larg)",
/* 349 */	"lac: BANDI4(larg,lac)",
/* 350 */	"lac: BORI4(lac,larg)",
/* 351 */	"lac: BORI4(larg,lac)",
/* 352 */	"lac: BXORI4(lac,larg)",
/* 353 */	"lac: BXORI4(larg,lac)",
/* 354 */	"stmt: LTI4(lac,larg)",
/* 355 */	"stmt: LEI4(lac,larg)",
/* 356 */	"stmt: GTI4(lac,larg)",
/* 357 */	"stmt: GEI4(lac,larg)",
/* 358 */	"stmt: LTU4(lac,larg)",
/* 359 */	"stmt: LEU4(lac,larg)",
/* 360 */	"stmt: GTU4(lac,larg)",
/* 361 */	"stmt: GEU4(lac,larg)",
/* 362 */	"stmt: NEI4(lac,larg)",
/* 363 */	"stmt: EQI4(lac,larg)",
/* 364 */	"stmt: NEU4(lac,larg)",
/* 365 */	"stmt: EQU4(lac,larg)",
/* 366 */	"stmt: LTI4(larg,lac)",
/* 367 */	"stmt: LEI4(larg,lac)",
/* 368 */	"stmt: GTI4(larg,lac)",
/* 369 */	"stmt: GEI4(larg,lac)",
/* 370 */	"stmt: LTU4(larg,lac)",
/* 371 */	"stmt: LEU4(larg,lac)",
/* 372 */	"stmt: GTU4(larg,lac)",
/* 373 */	"stmt: GEU4(larg,lac)",
/* 374 */	"stmt: NEI4(larg,lac)",
/* 375 */	"stmt: EQI4(larg,lac)",
/* 376 */	"stmt: NEU4(larg,lac)",
/* 377 */	"stmt: EQU4(larg,lac)",
/* 378 */	"asgn: ASGNI4(vdst,lac)",
/* 379 */	"asgn: ASGNI4(vdst,reg)",
/* 380 */	"asgn: ASGNI4(addr,INDIRI4(asrc))",
/* 381 */	"asgn: ASGNI4(ac,INDIRI4(asrc))",
/* 382 */	"asgn: ASGNI4(lddr,INDIRI4(lsrc))",
/* 383 */	"asgn: ASGNU4(vdst,lac)",
/* 384 */	"asgn: ASGNU4(vdst,reg)",
/* 385 */	"asgn: ASGNU4(addr,INDIRU4(asrc))",
/* 386 */	"asgn: ASGNU4(ac,INDIRU4(asrc))",
/* 387 */	"asgn: ASGNU4(lddr,INDIRU4(lsrc))",
/* 388 */	"stmt: fac",
/* 389 */	"farg: regx",
/* 390 */	"farg: INDIRF5(eac)",
/* 391 */	"reg: fac",
/* 392 */	"reg: INDIRF5(ac)",
/* 393 */	"reg: INDIRF5(lddr)",
/* 394 */	"reg: INDIRF5(addr)",
/* 395 */	"fac: reg",
/* 396 */	"fac: INDIRF5(ac)",
/* 397 */	"fac: INDIRF5(lddr)",
/* 398 */	"fac: INDIRF5(addr)",
/* 399 */	"fac: ADDF5(fac,farg)",
/* 400 */	"fac: ADDF5(farg,fac)",
/* 401 */	"fac: SUBF5(fac,farg)",
/* 402 */	"fac: SUBF5(farg,fac)",
/* 403 */	"fac: MULF5(fac,farg)",
/* 404 */	"fac: MULF5(farg,fac)",
/* 405 */	"fac: DIVF5(fac,farg)",
/* 406 */	"fac: DIVF5(farg,fac)",
/* 407 */	"fac: NEGF5(fac)",
/* 408 */	"stmt: EQF5(fac,farg)",
/* 409 */	"stmt: NEF5(fac,farg)",
/* 410 */	"stmt: LTF5(fac,farg)",
/* 411 */	"stmt: LEF5(fac,farg)",
/* 412 */	"stmt: GTF5(fac,farg)",
/* 413 */	"stmt: GEF5(fac,farg)",
/* 414 */	"stmt: EQF5(farg,fac)",
/* 415 */	"stmt: NEF5(farg,fac)",
/* 416 */	"stmt: LTF5(farg,fac)",
/* 417 */	"stmt: LEF5(farg,fac)",
/* 418 */	"stmt: GTF5(farg,fac)",
/* 419 */	"stmt: GEF5(farg,fac)",
/* 420 */	"asgn: ASGNF5(vdst,fac)",
/* 421 */	"asgn: ASGNF5(vdst,reg)",
/* 422 */	"asgn: ASGNF5(addr,INDIRF5(asrc))",
/* 423 */	"asgn: ASGNF5(ac,INDIRF5(asrc))",
/* 424 */	"asgn: ASGNF5(lddr,INDIRF5(lsrc))",
/* 425 */	"fac: CALLF5(addr)",
/* 426 */	"fac: CALLF5(reg)",
/* 427 */	"fac: CALLF5(ac)",
/* 428 */	"lac: CALLI4(addr)",
/* 429 */	"lac: CALLI4(reg)",
/* 430 */	"lac: CALLI4(ac)",
/* 431 */	"lac: CALLU4(addr)",
/* 432 */	"lac: CALLU4(reg)",
/* 433 */	"lac: CALLU4(ac)",
/* 434 */	"ac: CALLI2(addr)",
/* 435 */	"ac: CALLI2(reg)",
/* 436 */	"ac: CALLI2(ac)",
/* 437 */	"ac: CALLU2(addr)",
/* 438 */	"ac: CALLU2(reg)",
/* 439 */	"ac: CALLU2(ac)",
/* 440 */	"ac: CALLP2(addr)",
/* 441 */	"ac: CALLP2(reg)",
/* 442 */	"ac: CALLP2(ac)",
/* 443 */	"stmt: CALLV(addr)",
/* 444 */	"stmt: CALLV(reg)",
/* 445 */	"stmt: CALLV(ac)",
/* 446 */	"stmt: ARGF5(reg)",
/* 447 */	"stmt: ARGI4(reg)",
/* 448 */	"stmt: ARGU4(reg)",
/* 449 */	"stmt: ARGI2(reg)",
/* 450 */	"stmt: ARGU2(reg)",
/* 451 */	"stmt: ARGP2(reg)",
/* 452 */	"stmt: ARGF5(reg)",
/* 453 */	"stmt: ARGI4(reg)",
/* 454 */	"stmt: ARGU4(reg)",
/* 455 */	"stmt: ARGI2(reg)",
/* 456 */	"stmt: ARGU2(reg)",
/* 457 */	"stmt: ARGP2(reg)",
/* 458 */	"stmt: RETF5(fac)",
/* 459 */	"stmt: RETI4(lac)",
/* 460 */	"stmt: RETU4(lac)",
/* 461 */	"stmt: RETI2(ac)",
/* 462 */	"stmt: RETU2(ac)",
/* 463 */	"stmt: RETP2(ac)",
/* 464 */	"eac0: LOADI1(reg)",
/* 465 */	"eac0: LOADU1(reg)",
/* 466 */	"ac: LOADI1(ac)",
/* 467 */	"ac: LOADU1(ac)",
/* 468 */	"ac0: LOADI1(ac0)",
/* 469 */	"ac0: LOADU1(ac0)",
/* 470 */	"ac: LOADI2(ac)",
/* 471 */	"ac: LOADU2(ac)",
/* 472 */	"ac: LOADP2(ac)",
/* 473 */	"ac0: LOADI2(ac0)",
/* 474 */	"ac0: LOADU2(ac0)",
/* 475 */	"ac0: LOADP2(ac0)",
/* 476 */	"eac: LOADI1(eac)",
/* 477 */	"eac: LOADU1(eac)",
/* 478 */	"eac: LOADI2(eac)",
/* 479 */	"eac: LOADU2(eac)",
/* 480 */	"eac: LOADP2(eac)",
/* 481 */	"eac: LOADI2(reg)",
/* 482 */	"eac: LOADU2(reg)",
/* 483 */	"eac: LOADP2(reg)",
/* 484 */	"eac: LOADI2(lac)",
/* 485 */	"eac: LOADU2(lac)",
/* 486 */	"eac: LOADP2(lac)",
/* 487 */	"lac: LOADI4(lac)",
/* 488 */	"lac: LOADU4(lac)",
/* 489 */	"fac: LOADF5(fac)",
/* 490 */	"reg: LOADI1(reg)",
/* 491 */	"reg: LOADU1(reg)",
/* 492 */	"reg: LOADI1(ac)",
/* 493 */	"reg: LOADU1(ac)",
/* 494 */	"reg: LOADI4(reg)",
/* 495 */	"reg: LOADU4(reg)",
/* 496 */	"reg: LOADF5(reg)",
/* 497 */	"eac: CVII2(reg)",
/* 498 */	"eac0: CVUI2(reg)",
/* 499 */	"ac: CVII2(ac0)",
/* 500 */	"ac0: CVUI2(ac0)",
/* 501 */	"ac: CVII2(ac)",
/* 502 */	"ac0: CVUI2(ac)",
/* 503 */	"eac: CVII2(eac0)",
/* 504 */	"eac0: CVUI2(eac0)",
/* 505 */	"eac: CVII2(eac)",
/* 506 */	"eac0: CVUI2(eac)",
/* 507 */	"lac: CVIU4(ac)",
/* 508 */	"lac: CVII4(ac0)",
/* 509 */	"lac: CVII4(ac)",
/* 510 */	"lac: CVUU4(ac)",
/* 511 */	"lac: CVUI4(ac)",
/* 512 */	"reg: CVIU4(ac)",
/* 513 */	"reg: CVII4(ac0)",
/* 514 */	"reg: CVII4(ac)",
/* 515 */	"reg: CVUU4(ac)",
/* 516 */	"reg: CVUI4(ac)",
/* 517 */	"ac: CVFU2(fac)",
/* 518 */	"lac: CVFU4(fac)",
/* 519 */	"fac: CVUF5(ac)",
/* 520 */	"fac: CVUF5(lac)",
/* 521 */	"ac: CVFI2(fac)",
/* 522 */	"lac: CVFI4(fac)",
/* 523 */	"fac: CVIF5(ac0)",
/* 524 */	"fac: CVIF5(ac)",
/* 525 */	"fac: CVIF5(lac)",
/* 526 */	"stmt: LABELV",
/* 527 */	"stmt: JUMPV(addr)",
/* 528 */	"stmt: JUMPV(reg)",
/* 529 */	"stmt: JUMPV(ac)",
/* 530 */	"asgn: ASGNI2(spill,reg)",
/* 531 */	"asgn: ASGNU2(spill,reg)",
/* 532 */	"asgn: ASGNP2(spill,reg)",
/* 533 */	"asgn: ASGNI4(spill,reg)",
/* 534 */	"asgn: ASGNU4(spill,reg)",
/* 535 */	"asgn: ASGNF5(spill,reg)",
/* 536 */	"regx: con",
/* 537 */	"regx: conB",
/* 538 */	"ac: ADDI2(ac,con)",
/* 539 */	"ac: ADDU2(ac,con)",
/* 540 */	"ac: ADDP2(ac,con)",
/* 541 */	"ac: SUBI2(ac,con)",
/* 542 */	"ac: SUBU2(ac,con)",
/* 543 */	"ac: SUBP2(ac,con)",
/* 544 */	"eac: ADDI2(eac,con)",
/* 545 */	"eac: ADDU2(eac,con)",
/* 546 */	"eac: ADDP2(eac,con)",
/* 547 */	"eac: SUBI2(eac,con)",
/* 548 */	"eac: SUBU2(eac,con)",
/* 549 */	"eac: SUBP2(eac,con)",
/* 550 */	"ac: MULI2(con,ac)",
/* 551 */	"ac: MULU2(con,ac)",
/* 552 */	"ac: CVII2(ac)",
/* 553 */	"ac: NEGI2(ac)",
/* 554 */	"eac: MULI2(conBm,eac)",
/* 555 */	"eac: MULU2(conBm,eac)",
/* 556 */	"eac: CVII2(reg)",
/* 557 */	"eac: CVII2(eac)",
/* 558 */	"eac: NEGI2(eac)",
/* 559 */	"lac: NEGI4(lac)",
/* 560 */	"asgn: ASGNP2(ac,iarg)",
/* 561 */	"asgn: ASGNI2(ac,iarg)",
/* 562 */	"asgn: ASGNU2(ac,iarg)",
/* 563 */	"asgn: ASGNI1(ac,iarg)",
/* 564 */	"asgn: ASGNU1(ac,iarg)",
/* 565 */	"asgn: ASGNP2(ac,con)",
/* 566 */	"asgn: ASGNI2(ac,con)",
/* 567 */	"asgn: ASGNU2(ac,con)",
/* 568 */	"asgn: ASGNP2(ac,conB)",
/* 569 */	"asgn: ASGNI2(ac,conB)",
/* 570 */	"asgn: ASGNU2(ac,conB)",
/* 571 */	"asgn: ASGNI1(ac,conBs)",
/* 572 */	"asgn: ASGNU1(ac,conB)",
/* 573 */	"eac: INDIRI2(reg)",
/* 574 */	"eac: INDIRU2(reg)",
/* 575 */	"eac: INDIRP2(reg)",
/* 576 */	"eac0: INDIRI1(reg)",
/* 577 */	"eac0: INDIRU1(reg)",
/* 578 */	"eac: INDIRI2(INDIRP2(zddr))",
/* 579 */	"eac: INDIRU2(INDIRP2(zddr))",
/* 580 */	"eac: INDIRP2(INDIRP2(zddr))",
/* 581 */	"eac0: INDIRI1(INDIRP2(zddr))",
/* 582 */	"eac0: INDIRU1(INDIRP2(zddr))",
/* 583 */	"reg: conB",
/* 584 */	"reg: con",
/* 585 */	"reg: INDIRI2(ac)",
/* 586 */	"reg: INDIRU2(ac)",
/* 587 */	"reg: INDIRP2(ac)",
/* 588 */	"reg: INDIRI1(ac)",
/* 589 */	"reg: INDIRU1(ac)",
/* 590 */	"asgn: ASGNI1(rmw,conBs)",
/* 591 */	"asgn: ASGNU1(rmw,conB)",
/* 592 */	"asgn: ASGNI2(rmw,conB)",
/* 593 */	"asgn: ASGNU2(rmw,conB)",
/* 594 */	"asgn: ASGNP2(rmw,conB)",
/* 595 */	"asgn: ASGNI2(rmw,con)",
/* 596 */	"asgn: ASGNU2(rmw,con)",
/* 597 */	"asgn: ASGNP2(rmw,con)",
/* 598 */	"regx: LOADI1(conBs)",
/* 599 */	"regx: LOADU1(conB)",
/* 600 */	"regx: LOADI2(conB)",
/* 601 */	"regx: LOADU2(conB)",
/* 602 */	"regx: LOADP2(conB)",
/* 603 */	"regx: LOADI2(con)",
/* 604 */	"regx: LOADU2(con)",
/* 605 */	"regx: LOADP2(con)",
/* 606 */	"reg: LOADI2(reg)",
/* 607 */	"reg: LOADU2(reg)",
/* 608 */	"reg: LOADP2(reg)",
/* 609 */	"eac: INDIRI2(ADDP2(reg,con))",
/* 610 */	"eac: INDIRU2(ADDP2(reg,con))",
/* 611 */	"eac: INDIRP2(ADDP2(reg,con))",
/* 612 */	"ac: INDIRI2(ADDP2(ac,con))",
/* 613 */	"ac: INDIRU2(ADDP2(ac,con))",
/* 614 */	"ac: INDIRP2(ADDP2(ac,con))",
/* 615 */	"eac: INDIRI2(ADDP2(eac,con))",
/* 616 */	"eac: INDIRU2(ADDP2(eac,con))",
/* 617 */	"eac: INDIRP2(ADDP2(eac,con))",
/* 618 */	"asgn: ASGNI2(ADDP2(reg,con),ac)",
/* 619 */	"asgn: ASGNU2(ADDP2(reg,con),ac)",
/* 620 */	"asgn: ASGNP2(ADDP2(reg,con),ac)",
/* 621 */	"eac: INDIRI1(lddr)",
/* 622 */	"eac: INDIRU1(lddr)",
/* 623 */	"eac: INDIRI2(lddr)",
/* 624 */	"eac: INDIRU2(lddr)",
/* 625 */	"eac: INDIRP2(lddr)",
/* 626 */	"asgn: ASGNI2(lddr,ac)",
/* 627 */	"asgn: ASGNU2(lddr,ac)",
/* 628 */	"asgn: ASGNP2(lddr,ac)",
/* 629 */	"stmt: ARGI2(reg)",
/* 630 */	"stmt: ARGU2(reg)",
/* 631 */	"stmt: ARGP2(reg)",
/* 632 */	"rmw: VREGP",
/* 633 */	"rmw: zddr",
/* 634 */	"asgn: ASGNU1(rmw,LOADU1(ADDI2(CVUI2(INDIRU1(rmw)),con1)))",
/* 635 */	"asgn: ASGNI1(rmw,LOADI1(ADDI2(CVII2(INDIRI1(rmw)),con1)))",
/* 636 */	"asgn: ASGNI2(rmw,NEGI2(INDIRI2(rmw)))",
/* 637 */	"asgn: ASGNI4(rmw,NEGI4(INDIRI4(rmw)))",
/* 638 */	"asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),con1))",
/* 639 */	"asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),con1))",
/* 640 */	"asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),con1))",
/* 641 */	"asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),ac))",
/* 642 */	"asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),ac))",
/* 643 */	"asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),ac))",
/* 644 */	"asgn: ASGNI2(rmw,ADDI2(ac,INDIRI2(rmw)))",
/* 645 */	"asgn: ASGNU2(rmw,ADDU2(ac,INDIRU2(rmw)))",
/* 646 */	"asgn: ASGNP2(rmw,ADDP2(ac,INDIRP2(rmw)))",
/* 647 */	"asgn: ASGNI2(rmw,SUBI2(INDIRI2(rmw),ac))",
/* 648 */	"asgn: ASGNU2(rmw,SUBU2(INDIRU2(rmw),ac))",
/* 649 */	"asgn: ASGNP2(rmw,SUBP2(INDIRP2(rmw),ac))",
/* 650 */	"asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),conBa))",
/* 651 */	"asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),conBa))",
/* 652 */	"asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),conBa))",
/* 653 */	"asgn: ASGNP2(rmw,SUBP2(INDIRP2(rmw),conBa))",
/* 654 */	"asgn: ASGNU2(rmw,SUBU2(INDIRU2(rmw),conBa))",
/* 655 */	"asgn: ASGNI2(rmw,SUBI2(INDIRI2(rmw),conBa))",
/* 656 */	"ac0: RSHU2(reg,con8)",
/* 657 */	"ac0: RSHU2(ac,con8)",
/* 658 */	"reg: RSHU2(reg,con8)",
/* 659 */	"ac: RSHI2(ac,con8)",
/* 660 */	"ac: RSHI2(ac,con8)",
/* 661 */	"reg: RSHI2(reg,con8)",
/* 662 */	"reg: RSHI2(reg,con8)",
/* 663 */	"ac: LSHI2(ac,con8)",
/* 664 */	"ac: LSHU2(ac,con8)",
/* 665 */	"ac: LSHI2(ac,con8)",
/* 666 */	"ac: LSHU2(ac,con8)",
/* 667 */	"reg: LSHI2(ac,con8)",
/* 668 */	"reg: LSHI2(ac,con8)",
/* 669 */	"reg: LSHU2(ac,con8)",
/* 670 */	"reg: LSHU2(ac,con8)",
/* 671 */	"reg: LSHU2(CVUI2(LOADU1(ac)),con8)",
/* 672 */	"reg: LSHU2(CVUI2(LOADU1(ac)),con8)",
/* 673 */	"ac0: BANDI2(ac,conFF)",
/* 674 */	"ac0: BANDU2(ac,conFF)",
/* 675 */	"reg: BANDI2(ac0,conFF)",
/* 676 */	"reg: BANDU2(ac0,conFF)",
/* 677 */	"ac: BANDU2(ac,conFF00)",
/* 678 */	"ac: BANDI2(ac,conFF00)",
/* 679 */	"ac: BANDU2(ac,conFF00)",
/* 680 */	"ac: BANDI2(ac,conFF00)",
/* 681 */	"regx: ADDU2(LSHU2(CVUI2(LOADU1(iarg)),con8),CVUI2(LOADU1(ac)))",
/* 682 */	"regx: ADDU2(BANDU2(iarg,conFF00),CVUI2(LOADU1(ac)))",
/* 683 */	"regx: ADDU2(LSHU2(CVUI2(LOADU1(eac)),con8),CVUI2(LOADU1(ac)))",
/* 684 */	"regx: ADDU2(LSHU2(CVUI2(LOADU1(ac)),con8),CVUI2(LOADU1(eac)))",
/* 685 */	"regx: ADDU2(BANDU2(eac,conFF00),ac0)",
/* 686 */	"regx: ADDU2(BANDU2(ac,conFF00),eac0)",
/* 687 */	"regx: ADDU2(LSHU2(ac,con8),conB)",
/* 688 */	"regx: ADDU2(LSHU2(ac,con8),conB)",
/* 689 */	"regx: ADDU2(BANDU2(ac,conFF00),conB)",
/* 690 */	"regx: ADDU2(BANDU2(ac,conFF00),conB)",
/* 691 */	"regx: ADDU2(BANDU2(reg,conFF00),conB)",
/* 692 */	"regx: ADDU2(BANDU2(reg,conFF00),conB)",
/* 693 */	"regx: ADDU2(ac0,conXX00)",
/* 694 */	"ac: ADDU2(ac0,conXX00)",
/* 695 */	"asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),CVUI2(LOADU1(ac))))",
/* 696 */	"asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),CVUI2(LOADU1(ac))))",
/* 697 */	"asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),CVUI2(LOADU1(ac))))",
/* 698 */	"asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),BANDU2(ac,conFF00)))",
/* 699 */	"asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),BANDU2(ac,conFF00)))",
/* 700 */	"asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),BANDU2(ac,conFF00)))",
/* 701 */	"asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),BANDU2(regx,conFF00)))",
/* 702 */	"asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),BANDU2(regx,conFF00)))",
/* 703 */	"asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),BANDU2(regx,conFF00)))",
/* 704 */	"asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8)))",
/* 705 */	"asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8)))",
/* 706 */	"asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8)))",
/* 707 */	"asgn: ASGNU2(rmw,BANDU2(INDIRU2(rmw),conFF00))",
/* 708 */	"asgn: ASGNI2(rmw,BANDU2(INDIRI2(rmw),conFF00))",
/* 709 */	"asgn: ASGNP2(rmw,BANDU2(INDIRP2(rmw),conFF00))",
/* 710 */	"asgn: ASGNU2(rmw,BANDU2(INDIRU2(rmw),conFF00))",
/* 711 */	"asgn: ASGNI2(rmw,BANDU2(INDIRI2(rmw),conFF00))",
/* 712 */	"asgn: ASGNP2(rmw,BANDU2(INDIRP2(rmw),conFF00))",
/* 713 */	"asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),conB))",
/* 714 */	"asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),conB))",
/* 715 */	"asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),conB))",
/* 716 */	"asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),conB))",
/* 717 */	"asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),conB))",
/* 718 */	"asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),conB))",
/* 719 */	"asgn: ASGNU2(rmw,CVUI2(LOADU1(INDIRU2(rmw))))",
/* 720 */	"asgn: ASGNI2(rmw,CVUI2(LOADU1(INDIRI2(rmw))))",
/* 721 */	"asgn: ASGNP2(rmw,CVUI2(LOADU1(INDIRP2(rmw))))",
/* 722 */	"asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),conXX00))",
/* 723 */	"asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),conXX00))",
/* 724 */	"asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),conXX00))",
/* 725 */	"asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),conXX00))",
/* 726 */	"asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),conXX00))",
/* 727 */	"asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),conXX00))",
};

static short _decode_stmt[] = {
	0,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	58,
	59,
	60,
	242,
	243,
	244,
	245,
	246,
	247,
	248,
	249,
	250,
	251,
	252,
	253,
	254,
	255,
	256,
	257,
	258,
	259,
	260,
	261,
	262,
	263,
	264,
	265,
	266,
	267,
	268,
	269,
	270,
	271,
	272,
	273,
	274,
	275,
	276,
	277,
	278,
	279,
	280,
	281,
	282,
	283,
	284,
	285,
	286,
	287,
	299,
	354,
	355,
	356,
	357,
	358,
	359,
	360,
	361,
	362,
	363,
	364,
	365,
	366,
	367,
	368,
	369,
	370,
	371,
	372,
	373,
	374,
	375,
	376,
	377,
	388,
	408,
	409,
	410,
	411,
	412,
	413,
	414,
	415,
	416,
	417,
	418,
	419,
	443,
	444,
	445,
	446,
	447,
	448,
	449,
	450,
	451,
	452,
	453,
	454,
	455,
	456,
	457,
	458,
	459,
	460,
	461,
	462,
	463,
	526,
	527,
	528,
	529,
	629,
	630,
	631,
};

static short _decode_reg[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	62,
	63,
	303,
	304,
	305,
	306,
	307,
	308,
	309,
	391,
	392,
	393,
	394,
	490,
	491,
	492,
	493,
	494,
	495,
	496,
	512,
	513,
	514,
	515,
	516,
	583,
	584,
	585,
	586,
	587,
	588,
	589,
	606,
	607,
	608,
	658,
	661,
	662,
	667,
	668,
	669,
	670,
	671,
	672,
	675,
	676,
};

static short _decode_con0[] = {
	0,
	17,
	18,
	19,
	20,
	21,
};

static short _decode_con1[] = {
	0,
	22,
	23,
	24,
	25,
};

static short _decode_conB[] = {
	0,
	26,
	27,
	28,
	29,
	30,
};

static short _decode_zddr[] = {
	0,
	56,
	57,
};

static short _decode_conBs[] = {
	0,
	31,
	32,
};

static short _decode_conBn[] = {
	0,
	33,
};

static short _decode_conBm[] = {
	0,
	34,
	35,
};

static short _decode_conBa[] = {
	0,
	36,
	37,
};

static short _decode_con[] = {
	0,
	38,
	39,
	40,
	41,
	42,
	43,
};

static short _decode_addr[] = {
	0,
	53,
	54,
	55,
};

static short _decode_con8[] = {
	0,
	44,
	45,
};

static short _decode_conFF[] = {
	0,
	46,
	47,
};

static short _decode_conFF00[] = {
	0,
	48,
};

static short _decode_conXX00[] = {
	0,
	49,
	50,
};

static short _decode_lddr[] = {
	0,
	51,
	52,
};

static short _decode_asgn[] = {
	0,
	232,
	233,
	234,
	235,
	236,
	237,
	238,
	239,
	240,
	241,
	295,
	296,
	297,
	298,
	378,
	379,
	380,
	381,
	382,
	383,
	384,
	385,
	386,
	387,
	420,
	421,
	422,
	423,
	424,
	530,
	531,
	532,
	533,
	534,
	535,
	560,
	561,
	562,
	563,
	564,
	565,
	566,
	567,
	568,
	569,
	570,
	571,
	572,
	590,
	591,
	592,
	593,
	594,
	595,
	596,
	597,
	618,
	619,
	620,
	626,
	627,
	628,
	634,
	635,
	636,
	637,
	638,
	639,
	640,
	641,
	642,
	643,
	644,
	645,
	646,
	647,
	648,
	649,
	650,
	651,
	652,
	653,
	654,
	655,
	695,
	696,
	697,
	698,
	699,
	700,
	701,
	702,
	703,
	704,
	705,
	706,
	707,
	708,
	709,
	710,
	711,
	712,
	713,
	714,
	715,
	716,
	717,
	718,
	719,
	720,
	721,
	722,
	723,
	724,
	725,
	726,
	727,
};

static short _decode_ac[] = {
	0,
	66,
	67,
	82,
	83,
	84,
	95,
	96,
	97,
	98,
	99,
	100,
	101,
	102,
	103,
	104,
	105,
	106,
	107,
	108,
	109,
	110,
	111,
	112,
	113,
	114,
	115,
	116,
	117,
	118,
	119,
	120,
	121,
	122,
	123,
	124,
	125,
	126,
	127,
	128,
	129,
	130,
	131,
	132,
	133,
	134,
	135,
	136,
	137,
	138,
	139,
	140,
	141,
	142,
	143,
	144,
	145,
	146,
	147,
	148,
	149,
	150,
	151,
	152,
	153,
	154,
	155,
	156,
	157,
	158,
	161,
	162,
	163,
	164,
	165,
	166,
	169,
	170,
	171,
	172,
	173,
	174,
	434,
	435,
	436,
	437,
	438,
	439,
	440,
	441,
	442,
	466,
	467,
	470,
	471,
	472,
	499,
	501,
	517,
	521,
	538,
	539,
	540,
	541,
	542,
	543,
	550,
	551,
	552,
	553,
	612,
	613,
	614,
	659,
	660,
	663,
	664,
	665,
	666,
	677,
	678,
	679,
	680,
	694,
};

static short _decode_regx[] = {
	0,
	61,
	536,
	537,
	598,
	599,
	600,
	601,
	602,
	603,
	604,
	605,
	681,
	682,
	683,
	684,
	685,
	686,
	687,
	688,
	689,
	690,
	691,
	692,
	693,
};

static short _decode_ac0[] = {
	0,
	64,
	85,
	86,
	159,
	160,
	167,
	168,
	175,
	176,
	468,
	469,
	473,
	474,
	475,
	500,
	502,
	656,
	657,
	673,
	674,
};

static short _decode_eac0[] = {
	0,
	70,
	75,
	76,
	80,
	81,
	197,
	198,
	201,
	202,
	205,
	206,
	464,
	465,
	498,
	504,
	506,
	576,
	577,
	581,
	582,
};

static short _decode_eac[] = {
	0,
	65,
	68,
	69,
	71,
	72,
	73,
	74,
	77,
	78,
	79,
	177,
	178,
	179,
	180,
	181,
	182,
	183,
	184,
	185,
	186,
	187,
	188,
	189,
	190,
	191,
	192,
	193,
	194,
	195,
	196,
	199,
	200,
	203,
	204,
	207,
	208,
	209,
	210,
	211,
	212,
	213,
	214,
	215,
	216,
	217,
	218,
	219,
	220,
	221,
	222,
	223,
	224,
	225,
	226,
	227,
	228,
	229,
	230,
	231,
	476,
	477,
	478,
	479,
	480,
	481,
	482,
	483,
	484,
	485,
	486,
	497,
	503,
	505,
	544,
	545,
	546,
	547,
	548,
	549,
	554,
	555,
	556,
	557,
	558,
	573,
	574,
	575,
	578,
	579,
	580,
	609,
	610,
	611,
	615,
	616,
	617,
	621,
	622,
	623,
	624,
	625,
};

static short _decode_iarg[] = {
	0,
	87,
	88,
	89,
	90,
	92,
	93,
	94,
};

static short _decode_spill[] = {
	0,
	91,
};

static short _decode_vdst[] = {
	0,
	288,
	289,
	290,
};

static short _decode_asrc[] = {
	0,
	291,
	292,
	293,
};

static short _decode_lsrc[] = {
	0,
	294,
};

static short _decode_lac[] = {
	0,
	310,
	311,
	312,
	313,
	314,
	315,
	316,
	317,
	318,
	319,
	320,
	321,
	322,
	323,
	324,
	325,
	326,
	327,
	328,
	329,
	330,
	331,
	332,
	333,
	334,
	335,
	336,
	337,
	338,
	339,
	340,
	341,
	342,
	343,
	344,
	345,
	346,
	347,
	348,
	349,
	350,
	351,
	352,
	353,
	428,
	429,
	430,
	431,
	432,
	433,
	487,
	488,
	507,
	508,
	509,
	510,
	511,
	518,
	522,
	559,
};

static short _decode_larg[] = {
	0,
	300,
	301,
	302,
};

static short _decode_fac[] = {
	0,
	395,
	396,
	397,
	398,
	399,
	400,
	401,
	402,
	403,
	404,
	405,
	406,
	407,
	425,
	426,
	427,
	489,
	519,
	520,
	523,
	524,
	525,
};

static short _decode_farg[] = {
	0,
	389,
	390,
};

static short _decode_rmw[] = {
	0,
	632,
	633,
};

static int _rule(void *state, int goalnt) {
	if (goalnt < 1 || goalnt > 33)
		fatal("_rule", "Bad goal nonterminal %d\n", goalnt);
	if (!state)
		return 0;
	switch (goalnt) {
	case _stmt_NT:	return _decode_stmt[((struct _state *)state)->rule._stmt];
	case _reg_NT:	return _decode_reg[((struct _state *)state)->rule._reg];
	case _con0_NT:	return _decode_con0[((struct _state *)state)->rule._con0];
	case _con1_NT:	return _decode_con1[((struct _state *)state)->rule._con1];
	case _conB_NT:	return _decode_conB[((struct _state *)state)->rule._conB];
	case _zddr_NT:	return _decode_zddr[((struct _state *)state)->rule._zddr];
	case _conBs_NT:	return _decode_conBs[((struct _state *)state)->rule._conBs];
	case _conBn_NT:	return _decode_conBn[((struct _state *)state)->rule._conBn];
	case _conBm_NT:	return _decode_conBm[((struct _state *)state)->rule._conBm];
	case _conBa_NT:	return _decode_conBa[((struct _state *)state)->rule._conBa];
	case _con_NT:	return _decode_con[((struct _state *)state)->rule._con];
	case _addr_NT:	return _decode_addr[((struct _state *)state)->rule._addr];
	case _con8_NT:	return _decode_con8[((struct _state *)state)->rule._con8];
	case _conFF_NT:	return _decode_conFF[((struct _state *)state)->rule._conFF];
	case _conFF00_NT:	return _decode_conFF00[((struct _state *)state)->rule._conFF00];
	case _conXX00_NT:	return _decode_conXX00[((struct _state *)state)->rule._conXX00];
	case _lddr_NT:	return _decode_lddr[((struct _state *)state)->rule._lddr];
	case _asgn_NT:	return _decode_asgn[((struct _state *)state)->rule._asgn];
	case _ac_NT:	return _decode_ac[((struct _state *)state)->rule._ac];
	case _regx_NT:	return _decode_regx[((struct _state *)state)->rule._regx];
	case _ac0_NT:	return _decode_ac0[((struct _state *)state)->rule._ac0];
	case _eac0_NT:	return _decode_eac0[((struct _state *)state)->rule._eac0];
	case _eac_NT:	return _decode_eac[((struct _state *)state)->rule._eac];
	case _iarg_NT:	return _decode_iarg[((struct _state *)state)->rule._iarg];
	case _spill_NT:	return _decode_spill[((struct _state *)state)->rule._spill];
	case _vdst_NT:	return _decode_vdst[((struct _state *)state)->rule._vdst];
	case _asrc_NT:	return _decode_asrc[((struct _state *)state)->rule._asrc];
	case _lsrc_NT:	return _decode_lsrc[((struct _state *)state)->rule._lsrc];
	case _lac_NT:	return _decode_lac[((struct _state *)state)->rule._lac];
	case _larg_NT:	return _decode_larg[((struct _state *)state)->rule._larg];
	case _fac_NT:	return _decode_fac[((struct _state *)state)->rule._fac];
	case _farg_NT:	return _decode_farg[((struct _state *)state)->rule._farg];
	case _rmw_NT:	return _decode_rmw[((struct _state *)state)->rule._rmw];
	default:
		fatal("_rule", "Bad goal nonterminal %d\n", goalnt);
		return 0;
	}
}

static void _closure_reg(NODEPTR_TYPE, int);
static void _closure_conB(NODEPTR_TYPE, int);
static void _closure_zddr(NODEPTR_TYPE, int);
static void _closure_con(NODEPTR_TYPE, int);
static void _closure_addr(NODEPTR_TYPE, int);
static void _closure_lddr(NODEPTR_TYPE, int);
static void _closure_asgn(NODEPTR_TYPE, int);
static void _closure_ac(NODEPTR_TYPE, int);
static void _closure_regx(NODEPTR_TYPE, int);
static void _closure_ac0(NODEPTR_TYPE, int);
static void _closure_eac0(NODEPTR_TYPE, int);
static void _closure_eac(NODEPTR_TYPE, int);
static void _closure_lac(NODEPTR_TYPE, int);
static void _closure_fac(NODEPTR_TYPE, int);

static void _closure_reg(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 179;
	if (d + 0 < p->cost[_fac_NT]) {
		p->cost[_fac_NT] = d + 0;
		p->rule._fac = 1;
		_closure_fac(a, d + 0);
	}
	d = c + 120;
	if (d + 0 < p->cost[_lac_NT]) {
		p->cost[_lac_NT] = d + 0;
		p->rule._lac = 1;
		_closure_lac(a, d + 0);
	}
	d = c + (+if_asgnreuse(a,20,10));
	if (d + 0 < p->cost[_eac_NT]) {
		p->cost[_eac_NT] = d + 0;
		p->rule._eac = 2;
		_closure_eac(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_regx_NT]) {
		p->cost[_regx_NT] = d + 0;
		p->rule._regx = 1;
		_closure_regx(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_stmt_NT]) {
		p->cost[_stmt_NT] = d + 0;
		p->rule._stmt = 9;
	}
}

static void _closure_conB(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + (+mincpu7(28+4));
	if (d + 0 < p->cost[_reg_NT]) {
		p->cost[_reg_NT] = d + 0;
		p->rule._reg = 34;
		_closure_reg(a, d + 0);
	}
	d = c + (+mincpu6(28));
	if (d + 0 < p->cost[_regx_NT]) {
		p->cost[_regx_NT] = d + 0;
		p->rule._regx = 3;
		_closure_regx(a, d + 0);
	}
	d = c + (+if_asgnreuse(a,16,8));
	if (d + 0 < p->cost[_eac0_NT]) {
		p->cost[_eac0_NT] = d + 0;
		p->rule._eac0 = 1;
		_closure_eac0(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_zddr_NT]) {
		p->cost[_zddr_NT] = d + 0;
		p->rule._zddr = 2;
		_closure_zddr(a, d + 0);
	}
}

static void _closure_zddr(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 0;
	if (d + 0 < p->cost[_rmw_NT]) {
		p->cost[_rmw_NT] = d + 0;
		p->rule._rmw = 2;
	}
	d = c + 0;
	if (d + 0 < p->cost[_addr_NT]) {
		p->cost[_addr_NT] = d + 0;
		p->rule._addr = 3;
		_closure_addr(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_conB_NT]) {
		p->cost[_conB_NT] = d + 0;
		p->rule._conB = 5;
		_closure_conB(a, d + 0);
	}
}

static void _closure_con(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + (+mincpu7(30+4));
	if (d + 0 < p->cost[_reg_NT]) {
		p->cost[_reg_NT] = d + 0;
		p->rule._reg = 35;
		_closure_reg(a, d + 0);
	}
	d = c + (+mincpu7(30));
	if (d + 0 < p->cost[_regx_NT]) {
		p->cost[_regx_NT] = d + 0;
		p->rule._regx = 2;
		_closure_regx(a, d + 0);
	}
	d = c + (+if_asgnreuse(a,20,10));
	if (d + 0 < p->cost[_eac_NT]) {
		p->cost[_eac_NT] = d + 0;
		p->rule._eac = 4;
		_closure_eac(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_addr_NT]) {
		p->cost[_addr_NT] = d + 0;
		p->rule._addr = 2;
		_closure_addr(a, d + 0);
	}
}

static void _closure_addr(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 0;
	if (d + 0 < p->cost[_lsrc_NT]) {
		p->cost[_lsrc_NT] = d + 0;
		p->rule._lsrc = 1;
	}
	d = c + 0;
	if (d + 0 < p->cost[_asrc_NT]) {
		p->cost[_asrc_NT] = d + 0;
		p->rule._asrc = 1;
	}
	d = c + 0;
	if (d + 0 < p->cost[_vdst_NT]) {
		p->cost[_vdst_NT] = d + 0;
		p->rule._vdst = 1;
	}
	d = c + 0;
	if (d + 0 < p->cost[_con_NT]) {
		p->cost[_con_NT] = d + 0;
		p->rule._con = 6;
		_closure_con(a, d + 0);
	}
}

static void _closure_lddr(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 40;
	if (d + 0 < p->cost[_asrc_NT]) {
		p->cost[_asrc_NT] = d + 0;
		p->rule._asrc = 2;
	}
	d = c + 60;
	if (d + 0 < p->cost[_vdst_NT]) {
		p->cost[_vdst_NT] = d + 0;
		p->rule._vdst = 2;
	}
	d = c + 41;
	if (d + 0 < p->cost[_eac_NT]) {
		p->cost[_eac_NT] = d + 0;
		p->rule._eac = 3;
		_closure_eac(a, d + 0);
	}
}

static void _closure_asgn(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 0;
	if (d + 0 < p->cost[_stmt_NT]) {
		p->cost[_stmt_NT] = d + 0;
		p->rule._stmt = 10;
	}
}

static void _closure_ac(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 19;
	if (d + 0 < p->cost[_reg_NT]) {
		p->cost[_reg_NT] = d + 0;
		p->rule._reg = 10;
		_closure_reg(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_stmt_NT]) {
		p->cost[_stmt_NT] = d + 0;
		p->rule._stmt = 11;
	}
}

static void _closure_regx(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 21;
	if (d + 0 < p->cost[_farg_NT]) {
		p->cost[_farg_NT] = d + 0;
		p->rule._farg = 1;
	}
	d = c + 21;
	if (d + 0 < p->cost[_larg_NT]) {
		p->cost[_larg_NT] = d + 0;
		p->rule._larg = 1;
	}
	d = c + 0;
	if (d + 0 < p->cost[_iarg_NT]) {
		p->cost[_iarg_NT] = d + 0;
		p->rule._iarg = 1;
	}
	d = c + 0;
	if (d + 0 < p->cost[_reg_NT]) {
		p->cost[_reg_NT] = d + 0;
		p->rule._reg = 9;
		_closure_reg(a, d + 0);
	}
}

static void _closure_ac0(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 0;
	if (d + 0 < p->cost[_ac_NT]) {
		p->cost[_ac_NT] = d + 0;
		p->rule._ac = 1;
		_closure_ac(a, d + 0);
	}
}

static void _closure_eac0(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 0;
	if (d + 0 < p->cost[_eac_NT]) {
		p->cost[_eac_NT] = d + 0;
		p->rule._eac = 1;
		_closure_eac(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_ac0_NT]) {
		p->cost[_ac0_NT] = d + 0;
		p->rule._ac0 = 1;
		_closure_ac0(a, d + 0);
	}
}

static void _closure_eac(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 0;
	if (d + 0 < p->cost[_asrc_NT]) {
		p->cost[_asrc_NT] = d + 0;
		p->rule._asrc = 3;
	}
	d = c + 20;
	if (d + 0 < p->cost[_vdst_NT]) {
		p->cost[_vdst_NT] = d + 0;
		p->rule._vdst = 3;
	}
	d = c + 0;
	if (d + 0 < p->cost[_ac_NT]) {
		p->cost[_ac_NT] = d + 0;
		p->rule._ac = 2;
		_closure_ac(a, d + 0);
	}
}

static void _closure_lac(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 119;
	if (d + 0 < p->cost[_reg_NT]) {
		p->cost[_reg_NT] = d + 0;
		p->rule._reg = 11;
		_closure_reg(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_stmt_NT]) {
		p->cost[_stmt_NT] = d + 0;
		p->rule._stmt = 58;
	}
}

static void _closure_fac(NODEPTR_TYPE a, int c) {
	struct _state *p = STATE_LABEL(a);
	int d;
	d = c + 179;
	if (d + 0 < p->cost[_reg_NT]) {
		p->cost[_reg_NT] = d + 0;
		p->rule._reg = 18;
		_closure_reg(a, d + 0);
	}
	d = c + 0;
	if (d + 0 < p->cost[_stmt_NT]) {
		p->cost[_stmt_NT] = d + 0;
		p->rule._stmt = 83;
	}
}

static void _label(NODEPTR_TYPE a) {
	int c;
	struct _state *p;

	if (!a)
		fatal("_label", "Null tree\n", 0);
	STATE_LABEL(a) = p = allocate(sizeof *p, FUNC);
	p->rule._stmt = 0;
	p->cost[1] =
	p->cost[2] =
	p->cost[3] =
	p->cost[4] =
	p->cost[5] =
	p->cost[6] =
	p->cost[7] =
	p->cost[8] =
	p->cost[9] =
	p->cost[10] =
	p->cost[11] =
	p->cost[12] =
	p->cost[13] =
	p->cost[14] =
	p->cost[15] =
	p->cost[16] =
	p->cost[17] =
	p->cost[18] =
	p->cost[19] =
	p->cost[20] =
	p->cost[21] =
	p->cost[22] =
	p->cost[23] =
	p->cost[24] =
	p->cost[25] =
	p->cost[26] =
	p->cost[27] =
	p->cost[28] =
	p->cost[29] =
	p->cost[30] =
	p->cost[31] =
	p->cost[32] =
	p->cost[33] =
		0x7fff;
	switch (OP_LABEL(a)) {
	case 41: /* ARGB */
		_label(LEFT_CHILD(a));
		if (	/* asgn: ARGB(INDIRB(asrc)) */
			LEFT_CHILD(a)->op == 73 /* INDIRB */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_asrc_NT] + 200;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 11;
				_closure_asgn(a, c + 0);
			}
		}
		break;
	case 57: /* ASGNB */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* asgn: ASGNB(addr,INDIRB(asrc)) */
			RIGHT_CHILD(a)->op == 73 /* INDIRB */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_asrc_NT] + 200;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 12;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNB(ac,INDIRB(asrc)) */
			RIGHT_CHILD(a)->op == 73 /* INDIRB */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_asrc_NT] + 200;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 13;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNB(lddr,INDIRB(lsrc)) */
			RIGHT_CHILD(a)->op == 73 /* INDIRB */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_lsrc_NT] + 200;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 14;
				_closure_asgn(a, c + 0);
			}
		}
		break;
	case 73: /* INDIRB */
		_label(LEFT_CHILD(a));
		break;
	case 216: /* CALLV */
		_label(LEFT_CHILD(a));
		/* stmt: CALLV(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + (mincpu5(28));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 96;
		}
		/* stmt: CALLV(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 26;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 97;
		}
		/* stmt: CALLV(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 98;
		}
		break;
	case 217: /* CALLB */
		break;
	case 233: /* LOADB */
		break;
	case 248: /* RETV */
		break;
	case 584: /* JUMPV */
		_label(LEFT_CHILD(a));
		/* stmt: JUMPV(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + 14;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 118;
		}
		/* stmt: JUMPV(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 14;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 119;
		}
		/* stmt: JUMPV(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 14;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 120;
		}
		break;
	case 600: /* LABELV */
		/* stmt: LABELV */
		if (0 + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = 0 + 0;
			p->rule._stmt = 117;
		}
		break;
	case 711: /* VREGP */
		/* rmw: VREGP */
		if (0 + 0 < p->cost[_rmw_NT]) {
			p->cost[_rmw_NT] = 0 + 0;
			p->rule._rmw = 1;
		}
		break;
	case 1045: /* CNSTI1 */
		/* con0: CNSTI1 */
		c = (range(a,0,0));
		if (c + 0 < p->cost[_con0_NT]) {
			p->cost[_con0_NT] = c + 0;
			p->rule._con0 = 1;
		}
		/* con1: CNSTI1 */
		c = (range(a,1,1));
		if (c + 0 < p->cost[_con1_NT]) {
			p->cost[_con1_NT] = c + 0;
			p->rule._con1 = 1;
		}
		/* conBs: CNSTI1 */
		if (0 + 0 < p->cost[_conBs_NT]) {
			p->cost[_conBs_NT] = 0 + 0;
			p->rule._conBs = 1;
		}
		/* con: CNSTI1 */
		if (0 + 0 < p->cost[_con_NT]) {
			p->cost[_con_NT] = 0 + 0;
			p->rule._con = 1;
			_closure_con(a, 0 + 0);
		}
		break;
	case 1046: /* CNSTU1 */
		/* con0: CNSTU1 */
		c = (range(a,0,0));
		if (c + 0 < p->cost[_con0_NT]) {
			p->cost[_con0_NT] = c + 0;
			p->rule._con0 = 2;
		}
		/* con1: CNSTU1 */
		c = (range(a,1,1));
		if (c + 0 < p->cost[_con1_NT]) {
			p->cost[_con1_NT] = c + 0;
			p->rule._con1 = 2;
		}
		/* conB: CNSTU1 */
		if (0 + 0 < p->cost[_conB_NT]) {
			p->cost[_conB_NT] = 0 + 0;
			p->rule._conB = 4;
			_closure_conB(a, 0 + 0);
		}
		/* con: CNSTU1 */
		if (0 + 0 < p->cost[_con_NT]) {
			p->cost[_con_NT] = 0 + 0;
			p->rule._con = 2;
			_closure_con(a, 0 + 0);
		}
		break;
	case 1077: /* ASGNI1 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNI1(VREGP,reg) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			c = ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 1;
			}
		}
		/* asgn: ASGNI1(zddr,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 16;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 7;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI1(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 8;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI1(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 39;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI1(ac,conBs) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBs_NT] + (mincpu6(19));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 47;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI1(rmw,conBs) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBs_NT] + (mincpu6(if_not_asgn_tmp(a,27)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 49;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNI1(rmw,LOADI1(ADDI2(CVII2(INDIRI1(rmw)),con1))) */
			RIGHT_CHILD(a)->op == 1253 && /* LOADI1 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2357 && /* ADDI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2181 && /* CVII2 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 1093 /* INDIRI1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_con1_NT] + (if_rmw(a,16));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 64;
				_closure_asgn(a, c + 0);
			}
		}
		break;
	case 1078: /* ASGNU1 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNU1(VREGP,reg) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			c = ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 2;
			}
		}
		/* asgn: ASGNU1(zddr,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 16;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 9;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU1(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 10;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU1(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 40;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU1(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(19));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 48;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU1(rmw,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(if_not_asgn_tmp(a,27)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 50;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNU1(rmw,LOADU1(ADDI2(CVUI2(INDIRU1(rmw)),con1))) */
			RIGHT_CHILD(a)->op == 1254 && /* LOADU1 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2357 && /* ADDI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 1094 /* INDIRU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_con1_NT] + (if_rmw(a,16));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 63;
				_closure_asgn(a, c + 0);
			}
		}
		break;
	case 1093: /* INDIRI1 */
		_label(LEFT_CHILD(a));
		if (	/* reg: INDIRI1(VREGP) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			if (mayrecalc(a)) {
				struct _state *q = a->syms[RX]->u.t.cse->x.state;
				*p = *q;
			}
			c = 0;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 1;
				_closure_reg(a, c + 0);
			}
		}
		/* eac0: INDIRI1(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 17;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 2;
			_closure_eac0(a, c + 0);
		}
		/* eac0: INDIRI1(zddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + 18;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 4;
			_closure_eac0(a, c + 0);
		}
		/* ac0: INDIRI1(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 17;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 2;
			_closure_ac0(a, c + 0);
		}
		/* eac0: INDIRI1(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 17;
			_closure_eac0(a, c + 0);
		}
		if (	/* eac0: INDIRI1(INDIRP2(zddr)) */
			LEFT_CHILD(a)->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_zddr_NT] + (mincpu6(28));
			if (c + 0 < p->cost[_eac0_NT]) {
				p->cost[_eac0_NT] = c + 0;
				p->rule._eac0 = 19;
				_closure_eac0(a, c + 0);
			}
		}
		/* reg: INDIRI1(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu6(24+5));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 39;
			_closure_reg(a, c + 0);
		}
		/* eac: INDIRI1(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + (mincpu7(if_zoffset(a,38,58)));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 97;
			_closure_eac(a, c + 0);
		}
		break;
	case 1094: /* INDIRU1 */
		_label(LEFT_CHILD(a));
		if (	/* reg: INDIRU1(VREGP) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			if (mayrecalc(a)) {
				struct _state *q = a->syms[RX]->u.t.cse->x.state;
				*p = *q;
			}
			c = 0;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 2;
				_closure_reg(a, c + 0);
			}
		}
		/* eac0: INDIRU1(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 17;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 3;
			_closure_eac0(a, c + 0);
		}
		/* eac0: INDIRU1(zddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + 18;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 5;
			_closure_eac0(a, c + 0);
		}
		/* ac0: INDIRU1(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 17;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 3;
			_closure_ac0(a, c + 0);
		}
		/* eac0: INDIRU1(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 18;
			_closure_eac0(a, c + 0);
		}
		if (	/* eac0: INDIRU1(INDIRP2(zddr)) */
			LEFT_CHILD(a)->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_zddr_NT] + (mincpu6(28));
			if (c + 0 < p->cost[_eac0_NT]) {
				p->cost[_eac0_NT] = c + 0;
				p->rule._eac0 = 20;
				_closure_eac0(a, c + 0);
			}
		}
		/* reg: INDIRU1(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu6(24+5));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 40;
			_closure_reg(a, c + 0);
		}
		/* eac: INDIRU1(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + (mincpu7(if_zoffset(a,38,58)));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 98;
			_closure_eac(a, c + 0);
		}
		break;
	case 1157: /* CVII1 */
		break;
	case 1158: /* CVIU1 */
		break;
	case 1205: /* CVUI1 */
		break;
	case 1206: /* CVUU1 */
		break;
	case 1253: /* LOADI1 */
		_label(LEFT_CHILD(a));
		/* eac0: LOADI1(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 22;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 12;
			_closure_eac0(a, c + 0);
		}
		/* ac: LOADI1(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 0;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 91;
			_closure_ac(a, c + 0);
		}
		/* ac0: LOADI1(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + 0;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 10;
			_closure_ac0(a, c + 0);
		}
		/* eac: LOADI1(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 0;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 60;
			_closure_eac(a, c + 0);
		}
		/* reg: LOADI1(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 38;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 22;
			_closure_reg(a, c + 0);
		}
		/* reg: LOADI1(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 16;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 24;
			_closure_reg(a, c + 0);
		}
		/* regx: LOADI1(conBs) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conBs_NT] + (mincpu6(27));
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 4;
			_closure_regx(a, c + 0);
		}
		break;
	case 1254: /* LOADU1 */
		_label(LEFT_CHILD(a));
		/* eac0: LOADU1(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 22;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 13;
			_closure_eac0(a, c + 0);
		}
		/* ac: LOADU1(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 0;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 92;
			_closure_ac(a, c + 0);
		}
		/* ac0: LOADU1(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + 0;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 11;
			_closure_ac0(a, c + 0);
		}
		/* eac: LOADU1(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 0;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 61;
			_closure_eac(a, c + 0);
		}
		/* reg: LOADU1(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 38;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 23;
			_closure_reg(a, c + 0);
		}
		/* reg: LOADU1(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 16;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 25;
			_closure_reg(a, c + 0);
		}
		/* regx: LOADU1(conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(27));
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 5;
			_closure_regx(a, c + 0);
		}
		break;
	case 2069: /* CNSTI2 */
		/* con0: CNSTI2 */
		c = (range(a,0,0));
		if (c + 0 < p->cost[_con0_NT]) {
			p->cost[_con0_NT] = c + 0;
			p->rule._con0 = 3;
		}
		/* con1: CNSTI2 */
		c = (range(a,1,1));
		if (c + 0 < p->cost[_con1_NT]) {
			p->cost[_con1_NT] = c + 0;
			p->rule._con1 = 3;
		}
		/* conB: CNSTI2 */
		c = (range(a,0,255));
		if (c + 0 < p->cost[_conB_NT]) {
			p->cost[_conB_NT] = c + 0;
			p->rule._conB = 1;
			_closure_conB(a, c + 0);
		}
		/* conBs: CNSTI2 */
		c = (range(a,-128,+127));
		if (c + 0 < p->cost[_conBs_NT]) {
			p->cost[_conBs_NT] = c + 0;
			p->rule._conBs = 2;
		}
		/* conBn: CNSTI2 */
		c = (range(a,-255,-1));
		if (c + 0 < p->cost[_conBn_NT]) {
			p->cost[_conBn_NT] = c + 0;
			p->rule._conBn = 1;
		}
		/* conBm: CNSTI2 */
		c = (range(a,0,52));
		if (c + 0 < p->cost[_conBm_NT]) {
			p->cost[_conBm_NT] = c + 0;
			p->rule._conBm = 1;
		}
		/* conBa: CNSTI2 */
		c = (range(a,-64,64));
		if (c + 0 < p->cost[_conBa_NT]) {
			p->cost[_conBa_NT] = c + 0;
			p->rule._conBa = 1;
		}
		/* con: CNSTI2 */
		if (0 + 0 < p->cost[_con_NT]) {
			p->cost[_con_NT] = 0 + 0;
			p->rule._con = 3;
			_closure_con(a, 0 + 0);
		}
		/* con8: CNSTI2 */
		c = (range(a,8,8));
		if (c + 0 < p->cost[_con8_NT]) {
			p->cost[_con8_NT] = c + 0;
			p->rule._con8 = 2;
		}
		/* conFF: CNSTI2 */
		c = (range(a,0xff,0xff));
		if (c + 0 < p->cost[_conFF_NT]) {
			p->cost[_conFF_NT] = c + 0;
			p->rule._conFF = 2;
		}
		/* conXX00: CNSTI2 */
		c = (if_zlconst(a));
		if (c + 0 < p->cost[_conXX00_NT]) {
			p->cost[_conXX00_NT] = c + 0;
			p->rule._conXX00 = 2;
		}
		break;
	case 2070: /* CNSTU2 */
		/* con0: CNSTU2 */
		c = (range(a,0,0));
		if (c + 0 < p->cost[_con0_NT]) {
			p->cost[_con0_NT] = c + 0;
			p->rule._con0 = 4;
		}
		/* con1: CNSTU2 */
		c = (range(a,1,1));
		if (c + 0 < p->cost[_con1_NT]) {
			p->cost[_con1_NT] = c + 0;
			p->rule._con1 = 4;
		}
		/* conB: CNSTU2 */
		c = (range(a,0,255));
		if (c + 0 < p->cost[_conB_NT]) {
			p->cost[_conB_NT] = c + 0;
			p->rule._conB = 2;
			_closure_conB(a, c + 0);
		}
		/* conBm: CNSTU2 */
		c = (range(a,0,52));
		if (c + 0 < p->cost[_conBm_NT]) {
			p->cost[_conBm_NT] = c + 0;
			p->rule._conBm = 2;
		}
		/* conBa: CNSTU2 */
		c = (range(a,-64,64));
		if (c + 0 < p->cost[_conBa_NT]) {
			p->cost[_conBa_NT] = c + 0;
			p->rule._conBa = 2;
		}
		/* con: CNSTU2 */
		if (0 + 0 < p->cost[_con_NT]) {
			p->cost[_con_NT] = 0 + 0;
			p->rule._con = 4;
			_closure_con(a, 0 + 0);
		}
		/* con8: CNSTU2 */
		c = (range(a,8,8));
		if (c + 0 < p->cost[_con8_NT]) {
			p->cost[_con8_NT] = c + 0;
			p->rule._con8 = 1;
		}
		/* conFF: CNSTU2 */
		c = (range(a,0xff,0xff));
		if (c + 0 < p->cost[_conFF_NT]) {
			p->cost[_conFF_NT] = c + 0;
			p->rule._conFF = 1;
		}
		/* conFF00: CNSTU2 */
		c = (range(a,0xff00,0xff00));
		if (c + 0 < p->cost[_conFF00_NT]) {
			p->cost[_conFF00_NT] = c + 0;
			p->rule._conFF00 = 1;
		}
		/* conXX00: CNSTU2 */
		c = (if_zlconst(a));
		if (c + 0 < p->cost[_conXX00_NT]) {
			p->cost[_conXX00_NT] = c + 0;
			p->rule._conXX00 = 1;
		}
		break;
	case 2071: /* CNSTP2 */
		/* con0: CNSTP2 */
		c = (range(a,0,0));
		if (c + 0 < p->cost[_con0_NT]) {
			p->cost[_con0_NT] = c + 0;
			p->rule._con0 = 5;
		}
		/* conB: CNSTP2 */
		c = (if_zpconst(a));
		if (c + 0 < p->cost[_conB_NT]) {
			p->cost[_conB_NT] = c + 0;
			p->rule._conB = 3;
			_closure_conB(a, c + 0);
		}
		/* con: CNSTP2 */
		if (0 + 0 < p->cost[_con_NT]) {
			p->cost[_con_NT] = 0 + 0;
			p->rule._con = 5;
			_closure_con(a, 0 + 0);
		}
		break;
	case 2085: /* ARGI2 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGI2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_stk(a,100));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 102;
		}
		/* stmt: ARGI2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_reg_only(a));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 108;
		}
		/* stmt: ARGI2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu7(if_arg_stk(a,50)));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 121;
		}
		break;
	case 2086: /* ARGU2 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGU2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_stk(a,100));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 103;
		}
		/* stmt: ARGU2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_reg_only(a));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 109;
		}
		/* stmt: ARGU2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu7(if_arg_stk(a,50)));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 122;
		}
		break;
	case 2087: /* ARGP2 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGP2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_stk(a,100));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 104;
		}
		/* stmt: ARGP2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_reg_only(a));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 110;
		}
		/* stmt: ARGP2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu7(if_arg_stk(a,50)));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 123;
		}
		break;
	case 2101: /* ASGNI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNI2(VREGP,reg) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			c = ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 3;
			}
		}
		/* asgn: ASGNI2(zddr,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 3;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 4;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI2(spill,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 30;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 37;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu6(29));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 42;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu7(21));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 45;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI2(rmw,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(if_not_asgn_tmp(a,29)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 51;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI2(rmw,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu7(if_not_asgn_tmp(a,31)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 54;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNI2(ADDP2(reg,con),ac) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 57;
				_closure_asgn(a, c + 0);
			}
		}
		/* asgn: ASGNI2(lddr,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(if_zoffset(a,38,58)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 60;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNI2(rmw,NEGI2(INDIRI2(rmw))) */
			RIGHT_CHILD(a)->op == 2245 && /* NEGI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + (mincpu6(if_rmw(a, 26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 65;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),con1)) */
			RIGHT_CHILD(a)->op == 2357 && /* ADDI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_con1_NT] + (mincpu6(if_rmw(a, 22)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 69;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),ac)) */
			RIGHT_CHILD(a)->op == 2357 && /* ADDI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + (mincpu7(if_rmw(a, 30)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 70;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDI2(ac,INDIRI2(rmw))) */
			RIGHT_CHILD(a)->op == 2357 && /* ADDI2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + (mincpu7(if_rmw_a(a, 1, 29)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 73;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,SUBI2(INDIRI2(rmw),ac)) */
			RIGHT_CHILD(a)->op == 2373 && /* SUBI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + (mincpu7(if_rmw(a, 29)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 76;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),conBa)) */
			RIGHT_CHILD(a)->op == 2357 && /* ADDI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conBa_NT] + (mincpu7(if_rmw(a, 38)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 81;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,SUBI2(INDIRI2(rmw),conBa)) */
			RIGHT_CHILD(a)->op == 2373 && /* SUBI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conBa_NT] + (mincpu7(if_rmw(a, 38)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 84;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2117 && /* INDIRI2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_ac_NT] + (if_rmw(a,16));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 86;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),BANDU2(ac,conFF00))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2117 && /* INDIRI2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 89;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),BANDU2(regx,conFF00))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2117 && /* INDIRI2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_regx_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 92;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2117 && /* INDIRI2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2390 && /* LSHU2 */
			LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a))))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_con8_NT] + (if_rmw(a,16));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 95;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,BANDU2(INDIRI2(rmw),conFF00)) */
			RIGHT_CHILD(a)->op == 2438 && /* BANDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 98;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,BANDU2(INDIRI2(rmw),conFF00)) */
			RIGHT_CHILD(a)->op == 2438 && /* BANDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conFF00_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 101;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),conB)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conB_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 104;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),conB)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conB_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 107;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,CVUI2(LOADU1(INDIRI2(rmw)))) */
			RIGHT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 110;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),conXX00)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conXX00_NT] + (if_rmw(a,34));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 113;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),conXX00)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2117 /* INDIRI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conXX00_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 116;
				_closure_asgn(a, c + 0);
			}
		}
		break;
	case 2102: /* ASGNU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNU2(VREGP,reg) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			c = ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 4;
			}
		}
		/* asgn: ASGNU2(zddr,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 5;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 6;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU2(spill,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 31;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 38;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu6(29));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 43;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu7(21));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 46;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU2(rmw,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(if_not_asgn_tmp(a,29)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 52;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU2(rmw,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu7(if_not_asgn_tmp(a,31)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 55;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNU2(ADDP2(reg,con),ac) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 58;
				_closure_asgn(a, c + 0);
			}
		}
		/* asgn: ASGNU2(lddr,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(if_zoffset(a,38,58)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 61;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),con1)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_con1_NT] + (mincpu6(if_rmw(a, 22)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 68;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),ac)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + (mincpu7(if_rmw(a, 30)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 71;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(ac,INDIRU2(rmw))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + (mincpu7(if_rmw_a(a, 1, 29)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 74;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,SUBU2(INDIRU2(rmw),ac)) */
			RIGHT_CHILD(a)->op == 2374 && /* SUBU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + (mincpu7(if_rmw(a, 29)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 77;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),conBa)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conBa_NT] + (mincpu7(if_rmw(a, 38)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 80;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,SUBU2(INDIRU2(rmw),conBa)) */
			RIGHT_CHILD(a)->op == 2374 && /* SUBU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conBa_NT] + (mincpu7(if_rmw(a, 38)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 83;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2118 && /* INDIRU2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_ac_NT] + (if_rmw(a,16));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 85;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),BANDU2(ac,conFF00))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2118 && /* INDIRU2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 88;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),BANDU2(regx,conFF00))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2118 && /* INDIRU2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_regx_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 91;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2118 && /* INDIRU2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2390 && /* LSHU2 */
			LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a))))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_con8_NT] + (if_rmw(a,16));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 94;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,BANDU2(INDIRU2(rmw),conFF00)) */
			RIGHT_CHILD(a)->op == 2438 && /* BANDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 97;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,BANDU2(INDIRU2(rmw),conFF00)) */
			RIGHT_CHILD(a)->op == 2438 && /* BANDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conFF00_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 100;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),conB)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conB_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 103;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),conB)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conB_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 106;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,CVUI2(LOADU1(INDIRU2(rmw)))) */
			RIGHT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 109;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),conXX00)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conXX00_NT] + (if_rmw(a,34));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 112;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),conXX00)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2118 /* INDIRU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conXX00_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 115;
				_closure_asgn(a, c + 0);
			}
		}
		break;
	case 2103: /* ASGNP2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNP2(VREGP,reg) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			c = ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 5;
			}
		}
		/* asgn: ASGNP2(zddr,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 1;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNP2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 2;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNP2(spill,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 32;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNP2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 36;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNP2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu6(29));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 41;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNP2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu7(21));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 44;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNP2(rmw,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(if_not_asgn_tmp(a,29)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 53;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNP2(rmw,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu7(if_not_asgn_tmp(a,31)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 56;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNP2(ADDP2(reg,con),ac) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 59;
				_closure_asgn(a, c + 0);
			}
		}
		/* asgn: ASGNP2(lddr,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(if_zoffset(a,38,58)));
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 62;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),con1)) */
			RIGHT_CHILD(a)->op == 2359 && /* ADDP2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_con1_NT] + (mincpu6(if_rmw(a, 22)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 67;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),ac)) */
			RIGHT_CHILD(a)->op == 2359 && /* ADDP2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + (mincpu7(if_rmw(a, 30)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 72;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDP2(ac,INDIRP2(rmw))) */
			RIGHT_CHILD(a)->op == 2359 && /* ADDP2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + (mincpu7(if_rmw_a(a, 1, 29)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 75;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,SUBP2(INDIRP2(rmw),ac)) */
			RIGHT_CHILD(a)->op == 2375 && /* SUBP2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ac_NT] + (mincpu7(if_rmw(a, 29)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 78;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),conBa)) */
			RIGHT_CHILD(a)->op == 2359 && /* ADDP2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conBa_NT] + (mincpu7(if_rmw(a, 38)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 79;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,SUBP2(INDIRP2(rmw),conBa)) */
			RIGHT_CHILD(a)->op == 2375 && /* SUBP2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conBa_NT] + (mincpu7(if_rmw(a, 38)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 82;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2119 && /* INDIRP2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_ac_NT] + (if_rmw(a,16));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 87;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),BANDU2(ac,conFF00))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2119 && /* INDIRP2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 90;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),BANDU2(regx,conFF00))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2119 && /* INDIRP2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_regx_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 93;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2119 && /* INDIRP2 */
			RIGHT_CHILD(RIGHT_CHILD(a))->op == 2390 && /* LSHU2 */
			LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a))))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_con8_NT] + (if_rmw(a,16));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 96;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,BANDU2(INDIRP2(rmw),conFF00)) */
			RIGHT_CHILD(a)->op == 2438 && /* BANDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conFF00_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 99;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,BANDU2(INDIRP2(rmw),conFF00)) */
			RIGHT_CHILD(a)->op == 2438 && /* BANDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conFF00_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 102;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),conB)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conB_NT] + (if_rmw(a,32));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 105;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),conB)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2438 && /* BANDU2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conB_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 108;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,CVUI2(LOADU1(INDIRP2(rmw)))) */
			RIGHT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->x.state))->cost[_rmw_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 111;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),conXX00)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conXX00_NT] + (if_rmw(a,34));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 114;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),conXX00)) */
			RIGHT_CHILD(a)->op == 2358 && /* ADDU2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->op == 1254 && /* LOADU1 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a))))->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))))->x.state))->cost[_rmw_NT] + ((struct _state *)(RIGHT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_conXX00_NT] + (mincpu6(if_rmw(a,26)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 117;
				_closure_asgn(a, c + 0);
			}
		}
		break;
	case 2117: /* INDIRI2 */
		_label(LEFT_CHILD(a));
		if (	/* reg: INDIRI2(VREGP) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			if (mayrecalc(a)) {
				struct _state *q = a->syms[RX]->u.t.cse->x.state;
				*p = *q;
			}
			c = 0;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 3;
				_closure_reg(a, c + 0);
			}
		}
		/* eac: INDIRI2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 27;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 5;
			_closure_eac(a, c + 0);
		}
		/* eac: INDIRI2(zddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + 20;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 8;
			_closure_eac(a, c + 0);
		}
		/* ac: INDIRI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 27;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 3;
			_closure_ac(a, c + 0);
		}
		/* iarg: INDIRI2(zddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + 0;
		if (c + 0 < p->cost[_iarg_NT]) {
			p->cost[_iarg_NT] = c + 0;
			p->rule._iarg = 2;
		}
		/* iarg: INDIRI2(spill) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + 20;
		if (c + 0 < p->cost[_iarg_NT]) {
			p->cost[_iarg_NT] = c + 0;
			p->rule._iarg = 6;
		}
		/* eac: INDIRI2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 85;
			_closure_eac(a, c + 0);
		}
		if (	/* eac: INDIRI2(INDIRP2(zddr)) */
			LEFT_CHILD(a)->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_zddr_NT] + (mincpu6(28));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 88;
				_closure_eac(a, c + 0);
			}
		}
		/* reg: INDIRI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu6(30));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 36;
			_closure_reg(a, c + 0);
		}
		if (	/* eac: INDIRI2(ADDP2(reg,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 91;
				_closure_eac(a, c + 0);
			}
		}
		if (	/* ac: INDIRI2(ADDP2(ac,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_ac_NT]) {
				p->cost[_ac_NT] = c + 0;
				p->rule._ac = 110;
				_closure_ac(a, c + 0);
			}
		}
		if (	/* eac: INDIRI2(ADDP2(eac,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 94;
				_closure_eac(a, c + 0);
			}
		}
		/* eac: INDIRI2(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + (mincpu7(if_zoffset(a,38,58)));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 99;
			_closure_eac(a, c + 0);
		}
		break;
	case 2118: /* INDIRU2 */
		_label(LEFT_CHILD(a));
		if (	/* reg: INDIRU2(VREGP) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			if (mayrecalc(a)) {
				struct _state *q = a->syms[RX]->u.t.cse->x.state;
				*p = *q;
			}
			c = 0;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 4;
				_closure_reg(a, c + 0);
			}
		}
		/* eac: INDIRU2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 27;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 6;
			_closure_eac(a, c + 0);
		}
		/* eac: INDIRU2(zddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + 20;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 9;
			_closure_eac(a, c + 0);
		}
		/* ac: INDIRU2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 27;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 4;
			_closure_ac(a, c + 0);
		}
		/* iarg: INDIRU2(zddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + 0;
		if (c + 0 < p->cost[_iarg_NT]) {
			p->cost[_iarg_NT] = c + 0;
			p->rule._iarg = 3;
		}
		/* iarg: INDIRU2(spill) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + 20;
		if (c + 0 < p->cost[_iarg_NT]) {
			p->cost[_iarg_NT] = c + 0;
			p->rule._iarg = 5;
		}
		/* eac: INDIRU2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 86;
			_closure_eac(a, c + 0);
		}
		if (	/* eac: INDIRU2(INDIRP2(zddr)) */
			LEFT_CHILD(a)->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_zddr_NT] + (mincpu6(28));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 89;
				_closure_eac(a, c + 0);
			}
		}
		/* reg: INDIRU2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu6(30));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 37;
			_closure_reg(a, c + 0);
		}
		if (	/* eac: INDIRU2(ADDP2(reg,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 92;
				_closure_eac(a, c + 0);
			}
		}
		if (	/* ac: INDIRU2(ADDP2(ac,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_ac_NT]) {
				p->cost[_ac_NT] = c + 0;
				p->rule._ac = 111;
				_closure_ac(a, c + 0);
			}
		}
		if (	/* eac: INDIRU2(ADDP2(eac,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 95;
				_closure_eac(a, c + 0);
			}
		}
		/* eac: INDIRU2(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + (mincpu7(if_zoffset(a,38,58)));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 100;
			_closure_eac(a, c + 0);
		}
		break;
	case 2119: /* INDIRP2 */
		_label(LEFT_CHILD(a));
		if (	/* reg: INDIRP2(VREGP) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			if (mayrecalc(a)) {
				struct _state *q = a->syms[RX]->u.t.cse->x.state;
				*p = *q;
			}
			c = 0;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 5;
				_closure_reg(a, c + 0);
			}
		}
		/* eac: INDIRP2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 27;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 7;
			_closure_eac(a, c + 0);
		}
		/* eac: INDIRP2(zddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + 20;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 10;
			_closure_eac(a, c + 0);
		}
		/* ac: INDIRP2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 27;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 5;
			_closure_ac(a, c + 0);
		}
		/* iarg: INDIRP2(zddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_zddr_NT] + 0;
		if (c + 0 < p->cost[_iarg_NT]) {
			p->cost[_iarg_NT] = c + 0;
			p->rule._iarg = 4;
		}
		/* iarg: INDIRP2(spill) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + 20;
		if (c + 0 < p->cost[_iarg_NT]) {
			p->cost[_iarg_NT] = c + 0;
			p->rule._iarg = 7;
		}
		/* eac: INDIRP2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu6(28));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 87;
			_closure_eac(a, c + 0);
		}
		if (	/* eac: INDIRP2(INDIRP2(zddr)) */
			LEFT_CHILD(a)->op == 2119 /* INDIRP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_zddr_NT] + (mincpu6(28));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 90;
				_closure_eac(a, c + 0);
			}
		}
		/* reg: INDIRP2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu6(30));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 38;
			_closure_reg(a, c + 0);
		}
		if (	/* eac: INDIRP2(ADDP2(reg,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 93;
				_closure_eac(a, c + 0);
			}
		}
		if (	/* ac: INDIRP2(ADDP2(ac,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_ac_NT]) {
				p->cost[_ac_NT] = c + 0;
				p->rule._ac = 112;
				_closure_ac(a, c + 0);
			}
		}
		if (	/* eac: INDIRP2(ADDP2(eac,con)) */
			LEFT_CHILD(a)->op == 2359 /* ADDP2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con_NT] + (mincpu7(58));
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 96;
				_closure_eac(a, c + 0);
			}
		}
		/* eac: INDIRP2(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + (mincpu7(if_zoffset(a,38,58)));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 101;
			_closure_eac(a, c + 0);
		}
		break;
	case 2165: /* CVFI2 */
		_label(LEFT_CHILD(a));
		/* ac: CVFI2(fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 99;
			_closure_ac(a, c + 0);
		}
		break;
	case 2166: /* CVFU2 */
		_label(LEFT_CHILD(a));
		/* ac: CVFU2(fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 98;
			_closure_ac(a, c + 0);
		}
		break;
	case 2181: /* CVII2 */
		_label(LEFT_CHILD(a));
		/* eac: CVII2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_cv_from(a,1,66));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 71;
			_closure_eac(a, c + 0);
		}
		/* ac: CVII2(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + (if_cv_from(a,1,48));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 96;
			_closure_ac(a, c + 0);
		}
		/* ac: CVII2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,1,66));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 97;
			_closure_ac(a, c + 0);
		}
		/* eac: CVII2(eac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac0_NT] + (if_cv_from(a,1,48));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 72;
			_closure_eac(a, c + 0);
		}
		/* eac: CVII2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + (if_cv_from(a,1,66));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 73;
			_closure_eac(a, c + 0);
		}
		/* ac: CVII2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(if_cv_from(a,1,26)));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 108;
			_closure_ac(a, c + 0);
		}
		/* eac: CVII2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu7(if_cv_from(a,1,26)));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 82;
			_closure_eac(a, c + 0);
		}
		/* eac: CVII2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + (mincpu7(if_cv_from(a,1,26)));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 83;
			_closure_eac(a, c + 0);
		}
		break;
	case 2182: /* CVIU2 */
		break;
	case 2198: /* CVPU2 */
		break;
	case 2229: /* CVUI2 */
		_label(LEFT_CHILD(a));
		/* eac0: CVUI2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_cv_from(a,1,18));
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 14;
			_closure_eac0(a, c + 0);
		}
		/* ac0: CVUI2(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + (if_cv_from(a,1,0));
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 15;
			_closure_ac0(a, c + 0);
		}
		/* ac0: CVUI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,1,18));
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 16;
			_closure_ac0(a, c + 0);
		}
		/* eac0: CVUI2(eac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac0_NT] + (if_cv_from(a,1,0));
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 15;
			_closure_eac0(a, c + 0);
		}
		/* eac0: CVUI2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + (if_cv_from(a,1,18));
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 16;
			_closure_eac0(a, c + 0);
		}
		break;
	case 2230: /* CVUU2 */
		break;
	case 2231: /* CVUP2 */
		break;
	case 2245: /* NEGI2 */
		_label(LEFT_CHILD(a));
		/* ac: NEGI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 68;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 31;
			_closure_ac(a, c + 0);
		}
		/* ac: NEGI2(regx) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_regx_NT] + 48;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 32;
			_closure_ac(a, c + 0);
		}
		/* ac: NEGI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu6(26));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 109;
			_closure_ac(a, c + 0);
		}
		/* eac: NEGI2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + (mincpu6(26));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 84;
			_closure_eac(a, c + 0);
		}
		break;
	case 2261: /* CALLI2 */
		_label(LEFT_CHILD(a));
		/* ac: CALLI2(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + (mincpu5(28));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 82;
			_closure_ac(a, c + 0);
		}
		/* ac: CALLI2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 26;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 83;
			_closure_ac(a, c + 0);
		}
		/* ac: CALLI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 84;
			_closure_ac(a, c + 0);
		}
		break;
	case 2262: /* CALLU2 */
		_label(LEFT_CHILD(a));
		/* ac: CALLU2(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + (mincpu5(28));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 85;
			_closure_ac(a, c + 0);
		}
		/* ac: CALLU2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 26;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 86;
			_closure_ac(a, c + 0);
		}
		/* ac: CALLU2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 87;
			_closure_ac(a, c + 0);
		}
		break;
	case 2263: /* CALLP2 */
		_label(LEFT_CHILD(a));
		/* ac: CALLP2(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + (mincpu5(28));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 88;
			_closure_ac(a, c + 0);
		}
		/* ac: CALLP2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 26;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 89;
			_closure_ac(a, c + 0);
		}
		/* ac: CALLP2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 90;
			_closure_ac(a, c + 0);
		}
		break;
	case 2277: /* LOADI2 */
		_label(LEFT_CHILD(a));
		/* ac: LOADI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 0;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 93;
			_closure_ac(a, c + 0);
		}
		/* ac0: LOADI2(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + 0;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 12;
			_closure_ac0(a, c + 0);
		}
		/* eac: LOADI2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 0;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 62;
			_closure_eac(a, c + 0);
		}
		/* eac: LOADI2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_asgnreuse(a,20,8));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 65;
			_closure_eac(a, c + 0);
		}
		/* eac: LOADI2(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 20;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 68;
			_closure_eac(a, c + 0);
		}
		/* regx: LOADI2(conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(29));
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 6;
			_closure_regx(a, c + 0);
		}
		/* regx: LOADI2(con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu7(31));
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 9;
			_closure_regx(a, c + 0);
		}
		/* reg: LOADI2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu7(36));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 41;
			_closure_reg(a, c + 0);
		}
		break;
	case 2278: /* LOADU2 */
		_label(LEFT_CHILD(a));
		/* ac: LOADU2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 0;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 94;
			_closure_ac(a, c + 0);
		}
		/* ac0: LOADU2(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + 0;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 13;
			_closure_ac0(a, c + 0);
		}
		/* eac: LOADU2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 0;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 63;
			_closure_eac(a, c + 0);
		}
		/* eac: LOADU2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_asgnreuse(a,20,8));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 66;
			_closure_eac(a, c + 0);
		}
		/* eac: LOADU2(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 20;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 69;
			_closure_eac(a, c + 0);
		}
		/* regx: LOADU2(conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(29));
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 7;
			_closure_regx(a, c + 0);
		}
		/* regx: LOADU2(con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu7(31));
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 10;
			_closure_regx(a, c + 0);
		}
		/* reg: LOADU2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu7(36));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 42;
			_closure_reg(a, c + 0);
		}
		break;
	case 2279: /* LOADP2 */
		_label(LEFT_CHILD(a));
		/* ac: LOADP2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 0;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 95;
			_closure_ac(a, c + 0);
		}
		/* ac0: LOADP2(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + 0;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 14;
			_closure_ac0(a, c + 0);
		}
		/* eac: LOADP2(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 0;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 64;
			_closure_eac(a, c + 0);
		}
		/* eac: LOADP2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_asgnreuse(a,20,8));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 67;
			_closure_eac(a, c + 0);
		}
		/* eac: LOADP2(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 20;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 70;
			_closure_eac(a, c + 0);
		}
		/* regx: LOADP2(conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(29));
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 8;
			_closure_regx(a, c + 0);
		}
		/* regx: LOADP2(con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_con_NT] + (mincpu7(31));
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 11;
			_closure_regx(a, c + 0);
		}
		/* reg: LOADP2(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (mincpu7(36));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 43;
			_closure_reg(a, c + 0);
		}
		break;
	case 2293: /* RETI2 */
		_label(LEFT_CHILD(a));
		/* stmt: RETI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 1;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 114;
		}
		break;
	case 2294: /* RETU2 */
		_label(LEFT_CHILD(a));
		/* stmt: RETU2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 1;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 115;
		}
		break;
	case 2295: /* RETP2 */
		_label(LEFT_CHILD(a));
		/* stmt: RETP2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 1;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 116;
		}
		break;
	case 2311: /* ADDRGP2 */
		/* addr: ADDRGP2 */
		if (0 + 0 < p->cost[_addr_NT]) {
			p->cost[_addr_NT] = 0 + 0;
			p->rule._addr = 1;
			_closure_addr(a, 0 + 0);
		}
		/* zddr: ADDRGP2 */
		c = (if_zpglobal(a));
		if (c + 0 < p->cost[_zddr_NT]) {
			p->cost[_zddr_NT] = c + 0;
			p->rule._zddr = 1;
			_closure_zddr(a, c + 0);
		}
		break;
	case 2327: /* ADDRFP2 */
		/* lddr: ADDRFP2 */
		if (0 + 0 < p->cost[_lddr_NT]) {
			p->cost[_lddr_NT] = 0 + 0;
			p->rule._lddr = 2;
			_closure_lddr(a, 0 + 0);
		}
		break;
	case 2343: /* ADDRLP2 */
		/* lddr: ADDRLP2 */
		if (0 + 0 < p->cost[_lddr_NT]) {
			p->cost[_lddr_NT] = 0 + 0;
			p->rule._lddr = 1;
			_closure_lddr(a, 0 + 0);
		}
		/* spill: ADDRLP2 */
		c = (if_spill());
		if (c + 0 < p->cost[_spill_NT]) {
			p->cost[_spill_NT] = c + 0;
			p->rule._spill = 1;
		}
		break;
	case 2357: /* ADDI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: ADDI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 6;
			_closure_ac(a, c + 0);
		}
		/* ac: ADDI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 9;
			_closure_ac(a, c + 0);
		}
		if (	/* ac: ADDI2(LSHI2(iarg,con1),ac) */
			LEFT_CHILD(a)->op == 2389 /* LSHI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con1_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 56;
			if (c + 0 < p->cost[_ac_NT]) {
				p->cost[_ac_NT] = c + 0;
				p->rule._ac = 12;
				_closure_ac(a, c + 0);
			}
		}
		/* ac: ADDI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,25,10));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 16;
			_closure_ac(a, c + 0);
		}
		/* ac: ADDI2(ac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + (if_incr(a,25,10));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 19;
			_closure_ac(a, c + 0);
		}
		/* eac: ADDI2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 11;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDI2(eac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 14;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDI2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 35;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDI2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 38;
			_closure_eac(a, c + 0);
		}
		if (	/* eac: ADDI2(LSHI2(iarg,con1),eac) */
			LEFT_CHILD(a)->op == 2389 /* LSHI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con1_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 56;
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 41;
				_closure_eac(a, c + 0);
			}
		}
		/* ac: ADDI2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,if_incr(a,38,10)));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 100;
			_closure_ac(a, c + 0);
		}
		/* eac: ADDI2(eac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,38));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 74;
			_closure_eac(a, c + 0);
		}
		break;
	case 2358: /* ADDU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: ADDU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 7;
			_closure_ac(a, c + 0);
		}
		/* ac: ADDU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 10;
			_closure_ac(a, c + 0);
		}
		if (	/* ac: ADDU2(LSHU2(iarg,con1),ac) */
			LEFT_CHILD(a)->op == 2390 /* LSHU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con1_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 56;
			if (c + 0 < p->cost[_ac_NT]) {
				p->cost[_ac_NT] = c + 0;
				p->rule._ac = 13;
				_closure_ac(a, c + 0);
			}
		}
		/* ac: ADDU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,25,10));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 17;
			_closure_ac(a, c + 0);
		}
		/* ac: ADDU2(ac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + (if_incr(a,25,10));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 20;
			_closure_ac(a, c + 0);
		}
		/* eac: ADDU2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 12;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDU2(eac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 15;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDU2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 36;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDU2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 39;
			_closure_eac(a, c + 0);
		}
		if (	/* eac: ADDU2(LSHU2(iarg,con1),eac) */
			LEFT_CHILD(a)->op == 2390 /* LSHU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con1_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 56;
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 42;
				_closure_eac(a, c + 0);
			}
		}
		/* ac: ADDU2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,if_incr(a,38,10)));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 101;
			_closure_ac(a, c + 0);
		}
		/* eac: ADDU2(eac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,38));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 75;
			_closure_eac(a, c + 0);
		}
		if (	/* regx: ADDU2(LSHU2(CVUI2(LOADU1(iarg)),con8),CVUI2(LOADU1(ac))) */
			LEFT_CHILD(a)->op == 2390 && /* LSHU2 */
			LEFT_CHILD(LEFT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(a)))->op == 1254 && /* LOADU1 */
			RIGHT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(a))))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con8_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_ac_NT] + 50;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 12;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(BANDU2(iarg,conFF00),CVUI2(LOADU1(ac))) */
			LEFT_CHILD(a)->op == 2438 && /* BANDU2 */
			RIGHT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_conFF00_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_ac_NT] + 50;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 13;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(LSHU2(CVUI2(LOADU1(eac)),con8),CVUI2(LOADU1(ac))) */
			LEFT_CHILD(a)->op == 2390 && /* LSHU2 */
			LEFT_CHILD(LEFT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(a)))->op == 1254 && /* LOADU1 */
			RIGHT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(a))))->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con8_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_ac_NT] + 32;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 14;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(LSHU2(CVUI2(LOADU1(ac)),con8),CVUI2(LOADU1(eac))) */
			LEFT_CHILD(a)->op == 2390 && /* LSHU2 */
			LEFT_CHILD(LEFT_CHILD(a))->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(a)))->op == 1254 && /* LOADU1 */
			RIGHT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(a))))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con8_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_eac_NT] + 32;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 15;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(BANDU2(eac,conFF00),ac0) */
			LEFT_CHILD(a)->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac0_NT] + 50;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 16;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(BANDU2(ac,conFF00),eac0) */
			LEFT_CHILD(a)->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac0_NT] + 50;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 17;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(LSHU2(ac,con8),conB) */
			LEFT_CHILD(a)->op == 2390 /* LSHU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con8_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 50;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 18;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(LSHU2(ac,con8),conB) */
			LEFT_CHILD(a)->op == 2390 /* LSHU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con8_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(42));
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 19;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(BANDU2(ac,conFF00),conB) */
			LEFT_CHILD(a)->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 66;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 20;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(BANDU2(ac,conFF00),conB) */
			LEFT_CHILD(a)->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(60));
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 21;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(BANDU2(reg,conFF00),conB) */
			LEFT_CHILD(a)->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 66;
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 22;
				_closure_regx(a, c + 0);
			}
		}
		if (	/* regx: ADDU2(BANDU2(reg,conFF00),conB) */
			LEFT_CHILD(a)->op == 2438 /* BANDU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_conFF00_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (mincpu6(60));
			if (c + 0 < p->cost[_regx_NT]) {
				p->cost[_regx_NT] = c + 0;
				p->rule._regx = 23;
				_closure_regx(a, c + 0);
			}
		}
		/* regx: ADDU2(ac0,conXX00) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conXX00_NT] + 48;
		if (c + 0 < p->cost[_regx_NT]) {
			p->cost[_regx_NT] = c + 0;
			p->rule._regx = 24;
			_closure_regx(a, c + 0);
		}
		/* ac: ADDU2(ac0,conXX00) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conXX00_NT] + (mincpu7(16));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 123;
			_closure_ac(a, c + 0);
		}
		break;
	case 2359: /* ADDP2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: ADDP2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 8;
			_closure_ac(a, c + 0);
		}
		/* ac: ADDP2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 11;
			_closure_ac(a, c + 0);
		}
		if (	/* ac: ADDP2(LSHI2(iarg,con1),ac) */
			LEFT_CHILD(a)->op == 2389 /* LSHI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con1_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 56;
			if (c + 0 < p->cost[_ac_NT]) {
				p->cost[_ac_NT] = c + 0;
				p->rule._ac = 14;
				_closure_ac(a, c + 0);
			}
		}
		if (	/* ac: ADDP2(LSHU2(iarg,con1),ac) */
			LEFT_CHILD(a)->op == 2390 /* LSHU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con1_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 56;
			if (c + 0 < p->cost[_ac_NT]) {
				p->cost[_ac_NT] = c + 0;
				p->rule._ac = 15;
				_closure_ac(a, c + 0);
			}
		}
		/* ac: ADDP2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,25,10));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 18;
			_closure_ac(a, c + 0);
		}
		/* ac: ADDP2(ac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + (if_incr(a,25,10));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 21;
			_closure_ac(a, c + 0);
		}
		/* eac: ADDP2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 13;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDP2(eac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 16;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDP2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 37;
			_closure_eac(a, c + 0);
		}
		/* eac: ADDP2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 40;
			_closure_eac(a, c + 0);
		}
		if (	/* eac: ADDP2(LSHI2(iarg,con1),eac) */
			LEFT_CHILD(a)->op == 2389 /* LSHI2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con1_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 56;
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 43;
				_closure_eac(a, c + 0);
			}
		}
		if (	/* eac: ADDP2(LSHU2(iarg,con1),eac) */
			LEFT_CHILD(a)->op == 2390 /* LSHU2 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(LEFT_CHILD(a))->x.state))->cost[_con1_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 56;
			if (c + 0 < p->cost[_eac_NT]) {
				p->cost[_eac_NT] = c + 0;
				p->rule._eac = 44;
				_closure_eac(a, c + 0);
			}
		}
		/* ac: ADDP2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,if_incr(a,38,10)));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 102;
			_closure_ac(a, c + 0);
		}
		/* eac: ADDP2(eac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,38));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 76;
			_closure_eac(a, c + 0);
		}
		break;
	case 2373: /* SUBI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: SUBI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 22;
			_closure_ac(a, c + 0);
		}
		/* ac: SUBI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 25;
			_closure_ac(a, c + 0);
		}
		/* ac: SUBI2(ac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 28;
			_closure_ac(a, c + 0);
		}
		/* eac: SUBI2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 17;
			_closure_eac(a, c + 0);
		}
		/* eac: SUBI2(eac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 20;
			_closure_eac(a, c + 0);
		}
		/* eac: SUBI2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 45;
			_closure_eac(a, c + 0);
		}
		/* ac: SUBI2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,if_incr(a,38,10)));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 103;
			_closure_ac(a, c + 0);
		}
		/* eac: SUBI2(eac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,38));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 77;
			_closure_eac(a, c + 0);
		}
		break;
	case 2374: /* SUBU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: SUBU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 23;
			_closure_ac(a, c + 0);
		}
		/* ac: SUBU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 26;
			_closure_ac(a, c + 0);
		}
		/* ac: SUBU2(ac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 29;
			_closure_ac(a, c + 0);
		}
		/* eac: SUBU2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 18;
			_closure_eac(a, c + 0);
		}
		/* eac: SUBU2(eac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 21;
			_closure_eac(a, c + 0);
		}
		/* eac: SUBU2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 46;
			_closure_eac(a, c + 0);
		}
		/* ac: SUBU2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,if_incr(a,38,10)));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 104;
			_closure_ac(a, c + 0);
		}
		/* eac: SUBU2(eac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,38));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 78;
			_closure_eac(a, c + 0);
		}
		break;
	case 2375: /* SUBP2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: SUBP2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 24;
			_closure_ac(a, c + 0);
		}
		/* ac: SUBP2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 27;
			_closure_ac(a, c + 0);
		}
		/* ac: SUBP2(ac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 30;
			_closure_ac(a, c + 0);
		}
		/* eac: SUBP2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 19;
			_closure_eac(a, c + 0);
		}
		/* eac: SUBP2(eac,conBn) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conBn_NT] + 25;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 22;
			_closure_eac(a, c + 0);
		}
		/* eac: SUBP2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 47;
			_closure_eac(a, c + 0);
		}
		/* ac: SUBP2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,if_incr(a,38,10)));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 105;
			_closure_ac(a, c + 0);
		}
		/* eac: SUBP2(eac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + (mincpuf(6,addhi,38));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 79;
			_closure_eac(a, c + 0);
		}
		break;
	case 2389: /* LSHI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: LSHI2(ac,con1) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con1_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 33;
			_closure_ac(a, c + 0);
		}
		/* ac: LSHI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 100;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 35;
			_closure_ac(a, c + 0);
		}
		/* ac: LSHI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 39;
			_closure_ac(a, c + 0);
		}
		/* eac: LSHI2(eac,con1) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con1_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 23;
			_closure_eac(a, c + 0);
		}
		/* eac: LSHI2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 100;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 25;
			_closure_eac(a, c + 0);
		}
		/* ac: LSHI2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 44;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 115;
			_closure_ac(a, c + 0);
		}
		/* ac: LSHI2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + (mincpu6(42));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 117;
			_closure_ac(a, c + 0);
		}
		/* reg: LSHI2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 48;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 47;
			_closure_reg(a, c + 0);
		}
		/* reg: LSHI2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + (mincpu6(42));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 48;
			_closure_reg(a, c + 0);
		}
		break;
	case 2390: /* LSHU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: LSHU2(ac,con1) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con1_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 34;
			_closure_ac(a, c + 0);
		}
		/* ac: LSHU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 100;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 36;
			_closure_ac(a, c + 0);
		}
		/* ac: LSHU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 41;
			_closure_ac(a, c + 0);
		}
		/* eac: LSHU2(eac,con1) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con1_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 24;
			_closure_eac(a, c + 0);
		}
		/* eac: LSHU2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 100;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 26;
			_closure_eac(a, c + 0);
		}
		/* ac: LSHU2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 44;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 116;
			_closure_ac(a, c + 0);
		}
		/* ac: LSHU2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + (mincpu6(42));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 118;
			_closure_ac(a, c + 0);
		}
		/* reg: LSHU2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 48;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 49;
			_closure_reg(a, c + 0);
		}
		/* reg: LSHU2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + (mincpu6(42));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 50;
			_closure_reg(a, c + 0);
		}
		if (	/* reg: LSHU2(CVUI2(LOADU1(ac)),con8) */
			LEFT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(a))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(a)))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 48;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 51;
				_closure_reg(a, c + 0);
			}
		}
		if (	/* reg: LSHU2(CVUI2(LOADU1(ac)),con8) */
			LEFT_CHILD(a)->op == 2229 && /* CVUI2 */
			LEFT_CHILD(LEFT_CHILD(a))->op == 1254 /* LOADU1 */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(a)))->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + (mincpu6(42));
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 52;
				_closure_reg(a, c + 0);
			}
		}
		break;
	case 2405: /* MODI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: MODI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 56;
			_closure_ac(a, c + 0);
		}
		/* ac: MODI2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + 180;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 60;
			_closure_ac(a, c + 0);
		}
		break;
	case 2406: /* MODU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: MODU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 57;
			_closure_ac(a, c + 0);
		}
		/* ac: MODU2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + 180;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 61;
			_closure_ac(a, c + 0);
		}
		break;
	case 2421: /* RSHI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: RSHI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 100;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 37;
			_closure_ac(a, c + 0);
		}
		/* ac: RSHI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 40;
			_closure_ac(a, c + 0);
		}
		/* ac: RSHI2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 60;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 113;
			_closure_ac(a, c + 0);
		}
		/* ac: RSHI2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + (mincpu7(24));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 114;
			_closure_ac(a, c + 0);
		}
		/* reg: RSHI2(reg,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 74;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 45;
			_closure_reg(a, c + 0);
		}
		/* reg: RSHI2(reg,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + (mincpu7(44));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 46;
			_closure_reg(a, c + 0);
		}
		break;
	case 2422: /* RSHU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: RSHU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 100;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 38;
			_closure_ac(a, c + 0);
		}
		/* ac: RSHU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 42;
			_closure_ac(a, c + 0);
		}
		/* ac0: RSHU2(reg,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 18;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 17;
			_closure_ac0(a, c + 0);
		}
		/* ac0: RSHU2(ac,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 18;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 18;
			_closure_ac0(a, c + 0);
		}
		/* reg: RSHU2(reg,con8) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con8_NT] + 38;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 44;
			_closure_reg(a, c + 0);
		}
		break;
	case 2437: /* BANDI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: BANDI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 64;
			_closure_ac(a, c + 0);
		}
		/* ac: BANDI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 66;
			_closure_ac(a, c + 0);
		}
		/* ac: BANDI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 22;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 68;
			_closure_ac(a, c + 0);
		}
		/* ac0: BANDI2(ac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 22;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 4;
			_closure_ac0(a, c + 0);
		}
		/* eac: BANDI2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 22;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 29;
			_closure_eac(a, c + 0);
		}
		/* eac0: BANDI2(eac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 22;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 6;
			_closure_eac0(a, c + 0);
		}
		/* eac: BANDI2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 48;
			_closure_eac(a, c + 0);
		}
		/* eac: BANDI2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 50;
			_closure_eac(a, c + 0);
		}
		/* ac0: BANDI2(ac,conFF) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conFF_NT] + 18;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 19;
			_closure_ac0(a, c + 0);
		}
		/* reg: BANDI2(ac0,conFF) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conFF_NT] + 20;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 53;
			_closure_reg(a, c + 0);
		}
		/* ac: BANDI2(ac,conFF00) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conFF00_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 120;
			_closure_ac(a, c + 0);
		}
		/* ac: BANDI2(ac,conFF00) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conFF00_NT] + (mincpu6(26));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 122;
			_closure_ac(a, c + 0);
		}
		break;
	case 2438: /* BANDU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: BANDU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 65;
			_closure_ac(a, c + 0);
		}
		/* ac: BANDU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 67;
			_closure_ac(a, c + 0);
		}
		/* ac: BANDU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 22;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 69;
			_closure_ac(a, c + 0);
		}
		/* ac0: BANDU2(ac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 22;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 5;
			_closure_ac0(a, c + 0);
		}
		/* eac: BANDU2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 22;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 30;
			_closure_eac(a, c + 0);
		}
		/* eac0: BANDU2(eac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 22;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 7;
			_closure_eac0(a, c + 0);
		}
		/* eac: BANDU2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 49;
			_closure_eac(a, c + 0);
		}
		/* eac: BANDU2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 51;
			_closure_eac(a, c + 0);
		}
		/* ac0: BANDU2(ac,conFF) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conFF_NT] + 18;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 20;
			_closure_ac0(a, c + 0);
		}
		/* reg: BANDU2(ac0,conFF) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conFF_NT] + 20;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 54;
			_closure_reg(a, c + 0);
		}
		/* ac: BANDU2(ac,conFF00) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conFF00_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 119;
			_closure_ac(a, c + 0);
		}
		/* ac: BANDU2(ac,conFF00) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conFF00_NT] + (mincpu6(26));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 121;
			_closure_ac(a, c + 0);
		}
		break;
	case 2453: /* BCOMI2 */
		_label(LEFT_CHILD(a));
		/* ac: BCOMI2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 68;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 62;
			_closure_ac(a, c + 0);
		}
		break;
	case 2454: /* BCOMU2 */
		_label(LEFT_CHILD(a));
		/* ac: BCOMU2(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 68;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 63;
			_closure_ac(a, c + 0);
		}
		break;
	case 2469: /* BORI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: BORI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 70;
			_closure_ac(a, c + 0);
		}
		/* ac: BORI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 72;
			_closure_ac(a, c + 0);
		}
		/* ac: BORI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 74;
			_closure_ac(a, c + 0);
		}
		/* ac0: BORI2(ac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 6;
			_closure_ac0(a, c + 0);
		}
		/* eac: BORI2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 31;
			_closure_eac(a, c + 0);
		}
		/* eac0: BORI2(eac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 8;
			_closure_eac0(a, c + 0);
		}
		/* eac: BORI2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 52;
			_closure_eac(a, c + 0);
		}
		/* eac: BORI2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 54;
			_closure_eac(a, c + 0);
		}
		break;
	case 2470: /* BORU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: BORU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 71;
			_closure_ac(a, c + 0);
		}
		/* ac: BORU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 73;
			_closure_ac(a, c + 0);
		}
		/* ac: BORU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 75;
			_closure_ac(a, c + 0);
		}
		/* ac0: BORU2(ac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 7;
			_closure_ac0(a, c + 0);
		}
		/* eac: BORU2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 32;
			_closure_eac(a, c + 0);
		}
		/* eac0: BORU2(eac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 9;
			_closure_eac0(a, c + 0);
		}
		/* eac: BORU2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 53;
			_closure_eac(a, c + 0);
		}
		/* eac: BORU2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 55;
			_closure_eac(a, c + 0);
		}
		break;
	case 2485: /* BXORI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: BXORI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 76;
			_closure_ac(a, c + 0);
		}
		/* ac: BXORI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 78;
			_closure_ac(a, c + 0);
		}
		/* ac: BXORI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 80;
			_closure_ac(a, c + 0);
		}
		/* ac0: BXORI2(ac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 8;
			_closure_ac0(a, c + 0);
		}
		/* eac: BXORI2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 33;
			_closure_eac(a, c + 0);
		}
		/* eac0: BXORI2(eac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 10;
			_closure_eac0(a, c + 0);
		}
		/* eac: BXORI2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 56;
			_closure_eac(a, c + 0);
		}
		/* eac: BXORI2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 58;
			_closure_eac(a, c + 0);
		}
		break;
	case 2486: /* BXORU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: BXORU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 77;
			_closure_ac(a, c + 0);
		}
		/* ac: BXORU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 28;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 79;
			_closure_ac(a, c + 0);
		}
		/* ac: BXORU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 81;
			_closure_ac(a, c + 0);
		}
		/* ac0: BXORU2(ac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_ac0_NT]) {
			p->cost[_ac0_NT] = c + 0;
			p->rule._ac0 = 9;
			_closure_ac0(a, c + 0);
		}
		/* eac: BXORU2(eac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 34;
			_closure_eac(a, c + 0);
		}
		/* eac0: BXORU2(eac0,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac0_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 14;
		if (c + 0 < p->cost[_eac0_NT]) {
			p->cost[_eac0_NT] = c + 0;
			p->rule._eac0 = 11;
			_closure_eac0(a, c + 0);
		}
		/* eac: BXORU2(eac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 57;
			_closure_eac(a, c + 0);
		}
		/* eac: BXORU2(iarg,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 28;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 59;
			_closure_eac(a, c + 0);
		}
		break;
	case 2501: /* DIVI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: DIVI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 54;
			_closure_ac(a, c + 0);
		}
		/* ac: DIVI2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + 180;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 58;
			_closure_ac(a, c + 0);
		}
		break;
	case 2502: /* DIVU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: DIVU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 55;
			_closure_ac(a, c + 0);
		}
		/* ac: DIVU2(ac,con) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con_NT] + 180;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 59;
			_closure_ac(a, c + 0);
		}
		break;
	case 2517: /* MULI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: MULI2(conB,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 150;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 43;
			_closure_ac(a, c + 0);
		}
		/* ac: MULI2(conBn,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conBn_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 160;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 44;
			_closure_ac(a, c + 0);
		}
		/* ac: MULI2(conB,regx) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_regx_NT] + 150;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 45;
			_closure_ac(a, c + 0);
		}
		/* ac: MULI2(conBn,regx) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conBn_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_regx_NT] + 160;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 46;
			_closure_ac(a, c + 0);
		}
		/* ac: MULI2(con,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_con_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 47;
			_closure_ac(a, c + 0);
		}
		/* ac: MULI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 48;
			_closure_ac(a, c + 0);
		}
		/* ac: MULI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 49;
			_closure_ac(a, c + 0);
		}
		/* eac: MULI2(conB,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 150;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 27;
			_closure_eac(a, c + 0);
		}
		/* eac: MULI2(conB,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + 150;
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 28;
			_closure_eac(a, c + 0);
		}
		/* ac: MULI2(con,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_con_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(80));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 106;
			_closure_ac(a, c + 0);
		}
		/* eac: MULI2(conBm,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conBm_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + (mincpu7(80));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 80;
			_closure_eac(a, c + 0);
		}
		break;
	case 2518: /* MULU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ac: MULU2(conB,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conB_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 150;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 50;
			_closure_ac(a, c + 0);
		}
		/* ac: MULU2(con,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_con_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 51;
			_closure_ac(a, c + 0);
		}
		/* ac: MULU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 52;
			_closure_ac(a, c + 0);
		}
		/* ac: MULU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 200;
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 53;
			_closure_ac(a, c + 0);
		}
		/* ac: MULU2(con,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_con_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (mincpu7(80));
		if (c + 0 < p->cost[_ac_NT]) {
			p->cost[_ac_NT] = c + 0;
			p->rule._ac = 107;
			_closure_ac(a, c + 0);
		}
		/* eac: MULU2(conBm,eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_conBm_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_eac_NT] + (mincpu7(80));
		if (c + 0 < p->cost[_eac_NT]) {
			p->cost[_eac_NT] = c + 0;
			p->rule._eac = 81;
			_closure_eac(a, c + 0);
		}
		break;
	case 2533: /* EQI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQI2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 12;
		}
		/* stmt: EQI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 42;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 13;
		}
		/* stmt: EQI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 54;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 14;
		}
		/* stmt: EQI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 54;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 15;
		}
		break;
	case 2534: /* EQU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQU2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 20;
		}
		/* stmt: EQU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 42;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 21;
		}
		/* stmt: EQU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 54;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 22;
		}
		/* stmt: EQU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 54;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 23;
		}
		break;
	case 2549: /* GEI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEI2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 31;
		}
		/* stmt: GEI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,ifcpu7(56,64),8));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 37;
		}
		/* stmt: GEI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 45;
		}
		/* stmt: GEI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 53;
		}
		break;
	case 2550: /* GEU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,ifcpu7(56,64),8));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 41;
		}
		/* stmt: GEU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 49;
		}
		/* stmt: GEU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 57;
		}
		break;
	case 2565: /* GTI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTI2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 30;
		}
		/* stmt: GTI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,ifcpu7(56,64),8));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 36;
		}
		/* stmt: GTI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 44;
		}
		/* stmt: GTI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 52;
		}
		break;
	case 2566: /* GTU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTU2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 32;
		}
		/* stmt: GTU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,ifcpu7(56,64),8));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 40;
		}
		/* stmt: GTU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 48;
		}
		/* stmt: GTU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 56;
		}
		break;
	case 2581: /* LEI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEI2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 29;
		}
		/* stmt: LEI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,ifcpu7(56,64),8));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 35;
		}
		/* stmt: LEI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 43;
		}
		/* stmt: LEI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 51;
		}
		break;
	case 2582: /* LEU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEU2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 33;
		}
		/* stmt: LEU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,ifcpu7(56,64),8));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 39;
		}
		/* stmt: LEU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 47;
		}
		/* stmt: LEU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 55;
		}
		break;
	case 2597: /* LTI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTI2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 28;
		}
		/* stmt: LTI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,ifcpu7(56,64),8));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 34;
		}
		/* stmt: LTI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 42;
		}
		/* stmt: LTI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 50;
		}
		break;
	case 2598: /* LTU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + (if_incr(a,ifcpu7(56,64),8));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 38;
		}
		/* stmt: LTU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 46;
		}
		/* stmt: LTU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + (ifcpu7(56,84));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 54;
		}
		break;
	case 2613: /* NEI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEI2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 16;
		}
		/* stmt: NEI2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 42;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 17;
		}
		/* stmt: NEI2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 54;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 18;
		}
		/* stmt: NEI2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 54;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 19;
		}
		break;
	case 2614: /* NEU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEU2(ac,con0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_con0_NT] + 28;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 24;
		}
		/* stmt: NEU2(ac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 42;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 25;
		}
		/* stmt: NEU2(ac,iarg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_iarg_NT] + 54;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 26;
		}
		/* stmt: NEU2(iarg,ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_iarg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ac_NT] + 54;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 27;
		}
		break;
	case 4117: /* CNSTI4 */
		break;
	case 4118: /* CNSTU4 */
		break;
	case 4133: /* ARGI4 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGI4(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_stk(a,100));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 100;
		}
		/* stmt: ARGI4(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_reg_only(a));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 106;
		}
		break;
	case 4134: /* ARGU4 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGU4(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_stk(a,100));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 101;
		}
		/* stmt: ARGU4(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_reg_only(a));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 107;
		}
		break;
	case 4149: /* ASGNI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNI4(VREGP,reg) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			c = ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 6;
			}
		}
		/* asgn: ASGNI4(vdst,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_vdst_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 120;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 15;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNI4(vdst,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_vdst_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 120;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 16;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNI4(addr,INDIRI4(asrc)) */
			RIGHT_CHILD(a)->op == 4165 /* INDIRI4 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_asrc_NT] + 120;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 17;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI4(ac,INDIRI4(asrc)) */
			RIGHT_CHILD(a)->op == 4165 /* INDIRI4 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_asrc_NT] + 120;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 18;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNI4(lddr,INDIRI4(lsrc)) */
			RIGHT_CHILD(a)->op == 4165 /* INDIRI4 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_lsrc_NT] + 160;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 19;
				_closure_asgn(a, c + 0);
			}
		}
		/* asgn: ASGNI4(spill,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 33;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNI4(rmw,NEGI4(INDIRI4(rmw))) */
			RIGHT_CHILD(a)->op == 4293 && /* NEGI4 */
			LEFT_CHILD(RIGHT_CHILD(a))->op == 4165 /* INDIRI4 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_rmw_NT] + ((struct _state *)(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(a)))->x.state))->cost[_rmw_NT] + (mincpu6(if_rmw(a, 58)));
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 66;
				_closure_asgn(a, c + 0);
			}
		}
		break;
	case 4150: /* ASGNU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNU4(VREGP,reg) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			c = ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 7;
			}
		}
		/* asgn: ASGNU4(vdst,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_vdst_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 120;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 20;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNU4(vdst,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_vdst_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 120;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 21;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNU4(addr,INDIRU4(asrc)) */
			RIGHT_CHILD(a)->op == 4166 /* INDIRU4 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_asrc_NT] + 120;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 22;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU4(ac,INDIRU4(asrc)) */
			RIGHT_CHILD(a)->op == 4166 /* INDIRU4 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_asrc_NT] + 120;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 23;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNU4(lddr,INDIRU4(lsrc)) */
			RIGHT_CHILD(a)->op == 4166 /* INDIRU4 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_lsrc_NT] + 160;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 24;
				_closure_asgn(a, c + 0);
			}
		}
		/* asgn: ASGNU4(spill,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 34;
			_closure_asgn(a, c + 0);
		}
		break;
	case 4165: /* INDIRI4 */
		_label(LEFT_CHILD(a));
		if (	/* reg: INDIRI4(VREGP) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			if (mayrecalc(a)) {
				struct _state *q = a->syms[RX]->u.t.cse->x.state;
				*p = *q;
			}
			c = 0;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 6;
				_closure_reg(a, c + 0);
			}
		}
		/* larg: INDIRI4(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 0;
		if (c + 0 < p->cost[_larg_NT]) {
			p->cost[_larg_NT] = c + 0;
			p->rule._larg = 2;
		}
		/* reg: INDIRI4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 120;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 12;
			_closure_reg(a, c + 0);
		}
		/* reg: INDIRI4(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + 160;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 14;
			_closure_reg(a, c + 0);
		}
		/* reg: INDIRI4(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + 120;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 16;
			_closure_reg(a, c + 0);
		}
		/* lac: INDIRI4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 120;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 2;
			_closure_lac(a, c + 0);
		}
		/* lac: INDIRI4(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + 120;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 6;
			_closure_lac(a, c + 0);
		}
		break;
	case 4166: /* INDIRU4 */
		_label(LEFT_CHILD(a));
		if (	/* reg: INDIRU4(VREGP) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			if (mayrecalc(a)) {
				struct _state *q = a->syms[RX]->u.t.cse->x.state;
				*p = *q;
			}
			c = 0;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 7;
				_closure_reg(a, c + 0);
			}
		}
		/* larg: INDIRU4(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 0;
		if (c + 0 < p->cost[_larg_NT]) {
			p->cost[_larg_NT] = c + 0;
			p->rule._larg = 3;
		}
		/* reg: INDIRU4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 120;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 13;
			_closure_reg(a, c + 0);
		}
		/* reg: INDIRU4(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + 160;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 15;
			_closure_reg(a, c + 0);
		}
		/* reg: INDIRU4(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + 120;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 17;
			_closure_reg(a, c + 0);
		}
		/* lac: INDIRU4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 120;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 3;
			_closure_lac(a, c + 0);
		}
		/* lac: INDIRU4(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + 160;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 4;
			_closure_lac(a, c + 0);
		}
		/* lac: INDIRU4(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + 160;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 5;
			_closure_lac(a, c + 0);
		}
		/* lac: INDIRU4(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + 120;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 7;
			_closure_lac(a, c + 0);
		}
		break;
	case 4213: /* CVFI4 */
		_label(LEFT_CHILD(a));
		/* lac: CVFI4(fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 59;
			_closure_lac(a, c + 0);
		}
		break;
	case 4214: /* CVFU4 */
		_label(LEFT_CHILD(a));
		/* lac: CVFU4(fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 58;
			_closure_lac(a, c + 0);
		}
		break;
	case 4229: /* CVII4 */
		_label(LEFT_CHILD(a));
		/* lac: CVII4(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + (if_cv_from(a,2,50));
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 54;
			_closure_lac(a, c + 0);
		}
		/* lac: CVII4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,60));
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 55;
			_closure_lac(a, c + 0);
		}
		/* reg: CVII4(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + (if_cv_from(a,2,50));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 30;
			_closure_reg(a, c + 0);
		}
		/* reg: CVII4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,60));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 31;
			_closure_reg(a, c + 0);
		}
		break;
	case 4230: /* CVIU4 */
		_label(LEFT_CHILD(a));
		/* lac: CVIU4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,50));
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 53;
			_closure_lac(a, c + 0);
		}
		/* reg: CVIU4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,50));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 29;
			_closure_reg(a, c + 0);
		}
		break;
	case 4277: /* CVUI4 */
		_label(LEFT_CHILD(a));
		/* lac: CVUI4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,50));
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 57;
			_closure_lac(a, c + 0);
		}
		/* reg: CVUI4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,50));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 33;
			_closure_reg(a, c + 0);
		}
		break;
	case 4278: /* CVUU4 */
		_label(LEFT_CHILD(a));
		/* lac: CVUU4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,50));
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 56;
			_closure_lac(a, c + 0);
		}
		/* reg: CVUU4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,50));
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 32;
			_closure_reg(a, c + 0);
		}
		break;
	case 4293: /* NEGI4 */
		_label(LEFT_CHILD(a));
		/* lac: NEGI4(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 30;
			_closure_lac(a, c + 0);
		}
		/* lac: NEGI4(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + (mincpu6(58));
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 60;
			_closure_lac(a, c + 0);
		}
		break;
	case 4309: /* CALLI4 */
		_label(LEFT_CHILD(a));
		/* lac: CALLI4(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + (mincpu5(28));
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 45;
			_closure_lac(a, c + 0);
		}
		/* lac: CALLI4(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 26;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 46;
			_closure_lac(a, c + 0);
		}
		/* lac: CALLI4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 47;
			_closure_lac(a, c + 0);
		}
		break;
	case 4310: /* CALLU4 */
		_label(LEFT_CHILD(a));
		/* lac: CALLU4(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + (mincpu5(28));
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 48;
			_closure_lac(a, c + 0);
		}
		/* lac: CALLU4(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 26;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 49;
			_closure_lac(a, c + 0);
		}
		/* lac: CALLU4(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 50;
			_closure_lac(a, c + 0);
		}
		break;
	case 4325: /* LOADI4 */
		_label(LEFT_CHILD(a));
		/* lac: LOADI4(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 0;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 51;
			_closure_lac(a, c + 0);
		}
		/* reg: LOADI4(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 120;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 26;
			_closure_reg(a, c + 0);
		}
		break;
	case 4326: /* LOADU4 */
		_label(LEFT_CHILD(a));
		/* lac: LOADU4(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 0;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 52;
			_closure_lac(a, c + 0);
		}
		/* reg: LOADU4(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 120;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 27;
			_closure_reg(a, c + 0);
		}
		break;
	case 4341: /* RETI4 */
		_label(LEFT_CHILD(a));
		/* stmt: RETI4(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 1;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 112;
		}
		break;
	case 4342: /* RETU4 */
		_label(LEFT_CHILD(a));
		/* stmt: RETU4(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 1;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 113;
		}
		break;
	case 4405: /* ADDI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: ADDI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 8;
			_closure_lac(a, c + 0);
		}
		/* lac: ADDI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 10;
			_closure_lac(a, c + 0);
		}
		break;
	case 4406: /* ADDU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: ADDU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 9;
			_closure_lac(a, c + 0);
		}
		/* lac: ADDU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 11;
			_closure_lac(a, c + 0);
		}
		break;
	case 4421: /* SUBI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: SUBI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 12;
			_closure_lac(a, c + 0);
		}
		break;
	case 4422: /* SUBU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: SUBU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 13;
			_closure_lac(a, c + 0);
		}
		break;
	case 4437: /* LSHI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: LSHI4(lac,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 22;
			_closure_lac(a, c + 0);
		}
		/* lac: LSHI4(lac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 23;
			_closure_lac(a, c + 0);
		}
		break;
	case 4438: /* LSHU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: LSHU4(lac,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 24;
			_closure_lac(a, c + 0);
		}
		/* lac: LSHU4(lac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 25;
			_closure_lac(a, c + 0);
		}
		break;
	case 4453: /* MODI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: MODI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 20;
			_closure_lac(a, c + 0);
		}
		break;
	case 4454: /* MODU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: MODU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 21;
			_closure_lac(a, c + 0);
		}
		break;
	case 4469: /* RSHI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: RSHI4(lac,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 26;
			_closure_lac(a, c + 0);
		}
		/* lac: RSHI4(lac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 27;
			_closure_lac(a, c + 0);
		}
		break;
	case 4470: /* RSHU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: RSHU4(lac,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 28;
			_closure_lac(a, c + 0);
		}
		/* lac: RSHU4(lac,conB) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_conB_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 29;
			_closure_lac(a, c + 0);
		}
		break;
	case 4485: /* BANDI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: BANDI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 39;
			_closure_lac(a, c + 0);
		}
		/* lac: BANDI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 40;
			_closure_lac(a, c + 0);
		}
		break;
	case 4486: /* BANDU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: BANDU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 32;
			_closure_lac(a, c + 0);
		}
		/* lac: BANDU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 33;
			_closure_lac(a, c + 0);
		}
		break;
	case 4501: /* BCOMI4 */
		_label(LEFT_CHILD(a));
		/* lac: BCOMI4(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 38;
			_closure_lac(a, c + 0);
		}
		break;
	case 4502: /* BCOMU4 */
		_label(LEFT_CHILD(a));
		/* lac: BCOMU4(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 31;
			_closure_lac(a, c + 0);
		}
		break;
	case 4517: /* BORI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: BORI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 41;
			_closure_lac(a, c + 0);
		}
		/* lac: BORI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 42;
			_closure_lac(a, c + 0);
		}
		break;
	case 4518: /* BORU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: BORU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 34;
			_closure_lac(a, c + 0);
		}
		/* lac: BORU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 35;
			_closure_lac(a, c + 0);
		}
		break;
	case 4533: /* BXORI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: BXORI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 43;
			_closure_lac(a, c + 0);
		}
		/* lac: BXORI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 44;
			_closure_lac(a, c + 0);
		}
		break;
	case 4534: /* BXORU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: BXORU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 36;
			_closure_lac(a, c + 0);
		}
		/* lac: BXORU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 37;
			_closure_lac(a, c + 0);
		}
		break;
	case 4549: /* DIVI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: DIVI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 18;
			_closure_lac(a, c + 0);
		}
		break;
	case 4550: /* DIVU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: DIVU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 19;
			_closure_lac(a, c + 0);
		}
		break;
	case 4565: /* MULI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: MULI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 14;
			_closure_lac(a, c + 0);
		}
		/* lac: MULI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 16;
			_closure_lac(a, c + 0);
		}
		break;
	case 4566: /* MULU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* lac: MULU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 15;
			_closure_lac(a, c + 0);
		}
		/* lac: MULU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_lac_NT]) {
			p->cost[_lac_NT] = c + 0;
			p->rule._lac = 17;
			_closure_lac(a, c + 0);
		}
		break;
	case 4581: /* EQI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 100;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 68;
		}
		/* stmt: EQI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 100;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 80;
		}
		break;
	case 4582: /* EQU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 100;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 70;
		}
		/* stmt: EQU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 100;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 82;
		}
		break;
	case 4597: /* GEI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 62;
		}
		/* stmt: GEI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 74;
		}
		break;
	case 4598: /* GEU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 66;
		}
		/* stmt: GEU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 78;
		}
		break;
	case 4613: /* GTI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 61;
		}
		/* stmt: GTI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 73;
		}
		break;
	case 4614: /* GTU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 65;
		}
		/* stmt: GTU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 77;
		}
		break;
	case 4629: /* LEI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 60;
		}
		/* stmt: LEI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 72;
		}
		break;
	case 4630: /* LEU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 64;
		}
		/* stmt: LEU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 76;
		}
		break;
	case 4645: /* LTI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 59;
		}
		/* stmt: LTI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 71;
		}
		break;
	case 4646: /* LTU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 63;
		}
		/* stmt: LTU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 75;
		}
		break;
	case 4661: /* NEI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEI4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 100;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 67;
		}
		/* stmt: NEI4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 100;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 79;
		}
		break;
	case 4662: /* NEU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEU4(lac,larg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_larg_NT] + 100;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 69;
		}
		/* stmt: NEU4(larg,lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_larg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_lac_NT] + 100;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 81;
		}
		break;
	case 5137: /* CNSTF5 */
		break;
	case 5153: /* ARGF5 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGF5(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_stk(a,100));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 99;
		}
		/* stmt: ARGF5(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + (if_arg_reg_only(a));
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 105;
		}
		break;
	case 5169: /* ASGNF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNF5(VREGP,reg) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			c = ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 8;
			}
		}
		/* asgn: ASGNF5(vdst,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_vdst_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 180;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 25;
			_closure_asgn(a, c + 0);
		}
		/* asgn: ASGNF5(vdst,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_vdst_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 150;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 26;
			_closure_asgn(a, c + 0);
		}
		if (	/* asgn: ASGNF5(addr,INDIRF5(asrc)) */
			RIGHT_CHILD(a)->op == 5185 /* INDIRF5 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_asrc_NT] + 150;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 27;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNF5(ac,INDIRF5(asrc)) */
			RIGHT_CHILD(a)->op == 5185 /* INDIRF5 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_asrc_NT] + 150;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 28;
				_closure_asgn(a, c + 0);
			}
		}
		if (	/* asgn: ASGNF5(lddr,INDIRF5(lsrc)) */
			RIGHT_CHILD(a)->op == 5185 /* INDIRF5 */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_lsrc_NT] + 190;
			if (c + 0 < p->cost[_asgn_NT]) {
				p->cost[_asgn_NT] = c + 0;
				p->rule._asgn = 29;
				_closure_asgn(a, c + 0);
			}
		}
		/* asgn: ASGNF5(spill,reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_spill_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_reg_NT] + 20;
		if (c + 0 < p->cost[_asgn_NT]) {
			p->cost[_asgn_NT] = c + 0;
			p->rule._asgn = 35;
			_closure_asgn(a, c + 0);
		}
		break;
	case 5185: /* INDIRF5 */
		_label(LEFT_CHILD(a));
		if (	/* reg: INDIRF5(VREGP) */
			LEFT_CHILD(a)->op == 711 /* VREGP */
		) {
			if (mayrecalc(a)) {
				struct _state *q = a->syms[RX]->u.t.cse->x.state;
				*p = *q;
			}
			c = 0;
			if (c + 0 < p->cost[_reg_NT]) {
				p->cost[_reg_NT] = c + 0;
				p->rule._reg = 8;
				_closure_reg(a, c + 0);
			}
		}
		/* farg: INDIRF5(eac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_eac_NT] + 0;
		if (c + 0 < p->cost[_farg_NT]) {
			p->cost[_farg_NT] = c + 0;
			p->rule._farg = 2;
		}
		/* reg: INDIRF5(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 150;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 19;
			_closure_reg(a, c + 0);
		}
		/* reg: INDIRF5(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + 190;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 20;
			_closure_reg(a, c + 0);
		}
		/* reg: INDIRF5(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + 150;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 21;
			_closure_reg(a, c + 0);
		}
		/* fac: INDIRF5(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 180;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 2;
			_closure_fac(a, c + 0);
		}
		/* fac: INDIRF5(lddr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lddr_NT] + 220;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 3;
			_closure_fac(a, c + 0);
		}
		/* fac: INDIRF5(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + 180;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 4;
			_closure_fac(a, c + 0);
		}
		break;
	case 5233: /* CVFF5 */
		break;
	case 5249: /* CVIF5 */
		_label(LEFT_CHILD(a));
		/* fac: CVIF5(ac0) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac0_NT] + (if_cv_from(a,2,178));
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 20;
			_closure_fac(a, c + 0);
		}
		/* fac: CVIF5(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,180));
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 21;
			_closure_fac(a, c + 0);
		}
		/* fac: CVIF5(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + (if_cv_from(a,4,200));
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 22;
			_closure_fac(a, c + 0);
		}
		break;
	case 5297: /* CVUF5 */
		_label(LEFT_CHILD(a));
		/* fac: CVUF5(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + (if_cv_from(a,2,180));
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 18;
			_closure_fac(a, c + 0);
		}
		/* fac: CVUF5(lac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_lac_NT] + (if_cv_from(a,4,200));
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 19;
			_closure_fac(a, c + 0);
		}
		break;
	case 5313: /* NEGF5 */
		_label(LEFT_CHILD(a));
		/* fac: NEGF5(fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + 50;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 13;
			_closure_fac(a, c + 0);
		}
		break;
	case 5329: /* CALLF5 */
		_label(LEFT_CHILD(a));
		/* fac: CALLF5(addr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_addr_NT] + (mincpu5(28));
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 14;
			_closure_fac(a, c + 0);
		}
		/* fac: CALLF5(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 26;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 15;
			_closure_fac(a, c + 0);
		}
		/* fac: CALLF5(ac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ac_NT] + 26;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 16;
			_closure_fac(a, c + 0);
		}
		break;
	case 5345: /* LOADF5 */
		_label(LEFT_CHILD(a));
		/* fac: LOADF5(fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + 0;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 17;
			_closure_fac(a, c + 0);
		}
		/* reg: LOADF5(reg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_reg_NT] + 150;
		if (c + 0 < p->cost[_reg_NT]) {
			p->cost[_reg_NT] = c + 0;
			p->rule._reg = 28;
			_closure_reg(a, c + 0);
		}
		break;
	case 5361: /* RETF5 */
		_label(LEFT_CHILD(a));
		/* stmt: RETF5(fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + 1;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 111;
		}
		break;
	case 5425: /* ADDF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* fac: ADDF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 5;
			_closure_fac(a, c + 0);
		}
		/* fac: ADDF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 6;
			_closure_fac(a, c + 0);
		}
		break;
	case 5441: /* SUBF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* fac: SUBF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 7;
			_closure_fac(a, c + 0);
		}
		/* fac: SUBF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 8;
			_closure_fac(a, c + 0);
		}
		break;
	case 5569: /* DIVF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* fac: DIVF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 11;
			_closure_fac(a, c + 0);
		}
		/* fac: DIVF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 12;
			_closure_fac(a, c + 0);
		}
		break;
	case 5585: /* MULF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* fac: MULF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 9;
			_closure_fac(a, c + 0);
		}
		/* fac: MULF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_fac_NT]) {
			p->cost[_fac_NT] = c + 0;
			p->rule._fac = 10;
			_closure_fac(a, c + 0);
		}
		break;
	case 5601: /* EQF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 84;
		}
		/* stmt: EQF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 90;
		}
		break;
	case 5617: /* GEF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 89;
		}
		/* stmt: GEF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 95;
		}
		break;
	case 5633: /* GTF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 88;
		}
		/* stmt: GTF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 94;
		}
		break;
	case 5649: /* LEF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 87;
		}
		/* stmt: LEF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 93;
		}
		break;
	case 5665: /* LTF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 86;
		}
		/* stmt: LTF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 92;
		}
		break;
	case 5681: /* NEF5 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEF5(fac,farg) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_fac_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_farg_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 85;
		}
		/* stmt: NEF5(farg,fac) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_farg_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_fac_NT] + 200;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 91;
		}
		break;
	default:
		fatal("_label", "Bad terminal %d\n", OP_LABEL(a));
	}
}

static void _kids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE kids[]) {
	if (!p)
		fatal("_kids", "Null tree\n", 0);
	if (!kids)
		fatal("_kids", "Null kids\n", 0);
	switch (eruleno) {
	case 632: /* rmw: VREGP */
	case 526: /* stmt: LABELV */
	case 91: /* spill: ADDRLP2 */
	case 56: /* zddr: ADDRGP2 */
	case 53: /* addr: ADDRGP2 */
	case 52: /* lddr: ADDRFP2 */
	case 51: /* lddr: ADDRLP2 */
	case 50: /* conXX00: CNSTI2 */
	case 49: /* conXX00: CNSTU2 */
	case 48: /* conFF00: CNSTU2 */
	case 47: /* conFF: CNSTI2 */
	case 46: /* conFF: CNSTU2 */
	case 45: /* con8: CNSTI2 */
	case 44: /* con8: CNSTU2 */
	case 42: /* con: CNSTP2 */
	case 41: /* con: CNSTU2 */
	case 40: /* con: CNSTI2 */
	case 39: /* con: CNSTU1 */
	case 38: /* con: CNSTI1 */
	case 37: /* conBa: CNSTU2 */
	case 36: /* conBa: CNSTI2 */
	case 35: /* conBm: CNSTU2 */
	case 34: /* conBm: CNSTI2 */
	case 33: /* conBn: CNSTI2 */
	case 32: /* conBs: CNSTI2 */
	case 31: /* conBs: CNSTI1 */
	case 29: /* conB: CNSTU1 */
	case 28: /* conB: CNSTP2 */
	case 27: /* conB: CNSTU2 */
	case 26: /* conB: CNSTI2 */
	case 25: /* con1: CNSTU2 */
	case 24: /* con1: CNSTI2 */
	case 23: /* con1: CNSTU1 */
	case 22: /* con1: CNSTI1 */
	case 21: /* con0: CNSTP2 */
	case 20: /* con0: CNSTU2 */
	case 19: /* con0: CNSTI2 */
	case 18: /* con0: CNSTU1 */
	case 17: /* con0: CNSTI1 */
	case 8: /* reg: INDIRF5(VREGP) */
	case 7: /* reg: INDIRU4(VREGP) */
	case 6: /* reg: INDIRI4(VREGP) */
	case 5: /* reg: INDIRP2(VREGP) */
	case 4: /* reg: INDIRU2(VREGP) */
	case 3: /* reg: INDIRI2(VREGP) */
	case 2: /* reg: INDIRU1(VREGP) */
	case 1: /* reg: INDIRI1(VREGP) */
		break;
	case 16: /* stmt: ASGNF5(VREGP,reg) */
	case 15: /* stmt: ASGNU4(VREGP,reg) */
	case 14: /* stmt: ASGNI4(VREGP,reg) */
	case 13: /* stmt: ASGNP2(VREGP,reg) */
	case 12: /* stmt: ASGNU2(VREGP,reg) */
	case 11: /* stmt: ASGNI2(VREGP,reg) */
	case 10: /* stmt: ASGNU1(VREGP,reg) */
	case 9: /* stmt: ASGNI1(VREGP,reg) */
		kids[0] = RIGHT_CHILD(p);
		break;
	case 633: /* rmw: zddr */
	case 584: /* reg: con */
	case 583: /* reg: conB */
	case 537: /* regx: conB */
	case 536: /* regx: con */
	case 395: /* fac: reg */
	case 391: /* reg: fac */
	case 389: /* farg: regx */
	case 388: /* stmt: fac */
	case 310: /* lac: reg */
	case 303: /* reg: lac */
	case 300: /* larg: regx */
	case 299: /* stmt: lac */
	case 294: /* lsrc: addr */
	case 293: /* asrc: eac */
	case 292: /* asrc: lddr */
	case 291: /* asrc: addr */
	case 290: /* vdst: eac */
	case 289: /* vdst: lddr */
	case 288: /* vdst: addr */
	case 87: /* iarg: regx */
	case 71: /* eac: con */
	case 70: /* eac0: conB */
	case 69: /* eac: lddr */
	case 68: /* eac: reg */
	case 67: /* ac: eac */
	case 66: /* ac: ac0 */
	case 65: /* eac: eac0 */
	case 64: /* ac0: eac0 */
	case 63: /* reg: ac */
	case 62: /* reg: regx */
	case 61: /* regx: reg */
	case 60: /* stmt: ac */
	case 59: /* stmt: asgn */
	case 58: /* stmt: reg */
	case 57: /* zddr: conB */
	case 55: /* addr: zddr */
	case 54: /* addr: con */
	case 43: /* con: addr */
	case 30: /* conB: zddr */
		kids[0] = p;
		break;
	case 631: /* stmt: ARGP2(reg) */
	case 630: /* stmt: ARGU2(reg) */
	case 629: /* stmt: ARGI2(reg) */
	case 625: /* eac: INDIRP2(lddr) */
	case 624: /* eac: INDIRU2(lddr) */
	case 623: /* eac: INDIRI2(lddr) */
	case 622: /* eac: INDIRU1(lddr) */
	case 621: /* eac: INDIRI1(lddr) */
	case 608: /* reg: LOADP2(reg) */
	case 607: /* reg: LOADU2(reg) */
	case 606: /* reg: LOADI2(reg) */
	case 605: /* regx: LOADP2(con) */
	case 604: /* regx: LOADU2(con) */
	case 603: /* regx: LOADI2(con) */
	case 602: /* regx: LOADP2(conB) */
	case 601: /* regx: LOADU2(conB) */
	case 600: /* regx: LOADI2(conB) */
	case 599: /* regx: LOADU1(conB) */
	case 598: /* regx: LOADI1(conBs) */
	case 589: /* reg: INDIRU1(ac) */
	case 588: /* reg: INDIRI1(ac) */
	case 587: /* reg: INDIRP2(ac) */
	case 586: /* reg: INDIRU2(ac) */
	case 585: /* reg: INDIRI2(ac) */
	case 577: /* eac0: INDIRU1(reg) */
	case 576: /* eac0: INDIRI1(reg) */
	case 575: /* eac: INDIRP2(reg) */
	case 574: /* eac: INDIRU2(reg) */
	case 573: /* eac: INDIRI2(reg) */
	case 559: /* lac: NEGI4(lac) */
	case 558: /* eac: NEGI2(eac) */
	case 557: /* eac: CVII2(eac) */
	case 556: /* eac: CVII2(reg) */
	case 553: /* ac: NEGI2(ac) */
	case 552: /* ac: CVII2(ac) */
	case 529: /* stmt: JUMPV(ac) */
	case 528: /* stmt: JUMPV(reg) */
	case 527: /* stmt: JUMPV(addr) */
	case 525: /* fac: CVIF5(lac) */
	case 524: /* fac: CVIF5(ac) */
	case 523: /* fac: CVIF5(ac0) */
	case 522: /* lac: CVFI4(fac) */
	case 521: /* ac: CVFI2(fac) */
	case 520: /* fac: CVUF5(lac) */
	case 519: /* fac: CVUF5(ac) */
	case 518: /* lac: CVFU4(fac) */
	case 517: /* ac: CVFU2(fac) */
	case 516: /* reg: CVUI4(ac) */
	case 515: /* reg: CVUU4(ac) */
	case 514: /* reg: CVII4(ac) */
	case 513: /* reg: CVII4(ac0) */
	case 512: /* reg: CVIU4(ac) */
	case 511: /* lac: CVUI4(ac) */
	case 510: /* lac: CVUU4(ac) */
	case 509: /* lac: CVII4(ac) */
	case 508: /* lac: CVII4(ac0) */
	case 507: /* lac: CVIU4(ac) */
	case 506: /* eac0: CVUI2(eac) */
	case 505: /* eac: CVII2(eac) */
	case 504: /* eac0: CVUI2(eac0) */
	case 503: /* eac: CVII2(eac0) */
	case 502: /* ac0: CVUI2(ac) */
	case 501: /* ac: CVII2(ac) */
	case 500: /* ac0: CVUI2(ac0) */
	case 499: /* ac: CVII2(ac0) */
	case 498: /* eac0: CVUI2(reg) */
	case 497: /* eac: CVII2(reg) */
	case 496: /* reg: LOADF5(reg) */
	case 495: /* reg: LOADU4(reg) */
	case 494: /* reg: LOADI4(reg) */
	case 493: /* reg: LOADU1(ac) */
	case 492: /* reg: LOADI1(ac) */
	case 491: /* reg: LOADU1(reg) */
	case 490: /* reg: LOADI1(reg) */
	case 489: /* fac: LOADF5(fac) */
	case 488: /* lac: LOADU4(lac) */
	case 487: /* lac: LOADI4(lac) */
	case 486: /* eac: LOADP2(lac) */
	case 485: /* eac: LOADU2(lac) */
	case 484: /* eac: LOADI2(lac) */
	case 483: /* eac: LOADP2(reg) */
	case 482: /* eac: LOADU2(reg) */
	case 481: /* eac: LOADI2(reg) */
	case 480: /* eac: LOADP2(eac) */
	case 479: /* eac: LOADU2(eac) */
	case 478: /* eac: LOADI2(eac) */
	case 477: /* eac: LOADU1(eac) */
	case 476: /* eac: LOADI1(eac) */
	case 475: /* ac0: LOADP2(ac0) */
	case 474: /* ac0: LOADU2(ac0) */
	case 473: /* ac0: LOADI2(ac0) */
	case 472: /* ac: LOADP2(ac) */
	case 471: /* ac: LOADU2(ac) */
	case 470: /* ac: LOADI2(ac) */
	case 469: /* ac0: LOADU1(ac0) */
	case 468: /* ac0: LOADI1(ac0) */
	case 467: /* ac: LOADU1(ac) */
	case 466: /* ac: LOADI1(ac) */
	case 465: /* eac0: LOADU1(reg) */
	case 464: /* eac0: LOADI1(reg) */
	case 463: /* stmt: RETP2(ac) */
	case 462: /* stmt: RETU2(ac) */
	case 461: /* stmt: RETI2(ac) */
	case 460: /* stmt: RETU4(lac) */
	case 459: /* stmt: RETI4(lac) */
	case 458: /* stmt: RETF5(fac) */
	case 457: /* stmt: ARGP2(reg) */
	case 456: /* stmt: ARGU2(reg) */
	case 455: /* stmt: ARGI2(reg) */
	case 454: /* stmt: ARGU4(reg) */
	case 453: /* stmt: ARGI4(reg) */
	case 452: /* stmt: ARGF5(reg) */
	case 451: /* stmt: ARGP2(reg) */
	case 450: /* stmt: ARGU2(reg) */
	case 449: /* stmt: ARGI2(reg) */
	case 448: /* stmt: ARGU4(reg) */
	case 447: /* stmt: ARGI4(reg) */
	case 446: /* stmt: ARGF5(reg) */
	case 445: /* stmt: CALLV(ac) */
	case 444: /* stmt: CALLV(reg) */
	case 443: /* stmt: CALLV(addr) */
	case 442: /* ac: CALLP2(ac) */
	case 441: /* ac: CALLP2(reg) */
	case 440: /* ac: CALLP2(addr) */
	case 439: /* ac: CALLU2(ac) */
	case 438: /* ac: CALLU2(reg) */
	case 437: /* ac: CALLU2(addr) */
	case 436: /* ac: CALLI2(ac) */
	case 435: /* ac: CALLI2(reg) */
	case 434: /* ac: CALLI2(addr) */
	case 433: /* lac: CALLU4(ac) */
	case 432: /* lac: CALLU4(reg) */
	case 431: /* lac: CALLU4(addr) */
	case 430: /* lac: CALLI4(ac) */
	case 429: /* lac: CALLI4(reg) */
	case 428: /* lac: CALLI4(addr) */
	case 427: /* fac: CALLF5(ac) */
	case 426: /* fac: CALLF5(reg) */
	case 425: /* fac: CALLF5(addr) */
	case 407: /* fac: NEGF5(fac) */
	case 398: /* fac: INDIRF5(addr) */
	case 397: /* fac: INDIRF5(lddr) */
	case 396: /* fac: INDIRF5(ac) */
	case 394: /* reg: INDIRF5(addr) */
	case 393: /* reg: INDIRF5(lddr) */
	case 392: /* reg: INDIRF5(ac) */
	case 390: /* farg: INDIRF5(eac) */
	case 347: /* lac: BCOMI4(lac) */
	case 340: /* lac: BCOMU4(lac) */
	case 339: /* lac: NEGI4(lac) */
	case 316: /* lac: INDIRU4(addr) */
	case 315: /* lac: INDIRI4(addr) */
	case 314: /* lac: INDIRU4(lddr) */
	case 313: /* lac: INDIRU4(lddr) */
	case 312: /* lac: INDIRU4(ac) */
	case 311: /* lac: INDIRI4(ac) */
	case 309: /* reg: INDIRU4(addr) */
	case 308: /* reg: INDIRI4(addr) */
	case 307: /* reg: INDIRU4(lddr) */
	case 306: /* reg: INDIRI4(lddr) */
	case 305: /* reg: INDIRU4(ac) */
	case 304: /* reg: INDIRI4(ac) */
	case 302: /* larg: INDIRU4(eac) */
	case 301: /* larg: INDIRI4(eac) */
	case 152: /* ac: BCOMU2(ac) */
	case 151: /* ac: BCOMI2(ac) */
	case 121: /* ac: NEGI2(regx) */
	case 120: /* ac: NEGI2(ac) */
	case 94: /* iarg: INDIRP2(spill) */
	case 93: /* iarg: INDIRI2(spill) */
	case 92: /* iarg: INDIRU2(spill) */
	case 90: /* iarg: INDIRP2(zddr) */
	case 89: /* iarg: INDIRU2(zddr) */
	case 88: /* iarg: INDIRI2(zddr) */
	case 86: /* ac0: INDIRU1(ac) */
	case 85: /* ac0: INDIRI1(ac) */
	case 84: /* ac: INDIRP2(ac) */
	case 83: /* ac: INDIRU2(ac) */
	case 82: /* ac: INDIRI2(ac) */
	case 81: /* eac0: INDIRU1(zddr) */
	case 80: /* eac0: INDIRI1(zddr) */
	case 79: /* eac: INDIRP2(zddr) */
	case 78: /* eac: INDIRU2(zddr) */
	case 77: /* eac: INDIRI2(zddr) */
	case 76: /* eac0: INDIRU1(eac) */
	case 75: /* eac0: INDIRI1(eac) */
	case 74: /* eac: INDIRP2(eac) */
	case 73: /* eac: INDIRU2(eac) */
	case 72: /* eac: INDIRI2(eac) */
		kids[0] = LEFT_CHILD(p);
		break;
	case 694: /* ac: ADDU2(ac0,conXX00) */
	case 693: /* regx: ADDU2(ac0,conXX00) */
	case 680: /* ac: BANDI2(ac,conFF00) */
	case 679: /* ac: BANDU2(ac,conFF00) */
	case 678: /* ac: BANDI2(ac,conFF00) */
	case 677: /* ac: BANDU2(ac,conFF00) */
	case 676: /* reg: BANDU2(ac0,conFF) */
	case 675: /* reg: BANDI2(ac0,conFF) */
	case 674: /* ac0: BANDU2(ac,conFF) */
	case 673: /* ac0: BANDI2(ac,conFF) */
	case 670: /* reg: LSHU2(ac,con8) */
	case 669: /* reg: LSHU2(ac,con8) */
	case 668: /* reg: LSHI2(ac,con8) */
	case 667: /* reg: LSHI2(ac,con8) */
	case 666: /* ac: LSHU2(ac,con8) */
	case 665: /* ac: LSHI2(ac,con8) */
	case 664: /* ac: LSHU2(ac,con8) */
	case 663: /* ac: LSHI2(ac,con8) */
	case 662: /* reg: RSHI2(reg,con8) */
	case 661: /* reg: RSHI2(reg,con8) */
	case 660: /* ac: RSHI2(ac,con8) */
	case 659: /* ac: RSHI2(ac,con8) */
	case 658: /* reg: RSHU2(reg,con8) */
	case 657: /* ac0: RSHU2(ac,con8) */
	case 656: /* ac0: RSHU2(reg,con8) */
	case 628: /* asgn: ASGNP2(lddr,ac) */
	case 627: /* asgn: ASGNU2(lddr,ac) */
	case 626: /* asgn: ASGNI2(lddr,ac) */
	case 597: /* asgn: ASGNP2(rmw,con) */
	case 596: /* asgn: ASGNU2(rmw,con) */
	case 595: /* asgn: ASGNI2(rmw,con) */
	case 594: /* asgn: ASGNP2(rmw,conB) */
	case 593: /* asgn: ASGNU2(rmw,conB) */
	case 592: /* asgn: ASGNI2(rmw,conB) */
	case 591: /* asgn: ASGNU1(rmw,conB) */
	case 590: /* asgn: ASGNI1(rmw,conBs) */
	case 572: /* asgn: ASGNU1(ac,conB) */
	case 571: /* asgn: ASGNI1(ac,conBs) */
	case 570: /* asgn: ASGNU2(ac,conB) */
	case 569: /* asgn: ASGNI2(ac,conB) */
	case 568: /* asgn: ASGNP2(ac,conB) */
	case 567: /* asgn: ASGNU2(ac,con) */
	case 566: /* asgn: ASGNI2(ac,con) */
	case 565: /* asgn: ASGNP2(ac,con) */
	case 564: /* asgn: ASGNU1(ac,iarg) */
	case 563: /* asgn: ASGNI1(ac,iarg) */
	case 562: /* asgn: ASGNU2(ac,iarg) */
	case 561: /* asgn: ASGNI2(ac,iarg) */
	case 560: /* asgn: ASGNP2(ac,iarg) */
	case 555: /* eac: MULU2(conBm,eac) */
	case 554: /* eac: MULI2(conBm,eac) */
	case 551: /* ac: MULU2(con,ac) */
	case 550: /* ac: MULI2(con,ac) */
	case 549: /* eac: SUBP2(eac,con) */
	case 548: /* eac: SUBU2(eac,con) */
	case 547: /* eac: SUBI2(eac,con) */
	case 546: /* eac: ADDP2(eac,con) */
	case 545: /* eac: ADDU2(eac,con) */
	case 544: /* eac: ADDI2(eac,con) */
	case 543: /* ac: SUBP2(ac,con) */
	case 542: /* ac: SUBU2(ac,con) */
	case 541: /* ac: SUBI2(ac,con) */
	case 540: /* ac: ADDP2(ac,con) */
	case 539: /* ac: ADDU2(ac,con) */
	case 538: /* ac: ADDI2(ac,con) */
	case 535: /* asgn: ASGNF5(spill,reg) */
	case 534: /* asgn: ASGNU4(spill,reg) */
	case 533: /* asgn: ASGNI4(spill,reg) */
	case 532: /* asgn: ASGNP2(spill,reg) */
	case 531: /* asgn: ASGNU2(spill,reg) */
	case 530: /* asgn: ASGNI2(spill,reg) */
	case 421: /* asgn: ASGNF5(vdst,reg) */
	case 420: /* asgn: ASGNF5(vdst,fac) */
	case 419: /* stmt: GEF5(farg,fac) */
	case 418: /* stmt: GTF5(farg,fac) */
	case 417: /* stmt: LEF5(farg,fac) */
	case 416: /* stmt: LTF5(farg,fac) */
	case 415: /* stmt: NEF5(farg,fac) */
	case 414: /* stmt: EQF5(farg,fac) */
	case 413: /* stmt: GEF5(fac,farg) */
	case 412: /* stmt: GTF5(fac,farg) */
	case 411: /* stmt: LEF5(fac,farg) */
	case 410: /* stmt: LTF5(fac,farg) */
	case 409: /* stmt: NEF5(fac,farg) */
	case 408: /* stmt: EQF5(fac,farg) */
	case 406: /* fac: DIVF5(farg,fac) */
	case 405: /* fac: DIVF5(fac,farg) */
	case 404: /* fac: MULF5(farg,fac) */
	case 403: /* fac: MULF5(fac,farg) */
	case 402: /* fac: SUBF5(farg,fac) */
	case 401: /* fac: SUBF5(fac,farg) */
	case 400: /* fac: ADDF5(farg,fac) */
	case 399: /* fac: ADDF5(fac,farg) */
	case 384: /* asgn: ASGNU4(vdst,reg) */
	case 383: /* asgn: ASGNU4(vdst,lac) */
	case 379: /* asgn: ASGNI4(vdst,reg) */
	case 378: /* asgn: ASGNI4(vdst,lac) */
	case 377: /* stmt: EQU4(larg,lac) */
	case 376: /* stmt: NEU4(larg,lac) */
	case 375: /* stmt: EQI4(larg,lac) */
	case 374: /* stmt: NEI4(larg,lac) */
	case 373: /* stmt: GEU4(larg,lac) */
	case 372: /* stmt: GTU4(larg,lac) */
	case 371: /* stmt: LEU4(larg,lac) */
	case 370: /* stmt: LTU4(larg,lac) */
	case 369: /* stmt: GEI4(larg,lac) */
	case 368: /* stmt: GTI4(larg,lac) */
	case 367: /* stmt: LEI4(larg,lac) */
	case 366: /* stmt: LTI4(larg,lac) */
	case 365: /* stmt: EQU4(lac,larg) */
	case 364: /* stmt: NEU4(lac,larg) */
	case 363: /* stmt: EQI4(lac,larg) */
	case 362: /* stmt: NEI4(lac,larg) */
	case 361: /* stmt: GEU4(lac,larg) */
	case 360: /* stmt: GTU4(lac,larg) */
	case 359: /* stmt: LEU4(lac,larg) */
	case 358: /* stmt: LTU4(lac,larg) */
	case 357: /* stmt: GEI4(lac,larg) */
	case 356: /* stmt: GTI4(lac,larg) */
	case 355: /* stmt: LEI4(lac,larg) */
	case 354: /* stmt: LTI4(lac,larg) */
	case 353: /* lac: BXORI4(larg,lac) */
	case 352: /* lac: BXORI4(lac,larg) */
	case 351: /* lac: BORI4(larg,lac) */
	case 350: /* lac: BORI4(lac,larg) */
	case 349: /* lac: BANDI4(larg,lac) */
	case 348: /* lac: BANDI4(lac,larg) */
	case 346: /* lac: BXORU4(larg,lac) */
	case 345: /* lac: BXORU4(lac,larg) */
	case 344: /* lac: BORU4(larg,lac) */
	case 343: /* lac: BORU4(lac,larg) */
	case 342: /* lac: BANDU4(larg,lac) */
	case 341: /* lac: BANDU4(lac,larg) */
	case 338: /* lac: RSHU4(lac,conB) */
	case 337: /* lac: RSHU4(lac,reg) */
	case 336: /* lac: RSHI4(lac,conB) */
	case 335: /* lac: RSHI4(lac,reg) */
	case 334: /* lac: LSHU4(lac,conB) */
	case 333: /* lac: LSHU4(lac,reg) */
	case 332: /* lac: LSHI4(lac,conB) */
	case 331: /* lac: LSHI4(lac,reg) */
	case 330: /* lac: MODU4(lac,larg) */
	case 329: /* lac: MODI4(lac,larg) */
	case 328: /* lac: DIVU4(lac,larg) */
	case 327: /* lac: DIVI4(lac,larg) */
	case 326: /* lac: MULU4(larg,lac) */
	case 325: /* lac: MULI4(larg,lac) */
	case 324: /* lac: MULU4(lac,larg) */
	case 323: /* lac: MULI4(lac,larg) */
	case 322: /* lac: SUBU4(lac,larg) */
	case 321: /* lac: SUBI4(lac,larg) */
	case 320: /* lac: ADDU4(larg,lac) */
	case 319: /* lac: ADDI4(larg,lac) */
	case 318: /* lac: ADDU4(lac,larg) */
	case 317: /* lac: ADDI4(lac,larg) */
	case 287: /* stmt: GEU2(iarg,ac) */
	case 286: /* stmt: GTU2(iarg,ac) */
	case 285: /* stmt: LEU2(iarg,ac) */
	case 284: /* stmt: LTU2(iarg,ac) */
	case 283: /* stmt: GEI2(iarg,ac) */
	case 282: /* stmt: GTI2(iarg,ac) */
	case 281: /* stmt: LEI2(iarg,ac) */
	case 280: /* stmt: LTI2(iarg,ac) */
	case 279: /* stmt: GEU2(ac,iarg) */
	case 278: /* stmt: GTU2(ac,iarg) */
	case 277: /* stmt: LEU2(ac,iarg) */
	case 276: /* stmt: LTU2(ac,iarg) */
	case 275: /* stmt: GEI2(ac,iarg) */
	case 274: /* stmt: GTI2(ac,iarg) */
	case 273: /* stmt: LEI2(ac,iarg) */
	case 272: /* stmt: LTI2(ac,iarg) */
	case 271: /* stmt: GEU2(ac,conB) */
	case 270: /* stmt: GTU2(ac,conB) */
	case 269: /* stmt: LEU2(ac,conB) */
	case 268: /* stmt: LTU2(ac,conB) */
	case 267: /* stmt: GEI2(ac,conB) */
	case 266: /* stmt: GTI2(ac,conB) */
	case 265: /* stmt: LEI2(ac,conB) */
	case 264: /* stmt: LTI2(ac,conB) */
	case 263: /* stmt: LEU2(ac,con0) */
	case 262: /* stmt: GTU2(ac,con0) */
	case 261: /* stmt: GEI2(ac,con0) */
	case 260: /* stmt: GTI2(ac,con0) */
	case 259: /* stmt: LEI2(ac,con0) */
	case 258: /* stmt: LTI2(ac,con0) */
	case 257: /* stmt: NEU2(iarg,ac) */
	case 256: /* stmt: NEU2(ac,iarg) */
	case 255: /* stmt: NEU2(ac,conB) */
	case 254: /* stmt: NEU2(ac,con0) */
	case 253: /* stmt: EQU2(iarg,ac) */
	case 252: /* stmt: EQU2(ac,iarg) */
	case 251: /* stmt: EQU2(ac,conB) */
	case 250: /* stmt: EQU2(ac,con0) */
	case 249: /* stmt: NEI2(iarg,ac) */
	case 248: /* stmt: NEI2(ac,iarg) */
	case 247: /* stmt: NEI2(ac,conB) */
	case 246: /* stmt: NEI2(ac,con0) */
	case 245: /* stmt: EQI2(iarg,ac) */
	case 244: /* stmt: EQI2(ac,iarg) */
	case 243: /* stmt: EQI2(ac,conB) */
	case 242: /* stmt: EQI2(ac,con0) */
	case 241: /* asgn: ASGNU1(iarg,ac) */
	case 240: /* asgn: ASGNU1(zddr,ac) */
	case 239: /* asgn: ASGNI1(iarg,ac) */
	case 238: /* asgn: ASGNI1(zddr,ac) */
	case 237: /* asgn: ASGNU2(iarg,ac) */
	case 236: /* asgn: ASGNU2(zddr,ac) */
	case 235: /* asgn: ASGNI2(iarg,ac) */
	case 234: /* asgn: ASGNI2(zddr,ac) */
	case 233: /* asgn: ASGNP2(iarg,ac) */
	case 232: /* asgn: ASGNP2(zddr,ac) */
	case 231: /* eac: BXORU2(iarg,eac) */
	case 230: /* eac: BXORI2(iarg,eac) */
	case 229: /* eac: BXORU2(eac,iarg) */
	case 228: /* eac: BXORI2(eac,iarg) */
	case 227: /* eac: BORU2(iarg,eac) */
	case 226: /* eac: BORI2(iarg,eac) */
	case 225: /* eac: BORU2(eac,iarg) */
	case 224: /* eac: BORI2(eac,iarg) */
	case 223: /* eac: BANDU2(iarg,eac) */
	case 222: /* eac: BANDI2(iarg,eac) */
	case 221: /* eac: BANDU2(eac,iarg) */
	case 220: /* eac: BANDI2(eac,iarg) */
	case 219: /* eac: SUBP2(eac,iarg) */
	case 218: /* eac: SUBU2(eac,iarg) */
	case 217: /* eac: SUBI2(eac,iarg) */
	case 212: /* eac: ADDP2(iarg,eac) */
	case 211: /* eac: ADDU2(iarg,eac) */
	case 210: /* eac: ADDI2(iarg,eac) */
	case 209: /* eac: ADDP2(eac,iarg) */
	case 208: /* eac: ADDU2(eac,iarg) */
	case 207: /* eac: ADDI2(eac,iarg) */
	case 206: /* eac0: BXORU2(eac0,conB) */
	case 205: /* eac0: BXORI2(eac0,conB) */
	case 204: /* eac: BXORU2(eac,conB) */
	case 203: /* eac: BXORI2(eac,conB) */
	case 202: /* eac0: BORU2(eac0,conB) */
	case 201: /* eac0: BORI2(eac0,conB) */
	case 200: /* eac: BORU2(eac,conB) */
	case 199: /* eac: BORI2(eac,conB) */
	case 198: /* eac0: BANDU2(eac0,conB) */
	case 197: /* eac0: BANDI2(eac0,conB) */
	case 196: /* eac: BANDU2(eac,conB) */
	case 195: /* eac: BANDI2(eac,conB) */
	case 194: /* eac: MULI2(conB,eac) */
	case 193: /* eac: MULI2(conB,eac) */
	case 192: /* eac: LSHU2(eac,conB) */
	case 191: /* eac: LSHI2(eac,conB) */
	case 190: /* eac: LSHU2(eac,con1) */
	case 189: /* eac: LSHI2(eac,con1) */
	case 188: /* eac: SUBP2(eac,conBn) */
	case 187: /* eac: SUBU2(eac,conBn) */
	case 186: /* eac: SUBI2(eac,conBn) */
	case 185: /* eac: SUBP2(eac,conB) */
	case 184: /* eac: SUBU2(eac,conB) */
	case 183: /* eac: SUBI2(eac,conB) */
	case 182: /* eac: ADDP2(eac,conBn) */
	case 181: /* eac: ADDU2(eac,conBn) */
	case 180: /* eac: ADDI2(eac,conBn) */
	case 179: /* eac: ADDP2(eac,conB) */
	case 178: /* eac: ADDU2(eac,conB) */
	case 177: /* eac: ADDI2(eac,conB) */
	case 176: /* ac0: BXORU2(ac0,conB) */
	case 175: /* ac0: BXORI2(ac0,conB) */
	case 174: /* ac: BXORU2(ac,conB) */
	case 173: /* ac: BXORI2(ac,conB) */
	case 172: /* ac: BXORU2(iarg,ac) */
	case 171: /* ac: BXORI2(iarg,ac) */
	case 170: /* ac: BXORU2(ac,iarg) */
	case 169: /* ac: BXORI2(ac,iarg) */
	case 168: /* ac0: BORU2(ac0,conB) */
	case 167: /* ac0: BORI2(ac0,conB) */
	case 166: /* ac: BORU2(ac,conB) */
	case 165: /* ac: BORI2(ac,conB) */
	case 164: /* ac: BORU2(iarg,ac) */
	case 163: /* ac: BORI2(iarg,ac) */
	case 162: /* ac: BORU2(ac,iarg) */
	case 161: /* ac: BORI2(ac,iarg) */
	case 160: /* ac0: BANDU2(ac0,conB) */
	case 159: /* ac0: BANDI2(ac0,conB) */
	case 158: /* ac: BANDU2(ac,conB) */
	case 157: /* ac: BANDI2(ac,conB) */
	case 156: /* ac: BANDU2(iarg,ac) */
	case 155: /* ac: BANDI2(iarg,ac) */
	case 154: /* ac: BANDU2(ac,iarg) */
	case 153: /* ac: BANDI2(ac,iarg) */
	case 150: /* ac: MODU2(ac,con) */
	case 149: /* ac: MODI2(ac,con) */
	case 148: /* ac: DIVU2(ac,con) */
	case 147: /* ac: DIVI2(ac,con) */
	case 146: /* ac: MODU2(ac,iarg) */
	case 145: /* ac: MODI2(ac,iarg) */
	case 144: /* ac: DIVU2(ac,iarg) */
	case 143: /* ac: DIVI2(ac,iarg) */
	case 142: /* ac: MULU2(iarg,ac) */
	case 141: /* ac: MULU2(ac,iarg) */
	case 140: /* ac: MULU2(con,ac) */
	case 139: /* ac: MULU2(conB,ac) */
	case 138: /* ac: MULI2(iarg,ac) */
	case 137: /* ac: MULI2(ac,iarg) */
	case 136: /* ac: MULI2(con,ac) */
	case 135: /* ac: MULI2(conBn,regx) */
	case 134: /* ac: MULI2(conB,regx) */
	case 133: /* ac: MULI2(conBn,ac) */
	case 132: /* ac: MULI2(conB,ac) */
	case 131: /* ac: RSHU2(ac,iarg) */
	case 130: /* ac: LSHU2(ac,iarg) */
	case 129: /* ac: RSHI2(ac,iarg) */
	case 128: /* ac: LSHI2(ac,iarg) */
	case 127: /* ac: RSHU2(ac,conB) */
	case 126: /* ac: RSHI2(ac,conB) */
	case 125: /* ac: LSHU2(ac,conB) */
	case 124: /* ac: LSHI2(ac,conB) */
	case 123: /* ac: LSHU2(ac,con1) */
	case 122: /* ac: LSHI2(ac,con1) */
	case 119: /* ac: SUBP2(ac,conBn) */
	case 118: /* ac: SUBU2(ac,conBn) */
	case 117: /* ac: SUBI2(ac,conBn) */
	case 116: /* ac: SUBP2(ac,conB) */
	case 115: /* ac: SUBU2(ac,conB) */
	case 114: /* ac: SUBI2(ac,conB) */
	case 113: /* ac: SUBP2(ac,iarg) */
	case 112: /* ac: SUBU2(ac,iarg) */
	case 111: /* ac: SUBI2(ac,iarg) */
	case 110: /* ac: ADDP2(ac,conBn) */
	case 109: /* ac: ADDU2(ac,conBn) */
	case 108: /* ac: ADDI2(ac,conBn) */
	case 107: /* ac: ADDP2(ac,conB) */
	case 106: /* ac: ADDU2(ac,conB) */
	case 105: /* ac: ADDI2(ac,conB) */
	case 100: /* ac: ADDP2(iarg,ac) */
	case 99: /* ac: ADDU2(iarg,ac) */
	case 98: /* ac: ADDI2(iarg,ac) */
	case 97: /* ac: ADDP2(ac,iarg) */
	case 96: /* ac: ADDU2(ac,iarg) */
	case 95: /* ac: ADDI2(ac,iarg) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = RIGHT_CHILD(p);
		break;
	case 692: /* regx: ADDU2(BANDU2(reg,conFF00),conB) */
	case 691: /* regx: ADDU2(BANDU2(reg,conFF00),conB) */
	case 690: /* regx: ADDU2(BANDU2(ac,conFF00),conB) */
	case 689: /* regx: ADDU2(BANDU2(ac,conFF00),conB) */
	case 688: /* regx: ADDU2(LSHU2(ac,con8),conB) */
	case 687: /* regx: ADDU2(LSHU2(ac,con8),conB) */
	case 686: /* regx: ADDU2(BANDU2(ac,conFF00),eac0) */
	case 685: /* regx: ADDU2(BANDU2(eac,conFF00),ac0) */
	case 620: /* asgn: ASGNP2(ADDP2(reg,con),ac) */
	case 619: /* asgn: ASGNU2(ADDP2(reg,con),ac) */
	case 618: /* asgn: ASGNI2(ADDP2(reg,con),ac) */
	case 216: /* eac: ADDP2(LSHU2(iarg,con1),eac) */
	case 215: /* eac: ADDP2(LSHI2(iarg,con1),eac) */
	case 214: /* eac: ADDU2(LSHU2(iarg,con1),eac) */
	case 213: /* eac: ADDI2(LSHI2(iarg,con1),eac) */
	case 104: /* ac: ADDP2(LSHU2(iarg,con1),ac) */
	case 103: /* ac: ADDP2(LSHI2(iarg,con1),ac) */
	case 102: /* ac: ADDU2(LSHU2(iarg,con1),ac) */
	case 101: /* ac: ADDI2(LSHI2(iarg,con1),ac) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(p));
		kids[1] = RIGHT_CHILD(LEFT_CHILD(p));
		kids[2] = RIGHT_CHILD(p);
		break;
	case 582: /* eac0: INDIRU1(INDIRP2(zddr)) */
	case 581: /* eac0: INDIRI1(INDIRP2(zddr)) */
	case 580: /* eac: INDIRP2(INDIRP2(zddr)) */
	case 579: /* eac: INDIRU2(INDIRP2(zddr)) */
	case 578: /* eac: INDIRI2(INDIRP2(zddr)) */
	case 295: /* asgn: ARGB(INDIRB(asrc)) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(p));
		break;
	case 424: /* asgn: ASGNF5(lddr,INDIRF5(lsrc)) */
	case 423: /* asgn: ASGNF5(ac,INDIRF5(asrc)) */
	case 422: /* asgn: ASGNF5(addr,INDIRF5(asrc)) */
	case 387: /* asgn: ASGNU4(lddr,INDIRU4(lsrc)) */
	case 386: /* asgn: ASGNU4(ac,INDIRU4(asrc)) */
	case 385: /* asgn: ASGNU4(addr,INDIRU4(asrc)) */
	case 382: /* asgn: ASGNI4(lddr,INDIRI4(lsrc)) */
	case 381: /* asgn: ASGNI4(ac,INDIRI4(asrc)) */
	case 380: /* asgn: ASGNI4(addr,INDIRI4(asrc)) */
	case 298: /* asgn: ASGNB(lddr,INDIRB(lsrc)) */
	case 297: /* asgn: ASGNB(ac,INDIRB(asrc)) */
	case 296: /* asgn: ASGNB(addr,INDIRB(asrc)) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(RIGHT_CHILD(p));
		break;
	case 617: /* eac: INDIRP2(ADDP2(eac,con)) */
	case 616: /* eac: INDIRU2(ADDP2(eac,con)) */
	case 615: /* eac: INDIRI2(ADDP2(eac,con)) */
	case 614: /* ac: INDIRP2(ADDP2(ac,con)) */
	case 613: /* ac: INDIRU2(ADDP2(ac,con)) */
	case 612: /* ac: INDIRI2(ADDP2(ac,con)) */
	case 611: /* eac: INDIRP2(ADDP2(reg,con)) */
	case 610: /* eac: INDIRU2(ADDP2(reg,con)) */
	case 609: /* eac: INDIRI2(ADDP2(reg,con)) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(p));
		kids[1] = RIGHT_CHILD(LEFT_CHILD(p));
		break;
	case 635: /* asgn: ASGNI1(rmw,LOADI1(ADDI2(CVII2(INDIRI1(rmw)),con1))) */
	case 634: /* asgn: ASGNU1(rmw,LOADU1(ADDI2(CVUI2(INDIRU1(rmw)),con1))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)))));
		kids[2] = RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		break;
	case 637: /* asgn: ASGNI4(rmw,NEGI4(INDIRI4(rmw))) */
	case 636: /* asgn: ASGNI2(rmw,NEGI2(INDIRI2(rmw))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		break;
	case 712: /* asgn: ASGNP2(rmw,BANDU2(INDIRP2(rmw),conFF00)) */
	case 711: /* asgn: ASGNI2(rmw,BANDU2(INDIRI2(rmw),conFF00)) */
	case 710: /* asgn: ASGNU2(rmw,BANDU2(INDIRU2(rmw),conFF00)) */
	case 709: /* asgn: ASGNP2(rmw,BANDU2(INDIRP2(rmw),conFF00)) */
	case 708: /* asgn: ASGNI2(rmw,BANDU2(INDIRI2(rmw),conFF00)) */
	case 707: /* asgn: ASGNU2(rmw,BANDU2(INDIRU2(rmw),conFF00)) */
	case 655: /* asgn: ASGNI2(rmw,SUBI2(INDIRI2(rmw),conBa)) */
	case 654: /* asgn: ASGNU2(rmw,SUBU2(INDIRU2(rmw),conBa)) */
	case 653: /* asgn: ASGNP2(rmw,SUBP2(INDIRP2(rmw),conBa)) */
	case 652: /* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),conBa)) */
	case 651: /* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),conBa)) */
	case 650: /* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),conBa)) */
	case 649: /* asgn: ASGNP2(rmw,SUBP2(INDIRP2(rmw),ac)) */
	case 648: /* asgn: ASGNU2(rmw,SUBU2(INDIRU2(rmw),ac)) */
	case 647: /* asgn: ASGNI2(rmw,SUBI2(INDIRI2(rmw),ac)) */
	case 643: /* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),ac)) */
	case 642: /* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),ac)) */
	case 641: /* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),ac)) */
	case 640: /* asgn: ASGNI2(rmw,ADDI2(INDIRI2(rmw),con1)) */
	case 639: /* asgn: ASGNU2(rmw,ADDU2(INDIRU2(rmw),con1)) */
	case 638: /* asgn: ASGNP2(rmw,ADDP2(INDIRP2(rmw),con1)) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		kids[2] = RIGHT_CHILD(RIGHT_CHILD(p));
		break;
	case 646: /* asgn: ASGNP2(rmw,ADDP2(ac,INDIRP2(rmw))) */
	case 645: /* asgn: ASGNU2(rmw,ADDU2(ac,INDIRU2(rmw))) */
	case 644: /* asgn: ASGNI2(rmw,ADDI2(ac,INDIRI2(rmw))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(RIGHT_CHILD(p));
		kids[2] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		break;
	case 672: /* reg: LSHU2(CVUI2(LOADU1(ac)),con8) */
	case 671: /* reg: LSHU2(CVUI2(LOADU1(ac)),con8) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(p)));
		kids[1] = RIGHT_CHILD(p);
		break;
	case 684: /* regx: ADDU2(LSHU2(CVUI2(LOADU1(ac)),con8),CVUI2(LOADU1(eac))) */
	case 683: /* regx: ADDU2(LSHU2(CVUI2(LOADU1(eac)),con8),CVUI2(LOADU1(ac))) */
	case 681: /* regx: ADDU2(LSHU2(CVUI2(LOADU1(iarg)),con8),CVUI2(LOADU1(ac))) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(p))));
		kids[1] = RIGHT_CHILD(LEFT_CHILD(p));
		kids[2] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		break;
	case 682: /* regx: ADDU2(BANDU2(iarg,conFF00),CVUI2(LOADU1(ac))) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(p));
		kids[1] = RIGHT_CHILD(LEFT_CHILD(p));
		kids[2] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		break;
	case 697: /* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
	case 696: /* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
	case 695: /* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),CVUI2(LOADU1(ac)))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p))));
		kids[2] = RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		kids[3] = LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p))));
		break;
	case 703: /* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),BANDU2(regx,conFF00))) */
	case 702: /* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),BANDU2(regx,conFF00))) */
	case 701: /* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),BANDU2(regx,conFF00))) */
	case 700: /* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),BANDU2(ac,conFF00))) */
	case 699: /* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),BANDU2(ac,conFF00))) */
	case 698: /* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),BANDU2(ac,conFF00))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)))));
		kids[2] = LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		kids[3] = RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		break;
	case 706: /* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
	case 705: /* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
	case 704: /* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),LSHU2(CVUI2(LOADU1(ac)),con8))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)))));
		kids[2] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)))));
		kids[3] = RIGHT_CHILD(RIGHT_CHILD(RIGHT_CHILD(p)));
		break;
	case 718: /* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),conB)) */
	case 717: /* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),conB)) */
	case 716: /* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),conB)) */
	case 715: /* asgn: ASGNP2(rmw,ADDU2(BANDU2(INDIRP2(rmw),conFF00),conB)) */
	case 714: /* asgn: ASGNI2(rmw,ADDU2(BANDU2(INDIRI2(rmw),conFF00),conB)) */
	case 713: /* asgn: ASGNU2(rmw,ADDU2(BANDU2(INDIRU2(rmw),conFF00),conB)) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p))));
		kids[2] = RIGHT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)));
		kids[3] = RIGHT_CHILD(RIGHT_CHILD(p));
		break;
	case 721: /* asgn: ASGNP2(rmw,CVUI2(LOADU1(INDIRP2(rmw)))) */
	case 720: /* asgn: ASGNI2(rmw,CVUI2(LOADU1(INDIRI2(rmw)))) */
	case 719: /* asgn: ASGNU2(rmw,CVUI2(LOADU1(INDIRU2(rmw)))) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p))));
		break;
	case 727: /* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),conXX00)) */
	case 726: /* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),conXX00)) */
	case 725: /* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),conXX00)) */
	case 724: /* asgn: ASGNP2(rmw,ADDU2(CVUI2(LOADU1(INDIRP2(rmw))),conXX00)) */
	case 723: /* asgn: ASGNI2(rmw,ADDU2(CVUI2(LOADU1(INDIRI2(rmw))),conXX00)) */
	case 722: /* asgn: ASGNU2(rmw,ADDU2(CVUI2(LOADU1(INDIRU2(rmw))),conXX00)) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(LEFT_CHILD(RIGHT_CHILD(p)))));
		kids[2] = RIGHT_CHILD(RIGHT_CHILD(p));
		break;
	default:
		fatal("_kids", "Bad rule number %d\n", eruleno);
	}
}

#line 1176 "../src/gigatron.md"
/*---- BEGIN CODE --*/


static const char *segname() {
  if (cseg == CODE) return "CODE";
  if (cseg == DATA) return "DATA";
  if (cseg == LIT)  return "DATA";
  if (cseg == BSS)  return "BSS";
  return "?";
}

/* Collect lines for the module manifest */
static void lprint(const char *fmt, ...) {
  char buf[1024];
  SList n;
  va_list ap;
  va_start(ap, fmt);
  vfprint(NULL, buf, fmt, ap);
  va_end(ap);
  n = allocate(sizeof(struct slist)+strlen(buf), PERM);
  strcpy(n->s, buf);
  n->next = &lhead;
  n->prev = lhead.prev;
  n->next->prev = n;
  n->prev->next = n;
}

/* Sometimes gen outputs data in the middle of a function.
   We don't want that here. */
static void xprint_init(void)
{
  in_function = 1;
  xhead.prev = xhead.next = &xhead;
}
static void xprint(const char *fmt, ...)
{
  char buf[1024];
  SList n;
  va_list ap;
  va_start(ap, fmt);
  vfprint(NULL, buf, fmt, ap);
  va_end(ap);
  if (in_function) {
    n = allocate(sizeof(struct slist)+strlen(buf), FUNC);
    strcpy(n->s, buf);
    n->next = &xhead;
    n->prev = xhead.prev;
    n->next->prev = n;
    n->prev->next = n;
  } else {
    print("%s", buf);
  }
}
static void xprint_finish(void)
{
  SList n;
  for (n = xhead.next; n != &xhead; n = n->next)
    print("%s", n->s);
  xhead.prev = xhead.next = &xhead;
  in_function = 0;
}

/* Count mask bits */
static int bitcount(unsigned mask) {
  unsigned i, n = 0;
  for (i = 1; i; i <<= 1)
    if (mask&i)
      n++;
  return n;
}

/* Compare trees */
static int sametree(Node p, Node q) {
  return p == NULL && q == NULL
    || p && q && p->op == q->op && p->syms[0] == q->syms[0]
    && sametree(p->kids[0], q->kids[0])
    && sametree(p->kids[1], q->kids[1]);
}

/* Compare constants (aggregating signed and unsigned types) */
static int samecnst(Symbol a, Symbol b)
{
  if (! (a && b))
    return 0;
  if (a == b)
    return 1;
  if (a->scope == CONSTANTS && isint(a->type)
      && b->scope == CONSTANTS && isint(b->type)
      && (a->type->size == b->type->size)
      && (a->u.c.v.i == b->u.c.v.i) )
    return 1;
  return 0;
}

/* Find next tree (in forest or in next forest) */
static void nexttrees(Node tree, int n, Node trees[])
{
  int i = 0;
  Code cp = 0;
  while (i < n) {
    if (tree->link) {
      trees[i++] = tree = tree->link;
    } else {
      if (! cp)
        for (cp = &codehead; cp; cp = cp->next)
          if (cp->kind == Gen && cp->u.forest == tree)
            break;
      if (cp)
        while ((cp = cp->next))
          if (cp->kind != Defpoint)
            break;
      if (cp && (cp->kind == Gen || cp->kind == Label || cp->kind == Jump))
        trees[i++] = tree = cp->u.forest;
      else
        break;
    }
  }
  while (i < n)
    trees[i++] = 0;
}

/* Get constant or register symbols */
static Symbol cnst_or_vreg(Node p)
{
  int op;
  while (p && (op = generic(p->op))
         && (op == LOAD || op == CVI || op == CVU)
         && opsize(p->op) <= opsize(p->kids[0]->op) )
    p = p->kids[0];
  if (p && generic(p->op) == CNST)
    return p->syms[0];
  if (p && generic(p->op) == INDIR) {
    p = p->kids[0];
    if (specific(p->op) == VREG+P ||
        specific(p->op) == ADDRL+P ||
        specific(p->op) == ADDRF+P )
      return p->syms[0];
  }
  return 0;
}


/* Heuristically tests whether `tree` uses the constant-or-register
   `what` in a way that justifies leaving its value in vAC. */
static int simpleuse(Node p, Symbol what)
{
  int op;
  while (what && p && (op = generic(p->op))
         && op != CALL && op != LABEL && op != JUMP) {
    if (what == cnst_or_vreg(p))
      return 1;
    if (p->kids[1] && simpleuse(p->kids[1], what) )
      return 1;
    p = p->kids[0];
  }
  return 0;
}

/* Cost predicates */
static int if_arg_reg_only(Node p)
{
  return p->syms[2] ? LBURG_MAX : 1;
}

static int if_arg_stk(Node p, int c)
{
  return p->syms[2] ? c : LBURG_MAX;
}

static int if_zpconst(Node p)
{
  Symbol s = p->syms[0];
  assert(specific(p->op) == CNST+P);
  if ((size_t)s->u.c.v.p == ((size_t)s->u.c.v.p & 0xff))
    return 0;
  return LBURG_MAX;
}

static int if_zhconst(Node p)
{
  Symbol s = p->syms[0];
  assert(generic(p->op) == CNST);
  if ((size_t)s->u.c.v.p == ((size_t)s->u.c.v.p & 0xff))
    return 0;
  return LBURG_MAX;
}

static int if_zlconst(Node p)
{
  Symbol s = p->syms[0];
  assert(generic(p->op) == CNST);
  if ((size_t)s->u.c.v.p == ((size_t)s->u.c.v.p & 0xff00))
    return 0;
  return LBURG_MAX;
}

static int if_zpglobal(Node p)
{
  Symbol s = p->syms[0];
  if (s && s->type) {
    if (fnqual(s->type) == NEAR)
      return 0;
    if (isarray(s->type) && fnqual(unqual(s->type)->type) == NEAR)
      return 0;
  }
  return LBURG_MAX;
}

static int if_incr(Node a, int cost, int bonus)
{
  /* Reduces the cost of an increment/decrement operation
     when there is evidence that the previous value was preserved in a
     temporary. This is used to prefer dialect LDW(r);ADDI(i);STW(r)
     over the potentially more efficient LDI(i);ADDW(r);STW(r).
     This is hacky and very limited in fact. */
  extern Node head; /* declared in gen.c */
  Node k;
  Symbol syma;
  int op = (a) ? generic(a->op) : 0;
  if (op == ADD || op == SUB || op == GT || op == GE ||
      op == LT || op == LE || op == EQ || op == NE)
    if ((k = a->kids[0]) && generic(k->op) == INDIR &&
        k->kids[0] && specific(k->kids[0]->op) == VREG+P &&
        (syma = k->kids[0]->syms[0]) )
      if (syma->temporary ||  if_vac_contains(a, syma))
        return cost - bonus;
  return cost;
}

static int if_asgnreuse(Node a, int cost, int bonus)
{
  /* Gives a bonus to a load-into-ac opcode when there is evidence
     that this value will be used in a following assignement */
  extern Node head; /* declared in gen.c */
  Node h;
  /* Find ourselves in forest */
  for (h=head; h; h=h->link)
    if ((generic(h->op) == ASGN && h->kids[1] == a && a) ||
        (generic(h->op) == ARG && h->kids[0] == a && a) )
      break;
  if (! h)
    return cost;
  if (generic(h->op) == ARG)
    return (h->syms[2]) ? cost - bonus : cost;
  if (a->syms[RX] && a->syms[RX]->temporary
      && a->syms[RX]->generated && a->syms[RX]->u.t.cse == a) {
    /* This is a constant subexpression. */
    Node trees[2];
    nexttrees(h, 2, trees);
    if (simpleuse(trees[0], a->syms[RX]) &&
        simpleuse(trees[1], a->syms[RX]) )
      return cost - bonus;
  } else {
    Node trees[1];
    nexttrees(h, 1, trees);
    if (simpleuse(trees[0], cnst_or_vreg(a)) ||
        simpleuse(trees[0], a->syms[RX]) )
      return cost - bonus;
  }
  return cost;
}

static int if_zoffset(Node a, int c1, int c2)
{
  /* Because framesize is not known until much later, we guess it on
     the basis of current offsets in order to give a cost to _LDLW and
     _STLW which can use an actual LDLW and STLW or use the more
     expensive LDXW and STXW. */
  int op;
  Symbol s;
  if (a && a->kids[0] && (op = a->kids[0]->op)
      && (generic(op) == ADDRL || generic(op) == ADDRF)
      && (s = a->kids[0]->syms[0]) )
    {
      int mao = (argoffset > maxargoffset) ? argoffset : maxargoffset;
      int mo = (offset > maxoffset) ? offset : maxoffset;
      int guess = s->x.offset + (mo|3) + (mao|3) + 18;
      return (guess >= 0 && guess < 256) ? c1 : c2;
    }
  return LBURG_MAX;
}

static int if_rmw(Node a, int cost)
{
  return if_rmw_a(a, 0, cost);
}

static int if_rmw_a(Node a, int arg, int cost)
{
  Node r;
  assert(a);
  assert(generic(a->op) == ASGN);
  assert(a->kids[0]);
  assert(a->kids[1]);
  r = a->kids[1];
  if (arg != 0 && r->kids[arg])
    r = r->kids[arg];
  while (generic(r->op) != INDIR)
    {
      if (r->kids[0])
        r = r->kids[0];
      else
        return LBURG_MAX;
    }
  if (sametree(a->kids[0], r->kids[0]))
    return cost;
  if (r->syms[RX]->temporary && r->syms[RX]->generated && r->syms[RX]->u.t.cse)
    {
      r = r->syms[RX]->u.t.cse;
      if (generic(r->op) == LOAD && r->kids[0])
        r = r->kids[0];
      if (generic(r->op) == INDIR && sametree(a->kids[0], r->kids[0]))
        return cost;
    }
  return LBURG_MAX;
}

static int if_rmw_incr(Node a, int cost, int bonus)
{
  cost = if_rmw(a, cost);
  if (cost < LBURG_MAX && (a = a->kids[1]))
    if (generic(a->op) == ADD || generic(a->op) == SUB)
      return if_incr(a, cost, bonus);
  return cost;
}

static int if_not_asgn_tmp(Node p, int cost)
{
  Node n;
  assert(p);
  assert(generic(p->op) == ASGN);
  assert(p->kids[0]);
  n = p->kids[0];
  if (specific(n->op) == VREG+P && n->syms[0]->temporary)
    return LBURG_MAX;
  return cost;
}

static int if_cv_from(Node p, int sz, int cost)
{
  assert(p->syms[0]);
  assert(p->syms[0]->scope == CONSTANTS);
  assert(p->syms[0]->type = inttype);
  if (p->syms[0]->u.c.v.i == sz)
    return cost;
  return LBURG_MAX;
}

/* Utilities for the emitter state machine that conservatively tracks
   which registers or accumulators are equal and to what. */

static Symbol make_derived_symbol(const char *what, Symbol s)
{
  /* We masquerade frame offsets and memory values as string
     constants. Genuine string constants at this stage only appear as
     static identifiers. */
  Value v;
  Symbol r;
  v.p = stringf("%s\003%s", what, s->name);
  r = constant(array(chartype,strlen(v.p),0), v);
  return r;
}

static Symbol get_cnst_or_reg(Node p, int nt, int starred)
{
  Symbol rx;
  if (p)
    {
      p = reuse(p, nt);
      rx = (p->x.inst == nt) ? p->syms[RX] : 0;
      while (generic(p->op) == LOAD && p->kids[0])
        p = p->kids[0];
      if (generic(p->op) == CNST || generic(p->op) == ADDRG)
        return (starred) ? make_derived_symbol("*", p->syms[0]) : p->syms[0];
      if (p->syms[0] && (generic(p->op) == ADDRL || generic(p->op) == ADDRF))
        return make_derived_symbol((starred) ? "*%" : "%", p->syms[0]);
      if (generic(p->op) == INDIR && specific(p->kids[0]->op) == VREG+P && !starred)
        return (rx) ? rx : p->syms[RX] ? p->syms[RX] : p->kids[0]->syms[0];
    }
  return 0;
}

static Symbol get_source_sym(Node p, int nt, Node *kids, const short *nts, const char *tpl)
{
  const char *etpl;
  for (; *tpl; tpl++)
    if (tpl[0]=='%' && tpl[1]=='{' && tpl[2]=='=')
      break;
  for (etpl = tpl; *etpl; etpl++)
    if (*etpl=='}')
      break;
  if (*tpl && *etpl) {
    const char *s;
    if (tpl[3]=='%' && tpl[4]>='0' && tpl[4]<='9' && tpl[5]=='}')
      return get_cnst_or_reg(kids[tpl[4]-'0'], nts[tpl[4]-'0'], 0);
    s = stringn(tpl+3,etpl-tpl-3);
    if (s == ireg[31]->x.name /* vAC */)
      return ireg[31];
    if (s == lreg[31]->x.name /* LAC */)
      return lreg[31];
    if (s == freg[31]->x.name /* FAC */)
      return freg[31];
    assert(0);
  }
  return 0;
}

static Symbol get_target_reg(Node p, int nt)
{
  int op;
  Node q;
  switch (nt) {
  case _lac_NT:
    return lreg[31];
  case _fac_NT:
    return freg[31];
  case _eac0_NT: case _ac0_NT: case _eac_NT: case _ac_NT:
    return ireg[31];
  case _reg_NT: case _regx_NT:
    return (p) ? p->syms[RX] : 0;
  case _rmw_NT:
    return (p && specific(p->op) == VREG+P) ? p->syms[0] : 0;
  case _asgn_NT:
    if (p && generic(p->op) == ASGN && (q = p->kids[0])
        && (op = q->op) && q->syms[0]) {
      if (generic(op) == VREG)
        return q->syms[0];
      else if (generic(op) == ADDRG)
        return make_derived_symbol("*", q->syms[0]);
      else if (generic(op) == ADDRL || generic(op) == ADDRF)
        return make_derived_symbol("*%", q->syms[0]);
    }
  default:
    return 0;
  }
}

/* Guess whether previous tree leaves sym into vAC.
   This is imperfect, only used for cost predicates.
   It fails to recognize elided temporaries. */ 
static int if_vac_contains(Node a, Symbol sym)
{
  static Node cached_a;
  static Symbol cached_target, cached_source;
  if (a != cached_a) {
    Node h, ph;
    extern Node head; /* from gen.c */
    cached_a = a;
    cached_target = cached_source = 0;
    for (h = head; (ph = h->link); h=ph)
      if (ph == a || ph->kids[1] == a)
        break;
    if (ph && h && generic(h->op) == ASGN &&
        h->kids[0] && specific(h->kids[0]->op) == VREG+P) {
      /* we have one of the ASGN(VREGP,reg) rule */
      Node p = reuse(h->kids[1], _reg_NT);
      int rulenum = (*IR->x._rule)(p->x.state, _reg_NT);
      short *nts = IR->x._nts[rulenum];
      if (rulenum && get_target_reg(p, nts[0]) == ireg[31]) {
        cached_target = get_target_reg(h, _asgn_NT);
        while (generic(p->op) == LOAD && p->kids[0])
          p = p->kids[0];
        if (generic(p->op) == INDIR && specific(p->kids[0]->op) == VREG+P)
          cached_source = p->kids[0]->syms[0];
      }
    }
  }
  return sym && (sym == cached_target || sym == cached_source);
}


/* lcc callback: finalizer */
static void progend(void)
{
  extern char *firstfile; /* From input.c */
  SList s;
  print("# ======== (epilog)\n");
  print("code=[\n");
  for (s = lhead.next; s != &lhead; s = s->next)
    print("\t%s%s", s->s, (s->next == &lhead) ? "" : ",\n");
  print(" ]\n");
  print("module(code=code, ");
  if (firstfile)
    print("name='%s', ", firstfile);
  print("cpu=%d);\n", cpu);
  print("\n# Local Variables:"
        "\n# mode: python"
        "\n# indent-tabs-mode: t"
        "\n# End:\n");
}

/* lcc callback: pragma */
static int do_pragma()
{
  int i;
  unsigned char *s;
  static const char *patterns[] = {
    "option ( \"%*[^\"]\" ) %n",
    "lib ( \"%*[^\"]\" ) %n",
    0
  };

  while (*cp == ' ' || *cp == '\t')
    cp++;
  for(s = cp; *s; s++)
    if (*s == '\n')
      break;
  for (i=0; patterns[i]; i++) {
    int n = -1;
    int c = *s;
    *s = 0;
    sscanf((char*)cp, patterns[i], &n);
    *s = c;
    if (n >= 0 && cp + n == s) {
      xprint("# ======== pragma\npragma_%S\n\n", cp, n);
      return 1;
    }
  }
  return 0;
}


/* lcc callback: initializer */
static void progbeg(int argc, char *argv[])
{
  int i;
  /* Parse flags */
  parseflags(argc, argv);
  for (i=0; i<argc; i++)
    if (!strncmp(argv[i],"-cpu=", 5)) {
      char *s = argv[i] + 5;
      int cpumajor = *s++ - '0';
      while (*s == ',') {
        char *d = ++s;
        while (*d && *d != ',')
          d++;
        if (! strncmp("addhi", s, d-s))
          cpuflags.addhi = 1;
        else
          cpumajor = -1;
        s = d;
      }
      if (cpumajor >= 4 && cpumajor <= 7 && !*s)
        cpu = cpumajor;
      else
        warning("invalid cpu option %s\n", argv[i]+5);
    }
  /* Print header */
  print("#VCPUv%d\n\n",cpu);
  /* Prepare registers */
  for (i=0; i<24; i++)
    ireg[i] = mkreg("R%d", i, 1, IREG);
  for (i=0; i+1<24; i++)
    lreg[i] = mkreg("L%d", i, 3, IREG);
  for (i=0; i+2<24; i++)
    freg[i] = mkreg("F%d", i, 7, IREG);
  /* vAC/LAC/FAC */
  ireg[30] = mkreg("SP",  26, 1, IREG);
  ireg[31] = mkreg("vAC", 27, 1, IREG);
  freg[31] = mkreg("FAC", 28, 7, IREG);
  lreg[31] = mkreg("LAC", 29, 3, IREG);
  /* Prepare wildcards */
  iregw = mkwildcard(ireg);
  lregw = mkwildcard(lreg);
  fregw = mkwildcard(freg);
  tmask[IREG] = REGMASK_TEMPS; /* nonleaf */
  vmask[IREG] = REGMASK_SAVED; /* nonleaf */
  tmask[FREG] = vmask[FREG] = 0;
  /* No segment */
  cseg = -1;
  /* Pragmas */
  register_pragma("glcc", do_pragma);
}

/* Return register set for op */
static Symbol rmap(int opk)
{
  switch(optype(opk)) {
  case I: case U:
    return (opsize(opk)==4) ? lregw : iregw;
  case P: case B:
    return iregw;
  case F:
    return fregw;
  default:
    return 0;
  }
}

/* Find a register by type and name */
static Symbol findreg(const char *s)
{
  int i;
#define scanregs(regs) \
  for (i=0; i<NELEMS(regs); i++) \
    if (regs[i] && !strcmp(s, regs[i]->x.name)) \
      return regs[i];
  scanregs(ireg);
  scanregs(lreg);
  scanregs(freg);
#undef scanregs
  return 0;
}

/* Return register for argument passing or zero. */
static Symbol argreg(int argno, int ty, int sz, int *roffset)
{
  Symbol r = 0;
  if (argno == 0)
    *roffset = 8; /* First register is R8 */
  if (*roffset >= 16)
    return 0;
  if (ty == I || ty == U || ty == P)
    if (sz <= 2)
      r = ireg[*roffset];
    else
      r = lreg[*roffset];
  else if (ty == F)
    r = freg[*roffset];
  if (r == 0 || r->x.regnode->mask & ~REGMASK_ARGS)
    return 0;
  *roffset += roundup(sz,2)/2;
  return r;
}

/* lcc callback: provide explicit register targets */
static void target(Node p)
{
  assert(p);
  switch (specific(p->op))
    {
    case ARG+F: case ARG+I: case ARG+P: case ARG+U:
      if (p->syms[1])
        rtarget(p, 0, p->syms[1]);
      break;
    }
}

static int inst_contains_call(Node p)
{
  if ((generic(p->op) == CALL) ||
      (p->kids[0] && !p->kids[0]->x.inst && inst_contains_call(p->kids[0])) ||
      (p->kids[1] && !p->kids[1]->x.inst && inst_contains_call(p->kids[1])) )
    return 1;
  return 0;
}

/* lcc callback: mark caller-saved registers as clobbered. */
static void clobber(Node p)
{
  static unsigned argmask = 0;
  assert(p);
  if (generic(p->op) == ARG) {
    /* Mark argument register as used so that it is not allocated as a
       temporary while preparing the remaining args. */
    Symbol r = p->syms[1];
    if (p->x.argno == 0)
      argmask = 0;
    if (r && r->x.regnode) {
      assert(r->x.regnode->set == 0);
      argmask |= r->x.regnode->mask;
      freemask[0] &= ~r->x.regnode->mask;
    }
  }
  if (inst_contains_call(p)) {
    /* Clobber all caller-saved registers before a call. */
    unsigned mask =  REGMASK_TEMPS & ~REGMASK_SAVED;
    if (p->x.registered && p->syms[2] && p->syms[2]->x.regnode->set == IREG)
      mask &= ~p->syms[2]->x.regnode->mask;
    if (mask)
      spill(mask, IREG, p);
  }
  if (argmask && p->x.next && inst_contains_call(p->x.next)) {
    /* Free all argument registers before the call */
    freemask[0] |= argmask;
    argmask = 0;
  }
}

/* new lcc callback: preralloc is called before the normal register
   allocation pass and tries to eliminate temporaries when the
   instruction layout allows us to use an accumulator. This uses the
   same annotations as the emitter state machine. */

static void preralloc_scan(Node p, int nt, Symbol sym, int frag,
                           int *usecount, int *rclobbered)
{
  if ((p = reuse(p, nt))) {
    int rulenum = (*IR->x._rule)(p->x.state, nt);
    const short *nts = IR->x._nts[rulenum];
    const char *template = IR->x._templates[rulenum];
    const char *tpl = template;
    Node kids[10];
    Symbol t,s;
    (*IR->x._kids)(p, rulenum, kids);
    /* Scan template */
    if (*tpl == '#')
      return;
    while(--frag >= 0)
      while (*tpl && *tpl++ != '|')
        { }
    for(; *tpl && *tpl!='|' && !*rclobbered; tpl++)
      {
        if (tpl[0]=='%' && tpl[1]=='{' && tpl[2]=='!') {
          for(tpl=tpl+3; *tpl && *tpl!='|' && *tpl!='}'; tpl++)
            switch(*tpl) {
            case '4': if (cpu > 4) break;
            case '5': if (cpu > 5) break;
            case '6': if (cpu > 6) break;
            case 'A': vac_clobbered = 1; break;
            case 'L': case 'F': xac_clobbered = vac_clobbered = 1; break;
            default: break;
            }
        } else if (tpl[0]=='%' && isdigit(tpl[1])) { /* %0 */
          Node k = kids[tpl[1]-'0'];
          int knt = nts[tpl[1]-'0'];
          k = reuse(k, knt);
          if (knt == _regx_NT && k->x.inst && k->syms[RX] == sym)
            *rclobbered = 1; /* a regx nonterminal kills preralloc */
          else if (k->x.inst != knt)
            preralloc_scan(k, knt, sym, 0, usecount, rclobbered);
          else if (k->syms[RX] == sym)
            *usecount -= 1;
          tpl += 1;
        } else if (tpl[0]=='%' && tpl[1]=='['        /* %[0b] */
                   && isdigit(tpl[2]) && islower(tpl[3])  && tpl[4]==']') {
          Node k = kids[tpl[2]-'0'];
          int knt = nts[tpl[2]-'0'];
          k = reuse(k, knt);
          if (k->x.inst != knt)
            preralloc_scan(k, knt, sym, tpl[3]-'a', usecount, rclobbered);
          tpl += 3;
        } else if (tpl[0]=='%'&&                    /* %{?...::} assumed != */
                   tpl[1]=='{' && tpl[2]=='?') {
          int s = 2;
          for (tpl=tpl+3; *tpl && *tpl!='}' && *tpl!='|'; tpl++)
            if (*tpl == ':' && !--s)
              break;
        } else if (tpl[0]=='%' && tpl[1]=='{') {    /* %{...} skipped */
          for (tpl=tpl+2; *tpl && *tpl!='}' && *tpl!='|'; tpl++) /**/;
        }
      }
    /* Process non-terminal clobber */
    t = get_target_reg(p, nt);
    s = get_source_sym(p, nt, kids, nts, template);
    if (s != sym) {
      if (t == ireg[31])
        vac_clobbered = 1;
      else if (t == lreg[31] || t == freg[31])
        vac_clobbered = xac_clobbered = 1;
    }
  }
}

static void preralloc(Node p)
{
  if (p->x.inst == _reg_NT)
    {
      Node q;
      int usecount = -1;
      Symbol sym = p->syms[RX];
      if (sym->temporary)
        for (q = sym->x.lastuse; q; q = q->x.prevuse)
          usecount += 1;
      if (usecount > 0)
        {
          /* Determine accumulator */
          Symbol r = ireg[31];
          int *rclobbered = &xac_clobbered;
          if (optype(p->op) == FLOAT)
            r = freg[31];
          else if (opsize(p->op) == 4)
            r = lreg[31];
          if (r == ireg[31])
            rclobbered = &vac_clobbered;
          /* Hack because moves to/from FAC are costly on cpu<7 */
          if (r == freg[31] && usecount > 1 && cpu < 7)
            return;
          /* Search for references to sym until accumulator clobbered */
          *rclobbered = 0;
          for (q=p->x.next; q && usecount>0 && !*rclobbered; q = q->x.next)
            preralloc_scan(q, q->x.inst, sym, 0, &usecount, rclobbered);
          /* Did we find all uses of sym? */
          if (! usecount) {
            /* Optimize temporary sym out of existence */
            r->x.lastuse = sym->x.lastuse;
            for (q = sym->x.lastuse; q; q = q->x.prevuse) {
              q->syms[RX] = r;
              q->x.registered = 1;
            }
          }
        }
    }
}

/* lcc-callback: before calling the lcc callback gen(), the pregen
   pass rearraanges some trees in the forest to improve the code
   generation. */

static Node pregen(Node forest)
{
  Node p;
  /* Reorganize to make rmw instruction more obvious */
  for(p=forest; p; p=p->link) {
    Node q = p->link;
    if (generic(p->op) == ASGN && p->kids[0] &&
        q && generic(q->op) == ASGN && q->kids[0] ) 
      {
        /* Change ASGN(v,...(u,...));ASGN(u,v)
           into ASGN(u,...(u,...));ASGN(v,u)   */
        Node v = p->kids[0];
        Node u = p->kids[1];
        while(u && !isaddrop(u->op))
          u = u->kids[0];
        if (generic(v->op) == ADDRL &&
            v->syms[0] && v->syms[0]->temporary &&
            sametree(u, q->kids[0]) )
          {
            Node w = q->kids[1];
            if (w && generic(w->op) == LOAD)
              w = w->kids[0];
            if (w && generic(w->op) == INDIR &&
                (w = w->kids[0]) && sametree(v,w) )
              {
                /* Match! */
                q->kids[0]->op = v->op;
                q->kids[0]->syms[0] = v->syms[0];
                q->kids[0]->syms[0]->u.t.cse = q->kids[1];
                v->op = w->op = u->op;
                v->syms[0] = w->syms[0] = u->syms[0];
              }
          }
      }
  }
  /* Continue with gen */
  return gen(forest);
}


/* Enhanced emitter than understands additional %{..} template
   constructs and contains a state machine that conservatively keeps
   track of equality across registers. */

static void emitfmt2(const char *template, int len,
                     Node p, int nt, Node *kids, short *nts)
{
  /* Enhancements of emitfmt with respect to the original version:
     - delegates %{...} delegates to the IR function emit3(),
       which can call emitasm/emitfmt() recursively.
     - Templates might be split in sections with |. Writing $0 to $9
       only prints the first section of the specified kid template.
       The other sections can be accessed with syntax $[0b] where '0'
       is the kid number and 'b' is a letter indicating
       which section to process.
  */
  const char *fmt = template;
  static int alt_s;
  int s = alt_s;
  alt_s = 0;
  for (; len > 0 && *fmt; fmt++, len--)
    if (*fmt == '|' && s == 0)
      break;
    else if (*fmt == '|')
      s -= 1;
    else if (s > 0)
      continue;
    else if (*fmt != '%' || len <= 1)
      (void)putchar(*fmt);
    else if (--len && *++fmt == 'F')                          /* %F */
      print("%d", framesize);
    else if (*fmt >= 'a' && *fmt < 'a' + NELEMS(p->syms))     /* %a..%c */
      fputs(p->syms[*fmt - 'a']->x.name, stdout);
    else if (*fmt >= '0' && *fmt <= '9')                      /* %0..%9 */
      emitasm(kids[*fmt - '0'], nts[*fmt - '0']);
    else if (len>3 && fmt[0] == '[' && fmt[3] == ']'
             && fmt[1] >= '0' && fmt[1] <= '9'
             && fmt[2] >= 'a' && fmt[2] <= 'z') {      /* %[0a] */
      alt_s = fmt[2] - 'a';
      emitasm(kids[fmt[1] - '0'], nts[fmt[1] - '0']);
      fmt += 3;
      len -= 3;
      alt_s = 0;
    } else if (*fmt == '{') {
      int i;
      int level = 0;
      const char *s;
      for (i=0; i < len; i++)
        if (fmt[i] == '{')
          level += 1;
        else if (fmt[i] == '}' && !--level)
          break;
      assert(!level);
      emit3(fmt+1, i-1, p, nt, kids, nts);
      fmt += i;
      len -= i;
    } else
      (void)putchar(*fmt);

  /* This part of the state machine fires when on reaches the
     end of the template and updates the equality tracking information
     according to the nonterminal of the current rule and to the %{=...}
     annotations found in the template. */
  if (!fmt[len]) {
    Symbol t = get_target_reg(p, nt);
    Symbol s = get_source_sym(p, nt, kids, nts, template);
    if (nt == _asgn_NT) {
      vac_memval = lac_memval = fac_memval = 0;
      if (s == ireg[31])
        vac_memval = t;
      else if (s == lreg[31])
        lac_memval = t;
      else if (s == freg[31])
        fac_memval = t;
    } else if (t == ireg[31] /*vAC*/
               && !(s && !s->x.regnode && s == vac_constval) ) { 
      vac_clobbered = 1;
      vac_equiv = 0;
      vac_constval = vac_memval = 0;
      if (s && !s->x.regnode)
        vac_constval = s;
      else if (s && s->x.regnode && s->x.regnode->number < 24)
        vac_equiv = (1 << s->x.regnode->number);
    } else if (t == lreg[31] /*LAC*/) {
      vac_clobbered = xac_clobbered = 1;
      vac_equiv = lac_equiv = fac_equiv = 0;
      vac_constval = vac_memval = lac_memval = fac_memval = 0;
      if (s && s->x.regnode && s->x.regnode->number < 24)
        lac_equiv = (1 << s->x.regnode->number);
    } else if (t == freg[31] /*FAC*/) {
      vac_clobbered = xac_clobbered = 1;
      vac_equiv = lac_equiv = fac_equiv = 0;
      vac_constval = vac_memval = lac_memval = fac_memval = 0;
      if (s && s->x.regnode && s->x.regnode->number < 24)
        fac_equiv = (1 << s->x.regnode->number);
    } else if (t && t->x.regnode) {
      if (s == ireg[31] && t->x.regnode->number < 24)
        vac_equiv |= (1 << t->x.regnode->number);
      else if (s == lreg[31] && t->x.regnode->number < 24)
        lac_equiv |= (1 << t->x.regnode->number);
      else if (s == freg[31] && t->x.regnode->number < 24)
        fac_equiv |= (1 << t->x.regnode->number);
      else {
        vac_equiv &= ~t->x.regnode->mask;
        lac_equiv &= ~t->x.regnode->mask;
        fac_equiv &= ~t->x.regnode->mask;
      }
    }
  }
}

static void emitfmt1(const char *fmt, Node p, int nt, Node *kids, short *nts)
{
  emitfmt2(fmt, strlen(fmt), p, nt, kids, nts);
}

static void emit3(const char *fmt, int len, Node p, int nt, Node *kids, short *nts)
{
  int i = 0;
  /* Annotations %{!xxx} update the register equivalence
     information gleaned by the emitter state machine. */
  if (len > 0 && fmt[0] == '!')
    {
      for (i=1; i<len; i++)
        switch(fmt[i]) {
        case '4': if (cpu <= 4) goto xvac; break;
        case '5': if (cpu <= 5) goto xvac; break;
        case '6': if (cpu <= 6) goto xvac; break;
        case 'A': goto xvac;
        case 'L': case 'F':
          xac_clobbered = 1;
          lac_equiv = fac_equiv = 0;
          lac_memval = fac_memval = 0;
        xvac:
          vac_clobbered = 1;
          vac_constval = vac_memval = 0;
          vac_equiv = 0;
        default:
          break;
        }
      return;
    }
  /* Annotations %{=xxx} are processed elsewhere */
  if (len > 0 && fmt[0] == '=')
    return;
  /* %{?[0-9a-c]==...:ifeq:ifne} */
  /* %{?[0-9a-c]=~xAC:ifeq:ifne} */
  if (len > 3 && fmt[0] == '?')
    {
      int starred = 0;
      if (fmt[1] == '*')
        {
          starred = 1;
          fmt += 1;
          len -= 1;
        }
      if (fmt[1] == '%')
        {
          fmt += 1;
          len -= 1;
        }
      if (fmt[2] == '=' && (fmt[3] == '=' || fmt[3] == '~'))
        {
          int ifeq, ifne, level = 0;
          for (ifeq=4; ifeq<len; ifeq++)
            if (fmt[ifeq] == '{')
              level++;
            else if (fmt[ifeq] == '}')
              level--;
            else if (fmt[ifeq] == ':' && !level)
              break;
          for (ifne=ifeq+1; ifne<len; ifne++)
            if (fmt[ifne] == '{')
              level++;
            else if (fmt[ifne] == '}')
              level--;
            else if (fmt[ifne] == ':' && !level)
              break;
          if (ifeq < len)
            {
              int eq = 0;
              Symbol sym = 0;
              const char *cmp = stringn(fmt+4,ifeq-4);
              if (fmt[1] >= 'a' && fmt[1] <= 'c' && !starred)
                sym = p->syms[fmt[1]-'a'];
              else if (fmt[1] >= '0' && fmt[1] <= '9')
                sym = get_cnst_or_reg(kids[fmt[1]-'0'], nts[fmt[1]-'0'], starred);
              if (sym && sym->x.name == cmp)
                eq = 1;
              else if (fmt[3] == '=')
                eq = 0; /* literal comparison */
              else if (sym && cmp == ireg[31]->x.name && !sym->x.regnode )
                eq = (samecnst(vac_constval,sym) || vac_memval == sym);
              else if (sym && cmp == lreg[31]->x.name && !sym->x.regnode )
                eq = (lac_memval == sym);
              else if (sym && cmp == freg[31]->x.name && !sym->x.regnode )
                eq = (fac_memval == sym);
              else if (sym && cmp == ireg[31]->x.name /* vAC */
                       && sym->x.regnode && sym->x.regnode->number < 24
                       && (vac_equiv & (1 << sym->x.regnode->number)) )
                eq = 1;
              else if (sym && cmp == lreg[31]->x.name /* LAC */
                       && sym->x.regnode && sym->x.regnode->number < 24
                       && (lac_equiv & (1 << sym->x.regnode->number)) )
                eq = 1;
              else if (sym && cmp == freg[31]->x.name /* FAC */
                       && sym->x.regnode && sym->x.regnode->number < 24
                       && (fac_equiv & (1 << sym->x.regnode->number)) )
                eq = 1;
              if (eq)
                emitfmt2(fmt+ifeq+1, ifne-ifeq-1, p, nt, kids, nts);
              else if (ifne < len)
                emitfmt2(fmt+ifne+1, len-ifne-1, p, nt, kids, nts);
              return;
            }
        }
    }
  /* %{mulC[%R]} -- multiplication by a small constant */
  else if (len >= 4 && !strncmp(fmt, "mul", 3) && fmt[3] >= '0' && fmt[3] <= '9')
    {
      int c;
      Node k;
      const char *r = "T3";
      i = fmt[3] - '0';
      k = kids[i];
      assert(k);
      if (! (k->syms[0] && k->syms[0]->scope == CONSTANTS))
        if (generic(k->op) == INDIR && k->syms[2] && k->syms[2]->u.t.cse)
          k = k->syms[2]->u.t.cse;
      assert(k->syms[0] && k->syms[0]->scope == CONSTANTS);
      c = k->syms[0]->u.c.v.i;
      if (len > 4) {
        assert(fmt[4]=='%' && fmt[5]>='0' && fmt[5]<='9' && len==6);
        assert(kids[fmt[5]-'0'] && kids[fmt[5]-'0']->syms[RX]);
        r = kids[fmt[5]-'0']->syms[RX]->x.name;
      }
      if(c == 0) {
        print("LDI(0);");
      } else {
        int m = 0x80;
        int x = (c >= 0) ? c : -c;
        assert(x>=0 && x<256);
        while (m && !(m & x))
          m >>= 1;
        if (len > 4 && c < 0)
          print("LDI(0);SUBW(%s);", r);
        else if (len > 4)
          print("LDW(%s);", r);
        else if (c < 0)
          print("STW(T3);LDI(0);SUBW(T3);");
        else if (x & (m-1))
          print("STW(T3);");
        for (m >>= 1; m; m >>= 1) {
          print("LSLW();");
          if (m & x)
            print("%s(%s);", (c > 0) ? "ADDW" : "SUBW", r);
        }
      }
      return;
    }
  /* otherwise complain */
  assert(0);
}


/* placement constraints and other attributes */
struct constraints {
  char near_p, place_p, org_p, off_p, nohop_p;
  unsigned int amin, amax, aorg, aoff;
};

static int check_uintval(Attribute a, int n)
{
  Symbol c = a->args[n];
  if (c && c->scope == CONSTANTS && isint(c->type)) {
    unsigned long u = c->u.c.v.u;
    if (u != (u & 0xffff))
      warning("attribute `%s`: argument out of range\n", a->name);
    return 1;
  }
  return 0;
}

static unsigned int uintval(Symbol c)
{
  if (c && c->scope == CONSTANTS && isint(c->type))
    return (unsigned int) c->u.c.v.u;
  return 0;
}

static const char *check_strval(Attribute a, int n)
{
  Symbol c = a->args[n];
  if (c && c->scope == CONSTANTS &&
      isarray(c->type) && ischar(c->type->type))
    return (const char*)(c->u.c.v.p);
  return 0;
}

static const char* check_idval(Attribute a, int n)
{
  const char *str = check_strval(a, n);
  const char *s;
  int flag = 0;
  if (str) {
    for (s=str; *s; s++) {
      if (s[0] == '%' && s[1] == 's' && !flag)
        flag = 1;
      else if (isdigit(s[0]) && s != str)
        continue;
      else if (!isalpha(s[0]) && s[0] != '_')
        return 0;
    }
  }
  return stringf("'%s'", str);
}

static const char *check_attributes(Symbol p)
{
  Attribute a;
  const char *alias = 0;
  char has_org = 0;
  char has_place = 0;
  char has_off = 0;
  char is_static = (p->sclass == STATIC);
  char is_extern = (p->sclass == EXTERN);
  if (p->scope == GLOBAL || is_static || is_extern) {
    for (a = p->attr; a; a = a->link) {
      char yes = 0;
      if (a->name == string("place") && !is_extern) {
        if (has_org)
          error("incompatible placement constraints (org & place)\n");
        a->okay = (check_uintval(a,0) &&  check_uintval(a,1));
        yes = has_place = 1;
      } else if (a->name == string("org")) {
        if (has_org)
          error("incompatible placement constraints (multiple org)\n");
        else if (has_place)
          error("incompatible placement constraints (org & place)\n");
        a->okay = (check_uintval(a,0) && !a->args[1]);
        yes = has_org = 1;
      } else if (a->name == string("offset") && !is_extern) {
        if (has_off)
          error("incompatible placement constraints (multiple offsets)\n");
        a->okay = (check_uintval(a,0) && !a->args[1]);
        yes = has_off = 1;
      } else if (a->name == string("nohop") && !is_extern) {
        a->okay = (!a->args[0] && !a->args[1]);
        yes = 1;
      } else if (a->name == string("alias") && is_extern && !alias) {
        alias = check_idval(a, 0);
        a->okay = (alias!=0 && !a->args[1]);
        yes = 1;
      } else if (a->name == string("regalias") && is_extern && !alias) {
        alias = check_strval(a, 0);
        a->okay = (alias!=0 && findreg(alias) && !a->args[1]);
        yes = 1;
      }
      if (yes && !a->okay)
        error("illegal argument in `%s` attribute\n", a->name);
    }
  }
  return alias;
}

static void get_constraints(Symbol p, struct constraints *c)
{
  Attribute a;

  c->place_p = c->org_p = c->off_p = c->nohop_p = 0;
  c->amin = c->amax = c->aorg = c->aoff = 0;
  c->near_p = (fnqual(p->type) == NEAR);
  for (a = p->attr; a; a = a->link) {
    if (!a->okay) {
      continue;
    } else if (a->name == string("nohop")) {
      c->nohop_p = 1;
    } else if (a->name == string("org")) {
      c->aorg = uintval(a->args[0]);
      c->org_p = 1;
    } else if (a->name == string("offset")) {
      c->aoff = uintval(a->args[0]);
      c->off_p = 1;
    } else if (a->name == string("place")) {
      unsigned long a0 = uintval(a->args[0]);
      unsigned long a1 = uintval(a->args[1]);
      if (c->place_p) {
        if (a0 < c->amin) a0 = c->amin;
        if (a1 > c->amax) a1 = c->amax;
      }
      c->place_p = 1;
      c->amin = a0;
      c->amax = a1;
    }
  }
  if (c->near_p) {
    if (!c->place_p)
      c->amin = 0;
    if (!c->place_p || c->amax > 0xff)
      c->amax = 0xff;
  }
  if ((c->near_p || c->place_p) && (c->amin > c->amax))
    warning("unsatisfyable placement constraints (place)\n");
}

static void print_constraints(Symbol p, struct constraints *c)
{
  if (c->nohop_p)
    lprint("('NOHOP', %s)", p->x.name);
  if (c->org_p)
    lprint("('ORG', %s, 0x%x)", p->x.name, c->aorg);
  if (c->off_p)
    lprint("('OFFSET', %s, 0x%x)", p->x.name, c->aoff & 0xff);
  if (c->near_p || c->place_p)
    lprint("('PLACE', %s, 0x%x, 0x%x)", p->x.name, c->amin, c->amax);
}


/* lcc callback: annotates arg nodes with offset and register info. */
static void doarg(Node p)
{
  /* Important change in arg passing:
     - When calling a function, all arguments beyond
       those specified in the prototype are written to the stack
       in addition to being possibly passed in registers.
       In particular this happens for all arguments
       when calling a non prototyped function,
       and this happens for all supplementary arguments
       when the function has a variadic prototype.
     - When defining a function, prototyped or not,
       arguments are read from registers whenever possible.
     - The stdarg macros always read arguments from the stack.
  */
  static int argno;
  static int argmaxno;
  static int roffset;
  int offset;
  Symbol r;
  Node c;
  if (argoffset == 0) {
    argno = 0;
    argmaxno = 0;
    for (c=p; c; c=c->link)
      if (generic(c->op) == CALL ||
          (generic(c->op) == ASGN && generic(c->kids[1]->op) == CALL &&
           (c = c->kids[1]) ) )
        break;
    if (c && c->syms[0]->type->u.f.proto)
      while (c->syms[0]->type->u.f.proto[argmaxno] &&
             c->syms[0]->type->u.f.proto[argmaxno] != voidtype)
        argmaxno += 1;
    if (c && isstruct(c->syms[0]->type->type)) /* for callb */
      argmaxno += 1;
  }
  r  = argreg(argno, optype(p->op), opsize(p->op), &roffset);
  offset = mkactual(p->syms[1]->u.c.v.i, p->syms[0]->u.c.v.i);
  p->x.argno = argno++;
  p->syms[2] = (r && p->x.argno < argmaxno) ? 0 : intconst(offset);
  if (optype(p->op) != B) /* keep alignment info for structures */
    p->syms[1] = r;
}

/* lcc callback: place local variable. */
static void local(Symbol p)
{
  /* The size check restricts allocating registers for longs and
     floats local variables (but not temporaries). The register
     benefit for longs and floats is minimal. Best to heuristically
     keep enough of them for ints and pointers. */
  if (p->type->size > 2 && ! p->temporary)
    if (bitcount(vmask[IREG] & freemask[IREG]) < p->type->size)
      p->sclass = AUTO;
  if (askregvar(p, rmap(ttob(p->type))) == 0)
    mkauto(p);
}

/* Utility for printing function prologues and epilogues. */
static void printregmask(unsigned mask) {
  unsigned i, m;
  char *prefix = "R";
  for (i=0, m=1; i<31; i++, m<<=1)
    if (mask & m) {
      print("%s%d", prefix, i);
      prefix = ",";
      if (i<30 && (mask & (m+m))) {
        for (; i<30 && (mask & (m+m)); i++, m<<=1) {}
        print("-%d", i);
      }
    }
  if (!mask)
    print("None");
}

/* lcc callback: compile a function. */
static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls)
{
  /* Stack frame: 
   |
   |   SP+Framesize--> n bytes                   : arguments
   |                   maxoffset bytes           : local variables
   |                   0 to 2 bytes              : alignment pad
   |                   2 bytes                   : saved vLR
   |                   sizesave bytes            : saved registers
   |                   0 to 2 bytes              : alignment pad
   |             SP--> maxargoffset bytes        : argument building area
   */

  int i, roffset, sizesave, ty;
  unsigned savemask;
  char frameless;
  struct constraints place;
  Symbol r;

  usedmask[0] = usedmask[1] = 0;
  freemask[0] = freemask[1] = ~(unsigned)0;
  offset = maxoffset = 0;
  maxargoffset = 0;
  assert(f->type && f->type->type);
  ty = ttob(f->type->type);
  if (ncalls) {
    tmask[IREG] = REGMASK_TEMPS;
    vmask[IREG] = REGMASK_SAVED;
  } else {
    tmask[IREG] = REGMASK_TEMPS;
    vmask[IREG] = REGMASK_MOREVARS;
  }
  tmask[IREG] &= ~rmask;
  vmask[IREG] &= ~rmask;
  /* placement constraints */
  get_constraints(f, &place);
  /* locate incoming arguments */
  offset = 0;
  roffset = 0;
  for (i = 0; callee[i]; i++) {
    Symbol p = callee[i];
    Symbol q = caller[i];
    assert(q);
    offset = roundup(offset, q->type->align);
    p->x.offset = q->x.offset = offset;
    p->x.name = q->x.name = stringd(offset);
    r = argreg(i, optype(ttob(q->type)), q->type->size, &roffset);
    offset += q->type->size;
    if (r) {
      if (ncalls == 0 && !p->addressed && p->ref > 0) {
        /* Leaf function: leave register arguments in place */
        p->sclass = q->sclass = REGISTER;
        askregvar(p, r);
        assert(p->x.regnode && p->x.regnode->vbl == p);
        q->x = p->x;
        q->type = p->type;
      } else {
        /* Aggressively ask new registers for args passed in registers */
        if (!p->addressed && p->ref > 0)
          p->sclass = REGISTER;
        /* Let gencode know args were passed in register */
        q->sclass = REGISTER;
        q->x = r->x;
      }
    }
    if (p->sclass == REGISTER && !p->x.regnode && p->ref > 0) {
      /* Allocate register argument. Gencode does the rest. */
      askregvar(p, rmap(ttob(p->type)));
    }
  }
  /* gen code */
  assert(!caller[i]);
  offset = 0;
  gencode(caller, callee);
  /* compute framesize */
  savemask = usedmask[IREG] & REGMASK_SAVED;
  sizesave = 2 * bitcount(savemask);
  maxargoffset = (maxargoffset + 1) & ~0x1;
  maxoffset = (maxoffset + 1) & ~0x1;
  framesize = maxargoffset + sizesave + maxoffset + 2;
  assert(framesize >= 2);
  if (framesize > 32768)
    error("%s() framesize (%d) too large for a gigatron\n",
          f->name, framesize);
  /* can we make a frameless leaf function */
  if (ncalls == 0 && framesize == 2 && (tmask[IREG] & ~usedmask[IREG])) {
    framesize = (cpu < 7) ? 0 : 2;  /* are SP and vSP the same? */
    frameless = 1;
  } else if (IR->longmetric.align == 4) {
    int delta = ((framesize + 3) & ~0x3) - framesize;
    framesize += delta;
    maxargoffset += (maxargoffset) ? delta : 0;
    frameless = 0;
  }
  /* prologue */
  xprint_init();
  segment(CODE);
  lprint("('%s', %s, code%d)", segname(), f->x.name, codenum);
  print_constraints(f, &place);
  print("# ======== %s\n", lhead.prev->s);
  print("def code%d():\n", codenum++);
  print("\tlabel(%s);\n", f->x.name);
  if (frameless) {
    print("\tPUSH();\n");
  } else {
    print("\t_PROLOGUE(%d,%d,0x%x); # save=", framesize, maxargoffset, savemask);
    printregmask(savemask);
    print("\n");
  }
  /* Emit actual code */
  vac_constval = 0;
  vac_equiv = lac_equiv = fac_equiv = 0;
  emitcode();
  /* Epilogue */
  if (frameless) {
    print("\ttryhop(2);POP();RET()\n");
  } else {
    const char *saveac = "";
    if (opsize(ty) <= 2 && (optype(ty) == I || optype(ty) == U || optype(ty) == P))
      saveac = ",saveAC=True";
    printf("\t_EPILOGUE(%d,%d,0x%x%s);\n", framesize, maxargoffset, savemask, saveac);
  }
  /* print delayed data */
  xprint_finish();
}

/* lcc callback. Emit a constant. */
static void defconst(int suffix, int size, Value v)
{
  if (suffix == F) {
    double d = v.d;
    double m;
    int exp;
    unsigned long mantissa;
    assert(size == 5);
    assert(isfinite(d));
    m = floor(frexp(fabs(d),&exp) * 4294967296.0 + 0.5 - DBL_EPSILON);
    if (m > 0xffffffff) { m *= 0.5; exp += 1; }
    mantissa = (unsigned long)(m);
    if (exp < -127)
      mantissa = 0;
    if (mantissa == 0)
      xprint("\tbytes(0,0,0,0,0);");
    else if (exp > 127)
      error("floating point constant overflow\n");
    else
      xprint("\tbytes(%d,%d,%d,%d,%d);",
             exp+128, ((mantissa>>24)&0x7f)|((d<0.0)?0x80:0x00),
             (mantissa>>16)&0xff, (mantissa>>8)&0xff, (mantissa&0xff) );
    xprint(" # %g\n", d);
  } else {
    unsigned long x = (suffix == P) ? (unsigned)(size_t)v.p : (suffix == I) ? v.i : v.u;
    if (size == 1)
      xprint("\tbytes(%d);", x&0xff);
    else if (size == 2)
      xprint("\twords(%d);", x&0xffff);
    else if (size == 4)
      xprint("\twords(%d,%d);", x&0xffff, (x>>16)&0xffff);
    if (suffix == I)
      xprint(" # %D", (long)x);
    else if (suffix == U)
      xprint(" # %U", (unsigned long)x);
    xprint("\n");
  }
}

/* lcc callback - emit an address constant. */
static void defaddress(Symbol p)
{
  xprint("\twords(%s);\n", p->x.name);
}

/* lcc callback - emit a string constant. */
static void defstring(int n, char *str)
{
  int i;
  for (i=0; i<n; i++)
    xprint( ((i&7)==0) ? "\tbytes(%d" : ((i&7)==7) ? ",%d);\n" : ",%d", (int)str[i]&0xff );
  if (i&7)
    xprint(");\n");
}

/* lcc callback - mark imported symbol. */
static void import(Symbol p)
{
  if (p->ref > 0 && strncmp(p->x.name, "'__glink_weak_", 14) != 0) {
    Attribute a;
    for (a = p->attr; a; a = a->link)
      if (a && a->okay && a->name == string("org"))
        break;
    if (a)
      lprint("('IMPORT', %s, 'AT', 0x%x)", p->x.name, uintval(a->args[0]));
    else if (!isalpha(p->x.name[0]))
      lprint("('IMPORT', %s)", p->x.name);
  }
}

/* lcc callback - mark exported symbol. */
static void export(Symbol p)
{
  int isnear = fnqual(p->type) == NEAR;
  int iscommon = (p->u.seg == BSS && !isnear && !p->attr);
  if (! iscommon)
    lprint("('EXPORT', %s)", p->x.name);
}

/* lcc callback: determine symbol names in assembly code. */
static void defsymbol(Symbol p)
{
  /* this is the time to check that attributes are meaningful */
  Symbol r;
  const char *alias = check_attributes(p);
  if (p->scope >= LOCAL && p->sclass == STATIC)
    p->x.name = stringf("'.%d'", genlabel(1));
  else if (p->generated)
    p->x.name = stringf("'.%s'", p->name);
  else if (alias) {
    p->x.name = string(alias);
    if (isalpha(alias[0]) && (r = findreg(alias))) {
      rmask |= r->x.regnode->mask;
      p->x.regnode = r->x.regnode;
      r->x.regnode->vbl = p;
    }
  } else if (p->scope == GLOBAL || p->sclass == EXTERN)
    p->x.name = stringf("'%s'", p->name);
  else
    p->x.name = p->name;
}

/* lcc callback: construct address+offset symbols. */
static void address(Symbol q, Symbol p, long n)
{
  if (p->scope == GLOBAL || p->sclass == STATIC || p->sclass == EXTERN) {
    Type ty0 = p->type;
    Type ty1 = q->type;
    q->x.name = stringf("v(%s)%s%D", p->x.name, n >= 0 ? "+" : "", n);
    if (ty0 && fnqual(ty0) == NEAR && n >= 0 && n < ty0->size)
      if (ty1 && !fnqual(ty1))
        q->type = qual(NEAR, ty1);
  } else {
    assert(n <= INT_MAX && n >= INT_MIN);
    q->x.offset = p->x.offset + n;
    q->x.name = stringd(q->x.offset);
  }
}

/* lcc callback: construct global variable. */
static void global(Symbol p)
{
  struct constraints place;
  unsigned int size, align;
  const char *s = segname();
  const char *n;

  get_constraints(p, &place);
  size = p->type->size;
  align = (place.near_p) ? 1 : p->type->align;
  if (p->u.seg == BSS && p->sclass != STATIC && !place.near_p && !p->attr)
    s = "COMMON";               /* no 'common' in the presence of placement attributes */
  if (p->u.seg == LIT)
    size = 0; /* unreliable in switch tables */
  lprint("('%s', %s, code%d, %d, %d)",
          s, p->x.name, codenum, size, align);
  n = lhead.prev->s;
  print_constraints(p, &place);
  xprint("# ======== %s\n", n);
  xprint("def code%d():\n", codenum++);
  if (align > 1)
    xprint("\talign(%d);\n", align);
  xprint("\tlabel(%s);\n", p->x.name);
  if (p->u.seg == BSS)
    xprint("\tspace(%d);\n", p->type->size);
}

/* lcc callback: define current segment. */
static void segment(int n)
{
  cseg = n;
}

/* lcc callback: emit assembly to skip space. */
static void space(int n)
{
  if (cseg != BSS)
    xprint("\tspace(%d);\n", n);
}

Interface gigatronIR = {
        1, 1, 0,  /* char */
        2, 2, 0,  /* short */
        2, 2, 0,  /* int */
        4, 4, 1,  /* long */
        4, 4, 1,  /* long long */
        5, 1, 1,  /* float */
        5, 1, 1,  /* double */
        5, 1, 1,  /* long double */
        2, 2, 0,  /* pointer */
        0, 1, 0,  /* struct */
        1,        /* little_endian */
        0,        /* mulops_calls */
        0,        /* wants_callb */
        1,        /* wants_argb */
        1,        /* left_to_right */
        0,        /* wants_dag */
        1,        /* unsigned_char !!!! */
        1,        /* wants_cvfu_cvuf */
        address,
        blockbeg,
        blockend,
        defaddress,
        defconst,
        defstring,
        defsymbol,
        emit,
        export,
        function,
        pregen,
        global,
        import,
        local,
        progbeg,
        progend,
        segment,
        space,
        /* stabblock, stabend, 0, stabinit, stabline, stabsym, stabtype */
        0, 0, 0, 0, 0, 0, 0,
        /* Xinterface */
        { 1,
          rmap,
          blkfetch, blkstore, blkloop,
          _label,
          _rule,
          _nts,
          _kids,
          _string,
          _templates,
          _isinstruction,
          _ntname,
          emit2,
          emitfmt1,
          doarg,
          target,
          clobber,
          preralloc,
        }
};

/*---- END CODE --*/

/* Local Variables: */
/* mode: c */
/* c-basic-offset: 2 */
/* indent-tabs-mode: () */
/* End: */
