#ifndef WABPT_H
#define WABPT_H

#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include "pcm.h"
using namespace std;

#define kEY_MAX         ULONG_LONG_MAX
#define kEY_MIN         0
#define DUP             INT_MIN

struct VirNode                                                 // 4K-byte node
{
        uint64_t keyNums;                                      // number of keys in a node
        uint64_t numSlot;                                      // belongs to which memory slot 
        int64_t boudary_shift_pos;                             // boudary shift position 
        VirNode *parentBlock;                
        uint64_t *key;
        VirNode **childBlock;
};

class WAbpt
{
public:
        WAbpt(int porder, uint64_t memory_size, int shift); 
        void updateParentNode(VirNode *leftBlock, VirNode *rightBlock, uint64_t value);
        void splitBlock(VirNode *curBlock);
        void insertKey(VirNode *curBlock, uint64_t value);
        int insertTree(VirNode *curBlock, uint64_t value);
        void delete_a_key(VirNode *curBlock, uint64_t value);
        void redistributeBlock(VirNode *leftBlock, VirNode *rightBlock, bool isLeaf, int posOfLeftBlock, int whichIsCurBlock);
        void mergeBlock(VirNode *leftBlock, VirNode *rightBlock, bool isLeaf, int posOfRightBlock);
        void deleteNode(VirNode *curBlock, bool isLeaf, int curBlockPosition, bool &isRecycle);
        void deleteKey(VirNode *curBlock, uint64_t value, int curBlockPosition);
        void find(VirNode *curBlock, uint64_t value);
        void print(vector < VirNode* > Blocks);
        void printLeaf(VirNode *curBlock);
        void printTreeInformation(VirNode *curBlock);
        int getRealPosition(int pos);
        uint64_t getAscendKeys(VirNode *curBlock, uint64_t temp[]);
        int binarySearch(VirNode *curBlock, int size, uint64_t value);
        int pointer_binarySearch(VirNode *curBlock, uint64_t value, int dup); 
        int leftKeys_binarySearch(VirNode *curBlock, uint64_t value);
        int rightKeys_binarySearch(VirNode *curBlock, uint64_t value);
        bool checkLeftFull(VirNode *curBlock);        
        bool checkRightFull(VirNode *curBlock);
        VirNode *getPreLeaf(VirNode *curBlock);
        void setKeyNums(VirNode *curBlock, uint64_t value);
        uint64_t getKeyNums(VirNode *curBlock);
        void setNumSlot(VirNode *curBlock, uint64_t newSlot);
        uint64_t getNumSlot(VirNode *curBlock);
        void setBoundaryShift(VirNode *curBlock, int64_t value);
        int64_t getBoundaryShift(VirNode *curBlock); 
        void setKey(VirNode *curBlock, int pos, uint64_t value);
        uint64_t getKey(VirNode *curBlock, int pos); 
        void setPointer(VirNode *curBlock, int pos, VirNode *ptr);
        VirNode *getPointer(VirNode *curBlock, int pos);
        void setParent(VirNode *curBlock, VirNode *ptr);
        VirNode *getParent(VirNode *curBlock);
        int findMinPosition(VirNode *curBlock);  
        int findMaxPosition(VirNode *curBlock);  
        int getPivotPtr(VirNode *curBlock);
        int getBoundaryPos(VirNode *curBlock);
        VirNode *getNextBlock(VirNode *curBlock);
        VirNode *getSwappedBlock(VirNode *curBlock, double threshold);
        void initNode(VirNode *curBlock);
        void freeNode(VirNode *curBlock, int error_checking);
        void resetNode(VirNode *curBlock, uint64_t newSlot);
        bool checkIsLeaf(VirNode *curBlock);
        bool checkIsNoKey(VirNode *curBlock);
        bool checkIsRoot(VirNode *curBlock);

        int getOrder() const
        {
            return this->order;
        }

        void setKeyFound(const bool &found)
        {
            this->keyFound = found;
        }

        bool getKeyFound() const
        {
            return this->keyFound;
        }

        void setRoot(VirNode *root)
        {
            this->rootBlock = root;
        }

        VirNode *getRoot() const
        {
            return this->rootBlock;
        }

        int getPivotShift() const
        {
            return this->pivotShift;
        }

        void setNodeNums(int num)
        {
            this->nodeNums = num;
        }

        int getNodeNums() const
        {
            return this->nodeNums;
        }

        Pcm getPCM()
        {
            return this->pcm;
        }
         

private:        
        int order;                                          // number of pointers [max number of keys = order - 1]
        bool keyFound;
        VirNode *rootBlock;                                 // root node  
        int pivotShift;                                     // calculated by numbers of node allocation
        int nodeNums;
        Pcm pcm;
};

#endif
