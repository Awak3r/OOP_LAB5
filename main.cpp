#include "include/list.h"
#include "include/memory.h"
#include <iostream>


int main()
{
    CustomMemoryResource customMemory;  
    DoubleLinkedList<int> list(&customMemory);  
    //сосо
}
