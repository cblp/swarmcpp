//
// Created by gritzko on 6/16/16.
//
#include "../include/swarm.h"
#include <assert.h>

using namespace swarm;
using namespace std;

int main (int argn, char** args) {

    char buf[1024];
    bzero(buf, 1024);

    op_t::parser_t parser;
    op_t::parser_t writer(op_t::parser_t::HUMAN);

//    ; empty-value op
    const char* str1 = "/Swarm#database!0.on\n";
    op_t hs;
    int ret = parser.scan(hs, str1, strlen(str1)+1);
    assert(hs.name()==op_t::ON);
    assert(hs.stamp().isZero());
    assert(hs.isEmpty());
    assert(hs.value.get()==0);
    assert(ret==strlen(str1));
    assert(parser.stage==0);
    ret = writer.print(hs, buf, 1024);
    assert(ret>0);
    buf[ret] = 0;
    printf("%s", buf);

//    ; single line value op
    const char* str2 = "/Object#1CQZ38+Y~!1CQa4+Xusernm1Q.NumbrField 123\n";
    op_t line;
    ret = parser.scan (line, str2, strlen(str2));
    assert(ret>0);
    assert(line.size==3);
    assert(string("123")==string(line.value.get(),3));
    assert(parser.stage==0);

    ret = writer.print(line, buf, 1024);
    assert(ret>0);
    buf[ret] = 0;
    printf("%s", buf);

//
//    ; multiline op (chekc defaults, same object)
    const char* str3 =
        "\n\n\n"\
        "/Swarm#database!1CQAneD1+X~.~=\n"\
        "\t!1CQAneD+X~.Access OwnWriteAllRead\n"\
        " !1CQAneD1+X~.ReplicaIdScheme 163\n"\
        "!1CQa5+Xusernm1Q.Multiline=\n"\
        " First line\n"\
        " Second line\n"\
        " \tIndented third line\n\n"; // FIXME \n\n after multilines

    op_t state, multi;
    ret = parser.scan(state, str3, strlen(str3));
    assert(ret>0);
    assert(parser.stage==0);
    int ret2 = parser.scan(multi, str3+ret, strlen(str3)-ret);
    assert(parser.stage==0);
    assert(state.isSameObject(multi));
    assert(state.name()==op_t::STATE);

    ret = writer.print(state, buf, 1024);
    assert(ret>0);
    ret += writer.print(multi, buf+ret, 1024-ret);
    buf[ret] = 0;
    printf("%s", buf);

//    ; explicit length op
    const char* str4 = "!1CQa6+Xusernm1Q.Buffer=A\n";
    const char* str5 = "raw buffer\n";
    op_t raw;
    ret = parser.scan(raw, str4, strlen(str4));
    assert(ret>0);
    ret = parser.scan(raw, str5, strlen(str5));
    assert(ret>0);
    assert(raw.size==10);
    assert(0==memcmp(raw.value.get(), str5, 10));

    writer.humanMode = op_t::parser_t::ALL_FIXED;
    ret = writer.print(raw, buf, 1024);
    assert(ret>0);
    buf[ret]= 0;
    printf("%s\n", buf);

    return 0;
}
