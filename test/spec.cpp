//
// Created by gritzko on 6/15/16.
//
#include "../include/spec.h"
#include <assert.h>

using namespace swarm;
using namespace std;

int main (int argn, char** args) {

    char buf[1024];
    bzero(buf,1024);
    //; zero spec (noop)
    ///0#0!0.0
    spec_t zero;
    assert(zero.isNoop());
    assert(zero.type().isZero());
    assert(zero.id().isZero());
    assert(zero.stamp().isZero());
    assert(zero.name().isZero());
    spec_t::parser_t p;
    p.print(zero, buf, 1024);
    printf("%s\n", buf);

    //; read-only subscription, all transcendents
    spec_t simple_on ("/Object#id!~.on");
    assert(simple_on.stamp().isNever());
    printf("%s\n", string(simple_on).c_str());

    //; initial state (normalize the tailing 0)
    spec_t state("/Object#1CQAn+author!1CQAn0+author.~");
    assert(state.name().isNever());
    assert(state.id()==state.stamp());
    printf("%s\n", string(state).c_str());

    //; defaults - all same object
    const char* input = "/Object#1CQAn+author!1CQAn0+author.~\n"\
                        "!1CQk5+author.field\n"\
                        ".on";
    int length = strlen(input)+1;
    spec_t::parser_t parser;
    spec_t op1, op2, op3;
    int offset = parser.scan(op1, input, length);
    assert(offset>0);
    assert(op1.id()==op1.stamp());
    offset++;

    int ret = parser.scan(op2, input+offset, length-offset);
    assert(ret>0);
    assert(op1.type()==op2.type());
    assert(op1.id()==op2.id());
    offset+=ret+1;

    ret = parser.scan(op3, input+offset, length-offset);
    assert(ret>0);
    assert(offset+ret+1==length);
    assert(op3.type()==op2.type());
    assert(op3.id()==op2.id());
    assert(op3.stamp()==op2.stamp());

    parser = spec_t::parser_t();
    offset = parser.print(op1, buf, 1024);
    assert(offset>0);
    buf[offset++] = '\n';
    ret = parser.print(op2, buf+offset, 1024-offset);
    assert(ret>0);
    offset += ret;
    buf[offset++] = '\n';
    ret = parser.print(op3, buf+offset, 1024-offset);
    assert(ret>0);
    offset+=ret;
    buf[offset++] = '\n';
    buf[offset] = 0;

    printf("%s", buf);

    //; having no defaults - fill missing tokens with zeros
    spec_t error ("/Type.op");
    assert(error.id()==stamp_t::ZERO);
    assert(error.stamp()==stamp_t::ZERO);
    // having defaults, print explicit zeros
    char newbuf[1024];
    int pos = parser.print(error, newbuf, 1024);
    newbuf[pos] = 0;
    printf("%s\n", newbuf);

}