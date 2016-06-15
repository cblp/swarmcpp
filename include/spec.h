//
// Created by gritzko on 6/13/16.
//
#ifndef SWARMCPP_SPEC_H
#define SWARMCPP_SPEC_H
#include "stamp.h"

namespace swarm {

    struct spec_t {

        stamp_t tok[4];

        const stamp_t& type () const {
            return tok[0];
        }
        const stamp_t& id () const {
            return tok[1];
        }
        const stamp_t& stamp () const {
            return tok[2];
        }
        const stamp_t& name () const {
            return tok[3];
        }

        spec_t () {
            bzero(tok, SIZEOF);
        }

        spec_t (const spec_t& b) {
            memcpy(tok, b.tok, SIZEOF);
        }

        spec_t (const char* str, int length) {
            parser_t p;
            p.scan(*this, str, length);
        }

        spec_t (const char * str) {
            parser_t p;
            p.scan(*this, str, strlen(str)+1);
        }

        bool operator == (const spec_t& b) const {
            return memcmp(tok, b.tok, SIZEOF);
        }

        bool isNoop () const {
            return name()==stamp_t::ZERO;
        }

        operator std::string () const {
            char ret[MAX_CHARS];
            parser_t p;
            int len = p.print(*this, ret, MAX_CHARS);
            return std::string(ret,len);
        }

        static const stamp_t ON;
        static const char* QUANTS;
        static const int SIZEOF = sizeof(stamp_t)*4;
        static const int MAX_CHARS = (stamp_t::MAX_CHARS+1)*4;

        struct parser_t {
            int phase;
            int quant;
            stamp_t memory[4];
            stamp_t::parser_t parser;
            parser_t () : phase(0), quant(0), parser() {}
            int scan(spec_t &target, const char *buf, int length);
            int print(const spec_t &target, char *buf, int length);
        };
    };

    const char* spec_t::QUANTS = "/#!.";
    const stamp_t spec_t::ON("on");

    int spec_t::parser_t::scan(spec_t &target, const char *buf, int length) {
        int offset = 0;
        while (offset<length) {
            if (phase==0) {
                char q = buf[offset];
                const char* qp = strchr(QUANTS, q);
                if (qp==0) return -1;
                if (qp-QUANTS<quant) return -2;
                quant = qp-QUANTS;
                memory[quant] = stamp_t::ZERO;
                phase = 1;
                offset++;
            }
            parser = stamp_t::parser_t();
            int ret = parser.scan(memory[quant], buf+offset, length-offset);
            if (ret<0) {
                target.tok[quant] = stamp_t::ERROR;
                return -3;
            }
            offset+=ret;
            if (offset<length) {
                quant++;
                phase = 0;
                if (quant==4) {
                    quant = 0;
                    memcpy(target.tok, memory, sizeof(memory));
                    break;
                }
            }
        }
        return offset;
    }

    int spec_t::parser_t::print(const spec_t &target, char *buf, int length) {
        int offset = 0;
        while (offset<length && quant<4) {
            if (phase==0) {
                if (memory[quant]==target.tok[quant]) {
                    quant++;
                    continue;
                }
                buf[offset++] = QUANTS[quant];
                phase = 1;
                if (offset==length) break;
            }
            offset += parser.print(target.tok[quant], buf+offset, length-offset);
            if (offset<length) {
                quant++;
                phase = 0;
            }
        }
        if (quant==4) {
            quant = 0;
            memcpy(memory, target.tok, sizeof(memory));
        }
        return offset;
    }

}

#endif //SWARMCPP_SPEC_H
