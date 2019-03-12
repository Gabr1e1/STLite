# STLite

## Priority_Queue

### Implementation : Pairing Heap

### Things to notice: 
+ Implementation of the Pop() operation: first left to right merging every 2 nodes and then left to right making a forest into a tree
+ Needn't save father(?)

## Deque
### Implementation : Linked list nested with linked list

### Things to notice:
+ TODO: size_t not used for simplicity
+ TODO: when erase an element no delete ✔
+ ~~TODO: iterator + can be faster~~
+ TODO: doesn't need to make sure that the iterator is always valid ✔

### Things worth remembering:

+ ~~模板类里面继承就要很小心，因为没有实例化导致base class的对象就莫得，继承不下来；但是base class里面的函数就没事~~ 这个问题比较复杂，涉及到template中的是否dependent的问题，具体可以看我自己在stackoverflow上的提问，简单来说就是有些时候必须让编译器觉得你这个东西是dependent的，这样才会去别的地方解析这个名字