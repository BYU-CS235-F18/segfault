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