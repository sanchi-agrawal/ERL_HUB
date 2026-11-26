#include "Mission.h"
#include <iostream>
Mission::Mission(int i,const std::string& l,const std::string& n,int p,double d,bool m)
:id(i),location(l),need(n),priority(p),distance(d),medical(m),completed(false){}
void Mission::display() const{
    std::cout<<"Mission #"<<id<<" | "<<location<<" | Need: "<<need
             <<" | Priority: "<<priority<<" | Distance: "<<distance
             <<" km | "<<(medical?"URGENT MEDICAL":"Standard")<<"\n";
}
