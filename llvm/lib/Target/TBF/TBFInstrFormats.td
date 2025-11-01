//===-- TBFInstrFormats.td - TBF Instruction Formats -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class TBFOpClass<bits<3> val> {
  bits<3> Value = val;
}

def TBF_LD    : TBFOpClass<0x0>;
def TBF_LDX   : TBFOpClass<0x1>;
def TBF_ST    : TBFOpClass<0x2>;
def TBF_STX   : TBFOpClass<0x3>;
def TBF_ALU   : TBFOpClass<0x4>;
def TBF_JMP   : TBFOpClass<0x5>;
def TBF_PQR   : TBFOpClass<0x6>;
def TBF_ALU64 : TBFOpClass<0x7>;

def TBF_LD_V2 : TBFOpClass<0x4>;
def TBF_ST_V2 : TBFOpClass<0x7>;

class TBFSrcType<bits<1> val> {
  bits<1> Value = val;
}

def TBF_K : TBFSrcType<0x0>;
def TBF_X : TBFSrcType<0x1>;

class TBFArithOp<bits<4> val> {
  bits<4> Value = val;
}

def TBF_ADD  : TBFArithOp<0x0>;
def TBF_SUB  : TBFArithOp<0x1>;
def TBF_MUL  : TBFArithOp<0x2>;
def TBF_DIV  : TBFArithOp<0x3>;
def TBF_OR   : TBFArithOp<0x4>;
def TBF_AND  : TBFArithOp<0x5>;
def TBF_LSH  : TBFArithOp<0x6>;
def TBF_RSH  : TBFArithOp<0x7>;
def TBF_NEG  : TBFArithOp<0x8>;
def TBF_MOD  : TBFArithOp<0x9>;
def TBF_XOR  : TBFArithOp<0xa>;
def TBF_MOV  : TBFArithOp<0xb>;
def TBF_ARSH : TBFArithOp<0xc>;
def TBF_END  : TBFArithOp<0xd>;
def TBF_SDIV : TBFArithOp<0xe>;
def TBF_HOR  : TBFArithOp<0xf>;

def PQR_UHMUL : TBFArithOp<0x2>;
def PQR_UDIV  : TBFArithOp<0x4>;
def PQR_UREM  : TBFArithOp<0x6>;
def PQR_LMUL  : TBFArithOp<0x8>;
def PQR_SHMUL : TBFArithOp<0xa>;
def PQR_SDIV  : TBFArithOp<0xc>;
def PQR_SREM  : TBFArithOp<0xe>;

def TBF_XCHG    : TBFArithOp<0xe>;
def TBF_CMPXCHG : TBFArithOp<0xf>;

class TBFEndDir<bits<1> val> {
  bits<1> Value = val;
}

def TBF_TO_LE : TBFSrcType<0x0>;
def TBF_TO_BE : TBFSrcType<0x1>;

class TBFJumpOp<bits<4> val> {
  bits<4> Value = val;
}

def TBF_JA      : TBFJumpOp<0x0>;
def TBF_JEQ     : TBFJumpOp<0x1>;
def TBF_JGT     : TBFJumpOp<0x2>;
def TBF_JGE     : TBFJumpOp<0x3>;
def TBF_JNE     : TBFJumpOp<0x5>;
def TBF_JSGT    : TBFJumpOp<0x6>;
def TBF_JSGE    : TBFJumpOp<0x7>;
def TBF_CALL    : TBFJumpOp<0x8>;
def TBF_EXIT    : TBFJumpOp<0x9>;
def TBF_JLT     : TBFJumpOp<0xa>;
def TBF_JLE     : TBFJumpOp<0xb>;
def TBF_JSLT    : TBFJumpOp<0xc>;
def TBF_JSLE    : TBFJumpOp<0xd>;
def TBF_SYSCALL : TBFJumpOp<0x9>;

class TBFWidthModifer<bits<2> val> {
  bits<2> Value = val;
}

def TBF_W  : TBFWidthModifer<0x0>;
def TBF_H  : TBFWidthModifer<0x1>;
def TBF_B  : TBFWidthModifer<0x2>;
def TBF_DW : TBFWidthModifer<0x3>;

class TBFSizeModifier<bits<4> val> {
  bits<4> Value = val;
}

def TBF_B_V2  : TBFSizeModifier<0x2>;
def TBF_H_V2  : TBFSizeModifier<0x3>;
def TBF_W_V2  : TBFSizeModifier<0x8>;
def TBF_DW_V2 : TBFSizeModifier<0x9>;

class TBFModeModifer<bits<3> val> {
  bits<3> Value = val;
}

def TBF_IMM  : TBFModeModifer<0x0>;
def TBF_ABS  : TBFModeModifer<0x1>;
def TBF_IND  : TBFModeModifer<0x2>;
def TBF_MEM  : TBFModeModifer<0x3>;
def TBF_ATOMIC : TBFModeModifer<0x6>;

class TBFAtomicFlag<bits<4> val> {
  bits<4> Value = val;
}

def TBF_FETCH : TBFAtomicFlag<0x1>;

class InstTBF<dag outs, dag ins, string asmstr, list<dag> pattern>
  : Instruction {
  field bits<64> Inst;
  field bits<64> SoftFail = 0;
  let Size = 8;

  let Namespace = "TBF";
  let DecoderNamespace = "TBF";

  TBFOpClass TBFClass;
  let Inst{58-56} = TBFClass.Value;

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
  : InstTBF<outs, ins, asmstr, pattern> {
  let Inst{63-0} = 0;
  let isPseudo = 1;
}
