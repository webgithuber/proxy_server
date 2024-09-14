
#include "LRU.hpp"
#include<cstring>
#include<mutex>
#define MAX_CACHE (1<<20)




  
  LRU::LRU(){}
  
  void LRU::add(char*url,int url_bytes,char*data,int data_bytes){
    
    struct Node* temp=new Node;
    
    temp->url=new char[url_bytes/sizeof(char)];
    temp->data=new char[data_bytes/sizeof(char)];
    temp->len=data_bytes;
    
    for(int i=0;i<url_bytes/sizeof(char);i++)
    {
      temp->url[i]=url[i];
      //cout<<temp->url[i];
    }
    
    for(int i=0;i<data_bytes/sizeof(char);i++)
    {
      temp->data[i]=data[i];
      //cout<<temp->data[i];
    }
    
    if(this->lack.try_lock())
    {
    if(head==NULL){head=temp;}
    temp->next=head;
    head=temp;
    capacity+=32+url_bytes+data_bytes;

    while(capacity>MAX_CACHE)
    {
    
      remove_ele();
      
    }
    this->lack.unlock();
    }
    
  }
  
  void LRU::remove_ele(){
    struct Node *temp=head,*p=head;
    while(temp->next!=NULL)
    {
      p=temp;
      temp=temp->next;
    }
    p->next=NULL;
    
    capacity-=32-strlen(temp->url)-strlen(temp->data);
    delete temp;
  }
  
  
  char* LRU::find(char*url){
    
    struct Node *temp=head,*p=head;
    
    while(temp!=NULL)
    {
     
      if(strcmp(temp->url,url)==0)
      {
        
        p->next=temp->next;
        temp->next=head;
        head=temp;
        return temp->data;
        break;
      }
      p=temp;
      temp=temp->next;
    }
   
    return NULL;
  }
