//
// Created by gritzko on 6/16/16.
//

#ifndef SWARMCPP_OP_H
#define SWARMCPP_OP_H

#include <stdint.h>
#include <string>
#include <cstring>
#include <memory>
#include <cstdlib>

#include "spec.h"

namespace swarm {

    struct op_t {

        typedef std::shared_ptr<char> value_t;

        spec_t spec;
        int     size;
        value_t value;

        op_t () : spec(), value(), size() {}

        op_t  (const op_t& b) : spec(b.spec), value(b.value), size(b.size) {}

        op_t (char* buf, int length) {
            parser_t parser;
            parser.scan(*this, buf, length);
        }

        operator std::string () const {
            char *buf = (char*) malloc(size+64);
            parser_t printer(parser_t::mode_t::HUMAN);
            int len = printer.print(*this, buf, size+64);
            std::string ret = std::string(buf, len);
            free(buf);
            return ret;
        }

        static const int MAX_VALUE_SIZE = 1<<24; // 16MB?
        static const stamp_t ON;
        static const stamp_t STATE;

        bool isEmpty () const {return size==0;}

        bool isSameObject (const op_t& b) const {
            return spec.isSameObject(b.spec);
        }

        const stamp_t& type () const {
            return spec.type();
        }

        const stamp_t& id () const {
            return spec.id();
        }

        const stamp_t& stamp () const {
            return spec.stamp();
        }

        const stamp_t& name () const {
            return spec.name();
        }

        struct parser_t {

            enum class stage_t {
                SKIP=0,
                SPEC=1,
                SEPAR=2,
                LINE=3,
                MULTILINE=4,
                NL=5,
                BUF_SIZE=6,
                BUFFER=7,
                EOV=8
            };

            enum class mode_t {
                HUMAN=0,
                ALL_FIXED=1,
                ADAPT=2
            };

            stage_t stage;
            mode_t humanMode;
            spec_t::parser_t     spec_parser;
            int  value_pos;

            parser_t () :
                    stage(SKIP), spec_parser(), humanMode(HUMAN) {}
            parser_t (mode_t mode) :
                    stage(SKIP), spec_parser(), humanMode(mode) {}

            static const int NESTED_ERROR = -100;
            static const int SEPARATOR_ERROR = -2;
            static const int VALUE_LENGTH_ERROR = -3;

            int scan(op_t& target, const char *buf, size_t length);
            int print(const op_t& target, char *buf, size_t length);

            int append_bytes (op_t& target, const char* buf, size_t size);
        };
    };

    const stamp_t op_t::ON = stamp_t("on");
    const stamp_t op_t::STATE = stamp_t("~");

    int op_t::parser_t::append_bytes (op_t& target, const char* buf, size_t size) {
        int free_bytes = value_pos - target.size;
        if (free_bytes<size) {
            value_pos<<=1;
            target.value = value_t
                    ((char*)realloc(target.value.get(), value_pos));
        }
        memcpy(target.value.get()+target.size, buf, size);
        target.size += size;
    }

    int op_t::parser_t::scan(op_t& target, const char *buf, size_t length) {
        size_t offset = 0;
        int ret, take;
        while (offset < length) {

            //printf("%i %i %s\n", offset, stage, std::string(buf,offset).c_str());

            switch (stage) {

                case SKIP:
                    target.size = 0;

                    if (buf[offset]=='\n')
                        offset++;
                    else
                        stage = SPEC;
                    break;

                case SPEC:
                    ret = spec_parser.scan(target.spec, buf+offset, length-offset);
                    if (ret<0) return NESTED_ERROR+ret;
                    offset += ret;
                    if (offset<length)
                        stage = SEPAR;
                    break;

                case SEPAR:
                    target.size = 0;
                    switch (buf[offset]) {
                        case ' ':
                        case '\t':
                            stage = LINE;
                            value_pos = 16;
                            target.value = value_t((char*)malloc(value_pos));
                            break;
                        case '\n':
                            stage = SKIP;
                            value_pos = 0;
                            target.size = 0;
                            target.value = value_t(NULL);
                            return offset+1;
                        case '=':
                            value_pos = 0;
                            target.size = 0;
                            stage = BUF_SIZE;
                            break;
                        default:
                            return SEPARATOR_ERROR;
                    }
                    offset++;
                    break;

                case LINE:
                    if (buf[offset]=='\n') {
                        stage = SKIP;
                        return offset;
                    } else {
                        append_bytes(target, buf+offset, 1);
                        offset++;
                    }
                    break;

                case MULTILINE:
                    if (buf[offset]=='\n') {
                        stage = NL;
                    }
                    append_bytes(target, buf+offset, 1);
                    offset++;
                    break;

                case NL:
                    if (buf[offset]=='\t' || buf[offset]==' ') {
                        offset++;
                        stage = MULTILINE;
                    } else {
                        // NO offset++;
                        stage = SKIP;
                        return offset;
                    }
                    break;

                case BUF_SIZE:
                    if (buf[offset]=='\n') {
                        if (value_pos==0) {
                            stage = NL;
                            value_pos = 256;
                            target.value = value_t((char*)malloc(value_pos));
                        } else {
                            stage = BUFFER;
                            target.value = value_t((char*)malloc(target.size));
                        }
                    } else {
                        value_pos <<= 6; // FIXME SEPARATE EM, DEFINE EM
                        if (buf[offset]<0) return VALUE_LENGTH_ERROR;
                        int8_t i = base_t::CHAR2INT[buf[offset]];
                        if (i==-1) return VALUE_LENGTH_ERROR;
                        value_pos |= i;
                        if (value_pos>MAX_VALUE_SIZE) return VALUE_LENGTH_ERROR;
                    }
                    offset++;
                    break;

                case BUFFER:
                    size_t take = length-offset;
                    size_t take2 = value_pos-target.size;
                    if (take2<take) take=take2;
                    //printf("!!%s!%s,%i,%i\n", buf+offset, target.value.get(), target.size, value_pos);
                    append_bytes(target, buf+offset, take);
                    offset += take;
                    if (value_pos==target.size) {
                        stage = SKIP;
                        return offset;
                    }
                    break;

            }
        }
    }

    int op_t::parser_t::print(const op_t& target, char *buf, size_t length) {
        size_t offset = 0, fits, fits2;
        int ret, has_nl, i;
        char next;
        while (offset<length) {

            //printf("> %i %i %s\n", offset, stage, std::string(buf+offset, length-offset).c_str());

            switch (stage) {

                case SKIP:
                    stage = SPEC; // no break;
                case SPEC:
                    ret = spec_parser.print(target.spec, buf+offset, length-offset);
                    if (ret<0) return NESTED_ERROR + ret;
                    offset+=ret;
                    if (offset<length)
                        stage = SEPAR;
                    break;

                case SEPAR:
                    if (target.isEmpty()) {
                        stage = EOV;
                        continue;
                    }
                    value_pos = 0;
                    has_nl = (0!=memchr(target.value.get(), '\n', target.size));
                    //printf("...%i %i\n", target.size, has_nl);
                    if (!has_nl && humanMode!=ALL_FIXED) {
                        stage = LINE;
                        buf[offset] = '\t';
                    } else {
                        buf[offset] = '=';
                        value_pos=0;
                        fits=target.size;
                        do {
                            value_pos++;
                            fits>>=6;
                        } while (fits!=0);
                        stage = BUF_SIZE;
                    }
                    offset++;
                    break;

                case LINE:
                    buf[offset++] = target.value.get()[value_pos++];
                    if (value_pos==target.size)
                        stage = EOV;
                    break;

                case MULTILINE:
                    // by one char
                    next = target.value.get()[value_pos++];
                    buf[offset++] = next;
                    if (next=='\n') {
                        stage = NL;
                    }
                    if (value_pos==target.size) {
                        stage = EOV;
                    }
                    break;

                case NL:
                    buf[offset++] = '\t';
                    stage = MULTILINE;
                    break;

                case BUF_SIZE:
                    if (humanMode==HUMAN) {
                        buf[offset++] = '\n';
                        stage = NL;
                        value_pos = 0;
                    } else {
                        if (value_pos == 0) {
                            stage = BUFFER;
                            buf[offset++] = '\n';
                        } else {
                            i = 63 & (target.size >> (value_pos*6-6));
                            buf[offset++] = base_t::INT2CHAR[i];
                            value_pos--;
                        }
                    }
                    break;

                case BUFFER:
                    fits = length-offset;
                    fits2 = target.size-value_pos;
                    if (fits2<fits) fits=fits2;
                    memcpy(buf+offset, target.value.get(), fits);
                    offset += fits;
                    value_pos += fits;
                    if (value_pos==target.size) {
                        stage = EOV;
                    }
                    break;

                case EOV:
                    buf[offset++] = '\n';
                    stage = SKIP;
                    return offset;

            }
        }
    }


}
#endif //SWARMCPP_OP_H
