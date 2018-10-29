# Initial Code

This is a very basic (and currently flawed) implementation of a vector of type int.

```c++
#include <iostream>
#include <sstream>

using namespace std;

/* class IntVector */
class IntVector
{
private:
    int count;
    int capacity;
    int* vector; 
public:
    IntVector()
    {
        clear();
    }
    
    ~IntVector()
    {
        clear();
    }
    
    void append(int i)
    {
        vector[count] = i;
        count++;
    }

    void clear()
    {
        delete [] vector;
        capacity = 0;
        count = 0;
    }
    
    void setCapacity(int newCap)
    {
        clear();
        capacity = newCap;
        vector = new int[capacity];
    }
    
    int size()
    {
        return count;
    }
    
    string toString()
    {
        stringstream ss;
        for(int i = 0; i < count; i++)
        {
            if(i != 0)
                ss << ", ";
            ss << vector[i];
        }
        return ss.str();
    }
}; /* END class IntVector */

// Tester
int main()
{
    IntVector ints;
    ints.append(3);
    ints.append(7);
    ints.append(10);
    cout << ints.toString() << endl;
}
```

# Step 1 - Compile and Run

Use "make" to compile and "./seg" to run.

```bash
mjcleme:~/workspace/segfault (master) $ make
g++ -o seg *.cpp
mjcleme:~/workspace/segfault (master) $ ./seg
*** Error in `./seg': double free or corruption (out): 0x0000000000400b90 ***
Aborted
```
Notice the error saying something about "free". That is likely a reference to a "delete" being used before "new".
But we can use GDB to figure out exactly where the error is happening! So let's get started.

# Step 2 - Use GDB

To use GDB on our ./seg executable, run the following in your command line terminal:

```bash
gdb ./seg
```

You'll see something like this:

```bash
mjcleme:~/workspace/segfault (master) $ gdb ./seg
GNU gdb (Ubuntu 7.7.1-0ubuntu5~14.04.3) 7.7.1
Copyright (C) 2014 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
---Type <return> to continue, or q <return> to quit---
```

Hit return to continue. Now you'll see the gdb command shell:

```bash
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./seg...(no debugging symbols found)...done.
(gdb) 
```

In the GDB command shell, type "run" and hit return. This will run the executable "./seg" that was passed into gdb.

```bash
(gdb) run
Starting program: /home/ubuntu/workspace/segfault/seg 
*** Error in `/home/ubuntu/workspace/segfault/seg': double free or corruption (out): 0x0000000000400b90 ***

Program received signal SIGABRT, Aborted.
0x00007ffff751dc37 in __GI_raise (sig=sig@entry=6) at ../nptl/sysdeps/unix/sysv/linux/raise.c:56
56      ../nptl/sysdeps/unix/sysv/linux/raise.c: No such file or directory.
(gdb) 
```

So now you'll see the same error that you saw before. The question is where in the code did you end up? So type the command "where" and hit return.

```bash
(gdb) where
#0  0x00007ffff751dc37 in __GI_raise (sig=sig@entry=6) at ../nptl/sysdeps/unix/sysv/linux/raise.c:56
#1  0x00007ffff7521028 in __GI_abort () at abort.c:89
#2  0x00007ffff755a2a4 in __libc_message (do_abort=do_abort@entry=1, fmt=fmt@entry=0x7ffff766c310 "*** Error in `%s': %s: 0x%s ***\n")
    at ../sysdeps/posix/libc_fatal.c:175
#3  0x00007ffff756682e in malloc_printerr (ptr=<optimized out>, str=0x7ffff766c440 "double free or corruption (out)", action=1) at malloc.c:4998
#4  _int_free (av=<optimized out>, p=<optimized out>, have_lock=0) at malloc.c:3842
#5  0x0000000000400e43 in IntVector::clear() ()
#6  0x0000000000400dc6 in IntVector::IntVector() ()
#7  0x0000000000400c92 in main ()
(gdb) 
```

You can see that steps #0 through #4 are from C library code. Step #5 tells you that the program halted in your "clear()" method. Excellent! Now you know to look in your clear method!

To quit GDB, use the "quit" command and then "y" when it asks you if you are sure.

# Step 3 - Fix clear() Method

You realize that you're calling delete when you don't need to. You change this:

```c++
void clear()
{
    delete [] vector;
    capacity = 0;
    count = 0;
}
```

into this:

```c++
void clear()
{
    if(capacity > 0)
        delete [] vector;
    capacity = 0;
    count = 0;
}
```

# Step 4 - Compile and Run

Compile and run again:

```bash
mjcleme:~/workspace/segfault (master) $ make clean
rm -f seg
mjcleme:~/workspace/segfault (master) $ make
g++ -o seg *.cpp
mjcleme:~/workspace/segfault (master) $ ./seg
Segmentation fault
```

Another segfault. So what do you do? GDB again!

```bash
mjcleme:~/workspace/segfault (master) $ gdb ./seg
...
(gdb) run
Starting program: /home/ubuntu/workspace/segfault/seg 

Program received signal SIGSEGV, Segmentation fault.
0x0000000000400e07 in IntVector::append(int) ()
(gdb) where
#0  0x0000000000400e07 in IntVector::append(int) ()
#1  0x0000000000400ca3 in main ()
(gdb) 
```

Now the problem is in the "append" method. Go check it out!

# Step 5 - Fix append(int i) Method

You see the current implemenation of append:

```c++
void append(int i)
{
    vector[count] = i;
    count++;
}
```

The problem must be accessing the index "count" within "vector". You forgot to do a capacity check. Throw one in!

```c++
void append(int i)
{
    if(count >= capacity)
        setCapacity(capacity*2);
    vector[count] = i;
    count++;
}
```

There we go! Now compile and run again:

```bash
mjcleme:~/workspace/segfault (master) $ make clean
rm -f seg
mjcleme:~/workspace/segfault (master) $ make
g++ -o seg *.cpp
mjcleme:~/workspace/segfault (master) $ ./seg
10
```

Confound it! It's running now, but just printing "10" instead of the expected "3, 7, 10". The program is running to completion, so now using "where" in GDB isn't going to help you find the problem.

# Step 6 - Run with Valgrind

Try running it with valgrind to see what valgrind tells you:

```bash
mjcleme:~/workspace/segfault (master) $ valgrind ./seg
==2608== Memcheck, a memory error detector
==2608== Copyright (C) 2002-2013, and GNU GPL'd, by Julian Seward et al.
==2608== Using Valgrind-3.10.1 and LibVEX; rerun with -h for copyright info
==2608== Command: ./seg
==2608== 
==2608== Conditional jump or move depends on uninitialised value(s)
==2608==    at 0x400E9D: IntVector::clear() (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400E05: IntVector::IntVector() (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400CD1: main (in /home/ubuntu/workspace/segfault/seg)
==2608== 
==2608== Invalid write of size 4
==2608==    at 0x400E74: IntVector::append(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400CE2: main (in /home/ubuntu/workspace/segfault/seg)
==2608==  Address 0x5a41c80 is 0 bytes after a block of size 0 alloc'd
==2608==    at 0x4C2B800: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==2608==    by 0x400F25: IntVector::setCapacity(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400E59: IntVector::append(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400CE2: main (in /home/ubuntu/workspace/segfault/seg)
==2608== 
==2608== Invalid write of size 4
==2608==    at 0x400E74: IntVector::append(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400CF3: main (in /home/ubuntu/workspace/segfault/seg)
==2608==  Address 0x5a41cc0 is 0 bytes after a block of size 0 alloc'd
==2608==    at 0x4C2B800: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==2608==    by 0x400F25: IntVector::setCapacity(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400E59: IntVector::append(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400CF3: main (in /home/ubuntu/workspace/segfault/seg)
==2608== 
==2608== Invalid write of size 4
==2608==    at 0x400E74: IntVector::append(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400D04: main (in /home/ubuntu/workspace/segfault/seg)
==2608==  Address 0x5a41d00 is 0 bytes after a block of size 0 alloc'd
==2608==    at 0x4C2B800: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==2608==    by 0x400F25: IntVector::setCapacity(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400E59: IntVector::append(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400D04: main (in /home/ubuntu/workspace/segfault/seg)
==2608== 
==2608== Invalid read of size 4
==2608==    at 0x400FB4: IntVector::toString() (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400D17: main (in /home/ubuntu/workspace/segfault/seg)
==2608==  Address 0x5a41d00 is 0 bytes after a block of size 0 alloc'd
==2608==    at 0x4C2B800: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==2608==    by 0x400F25: IntVector::setCapacity(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400E59: IntVector::append(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400D04: main (in /home/ubuntu/workspace/segfault/seg)
==2608== 
10
==2608== 
==2608== HEAP SUMMARY:
==2608==     in use at exit: 72,704 bytes in 4 blocks
==2608==   total heap usage: 6 allocs, 2 frees, 73,268 bytes allocated
==2608== 
==2608== LEAK SUMMARY:
==2608==    definitely lost: 0 bytes in 3 blocks
==2608==    indirectly lost: 0 bytes in 0 blocks
==2608==      possibly lost: 0 bytes in 0 blocks
==2608==    still reachable: 72,704 bytes in 1 blocks
==2608==         suppressed: 0 bytes in 0 blocks
==2608== Rerun with --leak-check=full to see details of leaked memory
==2608== 
==2608== For counts of detected and suppressed errors, rerun with: -v
==2608== Use --track-origins=yes to see where uninitialised values come from
==2608== ERROR SUMMARY: 5 errors from 5 contexts (suppressed: 0 from 0)
```

Lots of invalid reads and writes. Notice this Valgrind error:

```bash
==2608==  Address 0x5a41cc0 is 0 bytes after a block of size 0 alloc'd
==2608==    at 0x4C2B800: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==2608==    by 0x400F25: IntVector::setCapacity(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400E59: IntVector::append(int) (in /home/ubuntu/workspace/segfault/seg)
==2608==    by 0x400CF3: main (in /home/ubuntu/workspace/segfault/seg)
```

Oops. You allocated an array of size 0 in your setCapacity function. That's no good. I guess your capacity is starting at 0 and then when the append method doubles it ... it's still 0. Bummer. You need to initialize it as a positive number.

# Step 7 - Fix the Constructor

```c++
IntVector()
{
    clear();
    setCapacity(100);
}
```

Now compile and run:

```bash
mjcleme:~/workspace/segfault (master) $ make clean
rm -f seg
mjcleme:~/workspace/segfault (master) $ make
g++ -o seg *.cpp
mjcleme:~/workspace/segfault (master) $ ./seg 
3, 7, 10
```

It works! GDB and Valgrind saved the day! Or at least did something to help with debugging.

- Designed Fall 2018 by Nate Fox
