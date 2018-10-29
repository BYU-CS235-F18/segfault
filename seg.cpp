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
        setCapacity(100);
    }
    
    ~IntVector()
    {
        clear();
    }
    
    void append(int i)
    {
        if(count >= capacity)
            setCapacity(capacity*2);
        vector[count] = i;
        count++;
    }

    void clear()
    {
        if(capacity > 0)
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