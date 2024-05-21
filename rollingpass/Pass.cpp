#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/AbstractCallSite.h"

#include "llvm/IR/Function.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/DataLayout.h"
#include <unordered_map>
#include <utility>




#include  <iostream>

using namespace llvm;



  


namespace {

// Function to compute the difference between two vectors
SmallVector<APInt> vectorDifference(llvm::SmallVector<APInt> &vec1, llvm::SmallVector<APInt> &vec2) {
    if (vec1.size() != vec2.size()) {
    }

    SmallVector<APInt> difference(vec1.size());
    for (size_t i = 0; i < vec1.size(); ++i) {
        difference[i] = vec1[i] - vec2[i];
    }

    return difference;
}

bool isEqual(std::vector<APInt> &vec1){
  APInt currentValue = vec1[0];
        bool Equal = true; 
        for(auto d : vec1) {
          if (currentValue != d){
            errs () << "IsEqual currentvalue and d: " << currentValue << " | "<< d << "\n";
            Equal = false;
            break;
          }
        }
        return Equal;
}


class LoopParams{
  public:
    APInt initial, end, stride;
};



class Node{
  public:
    std::vector<Instruction *> children;
    APInt stride;
    SmallVector<APInt> strideVec;

    //For store
    APInt start;
    APInt end;

    Node(Instruction*  I1,Instruction*  I2){
      Value* offsetV1 = I1 -> getOperand(2);
      ConstantInt* offsetInt1 = dyn_cast<ConstantInt>(offsetV1);
      APInt offset1 = offsetInt1->getValue();


      Value* offsetV2 = I2 -> getOperand(2);
      ConstantInt* offsetInt2 = dyn_cast<ConstantInt>(offsetV2);
      APInt offset2 = offsetInt2->getValue();
      stride = offset2 - offset1;
      errs()<< "Stride: "<<stride << "\n";

      children.push_back(I1);
      children.push_back(I2);

      start = offset1;
      end = offset2;
    }

    Node(Instruction* I1){
      children.push_back(I1);
     
    }



    Instruction* getLast(){
      return children.back();
    }

    int getSize(){
      return children.size();
    }

    SmallVector<APInt> getStride(){
      return strideVec;
    }

    APInt getStart(){
      Value* offsetV1 = children[0];
      ConstantInt* offsetInt1 = dyn_cast<ConstantInt>(offsetV1);
      APInt offset1 = offsetInt1->getValue();

      start = offset1;
      return start;
    }

     APInt getEnd(){
      Value* offsetV1 = children[getSize()-1];
      ConstantInt* offsetInt1 = dyn_cast<ConstantInt>(offsetV1);
      APInt offset1 = offsetInt1->getValue();

      end = offset1;
      return end;
    }

    BasicBlock* getParentBB(){
      return children[0]->getParent();
    }

    bool compareStrideAndAdd(Instruction* I2){

      Value* offsetV1 = getLast() -> getOperand(2);
      ConstantInt* offsetInt1 = dyn_cast<ConstantInt>(offsetV1);
      APInt offset1 = offsetInt1->getValue();

      Value* offsetV2 = I2 -> getOperand(2);
      ConstantInt* offsetInt2 = dyn_cast<ConstantInt>(offsetV2);
      APInt offset2 = offsetInt2->getValue();
      APInt newStride = offset2 - offset1;
      errs()<< "newStride: "<<newStride << "\n";

      if((newStride ==  stride) && getParentBB() == I2 -> getParent()){
        children.push_back(I2);
        errs()<<"New node added\n";
        end = offset2;
        return true;
      }else{
        return false;
      }
    }

    void add_inst(Instruction* I1){
      children.push_back(I1);
    }

    bool isRollable(){
      if(getSize()>2) return true;
      else return false;
    }

};






class Roll{
  public:
    struct ArgumentInstrPair {
    std::vector<Value*> Args;
    Instruction* Instr;

    ArgumentInstrPair(const std::vector<Value*>& args, Instruction* instr)
        : Args(args), Instr(instr) {}
};

    // private:
    std::unordered_map<Value *, std::vector<Instruction *> > Stores; 
    std::unordered_map<Value *, std::vector<Instruction *> > GEPs; // add GEP instructions to map
    std::unordered_map<std::string  , std::vector<ArgumentInstrPair>> FunctionCalls; 

    using StrideLengthPair = std::pair<APInt, int>;
    using StrideLengthVector = std::vector<StrideLengthPair>;
    std::unordered_map<llvm::Value*, StrideLengthVector> BaseAddressMap;
    std::vector<Node *> funcNodes,StoreNodes;
  



  void findStride(char rollType) {
    // store: s
    // function calls: f

    switch (rollType) {
      case 's':

      Value* stride;

      errs()<<"Finding  Strides -------"<< "\n";
       for(auto&  tempGEPsVec : GEPs){
     
      int sequenceCounter = 2; 
      Value* offsetV1 = tempGEPsVec.second[0] -> getOperand(tempGEPsVec.second[0] ->getNumOperands() -1);
      ConstantInt* offsetInt1 = dyn_cast<ConstantInt>(offsetV1);
      APInt offset1 = offsetInt1->getValue();


      Value* offsetV2 = tempGEPsVec.second[1] -> getOperand(tempGEPsVec.second[1] ->getNumOperands() -1);
      ConstantInt* offsetInt2 = dyn_cast<ConstantInt>(offsetV2);
      APInt offset2 = offsetInt2->getValue();

      APInt previousDiff = offset2 - offset1;
      APInt currentDifference;
      errs()<< "Previous Diff: "<<previousDiff << "\n";

      if(tempGEPsVec.second[0] -> getParent() == tempGEPsVec.second[1] -> getParent()){

      StoreNodes.push_back(new Node(tempGEPsVec.second[0],tempGEPsVec.second[1]));

      int k=0;
      

      for(int i=1; i<tempGEPsVec.second.size();i++){
                
        Instruction* tempInst = tempGEPsVec.second[i];
        errs()<<"Node size: "<<StoreNodes.size()<<"\n";

        if(!(StoreNodes[StoreNodes.size()-1] -> compareStrideAndAdd(tempInst))){
          if(tempGEPsVec.second[i-1] -> getParent() == tempGEPsVec.second[i] -> getParent()){
            StoreNodes.push_back(new Node(tempGEPsVec.second[i-1],tempGEPsVec.second[i]));
            errs()<<"New node added\n";
          }else{
            break;
          }
        }

      }

      }
    }
    
    break; 



      //NEW STRIDE FUNCTION FOR MULTI ARGUMENTS
      case 'f':

      

      for(auto fnames : FunctionCalls) { // iterating over keys
      errs() << "Function Name: " << fnames.first<< "\n";

      int iter_nodes=0;

      // iterate over fnames.second starting from i=0
      for (int i=0; i< fnames.second.size()-1; i++){ 
        int pushInstrFlag = 1; 
        errs() << "The value of i is now: " << i << "\n";
        int sequenceArgCounter = 2; 

        llvm::SmallVector<APInt> previousFnDiff;
        llvm::SmallVector<APInt> currentFnDiff;
        llvm::SmallVector<APInt> previousFnVec;
        llvm::SmallVector<APInt> currentFnVec;

        int argVecSize = fnames.second[0].Args.size(); 
        
        for (int j=0; j< argVecSize; j++) {
          Value* fnVec= fnames.second[i].Args[j];
          ConstantInt* fnVecConst = dyn_cast<ConstantInt>(fnVec);
          if (fnVecConst) { 
            APInt FnVecArg = fnVecConst->getValue();
            previousFnVec.push_back(FnVecArg); 
          } 
          else {
            errs() << "Argument is not a ConstantInt.\n";
          }
        }

       

        for (int j=0; j< argVecSize; j++) {
          Value* fnVec1= fnames.second[i+1].Args[j];
          errs() << "fnVec1: " << *fnVec1 << "\n";
          ConstantInt* fnVecConst1 = dyn_cast<ConstantInt>(fnVec1);
          if (fnVecConst1) { 
            APInt FnVecArg1 = fnVecConst1->getValue();
            currentFnVec.push_back(FnVecArg1); 
          } 
          else {
            errs() << "Argument is not a ConstantInt.\n";
          } 
        }
      
        previousFnDiff = vectorDifference(currentFnVec,previousFnVec);
        bool strideVecEqual = true;
        strideVecEqual = true; 

        if(!strideVecEqual) continue; 
        previousFnVec = currentFnVec;

        for (int j=i+2; j< fnames.second.size(); j++) { 
          
          llvm::SmallVector<APInt> currentFnVecTemp;
          for (int k=0; k< fnames.second[j].Args.size(); k++){
          
          Value* CurFnDiffVec= fnames.second[j].Args[k];
          ConstantInt* CurFnDiffVecConst = dyn_cast<ConstantInt>(CurFnDiffVec);
          if (CurFnDiffVecConst) { 
            APInt CurFnDiffVecFinal = CurFnDiffVecConst->getValue();
            currentFnVecTemp.push_back(CurFnDiffVecFinal);
          } 
          else {
            errs() << "Argument is not a ConstantInt.\n";
          }           

          }
          currentFnVec = currentFnVecTemp;
          currentFnDiff = vectorDifference(currentFnVec,previousFnVec);
          previousFnVec = currentFnVec; 

          

          
          for (int t=0; t< currentFnDiff.size(); t++){
            if (currentFnDiff[t] != previousFnDiff[t]) {
            strideVecEqual = false; 
            break;
          }
          else{
            previousFnDiff = currentFnDiff; 
            previousFnVec = currentFnVec;
            for(auto& v: previousFnDiff) {
              errs() << "Previous Fn Diff: "<< v << "\n";
            }
          }
          }
          if(!strideVecEqual) {
            break;
          }
          
          if (strideVecEqual != false){
            sequenceArgCounter++; 
            if (pushInstrFlag){
              errs() << "Instruction 0: " << *(fnames.second[i].Instr)<< "\n";
              errs() << "Instruction 1: " << *(fnames.second[i+1].Instr)<< "\n";
              funcNodes.push_back(new Node(fnames.second[i].Instr));
              funcNodes[funcNodes.size()-1]->add_inst(fnames.second[i+1].Instr);
              funcNodes[funcNodes.size()-1]->strideVec = previousFnDiff;

              pushInstrFlag = 0;
              }
            errs() << " Sequence length: " << sequenceArgCounter << "\n";
            errs() << "Instruction after: " << *(fnames.second[j].Instr)<< "\n";
            funcNodes[funcNodes.size()-1]->add_inst(fnames.second[j].Instr);
          }
         i = j; 
        }
        errs()<<"i: " << i<< "\n";        
      }
      
      
        
      


        for(Node* node: funcNodes){
          errs()<<"Sizes: "<<node ->getSize()<<"\n";
          errs()<<"isRollable: "<<node -> isRollable()<<"\n";
          for(Instruction* I:node -> children){
            errs()<<*I<<"\n";
          }
          for(auto s:node->strideVec){
            errs()<<s<<"\n";
          }
        }
        
        }
        break;

        default:
          errs() << "Something";

        break; 

        }
    }
  
 





  // Collect Seed Instructions
  void collectSeedInstructions(Function &F) {
  
  for (BasicBlock &BB : F){ 
    for (Instruction &I : BB) {
      Instruction* tempInst = &I;
      LLVMContext &context = F.getContext(); 
      IRBuilder<> builder(context); 

      if (auto *PI = dyn_cast<GetElementPtrInst>(&I)) {
        
        Value* offsetV = *(PI->op_end() - 1);
        auto &GEPs_Map = GEPs[getUnderlyingObject(PI->getPointerOperand())];
        GEPs_Map.push_back(PI);
        
        ConstantInt* offsetInt = dyn_cast<ConstantInt>(offsetV);
        if (offsetInt != nullptr) {
          APInt offset = offsetInt->getValue();
    
        } 
        else {
          continue;
        }
      }



      if (auto *CI = dyn_cast<CallInst>(&I)){ // if instruction is a function call 
        Function *Callee = CI->getCalledFunction(); 
        //Calls[Callee].push_back(tempInst);
        errs() << "Callee: "<<Callee->getName() << "\n";

        std::vector<Value *> Args;  

            for (Use &Arg : CI->args()) {
                Value *ArgVal = Arg.get();
                Args.push_back(ArgVal);
                errs() << "Arg: " << *ArgVal << "\n";
                
            }
            ArgumentInstrPair argInst(Args,CI);
            FunctionCalls[Callee->getName().str()].push_back(argInst);
        errs() << "Callee: "<<Callee->getName() << "\n";

    
      }

      
    }

  }


  }


 void generateLoop(BasicBlock &entryBB, Function &F,Node* currentNode){
    if(currentNode -> getSize() > 2){
    errs()<<"\n";
    errs()<<*(currentNode->getParentBB()->getTerminator())<<"\n";
    LLVMContext &context = F.getContext();
    IRBuilder<> builder(context);
    Instruction *entryTerminator = currentNode->getParentBB()->getTerminator();
    BasicBlock* entryBB1 = currentNode->getParentBB();
    Instruction *cloneTerminator = entryTerminator->clone();

    auto& gepGroupVec = currentNode -> children;
  
      errs()<<"Sizes: "<<currentNode -> getSize()<<"\n";

      BasicBlock *exitBB = BasicBlock::Create(context, "exit", &F);

      builder.SetInsertPoint(exitBB);
      builder.Insert(cloneTerminator);

  
      entryBB1 ->getTerminator() -> eraseFromParent();
      builder.SetInsertPoint(entryBB1);
      builder.CreateBr(exitBB);


      builder.SetInsertPoint(entryBB1->getTerminator());
      Type* type_32 = Type::getInt32Ty(context);


      APInt start = currentNode -> start;
      APInt end = currentNode -> end;
      APInt strideI = currentNode -> stride;

      Value* val1 = ConstantInt::get(builder.getInt32Ty(), start.trunc(32));
      Value* val2 = ConstantInt::get(builder.getInt32Ty(), end.trunc(32)); // add sequence length
      Value* stride = ConstantInt::get(builder.getInt32Ty(), strideI.trunc(32)); // add stride 

      AllocaInst* alloca = builder.CreateAlloca(builder.getInt32Ty(), 0, nullptr, "indexVar");
      StoreInst* new_store = builder.CreateStore(val1, alloca, false);


      BasicBlock* loopHeaderBB = BasicBlock::Create(context, "loopHeader", &F);
      builder.SetInsertPoint(loopHeaderBB);
      LoadInst* new_load = builder.CreateLoad(type_32,alloca,false,"");
  

      builder.SetInsertPoint(entryBB1);
      
      entryBB1 -> getTerminator() -> eraseFromParent();
      builder.CreateBr(loopHeaderBB);

      BasicBlock *loopBodyBB = BasicBlock::Create(context, "loopBody", &F);
      builder.SetInsertPoint(loopHeaderBB);
      Value* icmpResult = builder.CreateICmpSLT(new_load, val2, "icmp_result");
      builder.CreateCondBr(icmpResult, loopBodyBB, exitBB);


      builder.SetInsertPoint(loopBodyBB);
      LoadInst* loadIndexInst = builder.CreateLoad(type_32,alloca,false,"");
      Value* sextInst = builder.CreateSExt(loadIndexInst,builder.getInt64Ty(),"signex");

      bool insertedGep =false;
      StoreInst* seedStoreInst;
      for(auto& v: gepGroupVec) {
        if(!insertedGep){
          // add to 

          errs()<<"added GEP\n";
          errs()<<*v<<"\n";
          

          for (auto U:v->users()){
            if (auto I = dyn_cast<Instruction>(U) ){
              seedStoreInst = dyn_cast<StoreInst>(I -> clone());
              builder.Insert(seedStoreInst); 
            }
          }
          
          Instruction* seedGepInst =  v -> clone();
          seedGepInst->insertBefore(seedStoreInst);
          seedStoreInst -> setOperand(1,seedGepInst);
          insertedGep = true;   
        }

        for (auto U:v->users()){
          if (auto I = dyn_cast<Instruction>(U) ){
            I -> eraseFromParent();
          }
        }
        
        v->eraseFromParent();

      }

    
      

      
      BasicBlock *loopExitBB = BasicBlock::Create(context, "loopExit", &F);
      builder.CreateBr(loopExitBB);
      builder.SetInsertPoint(loopExitBB);
      LoadInst* exitLoadInst =  builder.CreateLoad(type_32,alloca,false,"");
      Value* nextIt = builder.CreateNSWAdd(exitLoadInst, stride,"");
      StoreInst* new_store1 = builder.CreateStore(nextIt, alloca, false);
      builder.CreateBr(loopHeaderBB);

    }
  }

  void generateLoopFn(BasicBlock &entryBB, Function &F,Node* currentNode){
     
    LLVMContext &context = F.getContext();
    IRBuilder<> builder(context);
    Instruction *entryTerminator = entryBB.getTerminator();

    Instruction *cloneTerminator = entryTerminator->clone();

    auto& callGroupVec = currentNode -> children;
    SmallVector<Value *> argBegin;
    SmallVector<AllocaInst *> argAlloca;
    SmallVector<StoreInst *> argStores;
    SmallVector<StoreInst *> argLoads;
    
    
    
    if(currentNode -> getSize() > 2){
      errs()<<"Sizes: "<<currentNode -> getSize()<<"\n";

      BasicBlock *exitBB = BasicBlock::Create(context, "exit", &F);

      builder.SetInsertPoint(exitBB);
      builder.Insert(cloneTerminator);

  
      entryBB.getTerminator() -> eraseFromParent();

      builder.SetInsertPoint(&entryBB);
      Type* type_32 = Type::getInt32Ty(context);

      CallInst* seedCall = dyn_cast<CallInst>(callGroupVec[0]);
      errs()<<*seedCall<<"\n";

      CallInst* lastCall = dyn_cast<CallInst>(callGroupVec[callGroupVec.size()-1]);
      errs()<<*lastCall<<"\n";

      Value *ArgVal;
      for (Use &Arg : seedCall->args()) {
        argBegin.push_back(Arg.get());
        
      }

      Value *endArgVal;
      for (Use &Arg : lastCall->args()) {
        endArgVal = Arg.get();
        errs() << "Arg: " << *Arg<< *endArgVal << "\n";
        break;
      }

      APInt start = currentNode -> start;
      APInt end = currentNode -> end;
      auto& strideVec = currentNode -> strideVec;
      APInt strideI = strideVec[0];


      Value* val1 = argBegin[0];
      Value* val2 = endArgVal;
      Value* stride = ConstantInt::get(builder.getInt32Ty(), strideI.trunc(32)); // add stride 

      AllocaInst* alloca = builder.CreateAlloca(type_32, 0, nullptr, "indexVar");
      StoreInst* new_store = builder.CreateStore(val1, alloca, false);  

      
      if(argBegin.size()>1){
        for(int i=1;i<argBegin.size();i++){
          argAlloca.push_back(builder.CreateAlloca(type_32, 0, nullptr, "indexVarp"));
          argStores.push_back(builder.CreateStore(argBegin[i], argAlloca[i-1], false));
        }
      }
      
      
      // Start of loop body
      LoadInst* new_load = builder.CreateLoad(type_32,alloca,false,"");
      BasicBlock* loopHeaderBB = entryBB.splitBasicBlock(new_load,"loopHeader",false);


      BasicBlock *loopBodyBB = BasicBlock::Create(context, "loopBody", &F);
      builder.SetInsertPoint(loopHeaderBB);
      Value* icmpResult = builder.CreateICmpSLT(new_load, val2, "icmp_result");
      builder.CreateCondBr(icmpResult, loopBodyBB, exitBB);


      builder.SetInsertPoint(loopBodyBB);
      LoadInst* loadIndexInst = builder.CreateLoad(type_32,alloca,false,"");


      bool insertedCall =false;
      for(auto& v: callGroupVec) {
        errs()<<*v<<"\n";
        if(!insertedCall){

          Instruction* seedCallInst =  v -> clone();

          seedCallInst -> setOperand(0,loadIndexInst);
          if(argAlloca.size()>0){
            for(int i=0;i<argAlloca.size();i++){
              LoadInst* ld = builder.CreateLoad(type_32,argAlloca[i],false,"");
              seedCallInst -> setOperand(i+1,ld);
            }
          }
          builder.Insert(seedCallInst);
          insertedCall = true;   
        }
        v->eraseFromParent();
      }

    
      

      
      BasicBlock *loopExitBB = BasicBlock::Create(context, "loopExit", &F);
      builder.CreateBr(loopExitBB);
      builder.SetInsertPoint(loopExitBB);
      LoadInst* exitLoadInst =  builder.CreateLoad(type_32,alloca,false,"");
      Value* nextIt = builder.CreateNSWAdd(exitLoadInst, stride,"");
      StoreInst* new_store1 = builder.CreateStore(nextIt, alloca, false);

      if(argAlloca.size()>0){
        for(int i=0;i<argAlloca.size();i++){
          errs()<<"created add in exit\n";
          Value* strideIt = ConstantInt::get(builder.getInt32Ty(), strideVec[i+1].trunc(32));
          LoadInst* eLoadInst =  builder.CreateLoad(type_32,argAlloca[i],false,"");
          Value* eIt = builder.CreateNSWAdd(eLoadInst,strideIt ,"");
          StoreInst* estore = builder.CreateStore(eIt, argAlloca[i], false);
        }
      }
      builder.CreateBr(loopHeaderBB);

    }
  }


  void rollLoop(Function &F) {

    BasicBlock &entryBB = F.getEntryBlock();

    for(auto& N:StoreNodes){

      generateLoop(entryBB,  F, N);
    }

    for(auto& N:funcNodes){
      generateLoopFn(entryBB,  F, N);
    }
    
    
    

    
 
  }   

};


struct RollingPass : public PassInfoMixin<RollingPass> {

PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    Roll R;
    R.collectSeedInstructions(F);
    R.findStride('s');
    R.findStride('f');
    R.rollLoop(F);

    return PreservedAnalyses::all();
  }
};
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "RollingPass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
        ArrayRef<PassBuilder::PipelineElement>) {
          if(Name == "loop-roll"){
            FPM.addPass(RollingPass());
            return true;
          }
          return false;
      }
      );
    }
  };
}
