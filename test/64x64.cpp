//
// Created by gritzko on 6/7/16.
//
#include <assert.h>
#include "../include/64x64.h"

using namespace swarm;
//using namespace std;

int main (int argn, char** args) {

    assert(sizeof(base_t)== sizeof(uint64_t));

    assert(base2long("0")==0);
    assert(base2long("~")==base_t::INFINITY);

    assert(std::string(base_t(0))=="0");
    assert(std::string(base_t::INFINITY)=="~");

    uint64_t onval = 932808072819113984L;
    assert(base_t("on")==onval);
    assert(base_t("on00")==onval);
    assert(std::string(base_t(onval))=="on");

    char buf[10];
    base_t::print_t pstate;
    base_t::scan_t sstate;

    for(base_t i=0; i<1+64*64; ++i) {

        bzero(&sstate, sizeof(sstate));
        bzero(&pstate,sizeof(pstate));

        int len = i.print(pstate, buf, 10);
        assert(len<=10);
        assert(len>0);

        base_t j;
        int len2 = j.scan(sstate, buf, len);
        assert(len2==len);
        assert(i==j);

        buf[len] = 0;
        printf("%s\n", buf);

    }
}
