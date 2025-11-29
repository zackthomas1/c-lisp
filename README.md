
**Compile**
```
mingw32-make
```

**Run**
```
./main
```

**gdb**
```
gdb ./main
```

```
lay next
```
```
break <line number>
```

```
```
**Pointers**

**The stack and the Heap**

The stack is the memory where the program lives. where temporary variables and data structures exist.
Every time a function is callled a neww area of the stack is put aside for it to use.

The heap is section of meory put aside for storage of objects with a long lifespan.
Memory in the heap must be manually allocated and deallocated using malloc and free.
Memory leak continuously allocating memory without freeing it.


