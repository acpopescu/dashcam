#pragma once

template<typename T>
class myModel
{
public:
    myModel() {myMemory = nullptr; mySize = 0;}

    void push_back(T val)
    {
        resize(mySize + 1);
        myMemory[mySize] = val;
        mySize++;
    }

    void resize(int size)
    {
        if(mySize!=size)
        {
            T* newMemory = new T[size];
            for(int i =0;i<mySize;i++)
            {
                newMemory[i] = std::move(myMemory[i]);
            }
            delete [] myMemory;
            myMemory = newMemory;
            mySize = size;
        }
    }

    typedef T* Aiterator;
    Aiterator Abegin() const { return myMemory; }
    Aiterator Aend() const { return myMemory+mySize; }
    void erase(Aiterator it) 
    {
        for(Aiterator k = it, nextK = it+1; nextK<Aend();k++, nextK++)
        {
            *k = std::move(*nextK);
        }
        mySize--;
    }

    T& operator[]  (int size) { return myMemory[size]; }

private:
    T * myMemory;
    int mySize;
};