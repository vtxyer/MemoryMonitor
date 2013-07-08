/*
 * 
 * Intel ACPI Component Architecture
 * ASL Optimizing Compiler version 20090123 [Nov 12 2010]
 * Copyright (C) 2000 - 2009 Intel Corporation
 * Supports ACPI Specification Revision 3.0a
 * 
 * Compilation of "ssdt_pm.asl" - Sun Jun  2 16:10:22 2013
 * 
 * C source code output
 *
 */
unsigned char ssdt_pm[] =
{
    0x53,0x53,0x44,0x54,0xD6,0x05,0x00,0x00,  /* 00000000    "SSDT...." */
    0x02,0xCC,0x58,0x65,0x6E,0x00,0x00,0x00,  /* 00000008    "..Xen..." */
    0x48,0x56,0x4D,0x00,0x00,0x00,0x00,0x00,  /* 00000010    "HVM....." */
    0x00,0x00,0x00,0x00,0x49,0x4E,0x54,0x4C,  /* 00000018    "....INTL" */
    0x23,0x01,0x09,0x20,0x10,0x41,0x5B,0x5C,  /* 00000020    "#.. .A[\" */
    0x5F,0x53,0x42,0x5F,0x5B,0x80,0x44,0x42,  /* 00000028    "_SB_[.DB" */
    0x47,0x41,0x01,0x0B,0x40,0xB0,0x01,0x5B,  /* 00000030    "GA..@..[" */
    0x81,0x0B,0x44,0x42,0x47,0x41,0x01,0x44,  /* 00000038    "..DBGA.D" */
    0x42,0x47,0x31,0x08,0x5B,0x80,0x44,0x42,  /* 00000040    "BG1.[.DB" */
    0x47,0x42,0x01,0x0B,0x44,0xB0,0x01,0x5B,  /* 00000048    "GB..D..[" */
    0x81,0x0B,0x44,0x42,0x47,0x42,0x01,0x44,  /* 00000050    "..DBGB.D" */
    0x42,0x47,0x32,0x08,0x5B,0x80,0x44,0x42,  /* 00000058    "BG2.[.DB" */
    0x47,0x43,0x01,0x0B,0x46,0xB0,0x01,0x5B,  /* 00000060    "GC..F..[" */
    0x81,0x0B,0x44,0x42,0x47,0x43,0x01,0x44,  /* 00000068    "..DBGC.D" */
    0x42,0x47,0x33,0x08,0x5B,0x80,0x44,0x42,  /* 00000070    "BG3.[.DB" */
    0x47,0x44,0x01,0x0B,0x48,0xB0,0x01,0x5B,  /* 00000078    "GD..H..[" */
    0x81,0x0B,0x44,0x42,0x47,0x44,0x01,0x44,  /* 00000080    "..DBGD.D" */
    0x42,0x47,0x34,0x08,0x5B,0x80,0x50,0x52,  /* 00000088    "BG4.[.PR" */
    0x54,0x31,0x01,0x0A,0xB2,0x0A,0x02,0x5B,  /* 00000090    "T1.....[" */
    0x81,0x10,0x50,0x52,0x54,0x31,0x01,0x50,  /* 00000098    "..PRT1.P" */
    0x42,0x32,0x5F,0x08,0x50,0x42,0x32,0x41,  /* 000000A0    "B2_.PB2A" */
    0x08,0x5B,0x80,0x50,0x52,0x54,0x32,0x01,  /* 000000A8    ".[.PRT2." */
    0x0A,0x86,0x01,0x5B,0x81,0x0B,0x50,0x52,  /* 000000B0    "...[..PR" */
    0x54,0x32,0x01,0x50,0x38,0x36,0x5F,0x08,  /* 000000B8    "T2.P86_." */
    0x5B,0x80,0x50,0x52,0x54,0x33,0x01,0x0A,  /* 000000C0    "[.PRT3.." */
    0x88,0x01,0x5B,0x81,0x0B,0x50,0x52,0x54,  /* 000000C8    "..[..PRT" */
    0x33,0x01,0x50,0x38,0x38,0x5F,0x08,0x5B,  /* 000000D0    "3.P88_.[" */
    0x01,0x53,0x59,0x4E,0x43,0x01,0x08,0x42,  /* 000000D8    ".SYNC..B" */
    0x55,0x46,0x30,0x11,0x04,0x0B,0x00,0x01,  /* 000000E0    "UF0....." */
    0x08,0x42,0x55,0x46,0x31,0x11,0x03,0x0A,  /* 000000E8    ".BUF1..." */
    0x08,0x8B,0x42,0x55,0x46,0x31,0x00,0x42,  /* 000000F0    "..BUF1.B" */
    0x55,0x46,0x41,0x8B,0x42,0x55,0x46,0x31,  /* 000000F8    "UFA.BUF1" */
    0x0A,0x04,0x42,0x55,0x46,0x42,0x14,0x14,  /* 00000100    "..BUFB.." */
    0x41,0x43,0x51,0x52,0x00,0x5B,0x23,0x53,  /* 00000108    "ACQR.[#S" */
    0x59,0x4E,0x43,0xFF,0xFF,0x70,0x00,0x42,  /* 00000110    "YNC..p.B" */
    0x55,0x46,0x41,0x14,0x31,0x49,0x4E,0x49,  /* 00000118    "UFA.1INI" */
    0x54,0x01,0x70,0x42,0x55,0x46,0x41,0x60,  /* 00000120    "T.pBUFA`" */
    0x75,0x60,0xA0,0x22,0x92,0x94,0x60,0x87,  /* 00000128    "u`."..`." */
    0x42,0x55,0x46,0x30,0x8C,0x42,0x55,0x46,  /* 00000130    "BUF0.BUF" */
    0x30,0x42,0x55,0x46,0x41,0x54,0x4D,0x50,  /* 00000138    "0BUFATMP" */
    0x31,0x70,0x68,0x54,0x4D,0x50,0x31,0x70,  /* 00000140    "1phTMP1p" */
    0x60,0x42,0x55,0x46,0x41,0x14,0x48,0x07,  /* 00000148    "`BUFA.H." */
    0x57,0x50,0x52,0x54,0x02,0x70,0x69,0x5C,  /* 00000150    "WPRT.pi\" */
    0x2E,0x5F,0x53,0x42,0x5F,0x50,0x38,0x36,  /* 00000158    "._SB_P86" */
    0x5F,0x70,0x68,0x5C,0x2E,0x5F,0x53,0x42,  /* 00000160    "_ph\._SB" */
    0x5F,0x50,0x42,0x32,0x5F,0x70,0x68,0x5C,  /* 00000168    "_PB2_ph\" */
    0x2E,0x5F,0x53,0x42,0x5F,0x44,0x42,0x47,  /* 00000170    "._SB_DBG" */
    0x32,0x70,0x69,0x5C,0x2E,0x5F,0x53,0x42,  /* 00000178    "2pi\._SB" */
    0x5F,0x44,0x42,0x47,0x34,0x70,0x5C,0x2E,  /* 00000180    "_DBG4p\." */
    0x5F,0x53,0x42,0x5F,0x50,0x42,0x32,0x5F,  /* 00000188    "_SB_PB2_" */
    0x60,0xA2,0x11,0x92,0x93,0x60,0x00,0x70,  /* 00000190    "`....`.p" */
    0x5C,0x2E,0x5F,0x53,0x42,0x5F,0x50,0x42,  /* 00000198    "\._SB_PB" */
    0x32,0x5F,0x60,0x70,0x5C,0x2E,0x5F,0x53,  /* 000001A0    "2_`p\._S" */
    0x42,0x5F,0x50,0x38,0x36,0x5F,0x61,0x70,  /* 000001A8    "B_P86_ap" */
    0x61,0x5C,0x2E,0x5F,0x53,0x42,0x5F,0x44,  /* 000001B0    "a\._SB_D" */
    0x42,0x47,0x33,0xA4,0x5C,0x2E,0x5F,0x53,  /* 000001B8    "BG3.\._S" */
    0x42,0x5F,0x50,0x38,0x36,0x5F,0x14,0x1D,  /* 000001C0    "B_P86_.." */
    0x48,0x4C,0x50,0x31,0x02,0xA0,0x16,0x95,  /* 000001C8    "HLP1...." */
    0x69,0x87,0x68,0x8C,0x68,0x69,0x54,0x4D,  /* 000001D0    "i.h.hiTM" */
    0x50,0x31,0x57,0x50,0x52,0x54,0x0A,0x7C,  /* 000001D8    "P1WPRT.|" */
    0x54,0x4D,0x50,0x31,0x14,0x23,0x48,0x4C,  /* 000001E0    "TMP1.#HL" */
    0x50,0x32,0x00,0x57,0x50,0x52,0x54,0x0A,  /* 000001E8    "P2.WPRT." */
    0x7B,0x00,0x70,0x00,0x60,0xA2,0x12,0x95,  /* 000001F0    "{.p.`..." */
    0x60,0x42,0x55,0x46,0x41,0x48,0x4C,0x50,  /* 000001F8    "`BUFAHLP" */
    0x31,0x42,0x55,0x46,0x30,0x60,0x75,0x60,  /* 00000200    "1BUF0`u`" */
    0x14,0x1F,0x48,0x4C,0x50,0x33,0x02,0xA0,  /* 00000208    "..HLP3.." */
    0x18,0x95,0x69,0x87,0x68,0x8C,0x68,0x69,  /* 00000210    "..i.h.hi" */
    0x54,0x4D,0x50,0x31,0x70,0x57,0x50,0x52,  /* 00000218    "TMP1pWPR" */
    0x54,0x0A,0x7D,0x00,0x54,0x4D,0x50,0x31,  /* 00000220    "T.}.TMP1" */
    0x14,0x23,0x48,0x4C,0x50,0x34,0x00,0x70,  /* 00000228    ".#HLP4.p" */
    0x00,0x60,0xA2,0x19,0x95,0x60,0x42,0x55,  /* 00000230    ".`...`BU" */
    0x46,0x42,0x72,0x42,0x55,0x46,0x41,0x60,  /* 00000238    "FBrBUFA`" */
    0x61,0x48,0x4C,0x50,0x33,0x42,0x55,0x46,  /* 00000240    "aHLP3BUF" */
    0x30,0x61,0x75,0x60,0x14,0x42,0x04,0x48,  /* 00000248    "0au`.B.H" */
    0x4C,0x50,0x35,0x00,0x48,0x4C,0x50,0x32,  /* 00000250    "LP5.HLP2" */
    0x70,0x57,0x50,0x52,0x54,0x0A,0x79,0x00,  /* 00000258    "pWPRT.y." */
    0x42,0x55,0x46,0x42,0x72,0x42,0x55,0x46,  /* 00000260    "BUFBrBUF" */
    0x41,0x42,0x55,0x46,0x42,0x60,0xA0,0x1C,  /* 00000268    "ABUFB`.." */
    0x95,0x87,0x42,0x55,0x46,0x30,0x60,0x70,  /* 00000270    "..BUF0`p" */
    0x87,0x42,0x55,0x46,0x30,0x60,0x74,0x60,  /* 00000278    ".BUF0`t`" */
    0x42,0x55,0x46,0x41,0x60,0x70,0x60,0x42,  /* 00000280    "BUFA`p`B" */
    0x55,0x46,0x42,0x48,0x4C,0x50,0x34,0x14,  /* 00000288    "UFBHLP4." */
    0x32,0x48,0x4C,0x50,0x36,0x00,0x70,0x42,  /* 00000290    "2HLP6.pB" */
    0x55,0x46,0x41,0x60,0x75,0x60,0xA0,0x21,  /* 00000298    "UFA`u`.!" */
    0x92,0x94,0x60,0x87,0x42,0x55,0x46,0x30,  /* 000002A0    "..`.BUF0" */
    0x8C,0x42,0x55,0x46,0x30,0x42,0x55,0x46,  /* 000002A8    ".BUF0BUF" */
    0x41,0x54,0x4D,0x50,0x31,0x70,0x60,0x42,  /* 000002B0    "ATMP1p`B" */
    0x55,0x46,0x41,0xA4,0x54,0x4D,0x50,0x31,  /* 000002B8    "UFA.TMP1" */
    0xA4,0x00,0x14,0x35,0x48,0x4C,0x50,0x37,  /* 000002C0    "...5HLP7" */
    0x00,0x70,0x42,0x55,0x46,0x41,0x60,0x72,  /* 000002C8    ".pBUFA`r" */
    0x60,0x0A,0x04,0x60,0xA0,0x21,0x92,0x94,  /* 000002D0    "`..`.!.." */
    0x60,0x87,0x42,0x55,0x46,0x30,0x8A,0x42,  /* 000002D8    "`.BUF0.B" */
    0x55,0x46,0x30,0x42,0x55,0x46,0x41,0x53,  /* 000002E0    "UF0BUFAS" */
    0x58,0x32,0x32,0x70,0x60,0x42,0x55,0x46,  /* 000002E8    "X22p`BUF" */
    0x41,0xA4,0x53,0x58,0x32,0x32,0xA4,0x00,  /* 000002F0    "A.SX22.." */
    0x14,0x1C,0x48,0x4C,0x50,0x38,0x02,0xA0,  /* 000002F8    "..HLP8.." */
    0x15,0x95,0x69,0x87,0x68,0x8C,0x68,0x69,  /* 00000300    "..i.h.hi" */
    0x54,0x4D,0x50,0x31,0x70,0x48,0x4C,0x50,  /* 00000308    "TMP1pHLP" */
    0x36,0x54,0x4D,0x50,0x31,0x14,0x16,0x48,  /* 00000310    "6TMP1..H" */
    0x4C,0x50,0x39,0x02,0x70,0x00,0x60,0xA2,  /* 00000318    "LP9.p.`." */
    0x0C,0x95,0x60,0x69,0x48,0x4C,0x50,0x38,  /* 00000320    "..`iHLP8" */
    0x68,0x60,0x75,0x60,0x14,0x22,0x48,0x4C,  /* 00000328    "h`u`."HL" */
    0x50,0x41,0x00,0x70,0x48,0x4C,0x50,0x36,  /* 00000330    "PA.pHLP6" */
    0x60,0x08,0x54,0x4D,0x50,0x5F,0x11,0x02,  /* 00000338    "`.TMP_.." */
    0x60,0x48,0x4C,0x50,0x39,0x54,0x4D,0x50,  /* 00000340    "`HLP9TMP" */
    0x5F,0x60,0xA4,0x54,0x4D,0x50,0x5F,0x14,  /* 00000348    "_`.TMP_." */
    0x0C,0x52,0x45,0x4C,0x5F,0x00,0x5B,0x27,  /* 00000350    ".REL_.['" */
    0x53,0x59,0x4E,0x43,0x5B,0x82,0x3C,0x41,  /* 00000358    "SYNC[.<A" */
    0x43,0x5F,0x5F,0x08,0x5F,0x48,0x49,0x44,  /* 00000360    "C__._HID" */
    0x0D,0x41,0x43,0x50,0x49,0x30,0x30,0x30,  /* 00000368    ".ACPI000" */
    0x33,0x00,0x08,0x5F,0x50,0x43,0x4C,0x12,  /* 00000370    "3.._PCL." */
    0x0F,0x03,0x5C,0x5F,0x53,0x42,0x5F,0x42,  /* 00000378    "..\_SB_B" */
    0x41,0x54,0x30,0x42,0x41,0x54,0x31,0x14,  /* 00000380    "AT0BAT1." */
    0x08,0x5F,0x50,0x53,0x52,0x00,0xA4,0x00,  /* 00000388    "._PSR..." */
    0x14,0x09,0x5F,0x53,0x54,0x41,0x00,0xA4,  /* 00000390    ".._STA.." */
    0x0A,0x0F,0x08,0x42,0x49,0x46,0x50,0x12,  /* 00000398    "...BIFP." */
    0x02,0x0D,0x14,0x49,0x0C,0x42,0x49,0x46,  /* 000003A0    "...I.BIF" */
    0x5F,0x01,0x41,0x43,0x51,0x52,0x49,0x4E,  /* 000003A8    "_.ACQRIN" */
    0x49,0x54,0x01,0x49,0x4E,0x49,0x54,0x68,  /* 000003B0    "IT.INITh" */
    0x48,0x4C,0x50,0x35,0x70,0x48,0x4C,0x50,  /* 000003B8    "HLP5pHLP" */
    0x37,0x88,0x42,0x49,0x46,0x50,0x00,0x00,  /* 000003C0    "7.BIFP.." */
    0x70,0x48,0x4C,0x50,0x37,0x88,0x42,0x49,  /* 000003C8    "pHLP7.BI" */
    0x46,0x50,0x01,0x00,0x70,0x48,0x4C,0x50,  /* 000003D0    "FP..pHLP" */
    0x37,0x88,0x42,0x49,0x46,0x50,0x0A,0x02,  /* 000003D8    "7.BIFP.." */
    0x00,0x70,0x48,0x4C,0x50,0x37,0x88,0x42,  /* 000003E0    ".pHLP7.B" */
    0x49,0x46,0x50,0x0A,0x03,0x00,0x70,0x48,  /* 000003E8    "IFP...pH" */
    0x4C,0x50,0x37,0x88,0x42,0x49,0x46,0x50,  /* 000003F0    "LP7.BIFP" */
    0x0A,0x04,0x00,0x70,0x48,0x4C,0x50,0x37,  /* 000003F8    "...pHLP7" */
    0x88,0x42,0x49,0x46,0x50,0x0A,0x05,0x00,  /* 00000400    ".BIFP..." */
    0x70,0x48,0x4C,0x50,0x37,0x88,0x42,0x49,  /* 00000408    "pHLP7.BI" */
    0x46,0x50,0x0A,0x06,0x00,0x70,0x48,0x4C,  /* 00000410    "FP...pHL" */
    0x50,0x37,0x88,0x42,0x49,0x46,0x50,0x0A,  /* 00000418    "P7.BIFP." */
    0x07,0x00,0x70,0x48,0x4C,0x50,0x37,0x88,  /* 00000420    "..pHLP7." */
    0x42,0x49,0x46,0x50,0x0A,0x08,0x00,0x70,  /* 00000428    "BIFP...p" */
    0x48,0x4C,0x50,0x41,0x88,0x42,0x49,0x46,  /* 00000430    "HLPA.BIF" */
    0x50,0x0A,0x09,0x00,0x70,0x48,0x4C,0x50,  /* 00000438    "P...pHLP" */
    0x41,0x88,0x42,0x49,0x46,0x50,0x0A,0x0A,  /* 00000440    "A.BIFP.." */
    0x00,0x70,0x48,0x4C,0x50,0x41,0x88,0x42,  /* 00000448    ".pHLPA.B" */
    0x49,0x46,0x50,0x0A,0x0B,0x00,0x70,0x48,  /* 00000450    "IFP...pH" */
    0x4C,0x50,0x41,0x88,0x42,0x49,0x46,0x50,  /* 00000458    "LPA.BIFP" */
    0x0A,0x0C,0x00,0x52,0x45,0x4C,0x5F,0xA4,  /* 00000460    "...REL_." */
    0x42,0x49,0x46,0x50,0x5B,0x82,0x4F,0x0B,  /* 00000468    "BIFP[.O." */
    0x42,0x41,0x54,0x30,0x08,0x5F,0x48,0x49,  /* 00000470    "BAT0._HI" */
    0x44,0x0C,0x41,0xD0,0x0C,0x0A,0x08,0x5F,  /* 00000478    "D.A...._" */
    0x55,0x49,0x44,0x01,0x08,0x5F,0x50,0x43,  /* 00000480    "UID.._PC" */
    0x4C,0x12,0x07,0x01,0x5C,0x5F,0x53,0x42,  /* 00000488    "L...\_SB" */
    0x5F,0x14,0x14,0x5F,0x53,0x54,0x41,0x00,  /* 00000490    "_.._STA." */
    0x70,0x5C,0x2E,0x5F,0x53,0x42,0x5F,0x50,  /* 00000498    "p\._SB_P" */
    0x38,0x38,0x5F,0x60,0xA4,0x60,0x14,0x0F,  /* 000004A0    "88_`.`.." */
    0x5F,0x42,0x49,0x46,0x00,0x70,0x42,0x49,  /* 000004A8    "_BIF.pBI" */
    0x46,0x5F,0x01,0x60,0xA4,0x60,0x14,0x46,  /* 000004B0    "F_.`.`.F" */
    0x07,0x5F,0x42,0x53,0x54,0x00,0x70,0x01,  /* 000004B8    "._BST.p." */
    0x5C,0x2E,0x5F,0x53,0x42,0x5F,0x44,0x42,  /* 000004C0    "\._SB_DB" */
    0x47,0x31,0x41,0x43,0x51,0x52,0x49,0x4E,  /* 000004C8    "G1ACQRIN" */
    0x49,0x54,0x0A,0x02,0x49,0x4E,0x49,0x54,  /* 000004D0    "IT..INIT" */
    0x01,0x48,0x4C,0x50,0x35,0x08,0x42,0x53,  /* 000004D8    ".HLP5.BS" */
    0x54,0x30,0x12,0x02,0x04,0x70,0x48,0x4C,  /* 000004E0    "T0...pHL" */
    0x50,0x37,0x88,0x42,0x53,0x54,0x30,0x00,  /* 000004E8    "P7.BST0." */
    0x00,0x70,0x48,0x4C,0x50,0x37,0x88,0x42,  /* 000004F0    ".pHLP7.B" */
    0x53,0x54,0x30,0x01,0x00,0x70,0x48,0x4C,  /* 000004F8    "ST0..pHL" */
    0x50,0x37,0x88,0x42,0x53,0x54,0x30,0x0A,  /* 00000500    "P7.BST0." */
    0x02,0x00,0x70,0x48,0x4C,0x50,0x37,0x88,  /* 00000508    "..pHLP7." */
    0x42,0x53,0x54,0x30,0x0A,0x03,0x00,0x52,  /* 00000510    "BST0...R" */
    0x45,0x4C,0x5F,0x70,0x0A,0x02,0x5C,0x2E,  /* 00000518    "EL_p..\." */
    0x5F,0x53,0x42,0x5F,0x44,0x42,0x47,0x31,  /* 00000520    "_SB_DBG1" */
    0xA4,0x42,0x53,0x54,0x30,0x5B,0x82,0x47,  /* 00000528    ".BST0[.G" */
    0x0A,0x42,0x41,0x54,0x31,0x08,0x5F,0x48,  /* 00000530    ".BAT1._H" */
    0x49,0x44,0x0C,0x41,0xD0,0x0C,0x0A,0x08,  /* 00000538    "ID.A...." */
    0x5F,0x55,0x49,0x44,0x0A,0x02,0x08,0x5F,  /* 00000540    "_UID..._" */
    0x50,0x43,0x4C,0x12,0x07,0x01,0x5C,0x5F,  /* 00000548    "PCL...\_" */
    0x53,0x42,0x5F,0x14,0x09,0x5F,0x53,0x54,  /* 00000550    "SB_.._ST" */
    0x41,0x00,0xA4,0x0A,0x0F,0x14,0x19,0x5F,  /* 00000558    "A......_" */
    0x42,0x49,0x46,0x00,0x70,0x5C,0x2E,0x5F,  /* 00000560    "BIF.p\._" */
    0x53,0x42,0x5F,0x50,0x42,0x32,0x5F,0x60,  /* 00000568    "SB_PB2_`" */
    0xA4,0x42,0x49,0x46,0x5F,0x0A,0x02,0x14,  /* 00000570    ".BIF_..." */
    0x4E,0x05,0x5F,0x42,0x53,0x54,0x00,0x41,  /* 00000578    "N._BST.A" */
    0x43,0x51,0x52,0x49,0x4E,0x49,0x54,0x0A,  /* 00000580    "CQRINIT." */
    0x02,0x49,0x4E,0x49,0x54,0x0A,0x02,0x48,  /* 00000588    ".INIT..H" */
    0x4C,0x50,0x35,0x08,0x42,0x53,0x54,0x31,  /* 00000590    "LP5.BST1" */
    0x12,0x02,0x04,0x70,0x48,0x4C,0x50,0x37,  /* 00000598    "...pHLP7" */
    0x88,0x42,0x53,0x54,0x31,0x00,0x00,0x70,  /* 000005A0    ".BST1..p" */
    0x48,0x4C,0x50,0x37,0x88,0x42,0x53,0x54,  /* 000005A8    "HLP7.BST" */
    0x31,0x01,0x00,0x70,0x48,0x4C,0x50,0x37,  /* 000005B0    "1..pHLP7" */
    0x88,0x42,0x53,0x54,0x31,0x0A,0x02,0x00,  /* 000005B8    ".BST1..." */
    0x70,0x48,0x4C,0x50,0x37,0x88,0x42,0x53,  /* 000005C0    "pHLP7.BS" */
    0x54,0x31,0x0A,0x03,0x00,0x52,0x45,0x4C,  /* 000005C8    "T1...REL" */
    0x5F,0xA4,0x42,0x53,0x54,0x31,
};
