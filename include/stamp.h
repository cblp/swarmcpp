//
// Created by gritzko on 6/13/16.
//

#ifndef SWARMCPP_STAMP_H
#define SWARMCPP_STAMP_H

#include <cstring>
#include "64x64.h"

// TODO
// [ ] size_t for scan/print
// [ ] scan/print methods of scan_t

namespace swarm {

    struct stamp_t {
        base_t  time;
        base_t  origin;

        stamp_t () : time(), origin() {}
        stamp_t (const stamp_t& copy) : time (copy.time), origin(copy.origin) {}

        stamp_t (const char* stamp) : time(), origin() {
            scan_t state;
            scan(state, stamp, (int)strlen(stamp));
        }
        stamp_t (std::string stamp) {
            scan_t state;
            scan(state, stamp.c_str(), (int)stamp.length());
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
            print_t state;
            int length = print(state, buf, 22);
            return std::string(buf, length);
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

        struct scan_t {
            scan_t () : stage(0), state() {}
            int stage;
            base_t::scan_t state;
        };
        struct print_t {
            print_t () : stage(0), state() {}
            int stage;
            base_t::print_t state;
        };

        int scan(scan_t &state, const char *buf, int length);
        int print(print_t &state, char *buf, int length) const;

    };

    int stamp_t::scan(stamp_t::scan_t &state, const char *buf, int length) {
        int offset = 0;
        if (state.stage==0) {
            offset = time.scan(state.state, buf, length);
            if (offset==length) { // FIXME invalidate on -1
                return offset;
            } else if (offset<0) {
                return offset;
            }
            state.state = base_t::scan_t();
            state.stage = 1;
        }
        if (state.stage==1) {
            if (buf[offset]!='+') {
                origin = base_t();
                return offset;
            }
            offset++;
            state.stage = 2;
        }
        if (state.stage==2) { // FIXME 0 terminator
            offset += origin.scan(state.state, buf+offset, length-offset);
            if (offset<0) { // FIXME  -1
                return offset;
            } else if (offset==length) {
                return offset;
            }
            state.stage = 0;
            state.state = base_t::scan_t();
        }
        return offset;
    }

    int stamp_t::print(stamp_t::print_t &state, char *buf, int length) const {
        int offset = 0;
        if (state.stage==0) {
            offset = time.print(state.state, buf, length);
            if (offset<0) {
                return offset;
            } else if (offset==length) {
                return offset;
            }
            state.state = base_t::print_t();
            state.stage = 1;
        }
        if (state.stage==1) {
            if (origin.isZero()) {
                state.stage = 0;
                state.state = base_t::print_t();
                return offset;
            }
            buf[offset++] = '+';
            state.stage = 2;
        }
        if (state.stage==2) {
            offset += origin.print(state.state, buf+offset, length-offset);
            if (offset<0) {
                return offset;
            } else if (offset==length) {
                return offset;
            }
            state.state = base_t::print_t();
            state.stage = 0;
        }
        return offset;
    }

}

#endif //SWARMCPP_STAMP_H
