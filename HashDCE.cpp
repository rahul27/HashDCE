//===- HashDCE.cpp - Dead Code Elimination using Hashing -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements the technique presented in "A New Technique for Copy
// Propagation And Dead Code Elimination Using Hash Based Value Numbering"
// by Dr. K.V.N.Sunitha and Dr V. Vijaya Kumar
//
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "hashdce"
#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include <map>
#include <queue>
#include <stack>

using namespace llvm;
using namespace std;


// States for CP_Table restore on encountering brnaches
#define SAVE 0
#define RESTORE 1

// Toggle printing of messages on the terminal
#define PRINT 0

STATISTIC(HashDCECounter, "Counts number of functions greeted");

// The following class is used as an entry to the backup stack that 
// restores the CP_Table for branches   
class CP_Table_Stack_Entry
{
    Value * key;
    Value * entry;
    int Depth; //enhancement for multilevel nesting

public:

    CP_Table_Stack_Entry(Value *key_val, Value *entry_val)
    {
        key = key_val;
        entry = entry_val;
    }

    Value * get_Key()
    {
        return key;
    }

    Value * get_Entry()
    {
        return entry;
    }
};

namespace {

struct HashDCE : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    HashDCE() : FunctionPass(ID) {}
    map<Value*, Value*> CP_Table; // Stores compiler temporaries for load elimination
    stack<CP_Table_Stack_Entry> CP_Table_Stack; // Used as a store to restore CP_Table values after a branch
    map<Value*,bool> Mark; // Mark[i] = True means statement i cannot be eliminated
    queue<Value*> WorkList;
    queue<Value*> IncList;
    map<Value*,int> HT; // Hash Table used for value numbering
    map<Value*,int> StatementContext; // Determines the HT entry value for an operand in statement i
    map<Value*,int> MarkContext; // Determines the HT value of the operand that must not be eliminated
    unsigned int IfElseRestore;

    virtual bool runOnFunction(Function &F) 
    {
        ++HashDCECounter;
        for (Function::iterator I=F.begin(),E=F.end(); I!=E; I++) 
        {
            BasicBlock* bb = I;
            Instruction& be =  bb->back();
            bool CP_Table_status;
            queue<Value*> UseList;
            
            // The following lines check whether the block ends with a
            // conditional branch or not
            // If it does then we need to restore the CP Table 
            // And delete phi nodes from being eliminated

            if (isa<BranchInst>(be))
            {
                if (PRINT)
                    errs() << bb->back()<<'\n';

                BranchInst *bi = dyn_cast<BranchInst>(&be);
                if (bi->isConditional()) 
                {                   
                    CP_Table_status = SAVE;
                    IfElseRestore = 1;
                }
                else
                    CP_Table_status = RESTORE;
            }
            
            // Scan every basic block in the program to determine copies and save 
            // its value to the CP_Table
           
            for (BasicBlock::iterator BI=bb->begin(),BE=bb->end(); BI!=BE; BI++) 
            {
                Instruction * i = BI;
                map<Value*,Value*>::iterator ii;
                
                // If the statement is a load instruction then the destination is 
                // a compiler temporary and the operand is an alloc instance, 
                // store it as CP_Table[source] = destination.
                // Uselist stores the instances that are modified in a basic block 
                // so that those entries can be eliminated if the block is within a
                // branch.

                if (isa<LoadInst>(i))
                {
                    ii = CP_Table.find(i->getOperand(0));
                    if (ii != CP_Table.end()) 
                    {                       
                        CP_Table[i] = ii->second;
                        i->replaceAllUsesWith(ii->second);
                        UseList.push(i);
                    }
                    else 
                    {
                        CP_Table[i->getOperand(0)] = i;
                        UseList.push(i->getOperand(0));
                    }
                    
                    // Statement Context stores the HT value of an operand 
                    // This helps determining the version that does should not be 
                    // eliminated.
                    
                    map<Value*,int>::iterator hi;
                    hi = HT.find(i->getOperand(0));
                    if (hi != HT.end()) 
                    {                        
                        StatementContext[i] = HT[i->getOperand(0)];
                        if (PRINT)
                            errs()<<"Context Save\n"<<HT[i->getOperand(0)]<<"\n";
                    }
                }
                
                // A store instruction contains 2 operands, the first is a temporary
                // The second is an alloc instance
                // We store the instance as CP_Table[second] = first
                // Again Uselist provides information about all alloc instances touched 
                // in the block
                
                else if (isa<StoreInst>(i))
                {
                    ii = CP_Table.find(i->getOperand(1));
                    if (ii != CP_Table.end()) 
                    {
                        CP_Table[i->getOperand(1)] = ii->second;
                        UseList.push(i->getOperand(1));
                    }
                    else 
                    {
                        CP_Table[i->getOperand(1)] = i->getOperand(0);
                        UseList.push(i->getOperand(1));
                    }
                    
                    // In case of a store we know that the HT value needs to be incremented.
                    // Inclist stores entries that are incremented so that they can be restored 
                    // in case of branches.
                    
                    map<Value*,int>::iterator hi;
                    hi = HT.find(i->getOperand(1));
                    if (hi != HT.end()) 
                    {
                        HT[i->getOperand(1)]++;
                        if (PRINT)
                            errs()<<"HT plus plus\n"<< HT[i->getOperand(1)]<<"\n";
                        StatementContext[i] = HT[i->getOperand(1)];
                        IncList.push(i->getOperand(1));
                    }
                }
                
                // Do nothing in case of Call or Branch Instructions
                
                else if (isa<CallInst>(i))
                {                    
                    i->getNumOperands();
                }
                else if (isa<BranchInst>(i))
                {
                    BranchInst* bi = dyn_cast<BranchInst>(i);
                    unsigned int count = 0;
                    if (bi->isConditional()) 
                    {
                        if (PRINT)
                            errs() << "Branch Inst " << i->getNumOperands()<<"\n";
                        count = 0;
                        while (count < i->getNumOperands())
                        {
                            if (PRINT)
                                  errs()<<i->getOperand(count)<<"\n";
                            count++;
                        }
                    }
                }
                
                // In case the instruction is an alloc initialize the HT entry
                // to zero 
                
                else if (isa<AllocaInst>(i))
                {
                    if (PRINT)	
                        errs()<<"Alloca "<< *(i)<<"\n";
                    HT[i] = 0;
                }

                if (PRINT)
                    errs()<<"\n"<<i<<"\n"<<*i<<"\n";
                
            }
            
            if (PRINT)
                errs() << "Block End && IfElse Restore "<< IfElseRestore<<"\n";
            
            if (CP_Table_status == RESTORE )
            {
                map<Value*,Value*>::iterator ii;
                
                while (!UseList.empty())
                {
                    Instruction *I = (Instruction *)UseList.front();
                    UseList.pop();
                    ii = CP_Table.find(I);
                    if (ii != CP_Table.end()) 
                    {
                        CP_Table.erase(I);
                    }
                }
                
                if( IfElseRestore != 0)
                {
                    while(!IncList.empty())
                    {
                        map<Value*,int>::iterator hi;
                
                        Instruction *I = (Instruction *)IncList.front();
                        IncList.pop();
                        hi = HT.find(I);
                        if (hi != HT.end()) 
                        {                      
                            HT[I] --;
                        }
                    }

                }

                IfElseRestore--;
                
                // Now we use the CP_Table_Store to restore the old values
                // of variables in the CP_Table 
                // We also use a backup stack to restore the CP_Table_Stack 
                // Since we invoke restore on two instance of branch
                
                stack<CP_Table_Stack_Entry> CP_Table_Stack_Bkp;

                while (!CP_Table_Stack.empty())
                {
                    CP_Table_Stack_Entry s = CP_Table_Stack.top();
                    CP_Table_Stack.pop();
                    map<Value*,Value*>::iterator ii;
                    ii = CP_Table.find(s.get_Key());
                    if (ii == CP_Table.end()) 
                    {
                        CP_Table[s.get_Key()] = s.get_Entry();
                    }
                    CP_Table_Stack_Bkp.push(s);
                }
                
                // Restore CP_Table_Stack using the backup 
                // stack

                while (!CP_Table_Stack_Bkp.empty())
                {
                    CP_Table_Stack_Entry s = CP_Table_Stack_Bkp.top();
                    CP_Table_Stack_Bkp.pop();
                    CP_Table_Stack.push(s);
                }
            }
            
            // Save the CP_Table into the CP_Table_Stack
            
            else if (CP_Table_status == SAVE )
            {
                map<Value*,Value*>::iterator it;
                for(it = CP_Table.begin(); it!= CP_Table.end(); it++) 
                {
                    CP_Table_Stack_Entry s(it->first, it->second);
                    CP_Table_Stack.push(s);
                }
            }
        }

        if (PRINT)
            errs().write_escaped(F.getName()) << '\n';
        
        // The following lines begin a backward traversal of the 
        // Program to identify useful instructions and mark them
        // Here we make use of StatementContext and MarkContext 
        // heavily

        for (inst_iterator I=inst_end(F),E=inst_begin(F); I!=E; I--) 
        {
            Instruction * i;
            i =&*I;
            
            if (i == NULL)
            {
                continue;
            }
            
            // If the statement is a Call/Return or Terminator Instruction we go ahead and 
            // mark it

            if (isa<CallInst>(i) or Mark[i] or isa<ReturnInst>(i) or isa<TerminatorInst>(i))
            {
                Mark[i] = true;
                unsigned int count = 0;
                
                // If the instruction is a Call instruction we mark it and 
                // all its operands as useful
                
                if(isa<CallInst>(i))
                {
                    Mark[i->getOperand(1)] = true;
                    if (PRINT)
                        errs() << "Marked" << *(i->getOperand(1))<<'\n';
                }
                
                // If the instruction is a load we need to make sure that
                // The correct HT value numbered operand is not eliminated
                // Here we use MarkContext
                
                else if (isa<LoadInst>(i))
                {
                    MarkContext[i->getOperand(0)]=StatementContext[i];
                }
                
                // For any other types of instructions we go ahead and mark all
                // its operands.
                
                else
                {
                    count = 0;
                    while (count < i->getNumOperands())
                    {
                        Mark[i->getOperand(count)] = true;
                        if (PRINT)
                            errs() << "Marked" << *(i->getOperand(count))<<'\n';
                        count++;
                    }
                }
                if (PRINT)
                    errs() << "Marked" << *i<<'\n';
            }
            
            // If an instruction is unmarked we can go ahead and erase it but only 
            // in case it is not a store an contains a HT valued operand that is 
            // in MarkContext
            // Worklist contains the instructions that need to be eliminated 
            // This list is maintained as a queue so that the instructions first to be 
            // eliminted are the uses and not the defs.
            
            else
            {
                unsigned int count = 0;
                if (isa<StoreInst>(i))
                {
                    map<Value*,int>::iterator hi;
                    hi = MarkContext.find(i->getOperand(1));
                    
                    // If operand is in MarkContext and a store we mark it and
                    // its operands.
                    
                    if (hi != HT.end()) 
                    {
                        if (StatementContext[i] == MarkContext[i->getOperand(1)])
                        {
                            Mark[i] = true;
                            count = 0;
                            while (count < i->getNumOperands())
                            {
                                Mark[i->getOperand(count)] = true;
                                if (PRINT)
                                    errs() << "Marked " << *(i->getOperand(count))<<'\n';
                                count++;
                            }
                        }
                        else
                            WorkList.push(i);
                    }
                    else
                        WorkList.push(i);
                }                
                else
                {                    
                    WorkList.push(i);
                }
            }
        }

    
    		// Finally the we simply go ahead an remove the instructions
        // in the WorkList queue from the program.

        while (!WorkList.empty())
        {
            Instruction *I = (Instruction *)WorkList.front();
            WorkList.pop();
            I->eraseFromParent();
        }
        
        return false;
    }
};
}

char HashDCE::ID = 0;
static RegisterPass<HashDCE> X("hashdce", "HashDCE  Pass");
