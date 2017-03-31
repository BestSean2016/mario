#ifndef PLUMBER_H
#define PLUMBER_H

namespace itat {

class plumber {
public:
    plumber() {}
    plumber(int plid) : plid_(plid) {}
    ~plumber() {}


private:
    int plid_ = 0;
};


} //namespace itat

#endif // PLUMBER_H
