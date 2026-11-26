#ifndef FUNCTORS_H
#define FUNCTORS_H
#include "Mission.h"

// Ordering functor: returns true when 'a' must be served BEFORE 'b'.
// Urgent medical first, then higher priority, then the older mission (lower id).
struct UrgentMedicalFirst {
    bool operator()(const Mission& a,const Mission& b) const {
        if(a.isMedical()!=b.isMedical()) return a.isMedical()>b.isMedical();
        if(a.getPriority()!=b.getPriority()) return a.getPriority()>b.getPriority();
        return a.getId()<b.getId();
    }
};
#endif
