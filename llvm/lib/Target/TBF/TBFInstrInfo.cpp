//===-- TBFInstrInfo.cpp - TBF Instruction Information ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the TBF implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "TBFInstrInfo.h"
#include "TBF.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>

#define GET_INSTRINFO_CTOR_DTOR
#include "TBFGenInstrInfo.inc"

using namespace llvm;

static inline bool isUncondBranchOpcode(int Opc) { return Opc == TBF::JMP; }

static inline bool isCondBranchOpcode(int Opc) {
  switch (Opc) {
  case TBF::JEQ_ri:
  case TBF::JEQ_rr:
  case TBF::JUGT_ri:
  case TBF::JUGT_rr:
  case TBF::JUGE_ri:
  case TBF::JUGE_rr:
  case TBF::JNE_ri:
  case TBF::JNE_rr:
  case TBF::JSGT_ri:
  case TBF::JSGT_rr:
  case TBF::JSGE_ri:
  case TBF::JSGE_rr:
  case TBF::JULT_ri:
  case TBF::JULT_rr:
  case TBF::JULE_ri:
  case TBF::JULE_rr:
  case TBF::JSLT_ri:
  case TBF::JSLT_rr:
  case TBF::JSLE_ri:
  case TBF::JSLE_rr:
    return true;
  default:
    return false;
  }
}

static void parseCondBranch(MachineInstr *LastInst, MachineBasicBlock *&Target,
                            SmallVectorImpl<MachineOperand> &Cond) {
  Cond.push_back(MachineOperand::CreateImm(LastInst->getOpcode()));
  Cond.push_back(LastInst->getOperand(0));
  Cond.push_back(LastInst->getOperand(1));
  Target = LastInst->getOperand(2).getMBB();
}

TBFInstrInfo::TBFInstrInfo()
    : TBFGenInstrInfo(TBF::ADJCALLSTACKDOWN, TBF::ADJCALLSTACKUP) {}

void TBFInstrInfo::initializeTargetFeatures(bool HasExplicitSext, bool NewMemEncoding) {
  this->HasExplicitSignExt = HasExplicitSext;
  this->NewMemEncoding = NewMemEncoding;
}

void TBFInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator I,
                               const DebugLoc &DL, MCRegister DestReg,
                               MCRegister SrcReg, bool KillSrc,
                               bool RenamableDest, bool RenamableSrc) const {
  if (TBF::GPRRegClass.contains(DestReg, SrcReg))
    BuildMI(MBB, I, DL, get(TBF::MOV_rr), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
  else if (TBF::GPR32RegClass.contains(DestReg, SrcReg)) {
    unsigned OpCode =
        HasExplicitSignExt ? TBF::MOV_rr_32_no_sext_v2
                           : TBF::MOV_rr_32_no_sext_v1;
    BuildMI(MBB, I, DL, get(OpCode), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
  }
  else
    llvm_unreachable("Impossible reg-to-reg copy");
}

void TBFInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                       MachineBasicBlock::iterator I,
                                       Register SrcReg, bool IsKill, int FI,
                                       const TargetRegisterClass *RC,
                                       const TargetRegisterInfo *TRI,
                                       Register VReg,
                                       MachineInstr::MIFlag Flags) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  if (RC == &TBF::GPRRegClass)
    BuildMI(MBB, I, DL, get(NewMemEncoding ?
                                           TBF::STD_V2 : TBF::STD_V1))
        .addReg(SrcReg, getKillRegState(IsKill))
        .addFrameIndex(FI)
        .addImm(0);
  else if (RC == &TBF::GPR32RegClass)
    BuildMI(MBB, I, DL, get(NewMemEncoding ?
                                           TBF::STW32_V2 : TBF::STW32_V1))
        .addReg(SrcReg, getKillRegState(IsKill))
        .addFrameIndex(FI)
        .addImm(0);
  else
    llvm_unreachable("Can't store this register to stack slot");
}

Register TBFInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                          int &FrameIndex,
                                          unsigned &MemBytes) const {
  switch (MI.getOpcode()) {
  default:
    break;
  case TBF::STD_V2:
  case TBF::STD_V1:
    MemBytes = 8;
    if (MI.getOperand(0).isReg() && MI.getOperand(1).isFI() &&
        MI.getOperand(2).isImm() && MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
    break;
  case TBF::STW32_V2:
  case TBF::STW32_V1:
    MemBytes = 4;
    if (MI.getOperand(0).isReg() && MI.getOperand(1).isFI() &&
        MI.getOperand(2).isImm() && MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
    break;
  }

  return 0;
}

Register TBFInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                          int &FrameIndex) const {
  unsigned MemBytes = 0;
  return isStoreToStackSlot(MI, FrameIndex, MemBytes);
}

void TBFInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator I,
                                        Register DestReg, int FI,
                                        const TargetRegisterClass *RC,
                                        const TargetRegisterInfo *TRI,
                                        Register VReg,
                                        MachineInstr::MIFlag Flags) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  if (RC == &TBF::GPRRegClass)
    BuildMI(MBB, I, DL, get(NewMemEncoding ?
                                           TBF::LDD_V2 : TBF::LDD_V1),
            DestReg).addFrameIndex(FI).addImm(0);
  else if (RC == &TBF::GPR32RegClass)
    BuildMI(MBB, I, DL, get(NewMemEncoding ?
                                           TBF::LDW32_V2 : TBF::LDW32_V1),
            DestReg).addFrameIndex(FI).addImm(0);
  else
    llvm_unreachable("Can't load this register from stack slot");
}

Register TBFInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                           int &FrameIndex,
                                           unsigned &MemBytes) const {
  switch (MI.getOpcode()) {
  default:
    break;
  case TBF::LDD_V2:
  case TBF::LDD_V1:
    MemBytes = 8;
    if (MI.getOperand(0).isReg() && MI.getOperand(1).isFI() &&
        MI.getOperand(2).isImm() && MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
    break;
  case TBF::LDW32_V2:
  case TBF::LDW32_V1:
    MemBytes = 4;
    if (MI.getOperand(0).isReg() && MI.getOperand(1).isFI() &&
        MI.getOperand(2).isImm() && MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
    break;
  }

  return 0;
}

Register TBFInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                           int &FrameIndex) const {
  unsigned MemBytes = 0;
  return isLoadFromStackSlot(MI, FrameIndex, MemBytes);
}

bool TBFInstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                 MachineBasicBlock *&TBB,
                                 MachineBasicBlock *&FBB,
                                 SmallVectorImpl<MachineOperand> &Cond,
                                 bool AllowModify) const {
  // If the block has no terminators, it just falls into the block after it.
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end())
    return false;

  if (!isUnpredicatedTerminator(*I))
    return false;

  // Get the last instruction in the block.
  MachineInstr *LastInst = &*I;

  // If there is only one terminator instruction, process it.
  unsigned LastOpc = LastInst->getOpcode();
  if (I == MBB.begin() || !isUnpredicatedTerminator(*--I)) {
    if (isUncondBranchOpcode(LastOpc)) {
      TBB = LastInst->getOperand(0).getMBB();
      return false;
    }
    if (isCondBranchOpcode(LastOpc)) {
      // Block ends with fall-through condbranch.
      parseCondBranch(LastInst, TBB, Cond);
      return false;
    }
    return true; // Unknown case
  }

  // Get the instruction before it if it is a terminator.
  MachineInstr *SecondLastInst = &*I;
  unsigned SecondLastOpc = SecondLastInst->getOpcode();

  // If AllowModify is true and the block ends with two or more unconditional
  // branches, delete all but the first unconditional branch.
  if (AllowModify && isUncondBranchOpcode(LastOpc)) {
    while (isUncondBranchOpcode(SecondLastOpc)) {
      LastInst->eraseFromParent();
      LastInst = SecondLastInst;
      LastOpc = LastInst->getOpcode();
      if (I == MBB.begin() || !isUnpredicatedTerminator(*--I)) {
        // Return now the only terminator is an unconditional branch.
        TBB = LastInst->getOperand(0).getMBB();
        return false;
      }
      SecondLastInst = &*I;
      SecondLastOpc = SecondLastInst->getOpcode();
    }
  }

  // If we're allowed to modify and the block ends in a unconditional branch
  // which could simply fallthrough, remove the branch.  (Note: This case only
  // matters when we can't understand the whole sequence, otherwise it's also
  // handled by BranchFolding.cpp.)
  if (AllowModify && isUncondBranchOpcode(LastOpc) &&
      MBB.isLayoutSuccessor(getBranchDestBlock(*LastInst))) {
    LastInst->eraseFromParent();
    LastInst = SecondLastInst;
    LastOpc = LastInst->getOpcode();
    if (I == MBB.begin() || !isUnpredicatedTerminator(*--I)) {
      assert(!isUncondBranchOpcode(LastOpc) &&
             "unreachable unconditional branches removed above");

      if (isCondBranchOpcode(LastOpc)) {
        // Block ends with fall-through condbranch.
        parseCondBranch(LastInst, TBB, Cond);
        return false;
      }
      return true; // Can't handle indirect branch.
    }
    SecondLastInst = &*I;
    SecondLastOpc = SecondLastInst->getOpcode();
  }

  // If there are three terminators, we don't know what sort of block this is.
  if (SecondLastInst && I != MBB.begin() && isUnpredicatedTerminator(*--I))
    return true;

  // If the block ends with a conditional jump and a JA, handle it.
  if (isCondBranchOpcode(SecondLastOpc) && isUncondBranchOpcode(LastOpc)) {
    parseCondBranch(SecondLastInst, TBB, Cond);
    FBB = LastInst->getOperand(0).getMBB();
    return false;
  }

  // If the block ends with two unconditional branches, handle it.  The second
  // one is not executed, so remove it.
  if (isUncondBranchOpcode(SecondLastOpc) && isUncondBranchOpcode(LastOpc)) {
    TBB = SecondLastInst->getOperand(0).getMBB();
    I = LastInst;
    if (AllowModify)
      I->eraseFromParent();
    return false;
  }

  // Otherwise, can't handle this.
  return true;
}

unsigned TBFInstrInfo::insertBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *TBB,
                                    MachineBasicBlock *FBB,
                                    ArrayRef<MachineOperand> Cond,
                                    const DebugLoc &DL, int *BytesAdded) const {
  assert(!BytesAdded && "code size not handled");

  // Shouldn't be a fall through.
  assert(TBB && "insertBranch must not be told to insert a fallthrough");

  if (BytesAdded)
    *BytesAdded = 8;

  if (Cond.empty()) {
    // Unconditional branch
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(TBF::JMP)).addMBB(TBB);
    return 1;
  }

  // See the order we parse the jump information in `parseCondBranch`
  BuildMI(&MBB, DL, get(Cond[0].getImm()))
      .add(Cond[1])
      .add(Cond[2])
      .addMBB(TBB);

  if (FBB) {
    BuildMI(&MBB, DL, get(TBF::JMP)).addMBB(FBB);
    if (BytesAdded)
      *BytesAdded += 8;
  }

  return 1;
}

unsigned TBFInstrInfo::removeBranch(MachineBasicBlock &MBB,
                                    int *BytesRemoved) const {
  assert(!BytesRemoved && "code size not handled");

  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end())
    return 0;

  if (!isUncondBranchOpcode(I->getOpcode()) &&
      !isCondBranchOpcode(I->getOpcode()))
    return 0;

  // Remove the branch.
  I->eraseFromParent();

  I = MBB.end();

  if (I == MBB.begin()) {
    if (BytesRemoved)
      *BytesRemoved = 8;
    return 1;
  }

  --I;
  if (!isCondBranchOpcode(I->getOpcode())) {
    if (BytesRemoved)
      *BytesRemoved = 8;
    return 1;
  }

  // Remove the branch.
  I->eraseFromParent();
  if (BytesRemoved)
    *BytesRemoved = 16;

  return 2;
}

bool TBFInstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  switch (Cond[0].getImm()) {
  default:
    llvm_unreachable("Unknown conditional branch!");
  case TBF::JEQ_ri:
    Cond[0].setImm(TBF::JNE_ri);
    break;
  case TBF::JEQ_rr:
    Cond[0].setImm(TBF::JNE_rr);
    break;
  case TBF::JUGT_ri:
    Cond[0].setImm(TBF::JULE_ri);
    break;
  case TBF::JUGT_rr:
    Cond[0].setImm(TBF::JULE_rr);
    break;
  case TBF::JUGE_ri:
    Cond[0].setImm(TBF::JULT_ri);
    break;
  case TBF::JUGE_rr:
    Cond[0].setImm(TBF::JULT_rr);
    break;
  case TBF::JNE_ri:
    Cond[0].setImm(TBF::JEQ_ri);
    break;
  case TBF::JNE_rr:
    Cond[0].setImm(TBF::JEQ_rr);
    break;
  case TBF::JSGT_ri:
    Cond[0].setImm(TBF::JSLE_ri);
    break;
  case TBF::JSGT_rr:
    Cond[0].setImm(TBF::JSLE_rr);
    break;
  case TBF::JSGE_ri:
    Cond[0].setImm(TBF::JSLT_ri);
    break;
  case TBF::JSGE_rr:
    Cond[0].setImm(TBF::JSLT_rr);
    break;
  case TBF::JULT_ri:
    Cond[0].setImm(TBF::JUGE_ri);
    break;
  case TBF::JULT_rr:
    Cond[0].setImm(TBF::JUGE_rr);
    break;
  case TBF::JULE_ri:
    Cond[0].setImm(TBF::JUGT_ri);
    break;
  case TBF::JULE_rr:
    Cond[0].setImm(TBF::JUGT_rr);
    break;
  case TBF::JSLT_ri:
    Cond[0].setImm(TBF::JSGE_ri);
    break;
  case TBF::JSLT_rr:
    Cond[0].setImm(TBF::JSGE_rr);
    break;
  case TBF::JSLE_ri:
    Cond[0].setImm(TBF::JSGT_ri);
    break;
  case TBF::JSLE_rr:
    Cond[0].setImm(TBF::JSGT_rr);
    break;
  }

  return false;
}

MachineBasicBlock *
TBFInstrInfo::getBranchDestBlock(const MachineInstr &MI) const {
  unsigned Opcode = MI.getOpcode();
  if (Opcode == TBF::JMP) {
    return MI.getOperand(0).getMBB();
  }

  if (isCondBranchOpcode(Opcode)) {
    return MI.getOperand(2).getMBB();
  }

  llvm_unreachable("unexpected opcode!");
}

unsigned TBFInstrInfo::getInstSizeInBytes(const MachineInstr &MI) const {
  if (MI.getOpcode() == TBF::LD_imm64)
    return 16;

  return 8;
}

std::optional<RegImmPair> TBFInstrInfo::isAddImmediate(const MachineInstr &MI,
                                                       Register Reg) const {
  const MachineOperand &Op0 = MI.getOperand(0);
  if (!Op0.isReg() || Reg != Op0.getReg())
    return std::nullopt;

  if (!MI.getOperand(1).isReg() || !MI.getOperand(2).isImm())
    return std::nullopt;

  int Sign = 1;
  int64_t Offset = 0;
  unsigned Opcode = MI.getOpcode();
  switch (Opcode) {
  default:
    return std::nullopt;
  case TBF::SUB_ri:
  case TBF::SUB_ri_32:
    Sign *= -1;
    [[fallthrough]];
  case TBF::ADD_ri:
  case TBF::ADD_ri_32: {
    Offset = MI.getOperand(2).getImm() * Sign;
    return RegImmPair{MI.getOperand(1).getReg(), Offset};
  }
  }
}
