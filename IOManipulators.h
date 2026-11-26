#ifndef IOMANIPULATORS_H
#define IOMANIPULATORS_H
#include <iostream>
#include <iomanip>

inline std::ostream& manifestHeader(std::ostream& out){
    out<<std::left<<std::setw(10)<<"ID"
       <<std::setw(28)<<"SUPPLY"
       <<std::setw(18)<<"CATEGORY"
       <<std::setw(10)<<"QTY"
       <<std::setw(12)<<"UNIT WT"<<'\n';
    return out;
}
#endif
