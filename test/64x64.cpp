//
// Created by gritzko on 6/7/16.
//
#include <assert.h>
#include <inttypes.h>
#include "../include/64x64.h"

using namespace swarm;
using namespace std;

int main (int argn, char** args) {

    assert(sizeof(base_t)== sizeof(uint64_t));

    assert( (base_t("0")) == ((base_t)0L) );
    assert(base_t("~")==base_t::INFINITY.value);

    base_t zero;
    string z(zero);
    printf("%s\n", ((string)zero).c_str());
    printf("%" PRIu64 "\n", base_t("0").value);

    assert(std::string(base_t(0L))=="0");
    assert(std::string(base_t::INFINITY)=="~");

    printf("%s\n", ((string)base_t::INFINITY).c_str());
    printf("%" PRIu64 "\n", base_t::INFINITY.value);
    base_t inf1 = base_t(base_t::INFINITY.value+1L);
    printf("%s\n", string(inf1).c_str());
    printf("%" PRIu64 "\n", inf1.value);

    base_t on("on");

    printf("%" PRIu64 "\n", on.value);
    printf("%s\n", string(on).c_str());

    base_t on00("on00");
    printf("%" PRIu64 "\n", on00.value);

    uint64_t onval = 932808072819113984L;
    assert(base_t("on")==onval);
    assert(base_t("on00")==onval);
    assert(std::string(base_t(onval))=="on");

    for(base_t i=0L; i<=4096L; ++i) {

        char buf[11];
        base_t::parser_t parser;
        int len = parser.print(i, buf, 10);
        assert(len<=10);
        assert(len>0);

        base_t::parser_t scanner;
        base_t j;
        int len2 = scanner.scan(j, buf, len);
        assert(len2==len);
        assert(i==j);

        buf[len] = 0;
        printf("%s\n", buf);

    }
    
}
