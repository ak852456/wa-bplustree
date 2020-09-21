#include "wa-bpt.h"

WAbpt::WAbpt(int porder, uint64_t memory_size, int shift)   
    :order(porder), keyFound(false), rootBlock(NULL), pivotShift(shift), nodeNums(0), pcm(porder, memory_size)                                          
{
    
}

void WAbpt::updateParentNode(VirNode *leftBlock, VirNode *rightBlock, uint64_t value)                            
{ 
        if(checkIsRoot(leftBlock))                                                                               
        {           
            VirNode *parentBlock = new VirNode;
            initNode(parentBlock);

            insertKey(parentBlock, value); 
            setPointer(parentBlock, 0, leftBlock);
            setPointer(parentBlock, 1, rightBlock);

            setParent(leftBlock, parentBlock);
            setParent(rightBlock, parentBlock);          
            setRoot(parentBlock);     
        }

        else                                                                                                    
        {            
            VirNode *parentBlock = getParent(leftBlock);
            insertKey(parentBlock, value);

            uint64_t keyNums = getKeyNums(parentBlock);
            int ptr_pos = binarySearch(parentBlock, keyNums, value);    

            for(int i = keyNums - 1; i >= ptr_pos + 1; i--)
            {
                VirNode *childBlock = getPointer(parentBlock, i);
                setPointer(parentBlock, i + 1, childBlock);
            }

            setPointer(parentBlock, ptr_pos + 1, rightBlock);
            setPointer(parentBlock, ptr_pos, leftBlock);
        }                                      
}

void WAbpt::splitBlock(VirNode *curBlock)
{      
        int nums_in_left = getOrder() / 2, ptr_pos;       
        VirNode *parentBlock = getParent(curBlock), *childBlock = NULL;                                                        
        uint64_t val, leftKeys[nums_in_left], rightKeys[getOrder() - nums_in_left]; 
                                                                                 
        VirNode *rightBlock = new VirNode;                                                                      
        initNode(rightBlock);

        VirNode *leftBlock = new VirNode;                                                                      
        initNode(leftBlock);          

        setParent(rightBlock, parentBlock);
        setParent(leftBlock, parentBlock);
      
        bool isLeaf = checkIsLeaf(curBlock);
        int leftMost = findMinPosition(curBlock), rightMost = findMaxPosition(curBlock), right_start = leftMost + nums_in_left;
        int lCount = 0, rCount = 0, leftMid = (leftMost + right_start) / 2, rightMid = (right_start + rightMost) / 2;
        uint64_t parent_key = getKey(curBlock, right_start);

/************************************** create a new left node **************************************/

        for(int i = leftMid; i >= leftMost; i--)
            leftKeys[lCount++] = getKey(curBlock, i);

        for(int i = leftMid + 1; i < right_start; i++)
            leftKeys[lCount++] = getKey(curBlock, i);

        if(lCount != nums_in_left)
            cout << "\nError: not the correct left counts!!!" << endl << endl;

        for(int j = 0; j < lCount; j++)
            insertKey(leftBlock, leftKeys[j]); 

        if(!isLeaf) 
        { 
            ptr_pos = 0;
            for(int cur_pos = 0; cur_pos <= nums_in_left; cur_pos++)                                                    
            {              
                childBlock = getPointer(curBlock, cur_pos);
                setPointer(leftBlock, ptr_pos++, childBlock);
                setParent(childBlock, leftBlock);
            }
        }

/************************************** create a new right node **************************************/

        for(int i = rightMid; i >= right_start; i--)
            rightKeys[rCount++] = getKey(curBlock, i);

        for(int i = rightMid + 1; i <= rightMost; i++)
            rightKeys[rCount++] = getKey(curBlock, i);

        if(rCount != getOrder() - nums_in_left)
            cout << "\nError: not the correct right counts!!!" << endl << endl;

        for(int j = 0; j < rCount; j++)
        { 
            if(!isLeaf && rightKeys[j] == parent_key)                                             
            {       
                continue;    
            }     

            insertKey(rightBlock, rightKeys[j]); 
        }

        if(!isLeaf) 
        { 
            ptr_pos = 0;
            for(int cur_pos = nums_in_left + 1; cur_pos <= getOrder(); cur_pos++)                                                    
            {              
                childBlock = getPointer(curBlock, cur_pos);
                setPointer(rightBlock, ptr_pos++, childBlock);
                setParent(childBlock, rightBlock);
            }
        }

/************************************** update leaf links **************************************/      
 
        if(isLeaf)                                                                               
        {
            VirNode *nextLeaf = getPointer(curBlock, getOrder());                                                           // find the next leafBlock of the curBlock
            if(nextLeaf != NULL)
                setPointer(rightBlock, getOrder(), nextLeaf);
            
            setPointer(leftBlock, getOrder(), rightBlock);

            VirNode *prevLeaf = getPreLeaf(curBlock);
            if(prevLeaf != NULL)
                setPointer(prevLeaf, getOrder(), leftBlock);                                                                 // find the prior leafBlock of the curBlock        
        }
        
/************************************** update the parent node **************************************/    

        updateParentNode(leftBlock, rightBlock, parent_key);   
        freeNode(curBlock, -1);     
}

void WAbpt::insertKey(VirNode *curBlock, uint64_t value)
{
        int key_pos, isShift = 0, pivot = getPivotPtr(curBlock);
        uint64_t val, pivotValue = getKey(curBlock, pivot);
        int64_t boundary = getBoundaryShift(curBlock);

        if(pivotValue == kEY_MAX)  
        {
            key_pos = pivot;
        }

        else
        {
            if(value > pivotValue)                                                              
            {
                if(checkRightFull(curBlock))                                                                                // if shift, boudary points to last valid key
                    isShift = 1;   
              
                key_pos = rightKeys_binarySearch(curBlock, value);
                for(int i = findMaxPosition(curBlock); i >= key_pos; i--)  
                {
                    val = getKey(curBlock, i); 
                    setKey(curBlock, i + 1, val);  
                }
         
                if(isShift) 
                {    
                    setBoundaryShift(curBlock, boundary + 1); 
                }         
            }

            else
            {
                if(checkLeftFull(curBlock))                                                                                 // if shift, boudary points to last valid key
                    isShift = 1;
              
                key_pos = leftKeys_binarySearch(curBlock, value);          
                for(int i = findMinPosition(curBlock); i <= key_pos; i++)  
                {
                    val = getKey(curBlock, i);
                    setKey(curBlock, i - 1, val);  
                }

                if(isShift)
                {   
                    if(boundary == -1)    
                        setBoundaryShift(curBlock, getOrder() - 1);

                    else
                        setBoundaryShift(curBlock, boundary - 1);
                }
            }
        }

        setKey(curBlock, key_pos, value);
        setKeyNums(curBlock, getKeyNums(curBlock) + 1);
}

int WAbpt::insertTree(VirNode *curBlock, uint64_t value)
{         
        int pos = pointer_binarySearch(curBlock, value, 1);
        if(pos == DUP)                                                                                             
        {
            return DUP;
        }
        
        VirNode *childBlock = getPointer(curBlock, pos);
        if(childBlock != NULL)                                                                   
        {   
            pos = insertTree(childBlock, value);     
            if(pos == DUP)
            {
                return DUP;
            }  
        }

        else                                                                                                  
        {   
            insertKey(curBlock, value);          
        }    
      
        if(getKeyNums(curBlock) == getOrder())
        {   
            splitBlock(curBlock); 
        }

        return 0;
}

void WAbpt::delete_a_key(VirNode *curBlock, uint64_t value)                             
{        
        int key_pos, pivot = getPivotPtr(curBlock), leftMost = findMinPosition(curBlock), rightMost = findMaxPosition(curBlock);
        int64_t boundary = getBoundaryShift(curBlock);
        uint64_t val;

        if(value >= getKey(curBlock, pivot))                                                              
        {
            key_pos = rightKeys_binarySearch(curBlock, value);
            for(int i = key_pos; i < rightMost; i++)  
            {
                val = getKey(curBlock, i + 1); 
                setKey(curBlock, i, val);  
            }
                
            setKey(curBlock, rightMost, kEY_MAX); 
            if(boundary != -1 && boundary < pivot)                                                                                
                setBoundaryShift(curBlock, boundary - 1); 
        }

        else
        {
            key_pos = leftKeys_binarySearch(curBlock, value);     
            for(int i = key_pos; i > leftMost; i--)  
            {
                val = getKey(curBlock, i - 1);
                setKey(curBlock, i, val);  
            }

            setKey(curBlock, leftMost, kEY_MIN); 
            if(boundary != -1 && boundary > pivot)                                                                                
                setBoundaryShift(curBlock, boundary + 1); 

            if(boundary == getOrder())    
                setBoundaryShift(curBlock, -1);    
        }

        setKeyNums(curBlock, getKeyNums(curBlock) - 1);
        if(getKey(curBlock, pivot) == kEY_MAX && getKeyNums(curBlock) != 0)
        {
            for(int i = pivot; i > leftMost; i--)  
            {
                val = getKey(curBlock, i - 1);
                setKey(curBlock, i, val);  
            }

            setKey(curBlock, leftMost, kEY_MIN);
            if(boundary != -1 && boundary > pivot)                                                                                
                setBoundaryShift(curBlock, boundary + 1); 

            if(boundary == getOrder())    
                setBoundaryShift(curBlock, -1);  
        }
}

void WAbpt::redistributeBlock(VirNode *leftBlock, VirNode *rightBlock, bool isLeaf, int posOfLeftBlock, int whichIsCurBlock)
{
        int rMin = findMinPosition(rightBlock), lMax = findMaxPosition(leftBlock);
        VirNode *childBlock = NULL, *parentBlock = getParent(leftBlock); 
        uint64_t val, update_val, parentNums = getKeyNums(parentBlock), parentKeys[getOrder()], leftNums = getKeyNums(leftBlock), rightNums = getKeyNums(rightBlock);
   
        getAscendKeys(parentBlock, parentKeys);
        if(whichIsCurBlock == 0)                                                                                // LeftBlock is curBlock, borrow the key from rightBlock
        {   
            if(!isLeaf)
            {    
                val = getKey(rightBlock, rMin);                                                                 // send up the min key in rightBlock to the parentBlock                           
                delete_a_key(rightBlock, val);
                insertKey(leftBlock, parentKeys[posOfLeftBlock]);                                               // bring down the key from parentBlock to leftBlock

                childBlock = getPointer(rightBlock, 0);
                setPointer(leftBlock, leftNums + 1, childBlock);
                setParent(childBlock, leftBlock);

                for(int i = 0; i <= rightNums; i++)                                                                                                                                                               
                {
                    childBlock = getPointer(rightBlock, i + 1);
                    setPointer(rightBlock, i, childBlock);      
                }                                                      
            }

            else
            { 
                val = getKey(rightBlock, rMin);                                                                 // borrow the min key in rightBlock to leftBlock
                insertKey(leftBlock, val);                   
                delete_a_key(rightBlock, val);
                val = getKey(rightBlock, rMin + 1);                                                             // take the current minimum key to update parentBlock             
            }
        }
            
        else                                                                                                    // RightBlock is curBlock, borrow the key from leftBlock
        { 
            if(!isLeaf)
            {
                for(int i = rightNums; i >= 0; i--)                                                                                                           
                {
                    childBlock = getPointer(rightBlock, i);
                    setPointer(rightBlock, i + 1, childBlock); 
                }

                childBlock = getPointer(leftBlock, leftNums);                                                   // the new first childBlock in rightBlock will be the last childBlock of leftBlock
                setPointer(rightBlock, 0, childBlock);
                setParent(childBlock, rightBlock);
                setPointer(leftBlock, leftNums, NULL);
 
                insertKey(rightBlock, parentKeys[posOfLeftBlock]);                                              // bring down the key from parentBlock to rightBlock
                val = getKey(leftBlock, lMax);                                                                  // send up the max key in leftBlock to the parentBlock  
                delete_a_key(leftBlock, val);
            }

            else
            {
                val = getKey(leftBlock, lMax);                                                                  // borrow the max key in leftBlock to rightBlock    
                delete_a_key(leftBlock, val);
                insertKey(rightBlock, val);
            }
        }
    
        for(int i = 0; i < parentNums; i++)                                                                                                                                          
        {
            if(i == posOfLeftBlock)                                                                             // find which key is related to the update position                                                                                                                                             
            {
                update_val = parentKeys[i];
                break;
            }    
        }  

        int pMin = findMinPosition(parentBlock), pMax = findMaxPosition(parentBlock);       
        for(int i = pMin; i <= pMax; i++)
        {
            if(getKey(parentBlock, i) == update_val)                                                            
            {
                setKey(parentBlock, i, val);
            }
        }
}

void WAbpt::mergeBlock(VirNode *leftBlock, VirNode *rightBlock, bool isLeaf, int posOfRightBlock)
{
        VirNode *childBlock = NULL, *parentBlock = getParent(leftBlock);                                                        
        uint64_t val, parentNums = getKeyNums(parentBlock), parentKeys[getOrder()], leftNums = getKeyNums(leftBlock), rightNums = getKeyNums(rightBlock); 
        
/************************************** create a new left node **************************************/    
    
        uint64_t newSlot = pcm.allocateSlot();  
        resetNode(leftBlock, newSlot);

/************************************** copy the right node to the left node **************************************/   

        int rMin = findMinPosition(rightBlock), rMax = findMaxPosition(rightBlock);        
        for(int i = rMin; i <= rMax; i++)                                                                                    
        {    
            val = getKey(rightBlock, i);
            insertKey(leftBlock, val); 
        }

        if(!isLeaf) 
        {
            int ptr_pos = leftNums + 1;
            for(int cur_pos = 0; cur_pos <= rightNums; cur_pos++)                                                    
            {              
                childBlock = getPointer(rightBlock, cur_pos);
                setPointer(leftBlock, ptr_pos++, childBlock);
                setParent(childBlock, leftBlock);
            }
        }

/*****************************************************************************************************************/

        getAscendKeys(parentBlock, parentKeys); 
        if(isLeaf)                                                                                              // update next leaf link                                                                                             
        {
            VirNode *nextLeaf = getPointer(rightBlock, getOrder());
            setPointer(leftBlock, getOrder(), nextLeaf); 
        }

        else
            insertKey(leftBlock, parentKeys[posOfRightBlock - 1]);                                              // take down the key from the parentBlock

    
/************************************** update the parent node **************************************/ 

        int pMin = findMinPosition(parentBlock), pMax = findMaxPosition(parentBlock); 
        for(int i = pMin; i <= pMax; i++)
        {
            if(getKey(parentBlock, i) == parentKeys[posOfRightBlock - 1])                                       // get the position of demand deleting key
            {
                delete_a_key(parentBlock, parentKeys[posOfRightBlock - 1]);
                break;
            }         
        }   
        
        for(int i = posOfRightBlock; i <= parentNums; i++)                                                      // update pointers in parentBlock                                                               
        {
            childBlock = getPointer(parentBlock, i + 1);
            setPointer(parentBlock, i, childBlock); 
        }
}

void WAbpt::deleteNode(VirNode *curBlock, bool isLeaf, int curBlockPosition, bool &isRecycle)
{       
        VirNode *leftBlock = NULL, *rightBlock = NULL, *parentBlock = getParent(curBlock);             
        
        if(curBlockPosition != 0)                                                                               // find its neighbor leftBlock                            
        {    
            leftBlock = getPointer(parentBlock, curBlockPosition - 1);                                                                
            pcm.addReadCounts();
        }

        if(curBlockPosition != getKeyNums(parentBlock))                                                         // find its neighbor rightBlock
        {    
            rightBlock = getPointer(parentBlock, curBlockPosition + 1); 
            pcm.addReadCounts();
        }

        if(leftBlock != NULL && getKeyNums(leftBlock) >= ceil((double)getOrder() / 2))                          // redistribute first
        {
            redistributeBlock(leftBlock, curBlock, isLeaf, curBlockPosition - 1, 1);          
        }
    
        else if(rightBlock != NULL && getKeyNums(rightBlock) >= ceil((double)getOrder() / 2))
        { 
            redistributeBlock(curBlock, rightBlock, isLeaf, curBlockPosition, 0);                  
        }

        else if(leftBlock != NULL)                                                                              // if it can't redistribute, try to merge 
        {
            isRecycle = true;
            mergeBlock(leftBlock, curBlock, isLeaf, curBlockPosition);   
        }

        else if(rightBlock != NULL)
        {  
            mergeBlock(curBlock, rightBlock, isLeaf, curBlockPosition + 1);
            freeNode(rightBlock, -5);  
        }     
}

void WAbpt::deleteKey(VirNode *curBlock, uint64_t value, int curBlockPosition)                             // curBlockPosition is the position of curBlock in its parentBlock
{        
        bool isLeaf = false;
        if(checkIsLeaf(curBlock))                                                                            
            isLeaf = true;

        // the minimum key could be changed due to merge or redistribute later,
        // so keep it to check and replace it's copy from it's ancestor 

        bool isLeftMost = false;
        uint64_t minValue = getKey(curBlock, findMinPosition(curBlock));

        if(minValue == value && isLeaf)
            isLeftMost = true;
  
        int ptr_pos = pointer_binarySearch(curBlock, value, 0);
        VirNode *childBlock = getPointer(curBlock, ptr_pos);

        if(childBlock != NULL)                                                                    
        {   
            deleteKey(childBlock, value, ptr_pos);     
        }

        else                                                                                                      
        {   
            delete_a_key(curBlock, value);
            setKeyFound(true);

            if(checkIsNoKey(curBlock))
            {           
                freeNode(curBlock, -2);                                                                         
                return;
            }
        }
        
        bool isRoot = false;
        if(checkIsRoot(curBlock))                                                                            
            isRoot = true;

        if(isRoot && isLeaf)                                                                                    // the only node is root
        {  
            return;          
        }

        if(isRoot && !isLeaf && checkIsNoKey(curBlock))                                                         // after deleting, the leftBlock of root becomes as root                  
        {     
            VirNode *leftMostChild = getPointer(curBlock, 0);
            setRoot(leftMostChild);
            setParent(rootBlock, NULL);

            freeNode(curBlock, -3);
            return;
        }
      
        bool recycle = false;
        if(getKeyNums(curBlock) < ceil((double)getOrder() / 2) - 1 && !isRoot)                                  // root can has least one key
        {
            deleteNode(curBlock, isLeaf, curBlockPosition, recycle); 
        }
    
           
//      1.  (7, 10)         after deleting 7    2.  (7, 10)         must be    3.  (8, 10)          
//             |                                       |                              |
//             |                                       |                              |
//          (7, 8, 9)                               (8, 9)                         (8, 9)

// if the deleted key is minimum in leaf, delete the duplicate if any in the ancestor block

        
        if(isLeftMost && !recycle)
        {             
            VirNode *dupBlock = getParent(curBlock);    
            while(dupBlock != NULL)
            {
                pcm.addReadCounts();
                int min = findMinPosition(dupBlock), max = findMaxPosition(dupBlock); 

                for(int i = min; i <= max; i++)
                {
                    if(getKey(dupBlock, i) == minValue)
                    {
                        uint64_t key = getKey(curBlock, findMinPosition(curBlock));
                        setKey(dupBlock, i, key);
                        break;
                    }
                }

                dupBlock = getParent(dupBlock);
            }
        }

        if(recycle)
        {
            freeNode(curBlock, -4);
        }
}

void WAbpt::find(VirNode *curBlock, uint64_t value)
{
        int pos = pointer_binarySearch(curBlock, value, 0);
        VirNode *child = getPointer(curBlock, pos);

        if(child != NULL)                                                                    
        {   
            find(child, value);     
        }

        else                                                                                                      
        {   
            uint64_t keys[getOrder()];
            getAscendKeys(curBlock, keys);

            if(value == keys[pos])
            {    
                setKeyFound(true);
            }
        }    
}

void WAbpt::print(vector < VirNode* > blocks)
{       
        vector < VirNode* > newBlocks;
        for(int i = 0; i < blocks.size(); i++)                                                                
        { 
            VirNode *curBlock = blocks[i]; 
            cout << "[|";

            for(int i = 0; i < getOrder(); i++)                                                             
            {  
                uint64_t val = getKey(curBlock, i); 
                if(val != kEY_MIN && val != kEY_MAX)
                {    
                    cout << val << "|";
                }
            }

            int nums = getKeyNums(curBlock);
            for(int i = 0; i <= nums; i++)                                                             
            {  
                VirNode *child = getPointer(curBlock, i);
                if(child != NULL)
                {
                    newBlocks.push_back(child); 
                }
            }
           
            cout << "]  ";
        }

        cout << endl << endl;
        blocks.clear();

        if(newBlocks.size() != 0)                                                                              
            print(newBlocks);
}

void WAbpt::printLeaf(VirNode *curBlock)
{
        while(!checkIsLeaf(curBlock))
        {    
            curBlock = getPointer(curBlock, 0);
        }

        do
        {
            cout << "|";
            for(int i = 0; i < getOrder(); i++)                                                             
            {  
                uint64_t val = getKey(curBlock, i);
                if(val != kEY_MIN && val != kEY_MAX)
                {    
                    cout << " " << val << " ";
                }     
            }

            cout << "|";
            curBlock = getPointer(curBlock, getOrder());
        }
        while(curBlock != NULL);

        cout << endl << endl;
}

void WAbpt::printTreeInformation(VirNode *curBlock)
{
        int height = 0, totalKeys = 0;
        uint64_t minValue = kEY_MAX, maxValue = kEY_MIN;

        while(!checkIsLeaf(curBlock))
        {    
            curBlock = getPointer(curBlock, 0);
            height++;
        }

        do
        {
            for(int i = 0; i < getOrder(); i++)                                                             
            {  
                uint64_t val = getKey(curBlock, i);
                if(val != kEY_MIN && val != kEY_MAX)
                {    
                    if(val < minValue)
                        minValue = val;

                    if(val > maxValue)
                        maxValue = val;

                    totalKeys++;
                }     
            }

            curBlock = getPointer(curBlock, getOrder());
        }
        while(curBlock != NULL);

        cout << "\nThe height of the tree is: " << height << endl;
        cout << "The total of keys in leafs is: " << totalKeys << endl;
        cout << "The total of nodes in the tree is: " << getNodeNums() << endl;
        cout << "The Max key in the tree is: " << maxValue << endl;
        cout << "The Min key in the tree is: " << minValue << endl;
}

int WAbpt::getRealPosition(int pos)
{
        if(pos >= getOrder())
            pos -= getOrder();

        if(pos < 0)
            pos += getOrder();

        return pos;
}


uint64_t WAbpt::getAscendKeys(VirNode *curBlock, uint64_t temp[])                                                                     // using sort() is too slow
{
        int shift, start = findMinPosition(curBlock), end = findMaxPosition(curBlock);       
        for(int i = 0; i < getOrder(); i++, start++)   
        {       
            shift = getRealPosition(start);
            if(start >= start && start <= end)
                temp[i] = curBlock->key[shift];

            else
                temp[i] = kEY_MAX; 
        }
}

int WAbpt::binarySearch(VirNode *curBlock, int size, uint64_t value)                        
{     
        uint64_t keys[getOrder()];
        getAscendKeys(curBlock, keys);

        int mid = 0, low = 0, high = size;
        while(low <= high)
        { 
            mid = (low + high) / 2;
            
            if(keys[mid] > value)
            {
                high = mid - 1;
            }

            else if(keys[mid] < value)
            {

                low = mid + 1;
            }

            else
            {
                 return mid;
            }
        }
}

int WAbpt::pointer_binarySearch(VirNode *curBlock, uint64_t value, int dup)                        
{     
        pcm.addReadCounts();
        uint64_t keys[getOrder()];
        getAscendKeys(curBlock, keys);

        int mid = 0, low = 0, high = getKeyNums(curBlock);
        while(low <= high)
        { 
            mid = (low + high) / 2;
            
            if(keys[mid] > value)
            {
                high = mid - 1;
            }

            else if(keys[mid] < value)
            {

                low = mid + 1;
            }

            else
            {
                if(dup)                                                                                       
                    return DUP;

                else
                {    
                    if(checkIsLeaf(curBlock))
                    {
                        return mid;
                    }

                    else                                                                                       
                    {               
                        return mid + 1;
                    }
                }
            }
        }

        return keys[mid] > value ? mid : mid + 1;                                                                                               
}

int WAbpt::leftKeys_binarySearch(VirNode *curBlock, uint64_t value)
{       
        int mid = 0, low = findMinPosition(curBlock), high = getPivotPtr(curBlock);
        uint64_t realValue = 0;
        
        while(low <= high)
        {   
            mid = (low + high) / 2;
            realValue = getKey(curBlock, mid);
     
            if(realValue > value)
            {
                high = mid - 1;
            }

            else if(realValue < value)
            {
                low = mid + 1;
            }

            else
            {
                return mid;    
            }
        } 
        
        return realValue > value ? mid - 1 : mid;  
}
    
int WAbpt::rightKeys_binarySearch(VirNode *curBlock, uint64_t value)
{
        int mid = 0, low = getPivotPtr(curBlock), high = findMaxPosition(curBlock);
        uint64_t realValue = 0;

        while(low <= high)
        { 
            mid = (low + high) / 2;
            realValue = getKey(curBlock, mid);
       
            if(realValue > value)
            {
                high = mid - 1;
            }

            else if(realValue < value)
            {

                low = mid + 1;
            }

            else
            {
                return mid;
            }
        }

        return realValue > value ? mid : mid + 1; 
}

bool WAbpt::checkLeftFull(VirNode *curBlock)
{       
        int64_t boundary = getBoundaryShift(curBlock);
        int pivot = getPivotPtr(curBlock);

        if(boundary != -1 && boundary > pivot)
            return true;

        for(int i = 0; i <= pivot; i++)
            if(getKey(curBlock, i) == kEY_MIN || getKey(curBlock, i) == kEY_MAX)
                return false;

        return true;
}

bool WAbpt::checkRightFull(VirNode *curBlock)
{
        int64_t boundary = getBoundaryShift(curBlock);
        int pivot = getPivotPtr(curBlock);

        if(boundary != -1 && boundary < pivot)
            return true;

        for(int i = getOrder() - 1; i >= pivot; i--)
            if(getKey(curBlock, i) == kEY_MIN || getKey(curBlock, i) == kEY_MAX)
                return false;

        return true;
}

VirNode * WAbpt::getPreLeaf(VirNode *curBlock)
{
        VirNode *prev = NULL;
        if(getRoot())
            prev = getPointer(getRoot(), 0);

        if(prev != NULL)
            while(getPointer(prev, 0) != NULL)
                prev = getPointer(prev, 0);

        while(prev != NULL)
        {       
            if(getPointer(prev, getOrder()) == curBlock)
            { 
                break;
            }

            else
                prev = getPointer(prev, getOrder());
        }        

        return prev;
}

void WAbpt::setKeyNums(VirNode *curBlock, uint64_t value)
{
        pcm.accessNodeCounter(getNumSlot(curBlock));
        curBlock->keyNums = value;
}

uint64_t WAbpt::getKeyNums(VirNode *curBlock)
{
        return curBlock->keyNums;
}

void WAbpt::setNumSlot(VirNode *curBlock, uint64_t newSlot)
{
        curBlock->numSlot = newSlot;
        curBlock->keyNums = 0;
        curBlock->boudary_shift_pos = -1;
        curBlock->parentBlock = NULL;

        int pivot = getPivotPtr(curBlock);
        for(int i = 0; i < getOrder(); i++)    
        {
            if(i < pivot)                                                                                       
                curBlock->key[i] = kEY_MIN;                                                                 // keys in left are intialized as 0

            else                                                                                                
                curBlock->key[i] = kEY_MAX;                                                                 // keys in right are intialized as MAX(include pivot)                                                             
                                                       
            curBlock->childBlock[i] = NULL;
        }

        curBlock->childBlock[getOrder()] = NULL; 
}

uint64_t WAbpt::getNumSlot(VirNode *curBlock)
{
        return curBlock->numSlot;
}
        
void WAbpt::setBoundaryShift(VirNode *curBlock, int64_t value)
{       
        int pos = getBoundaryPos(curBlock);
        pcm.accessKey(getNumSlot(curBlock), pos * 2 + 1);
        curBlock->boudary_shift_pos = value;
}
        
int64_t WAbpt::getBoundaryShift(VirNode *curBlock)
{
        return curBlock->boudary_shift_pos;
}

void WAbpt::setKey(VirNode *curBlock, int pos, uint64_t value)                                                       
{
        pos = getRealPosition(pos);
        pcm.accessKey(getNumSlot(curBlock), pos * 2 + 1);
        curBlock->key[pos] = value;
}
       
uint64_t WAbpt::getKey(VirNode *curBlock, int pos)                                                                              
{
        pos = getRealPosition(pos);
        return curBlock->key[pos];
}

void WAbpt::setPointer(VirNode *curBlock, int pos, VirNode *ptr)
{
        if(pos < 0 || pos > getOrder())
            cout << "Error: the wrong position to set the pointer " << pos << endl << endl;

        pcm.accessPointer(getNumSlot(curBlock), pos * 2);
        curBlock->childBlock[pos] = ptr;
}
        
VirNode * WAbpt::getPointer(VirNode *curBlock, int pos)
{
        if(pos < 0 || pos > getOrder())
            cout << "Error: the wrong position to get the pointer " << pos << endl << endl;

        return curBlock->childBlock[pos];
}

void WAbpt::setParent(VirNode *curBlock, VirNode *ptr)
{
        curBlock->parentBlock = ptr;
}

VirNode * WAbpt::getParent(VirNode *curBlock)
{
        return curBlock->parentBlock;
}

int WAbpt::findMinPosition(VirNode *curBlock)
{
        uint64_t val;
        int pivot = getPivotPtr(curBlock);
        int64_t boundary = getBoundaryShift(curBlock);

        if(boundary == -1)                                                                                     
        {   
            for(int i = 0; i < getOrder(); i++)
            { 
                if(getKey(curBlock, i) != kEY_MIN && getKey(curBlock, i) != kEY_MAX)
                    return i;
            }
        }
        
        else
        {   
            if(boundary > pivot)                                                                                // left full
                return boundary - getOrder();

            else                                                                                                // right full
            {
                for(int i = boundary + 1; i <= pivot; i++)
                { 
                    if(getKey(curBlock, i) != kEY_MIN && getKey(curBlock, i) != kEY_MAX)
                        return i;
                }
            } 
        }
}

int WAbpt::findMaxPosition(VirNode *curBlock)
{
        uint64_t val;
        int pivot = getPivotPtr(curBlock);
        int64_t boundary = getBoundaryShift(curBlock);

        if(boundary == -1)                                                                                     
        {   
            for(int i = getOrder() - 1; i >= 0; i--)
            { 
                if(getKey(curBlock, i) != kEY_MIN && getKey(curBlock, i) != kEY_MAX)
                    return i;
            }
        }
        
        else
        {   
            if(boundary > pivot)                                                                                // left full
            {
                for(int i = boundary - 1; i >= pivot; i--)
                { 
                    if(getKey(curBlock, i) != kEY_MIN && getKey(curBlock, i) != kEY_MAX)
                        return i;
                }
            } 

            else                                                                                                // right full
                return boundary + getOrder();
        }
}

int WAbpt::getPivotPtr(VirNode *curBlock)
{
        int pivotPtr = getOrder() / 2;
        for(int i = 0; i < pcm.getSlotAlloCount(getNumSlot(curBlock)) - 1; i++)
        {
            pivotPtr += getPivotShift();
            if(pivotPtr >= getOrder())                                                                 
                pivotPtr -= getOrder();
        }

        return pivotPtr;
}

int WAbpt::getBoundaryPos(VirNode *curBlock)
{
        int pivotPtr = getPivotPtr(curBlock), pos = pivotPtr + getOrder() / 2;
        if(pos >= getOrder())
            pos -= getOrder();

        return pos;
}
   
void WAbpt::initNode(VirNode *curBlock)
{       
        curBlock->key = new uint64_t [getOrder()];
        if(!curBlock->key)
            cout << "\nError: fail to allocate keys" << endl << endl;

        curBlock->childBlock = new VirNode *[getOrder() + 1];
        if(!curBlock->childBlock)
            cout << "\nError: fail to allocate childBlocks" << endl << endl;
    
        setNumSlot(curBlock, pcm.allocateSlot());
        setNodeNums(getNodeNums() + 1);        
}

void WAbpt::freeNode(VirNode *curBlock, int error_checking)
{
        pcm.freeSlot(curBlock->numSlot);
        curBlock->numSlot = curBlock->keyNums = 0;
        curBlock->boudary_shift_pos = -1;

        curBlock->parentBlock = NULL;
        delete[] curBlock->key;

        for(int i = 0; i < getOrder() + 1; i++)    
        {
            curBlock->childBlock[i] = NULL;
            delete curBlock->childBlock[i];
        }   
                    
        delete[] curBlock->childBlock;
        delete curBlock;
        curBlock = NULL;
        setNodeNums(getNodeNums() - 1);   
}

void WAbpt::resetNode(VirNode *curBlock, uint64_t newSlot)
{
        int count = 0,  min = findMinPosition(curBlock), max = findMaxPosition(curBlock);
        uint64_t keyNums = getKeyNums(curBlock), *keys = new uint64_t [keyNums];
        VirNode *parentBlock = getParent(curBlock);

        for(int i = (min + max) / 2; i >= min; i--)
            keys[count++] = getKey(curBlock, i);

        for(int i = (min + max) / 2 + 1; i <= max; i++)
            keys[count++] = getKey(curBlock, i);

        VirNode **childs = new VirNode *[keyNums + 1];
        for(int i = 0; i <= keyNums; i++)
            childs[i] = getPointer(curBlock, i);

        pcm.freeSlot(curBlock->numSlot);
        setNumSlot(curBlock, newSlot);
        setParent(curBlock, parentBlock); 
      
        for(int i = 0; i <= keyNums; i++)
        {     
            if(i < keyNums)
                insertKey(curBlock, keys[i]); 

            setPointer(curBlock, i, childs[i]);
            childs[i] = NULL;
            delete childs[i];
        }

        delete[] childs;
        delete[] keys;
}

bool WAbpt::checkIsLeaf(VirNode *curBlock)
{
        if(getPointer(curBlock, 0) == NULL)
        {              
            return true;
        }

        return false;       
}

bool WAbpt::checkIsNoKey(VirNode *curBlock)
{
        if(getKeyNums(curBlock) == 0)
            return true;
        
        else
            return false;
}

bool WAbpt::checkIsRoot(VirNode *curBlock)
{
        if(getParent(curBlock) == NULL)
            return true;
        
        else
            return false;
}
