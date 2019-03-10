# STLite
## Priority_Queue
### Implementation : Pairing Heap

### Things to notice: 
+ Implementation of the Pop() operation: first left to right merging every 2 nodes and then left to right making a forest into a tree
+ Needn't save father(?)

## Deque
### Implementation : Linked list nested with linked list

### Things to notice:
+ iterator and const_iterator implementation very ugly
+ size_t not used for simplicity
+ TODO : when erase an element no delete
+ TODO : iterator + can be faster