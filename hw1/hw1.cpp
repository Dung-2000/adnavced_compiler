#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

// #include <algorithm>
// #include <cmath>
// #include <llvm-17/llvm/ADT/StringRef.h>
// #include <llvm-17/llvm/Analysis/LoopInfo.h>
// #include <llvm-17/llvm/IR/DerivedTypes.h>
// #include <llvm-17/llvm/IR/InstrTypes.h>
// #include <llvm-17/llvm/IR/Instruction.h>
// #include <llvm-17/llvm/IR/Instructions.h>
// #include <llvm-17/llvm/Support/Casting.h>
// #include <llvm-17/llvm/Support/raw_ostream.h>

#include <iostream>
#include <string>
#include <vector>

using namespace llvm;

namespace {

struct HW1Pass : public PassInfoMixin<HW1Pass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  // std::map<int, std::string>  rmap, lmap;
private:
  std::vector<std::string > rIdxVec, lIdxVec, output, anti, flow;
  int iniVal=0, finiVal=0;
  std::string stepVarName;
  void ParseArrIdxContent(Instruction *idxprom_inst, std::string *ArrStr);

  void CheckOutput();
  void CheckFlowAnti();
  void paraParse(std::string s, int &a, int &c);
  int  exgcd(int a, int b, int &x, int &y);
  void DiophantineSolver(std::string lIdxVec, std::string rIdxVec, int &lb, int &ub, int i, int j);
  void OnlyAddorSub(std::string s1, std::string s2, int s1idx, int s2idx, bool isOutput);
};

PreservedAnalyses HW1Pass::run(Function &F, FunctionAnalysisManager &FAM) {
  
  errs() << "[HW1]: " << F.getName() << '\n';
  for(auto &BB : F)
  {
    StringRef BBName = BB.getName();
    // errs() << "\nBBName:\t" << BBName << '\n';
    // entry
    std::string Entry = "entry";
    if(Entry == BBName.str())
    {
      for (auto &Inst : BB) 
      {
        if(auto *allocaInst = dyn_cast<AllocaInst>(&Inst)) 
        {// check alloca instruction 
          Type *allcatedType = allocaInst->getAllocatedType();
          if(ArrayType *arrayType = dyn_cast<ArrayType>(allcatedType))
          {// check is arrayType or just pointerType
            StringRef Iname = allocaInst->getName();
            // errs() << "Var Name:\t" << Iname << "[" << arrayType->getNumElements() << "]" << '\n';
          }
        }
        if(auto *storeInst = dyn_cast<StoreInst>(&Inst))
        {// get var initial value
          // auto *iniCI = dyn_cast<ConstantInt>(storeInst->getValueOperand());
          APInt iniCIval = dyn_cast<ConstantInt>(storeInst->getValueOperand())->getValue();
          std::string iniCIstr = std::to_string(iniCIval.getSExtValue());
          StringRef vName = storeInst->getPointerOperand()->getName();
          // errs() << "var:\t" << vName.str() << "=" << iniCIstr <<'\n';
          iniVal = std::stoi(iniCIstr);
          stepVarName = vName.str();
        }
        
      }
    }
    // for.cond
    std::string Cond = "for.cond";
    if(Cond == BBName.str())
    {
      for (auto &Inst : BB)
      {  
        if(auto *icmpInst = dyn_cast<ICmpInst>(&Inst))
        {//get ICmpInst
          ConstantInt *finiInst;
          if(auto *tmp = dyn_cast<ConstantInt>(icmpInst->getOperand(0)))
            finiInst = tmp;
          else if(auto *tmp = dyn_cast<ConstantInt>(icmpInst->getOperand(1)))
            finiInst = tmp;
          // errs() << "fini value:\t" << finiInst->getValue() << '\n';// APIInt
          finiVal = finiInst->getSExtValue();
        }
      }
    }
    // for.body
    std::string Body = "for.body";
    if(Body == BBName.str())
    { 
      int sCount = 0;
      // std::map<char, std::string> Smap;

      for(BasicBlock::iterator I = BB.begin(), RE = BB.end();I != RE; I++)
      {
        if(auto *storeInst = dyn_cast<StoreInst>(&*I))
        {//get store inst
          // errs() << "assign Inst: " << *storeInst << '\n';
          //right
          auto *tmp_right_reg = dyn_cast<LoadInst>(storeInst->getValueOperand());
          auto *arrayidx_right = dyn_cast<GetElementPtrInst>(tmp_right_reg->getPointerOperand());
          std::string rArrStrVar = arrayidx_right->getPointerOperand()->getName().str();
          char rArrVar = rArrStrVar[0];
          auto *idxprom_inst = dyn_cast<Instruction>(arrayidx_right->getOperand(2));
          
          std::string idxStr="";
          ParseArrIdxContent(idxprom_inst, &idxStr);
          // errs() << "s" << sCount << ":" << rArrVar << "\tidx:" << idxStr << '\n';
          
          // Smap.insert(std::pair<char, std::string>(rArrVar, idxStr));
          // rmap.insert(std::pair<int, std::string>(sCount, rArrStrVar+idxStr));
          rIdxVec.push_back(rArrStrVar+idxStr);
          //left
          auto *arrayidx_left = dyn_cast<GetElementPtrInst>(storeInst->getPointerOperand());
          std::string lArrStrVar = arrayidx_left->getPointerOperand()->getName().str();
          char lArrVar = lArrStrVar[0];
          idxprom_inst = dyn_cast<Instruction>(arrayidx_left->getOperand(2));

          idxStr="";
          ParseArrIdxContent(idxprom_inst, &idxStr);
          // errs() << "s" << sCount << ":" << lArrVar << "\tidx:" << idxStr << '\n';

          // Smap.insert(std::pair<char, std::string>(lArrVar, idxStr));
          // rmap.insert(std::pair<int, std::string>(sCount, lArrStrVar+idxStr));
          lIdxVec.push_back(lArrStrVar+idxStr);
          sCount += 1;
        }
      }
    }
    // for.string
    std::string Inc = "for.inc";
    if(Inc == BBName.str())
    { 
      for (auto &Inst : BB)
      {  
        if(auto *storeInst = dyn_cast<StoreInst>(&Inst))
        {// from storeInst get stepValue and direction
          // errs() << "\tInst type:" << *Inst.getOperand(0) << '\n';
          auto *stepInst = dyn_cast<BinaryOperator>(storeInst->getValueOperand());
          auto *stepValue = dyn_cast<ConstantInt>(stepInst->getOperand(1));

          // errs() << "step var:\t" << storeInst->getPointerOperand()->getName() << '\n';
          // errs() << "step Direct:\t" << stepInst->getOpcodeName() << '\n';
          // errs() << "step value:\t" << stepValue->getValue() <<'\n';
        }
      }
    }
  }
  // check Vec
  CheckFlowAnti();
  CheckOutput();

  return PreservedAnalyses::all();
}

} // end anonymous namespace

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HW1Pass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "hw1") {
                    FPM.addPass(HW1Pass());
                    return true;
                  }
                  return false;
                });
          }};
}

void HW1Pass::ParseArrIdxContent(Instruction *idxprom_inst, std::string *idxStr)
{
  Instruction *tmpInst=idxprom_inst;
  std::string MUL = "mul";
  int Operandidx = 0;
  std::string tmpstr;
  while(!dyn_cast<LoadInst>(tmpInst->getOperand(Operandidx)))
  {
    tmpInst = dyn_cast<BinaryOperator>(tmpInst->getOperand(Operandidx));
    std::string OP = tmpInst->getOpcodeName();
    if(OP == "mul") OP = "*";
    if(OP == "add") OP = "+";
    if(OP == "sub") OP = "-";
    for(int i=0, tmpNumOP = tmpInst->getNumOperands(); i<tmpNumOP; i++)
    {
      if(auto *CI = dyn_cast<ConstantInt>(tmpInst->getOperand(i)))
      {
        tmpstr += std::to_string(CI->getValue().getSExtValue());
        Operandidx = 1-i;
        break;
      }
    }
    tmpstr += OP;
  }
  if(auto *idxInst = dyn_cast<LoadInst>(tmpInst->getOperand(Operandidx)))
  {
    std::string idxVar = idxInst->getPointerOperand()->getName().str();
    tmpstr += idxVar;
  }
  tmpstr += "[";
  //reverse idxstr
  for(int strlen=tmpstr.length()-1; strlen >= 0; strlen--)
  {
    *idxStr += tmpstr[strlen];
  }
  *idxStr += "]";
}

void HW1Pass::CheckFlowAnti()
{
  // errs() << "this is FlowAnti:\n";
  for(int i = 0; i < lIdxVec.size(); ++i)
  {
    for(int j = 0; j < rIdxVec.size(); ++j)
    {
      if(lIdxVec[i][0]==rIdxVec[j][0])
      {
        // errs() << lIdxVec[i] << "and" << rIdxVec[j] << '\n';
        if(lIdxVec[i].find('*') != std::string::npos || rIdxVec[j].find('*') != std::string::npos)
        {
          // errs() << "use equation\n";
          int lb, ub;

          // DiophantineSolver(lIdxVec[i], rIdxVec[j], lb, ub, i, j);

          int u=0, v=0, a=0, b=0, c1=0, c2=0;
          paraParse(lIdxVec[i], c1, a);
          paraParse(rIdxVec[j], c2, b);
          b = -b, c1 = c2 -c1;
          int g = exgcd(a, b, u, v);
          // errs() << "a: " << a << " b: " << b << " c1: " << c1 << " u: " << u << " v: " << v << " g: " << g << '\n';
          double x1 = (u*c1)/g, x2 = b/g, y1 = (v*c1)/g, y2 = -(a/g);
          if(x2>0 && y2>0)
          {
            int t1lb = std::ceil((iniVal-x1)/x2), t1ub = std::floor((finiVal-x1)/x2);
            int t2lb = std::ceil((iniVal-y1)/y2), t2ub = std::floor((finiVal-y1)/y2);
            lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
            // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
          }
          if(x2>0 && y2<0)
          {
            int t1lb = std::ceil((iniVal-x1)/x2), t1ub = std::floor((finiVal-x1)/x2);
            int t2ub = std::floor((iniVal-y1)/y2), t2lb = std::ceil((finiVal-y1)/y2);
            lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
            // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
          }
          if(x2<0 && y2>0)
          {
            int t1ub = std::floor((iniVal-x1)/x2), t1lb = std::ceil((finiVal-x1)/x2);
            int t2lb = std::ceil((iniVal-y1)/y2), t2ub = std::floor((finiVal-y1)/y2);
            lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
            // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
          }
          if(x2<0 && y2<0)
          {
            int t1ub = std::floor((iniVal-x1)/x2), t1lb = std::ceil((finiVal-x1)/x2);
            int t2ub = std::floor((iniVal-y1)/y2), t2lb = std::ceil((finiVal-y1)/y2);
            lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
            // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
          }

          int i1, i2;
          for(int t=lb; t<=ub; t++)
          {
            i1 = int(x1+x2*t), i2 = int(y1+y2*t);
            std::string res;
            if(i1<=i2)
            {
              res = "(i=" + std::to_string(i1) + ",i=" + std::to_string(i2) + ")\n";
              res.push_back(rIdxVec[j][0]);
              res += ":S" + std::to_string(i+1) + " ------> S" + std::to_string(j+1) + '\n';
              // errs() << "(i=" << i1 << ",i=" << i2 << ")\n";
              // errs() << rIdxVec[i][0] << ":S" << i+1 << " ------> S" << j+1 << '\n';
              flow.push_back(res);
            }
            else { 
              // errs() << "====Anti Dependency====\n";
              res = "(i=" + std::to_string(i2) + ",i=" + std::to_string(i1) + ")\n";
              res.push_back(rIdxVec[j][0]);
              res += ":S" + std::to_string(j+1) + " --A---> S" + std::to_string(i+1) + '\n';
              // errs() << "(i=" << i1 << ",i=" << i2 << ")\n";
              // errs() << rIdxVec[i][0] << ":S" << i+1 << " --A---> S" << j+1 << '\n';
              anti.push_back(res);
            }
            // errs() << int(x1+x2*t) << "   " << int(y1+y2*t) << '\n'; 
          }
        }
        else {
          OnlyAddorSub(lIdxVec[i], rIdxVec[j], i, j, false);
        }
      }
    }
  }

  // print to stdout
  errs() << "====Flow Dependency====\n";
  for( auto r : flow)
  {
    errs() << r;
  }
  errs() << "====Anti Dependency====\n";
  for( auto r : anti)
  {
    errs() << r;
  }
}

void HW1Pass::CheckOutput()
{
  // errs() << "this is Output:\n";
  for(int i = 0; i < lIdxVec.size(); ++i)
  {
    for(auto j = i+1; j < lIdxVec.size(); j++)
    {
      if(lIdxVec[i][0]==lIdxVec[j][0])
      {
        if(lIdxVec[i].find('*') != std::string::npos || lIdxVec[j].find('*') != std::string::npos)
        {
          int u=0, v=0, a=0, b=0, c1=0, c2=0;
          paraParse(lIdxVec[i], c1, a);
          paraParse(lIdxVec[j], c2, b);
          b = -b, c1 = c2 - c1;
          int g = exgcd(a, b, u, v);
          // errs() << "a: " << a << " b: " << b << " c1: " << c1 << " u: " << u << " v: " << v << " g: " << g << '\n';
          double x1 = (u*c1)/g, x2 = b/g, y1 = (v*c1)/g, y2 = -(a/g);
          int lb, ub;
          if(x2>0 && y2>0)
          {
            int t1lb = std::ceil((iniVal-x1)/x2), t1ub = std::floor((finiVal-x1)/x2);
            int t2lb = std::ceil((iniVal-y1)/y2), t2ub = std::floor((finiVal-y1)/y2);
            lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
            // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
          }
          if(x2>0 && y2<0)
          {
            int t1lb = std::ceil((iniVal-x1)/x2), t1ub = std::floor((finiVal-x1)/x2);
            int t2ub = std::floor((iniVal-y1)/y2), t2lb = std::ceil((finiVal-y1)/y2);
            lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
            // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
          }
          if(x2<0 && y2>0)
          {
            int t1ub = std::floor((iniVal-x1)/x2), t1lb = std::ceil((finiVal-x1)/x2);
            int t2lb = std::ceil((iniVal-y1)/y2), t2ub = std::floor((finiVal-y1)/y2);
            lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
            // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
          }
          if(x2<0 && y2<0)
          {
            int t1ub = std::floor((iniVal-x1)/x2), t1lb = std::ceil((finiVal-x1)/x2);
            int t2ub = std::floor((iniVal-y1)/y2), t2lb = std::ceil((finiVal-y1)/y2);
            lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
            errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
          }

          int i1, i2;
          for(int t=lb; t<=ub; t++)
          {
            i1 = int(x1+x2*t), i2 = int(y1+y2*t);
            std::string res;
            // errs() << "(i=" << i1 << ",i=" << i2 << ")\n";
            if(i1<finiVal && i2 <finiVal)
            {
              if(i1<=i2)
              {
                res = "(i=" + std::to_string(i1) + ",i=" + std::to_string(i2) + ")\n";
                res.push_back(lIdxVec[i][0]);
                res += ":S" + std::to_string(i+1) + " --o---> S" + std::to_string(j+1) + '\n';
                // errs() << "(i=" << i1 << ",i=" << i2 << ")\n";
                // errs() << rIdxVec[i][0] << ":S" << i+1 << " ------> S" << j+1 << '\n';
                output.push_back(res);
              }
              else { 
                // errs() << "====Anti Dependency====\n";
                res = "(i=" + std::to_string(i2) + ",i=" + std::to_string(i1) + ")\n";
                res.push_back(lIdxVec[i][0]);
                res += ":S" + std::to_string(j+1) + " --o---> S" + std::to_string(i+1) + '\n';
                // errs() << "(i=" << i1 << ",i=" << i2 << ")\n";
                // errs() << rIdxVec[i][0] << ":S" << i+1 << " --A---> S" << j+1 << '\n';
                output.push_back(res);
              }
            }
          }
        }
        else {  
          OnlyAddorSub(lIdxVec[i], lIdxVec[j], i, j, true);
        }

        
      }
    }
  }

  // print to stdout
  errs() << "====Output Dependency====\n";
  for( auto r : output)
  {
    errs() << r;
  }
}

void HW1Pass::OnlyAddorSub(std::string s1, std::string s2, int s1idx, int s2idx, bool isOutput)
{
  // errs() << "OnlyAddorSub\n";
  std::string deps;
  std::string res;
  std::string outS1;
  std::string outS2;
  //find Dependency distance
  int startpos = s1.find(stepVarName)+1, endpos = s1.find(']');
  auto idxStr = s1.substr(startpos, endpos-startpos);
  // errs() << "idxStr: " << idxStr << '\n';
  int s1idxVal = 0;
  if(!idxStr.empty()) s1idxVal = std::stoi(idxStr);

  startpos = s2.find(stepVarName)+1, endpos = s2.find(']');
  idxStr = s2.substr(startpos, endpos-startpos);
  // errs() << "idxStr: " << idxStr << '\n';
  int s2idxVal = 0;
  if(!idxStr.empty()) s2idxVal = std::stoi(idxStr);

  if(isOutput)
  {
    std::string deps = "--o--->";
    if(s1idxVal>s2idxVal) // Vec[i] --------> Vec[j]
      outS2 = ":S" + std::to_string(s1idx+1) + " " + deps +" S" + std::to_string(s2idx+1);
    else
      outS2 = ":S" + std::to_string(s2idx+1) + " " + deps +" S" + std::to_string(s1idx+1);
  }
  else {
    if(s1idxVal>s2idxVal) // Vec[i] --------> Vec[j]
    {
      deps = "------>";
      outS2 = ":S" + std::to_string(s1idx+1) + " " + deps +" S" + std::to_string(s2idx+1);
    } 
    else { // Vec[j] -------> Vec[i]
      deps = "--A--->";
      outS2 = ":S" + std::to_string(s2idx+1) + " " + deps +" S" + std::to_string(s1idx+1);
    }
  }

  for(int k=iniVal;k<finiVal;k++)
  {
    int i2 = k+s1idxVal-s2idxVal;
    if(i2 >= finiVal)
      break;
    outS1 = "(i=" + std::to_string(k) + ",i=" + std::to_string(i2) + ")\n";
    res = outS1;
    res.push_back(s1[0]);
    res += outS2;
    res.push_back('\n');
    if(isOutput){
      output.push_back(res);
    }
    else {
      if(deps == "--A--->")
      {
        anti.push_back(res);
      }
      else{
        flow.push_back(res);
      }
    }
    // errs() << outS1 << s1[0] << outS2 << '\n';
  }
}

int HW1Pass::exgcd(int a, int b, int &x, int &y)
{

  // errs() << "a: " << a << " b: " << b << " x: " << x << " y: " << y << '\n';
  if(b == 0)
  {
    x=1;
    y=0;
    return a;
  }
  int x1, y1;
  int g = exgcd(b, a%b, x1 ,y1);

  x = y1;
  y = x1 - (a/b) * y1;
  return g;
}

void HW1Pass::paraParse(std::string s, int &a, int &c)
{
  std::string idxStr = "";
  int cVal = 1, aVal = 0;

  int startpos = s.find('*'), endpos = s.find('+');
  bool aFlag = true;

  // errs() << "string:" << s << '\n';
  // idx = "[i*c+a]"
  if(startpos == std::string::npos)
  {// idx = [i +a],  c = 1
    if(endpos == std::string::npos)
    {// idx = "[i -a]" 
      endpos = s.find('-');
      if(endpos == std::string::npos)
      {// idx = "[i]"
        endpos = s.find(']');
        aFlag = false;
        // errs() << "idx = [i]\n";
      }
    }
  }
  else {// idx = "[i*c +a]"
    if(endpos == std::string::npos)
    {// idx == "[i*c -a]"
      endpos = s.find('-');
      if (endpos == std::string::npos)
      {// idx = "[i*c]"
        endpos = s.find(']');
        aFlag = false;
        // errs() << "idx = [i*c]\n";
      }
    } 
    idxStr = s.substr(startpos+1, endpos-startpos-1);
    cVal = std::stoi(idxStr);
    // errs() << "idxStr" << idxStr << '\n';
  }

  if(aFlag)
  {// case [i +a], [i*c +a]
    // errs() << "in aFlag\n";
    if(cVal != 1)
    {// case [i*c +a]
      startpos = s.find(idxStr); endpos = s.find(']');
      aVal = std::stoi(s.substr(startpos+1, endpos-startpos-1));
    }
    else {// case [i +a]
      startpos = s.find(stepVarName), endpos = s.find("]");
      aVal = std::stoi(s.substr(startpos+1, endpos-startpos-1));
    }
  }
  c = cVal, a = aVal;
  // errs() << "cVal:" << cVal << "aVal:" << aVal << '\n';
}

void HW1Pass::DiophantineSolver(std::string lIdxstr, std::string rIdxstr, int &lb, int &ub, int i, int j)
{

  int u=0, v=0, a=0, b=0, c1=0, c2=0;
  paraParse(lIdxstr, c1, a);
  paraParse(lIdxstr, c2, b);
  b = -b, c1 = c2 - c1;
  int g = exgcd(a, b, u, v);
  // errs() << "a: " << a << " b: " << b << " c1: " << c1 << " u: " << u << " v: " << v << " g: " << g << '\n';
  double x1 = (u*c1)/g, x2 = b/g, y1 = (v*c1)/g, y2 = -(a/g);
  // int lb, ub;
  if(x2>0 && y2>0)
  {
    int t1lb = std::ceil((iniVal-x1)/x2), t1ub = std::floor((finiVal-x1)/x2);
    int t2lb = std::ceil((iniVal-y1)/y2), t2ub = std::floor((finiVal-y1)/y2);
    lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
    // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
  }
  if(x2>0 && y2<0)
  {
    int t1lb = std::ceil((iniVal-x1)/x2), t1ub = std::floor((finiVal-x1)/x2);
    int t2ub = std::floor((iniVal-y1)/y2), t2lb = std::ceil((finiVal-y1)/y2);
    lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
    // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
  }
  if(x2<0 && y2>0)
  {
    int t1ub = std::floor((iniVal-x1)/x2), t1lb = std::ceil((finiVal-x1)/x2);
    int t2lb = std::ceil((iniVal-y1)/y2), t2ub = std::floor((finiVal-y1)/y2);
    lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
    // errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
  }
  if(x2<0 && y2<0)
  {
    int t1ub = std::floor((iniVal-x1)/x2), t1lb = std::ceil((finiVal-x1)/x2);
    int t2ub = std::floor((iniVal-y1)/y2), t2lb = std::ceil((finiVal-y1)/y2);
    lb=std::max(t1lb,t2lb) ,ub=std::min(t1ub,t2ub);
    errs() << "t1lb:" << t1lb << " t2lb:" << t2lb << " t1ub:" << t1ub << " t2ub:" << t2ub << '\n';
  }

  int i1, i2;
  for(int t=lb; t<=ub; t++)
  {
    i1 = int(x1+x2*t), i2 = int(y1+y2*t);
    std::string res;
    // errs() << "(i=" << i1 << ",i=" << i2 << ")\n";
    if(i1<finiVal && i2 <finiVal)
    {
      if(i1<=i2)
      {
        res = "(i=" + std::to_string(i1) + ",i=" + std::to_string(i2) + ")\n";
        res.push_back(lIdxstr[0]);
        res += ":S" + std::to_string(i+1) + " --o---> S" + std::to_string(j+1) + '\n';
        // errs() << "(i=" << i1 << ",i=" << i2 << ")\n";
        // errs() << rIdxVec[i][0] << ":S" << i+1 << " ------> S" << j+1 << '\n';
        output.push_back(res);
      }
      else { 
        // errs() << "====Anti Dependency====\n";
        res = "(i=" + std::to_string(i2) + ",i=" + std::to_string(i1) + ")\n";
        res.push_back(lIdxstr[0]);
        res += ":S" + std::to_string(j+1) + " --o---> S" + std::to_string(i+1) + '\n';
        // errs() << "(i=" << i1 << ",i=" << i2 << ")\n";
        // errs() << rIdxVec[i][0] << ":S" << i+1 << " --A---> S" << j+1 << '\n';
        output.push_back(res);
      }
    }
  }

}
