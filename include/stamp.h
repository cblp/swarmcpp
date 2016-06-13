//
// Created by gritzko on 6/13/16.
//

#ifndef SWARMCPP_STAMP_H
#define SWARMCPP_STAMP_H

#include <cstring>
#include "64x64.h"

namespace swarm {

    struct stamp_t {
        base_t  time;
        base_t  origin;

        stamp_t () : time(), origin() {}
        stamp_t (const stamp_t& copy) : time (copy.time), origin(copy.origin) {}

        stamp_t (const char* stamp) : time(), origin() {
            parser_t parser;
            parser.scan(*this, stamp, (int)strlen(stamp)+1);
        }
        stamp_t (std::string stamp) {
            parser_t parser;
            parser.scan(*this, stamp.c_str(), (int)stamp.length());
        }

        stamp_t (tm datetime, base_t orig) : origin(orig) {
            int year = datetime.tm_year - 110;
            if (year<0) {
                time = base_t::INCORRECT;
                return;
            }
            int month = year*12 + datetime.tm_mon;
            uint64_t t = (uint64_t)month;
            t <<= 6;
            t |= datetime.tm_mday-1;
            t <<= 6;
            t |= datetime.tm_hour;
            t <<= 6;
            t |= datetime.tm_min;
            t <<= 6;
            t |= datetime.tm_sec;
            t <<= 4*6; // msec, seq
            time = t;
        }

        bool operator == (const stamp_t& b) const {
            return time==b.time && origin==b.origin;
        }
        bool operator < (const stamp_t& b) const {
            return time<b.time || (time==b.time && origin<b.origin);
        }
        bool operator > (const stamp_t& b) const {
            return time>b.time || (time==b.time && origin>b.origin);
        }
        bool operator <= (const stamp_t& b) const {
            return time<b.time || (time==b.time && origin<=b.origin);
        }
        bool operator >= (const stamp_t& b) const {
            return time>b.time || (time==b.time && origin>=b.origin);
        }

        operator std::string () const {
            char buf[22];
            parser_t parser;
            int length = parser.print(*this, buf, 22);
            return std::string(buf, (size_t)length);
        }

        bool isTranscendent () const {
            return origin.isZero() || origin.isAbnormal();
        }
        bool isAbnormal () const {
            return time.isAbnormal();
        }
        bool isError () const {
            return time==base_t::INCORRECT;
        }
        bool isZero () const {
            return time.isZero();
        }
        bool isNever () const {
            return time.isInfinity();
        }

        tm datetime () const {
            tm ret;
            bzero(&ret, sizeof(ret));
            uint64_t t = time.value;
            t >>= 4*6; // ms, seq
            int mask = 63;
            ret.tm_sec = int(t)&mask;
            t >>= 6;
            ret.tm_min = int(t)&mask;
            t >>= 6;
            ret.tm_hour = int(t)&mask;
            t >>= 6;
            ret.tm_mday = 1 + (int(t)&mask);
            t >>= 6;
            int months = int(t);
            ret.tm_mon = months%12;
            ret.tm_year = (months/12) + 110;
            time_t epoch = mktime(&ret);
            tm utc = *gmtime(&epoch);
            return utc;
        }

        bool isParentOf (const stamp_t& child) const {
            if (isTranscendent()) return false;
            uint64_t a=origin.value, b=child.origin.value;
            while ( (a&63)==0 ) {
                a >>= 6;
                b >>= 6;
            }
            return a==b;
        }

        struct parser_t {
            int stage;
            base_t::parser_t parser;
            parser_t () : stage(0), parser() {}
            int scan(stamp_t &target, const char *buf, int length);
            int print(const stamp_t &target, char *buf, int length);
        };


    };

    int stamp_t::parser_t::scan(stamp_t &target, const char *buf, int length) {
        int offset = 0;
        if (stage==0) {
            offset = parser.scan(target.time, buf, length);
            if (offset<0) {
                return offset;
            } else if (offset==length) {
                return offset;
            }
            parser = base_t::parser_t();
            stage = 1;
        }
        if (stage==1) {
            if (buf[offset]!='+') {
                target.origin = base_t();
                return offset;
            }
            offset++;
            stage = 2;
        }
        if (stage==2) {
            offset += parser.scan(target.origin, buf+offset, length-offset);
            if (offset<0) {
                return offset;
            } else if (offset==length) {
                return offset;
            }
            stage = 0;
            parser = base_t::parser_t();
        }
        return offset;
    }

    int stamp_t::parser_t::print(const stamp_t &target, char *buf, int length) {
        int offset = 0;
        if (stage==0) {
            offset = parser.print(target.time, buf, length);
            if (offset<0) {
                return offset;
            } else if (offset==length) {
                return offset;
            }
            parser = base_t::parser_t();
            stage = 1;
        }
        if (stage==1) {
            if (target.origin.isZero()) {
                stage = 0;
                parser = base_t::parser_t();
                return offset;
            }
            buf[offset++] = '+';
            stage = 2;
        }
        if (stage==2) {
            offset += parser.print(target.origin, buf+offset, length-offset);
            if (offset<0) {
                return offset;
            } else if (offset==length) {
                return offset;
            }
            parser = base_t::parser_t();
            stage = 0;
        }
        return offset;
    }

}

#endif //SWARMCPP_STAMP_H
