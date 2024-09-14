#include<iostream>

#ifndef LRU_HPP
#define LRU_HPP
#include<mutex>

//#define MAX_CACHE (1<<11)


struct Node
{
  char *data;
  char *url;
  int len;
  struct Node *next=NULL;
};

class LRU
{

  struct Node *head=NULL;
  long int capacity=0;
  public:
  std::mutex lack;
  LRU();
  
  void add(char*url,int url_bytes,char*data,int data_bytes);
  
  void remove_ele();

  char* find(char*url);
};
#endif