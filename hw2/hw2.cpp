#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"


#include <llvm-17/llvm/IR/Constants.h>
#include <llvm-17/llvm/IR/Instruction.h>
#include <llvm-17/llvm/IR/Instructions.h>
#include <llvm-17/llvm/Support/Casting.h>
#include <llvm-17/llvm/Support/raw_ostream.h>

using namespace llvm;

#ifdef DEBUG
  #define DBG_PRINT(fmt, ...) \
    errs() << "DEBUG: "<< fmt << __VA_ARGS__ << '\n';
  #define DBG_PRINTM(fmt) \
    errs() << "DEBUG: "<< fmt << '\n';
  
#else 
  #define DBG_PRINT(fmt, ...) \
    do {} while(0)
  #define DBG_PRINTM(fmt) \
    do {} while(0)

#endif


namespace {

enum Opidx
{
  rhs=0, lhs
};


struct HW2Pass : public PassInfoMixin<HW2Pass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

  // void ParseBinOP(Instruction *tmpInst, std::string &tmpstr);
  void Parse(Instruction *tmpInst, std::string &tmpstr, int OpIdx);
  void TypeCheck(Type &type, std::string &tmpStr);
  #ifdef DEBUG
    int strCount;
    bool isPointerToPointer(const Type *T);
  #endif
};

PreservedAnalyses HW2Pass::run(Function &F, FunctionAnalysisManager &FAM) {
  errs() << "[HW2]: " << F.getName() << '\n';

  for(auto &BB : F)
  {
    for (auto &Inst : BB)
    { 
      if(auto *decInst = dyn_cast<AllocaInst>(&Inst)) 
      {// get declear info
        errs() << '\n';
        DBG_PRINTM("declare part");
        DBG_PRINT("decInst", *decInst);
        auto *VarType = decInst->getAllocatedType();
        auto VarName = decInst->getName();
        DBG_PRINT("VarType:", *VarType);
        DBG_PRINT("VarName:", VarName);
      #ifdef DEBUG
        // const Type* T = decInst->getType();
        // isPointerToPointer(T);
      #endif
      }
      else if(auto *StInst = dyn_cast<StoreInst>(&Inst))
      {//get StoreInst equal to 1 statement
        // lhs
        errs() << '\n';
        DBG_PRINT("store part:", strCount);
      #ifdef DEBUG
        strCount++;
      #endif
        errs() << *StInst->getValueOperand()->getType() <<" level"<< '\n';
        DBG_PRINTM("lhs");
        std::string tmpStr = "";
        Parse(StInst, tmpStr, lhs);

        // rhs 
        DBG_PRINTM("rhs");
        tmpStr += "=";
        Parse(StInst, tmpStr, rhs);
        errs() << "tmpstr:" << tmpStr << "\n";

      }
    }
  }


  return PreservedAnalyses::all();
}

#ifdef DEBUG
  bool HW2Pass::isPointerToPointer(const Type* T)
  {
    return T->isPointerTy() && T->getContainedType(0)->isPointerTy();
  }
#endif

void HW2Pass::TypeCheck(Type &type, std::string &tmpStr)
{
  // errs() << type << '\n';
  if(type.isPointerTy())
  {
    errs() << "ptr\n";
    // tmpStr += '*';
  }
  if(type.isIntegerTy())
  {
    errs() << "i32\n";
  }
}

void HW2Pass::Parse(Instruction *Inst, std::string &tmpStr, int OpIdx)
{
  Instruction *tmpInst = Inst; 
  DBG_PRINT("processing",*tmpInst);

  while (true)
  {
    // two end case alloca or constint
    if(dyn_cast<AllocaInst>(tmpInst->getOperand(OpIdx)))
    {
      auto AllocInst = dyn_cast<AllocaInst>(tmpInst->getOperand(OpIdx));
      DBG_PRINT("in Alloc CASE:", *AllocInst); 
      DBG_PRINT("alloc var name:", AllocInst->getName());
      DBG_PRINT("alloc var type:", *AllocInst->getAllocatedType());
      tmpStr += AllocInst->getName().str();
      break;
    }
    if(dyn_cast<ConstantInt>(tmpInst->getOperand(OpIdx)))
    {
      auto CI = dyn_cast<ConstantInt>(tmpInst->getOperand(OpIdx));
      DBG_PRINT("ConstantInt val:", CI->getValue());
      tmpStr += std::to_string(CI->getValue().getSExtValue());
      break;
    }

    //load case
    if(dyn_cast<LoadInst>(tmpInst->getOperand(OpIdx)))
    {
      tmpInst = dyn_cast<LoadInst>(tmpInst->getOperand(OpIdx));
      DBG_PRINT("in Load CASE:", *tmpInst);
      TypeCheck(*tmpInst->getType(), tmpStr);
      OpIdx = 0;
    }
    //add sub mut case
    if(dyn_cast<BinaryOperator>(tmpInst->getOperand(OpIdx)))
    {
      tmpInst = dyn_cast<BinaryOperator>(tmpInst->getOperand(OpIdx));
      DBG_PRINT("in BinOP CASE: first op inst", *tmpInst);

      std::string OpName = tmpInst->getOpcodeName();
      std::string Op = "";
      if(OpName == "add")
        Op = "+";
      else if(OpName == "sub")
        Op = "-";
      else if (OpName == "mul")
        Op = "*";

      DBG_PRINT("tmpInst", *tmpInst);
      Parse(tmpInst, tmpStr, OpIdx);
      tmpStr += Op; 
      OpIdx = 1;
      // break;
    }
  }
}

// void HW2Pass::ParseBinOP(Instruction *tmpInst, std::string &tmpstr)
// {
//   int OpIdx=0;

//   while (true)
//   {
//     errs() << "In BinParse Process:" << *tmpInst << '\n';
//     // two end case alloca or constint
//     if(dyn_cast<AllocaInst>(tmpInst->getOperand(OpIdx)))
//     {
//       tmpInst = dyn_cast<AllocaInst>(tmpInst->getOperand(OpIdx));
//       tmpstr += tmpInst->getName().str();
//       // errs() << tmpInst->getName().str() << '\n';
//       break;
//     }
//     if(dyn_cast<ConstantInt>(tmpInst->getOperand(OpIdx)))
//     {
//       auto CI = dyn_cast<ConstantInt>(tmpInst->getOperand(OpIdx));
//       tmpstr += std::to_string(CI->getValue().getSExtValue());
//       // errs() << *CI << '\n';
//       break;
//     }

//     if(dyn_cast<LoadInst>(tmpInst->getOperand(OpIdx)))
//     {
//       tmpInst = dyn_cast<LoadInst>(tmpInst->getOperand(OpIdx));
//       // TypeCheck(*tmpInst->getType());
//       errs() << *tmpInst << '\n';
//       OpIdx = 0;
//     }

//     if(dyn_cast<BinaryOperator>(tmpInst->getOperand(OpIdx)))
//     {
//       tmpInst = dyn_cast<BinaryOperator>(tmpInst->getOperand(OpIdx));
//       
//       std::string OpName = tmpInst->getOpcodeName();
//       std::string Op = "";
//       if(OpName == "add")
//         Op = "+";
//       else if(OpName == "sub")
//         Op = "-";
//       else if (OpName == "mul")
//         Op = "*";

//       errs() << *tmpInst << '\n';
//       ParseBinOP(tmpInst, tmpstr);
//       tmpstr += Op;
//       OpIdx = 1;
//     }
//   }
// }

} // end anonymous namespace

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() 
{
  return {
    LLVM_PLUGIN_API_VERSION, "HW2Pass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) 
            {
              PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) 
                  {
                    if (Name == "hw2") {
                      FPM.addPass(HW2Pass());
                      return true;
                    }
                    return false;
                  });
            }};
}
