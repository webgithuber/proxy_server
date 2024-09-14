#include<iostream>
#include<thread>
#include<bits/stdc++.h>

using namespace std;
void fun(int t)
{
    cout<<this_thread::get_id()<<" "<<t<<"\n"; 
    
}

int main()
{
    vector<thread>pool(5);
    for (int i = 0; i < 5; i++)
    {
        pool[i]=thread(fun,i);
    }
    for (size_t i = 0; i < 5; i++)
    {
        pool[i].join();
    }
    
    return 0;
}