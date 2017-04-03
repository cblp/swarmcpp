#ifndef SWARMCPP_64x64_H
#define SWARMCPP_64x64_H

#include <string>

#include "slice.h"

namespace swarm {

    struct base_t {
        // the number itself
        uint64_t value;

        // Constructors

        base_t(uint64_t val64) : value(val64) { }

        base_t(long int val64) : value(val64) { }

        base_t () : value (0) {}

        base_t (const char* str64) : value(0) {
            const_slice_t slice(str64);
            assert(scan(slice, 0)==result_t::DONE);
        }

        base_t (const std::string str64) {
            const_slice_t slice(str64.c_str());
            assert(scan(slice, 0)==result_t::DONE);
        }

        bool operator == (const base_t& b) const {
            return value==b.value;
        }
        bool operator <  (const base_t& b) const {
            return value<b.value;
        }
        bool operator <=  (const base_t& b) const {
            return value<=b.value;
        }
        bool operator >  (const base_t& b) const {
            return value>b.value;
        }
        bool operator >=  (const base_t& b) const {
            return value>=b.value;
        }
        bool isZero () const {
            return value == 0L;
        }
        bool isAbnormal () const {
            return value >= INFINITE.value;
        }
        bool isINFINITE () const {
            return value == INFINITE.value;
        }

        const base_t& operator ++ () {
            value++;
            return *this;
        }

        operator std::string() const {
            char str[11];
            slice_t slice(str,10);
            print(slice, 0);
            *slice = 0;
            return std::string(str, slice.from-str);
        }

        // Constants

        static const int CHAR_BITS = 6;
        static const int MAX_CHARS = 10;
        static const int MAX_BITS = CHAR_BITS * MAX_CHARS;
        static const uint64_t CHAR_BIT_MASK;
        static const base_t INFINITE;
        static const base_t INCORRECT;
        static const char INT2CHAR[];
        static const int8_t CHAR2INT[128];
        static const base_t ZERO;

        // static utils

        inline static int8_t char2int8 (uint8_t char64) { return CHAR2INT[char64]; }

        result_t scan (const_slice_t& buf, size_t bm);

        result_t print (slice_t& buf, size_t bm) const;

    };

    const base_t base_t::ZERO = base_t();
    const uint64_t base_t::CHAR_BIT_MASK = (1 << CHAR_BITS) - 1;
    const base_t base_t::INFINITE = 63L << (6 * 9);
    const base_t base_t::INCORRECT = (1L<<60)-1;
    const char base_t::INT2CHAR[65] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~";
    const int8_t base_t::CHAR2INT[] =
           {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,-1, 10,
             11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
             28, 29, 30, 31, 32, 33, 34, 35,-1,-1,-1,-1, 36,-1, 37, 38, 39, 40,
             41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
             58, 59, 60, 61, 62,-1,-1,-1, 63, -1};


    result_t base_t::scan (const_slice_t& buf, size_t pos) {
        while (!buf.empty() && pos<10) {
            char c = *buf;
            if (c<0 || c>127) break;
            const int8_t i = CHAR2INT[c];
            if (i==-1) break;
            value = (value<<6) | i;
            pos++;
            buf.skip();
        }
        if (buf.empty() && pos<10) {
            return result_t::INCOMPLETE;
        } else if (pos==0) {
            return result_t::BAD_INPUT;
        } else {
            while (pos++<10) value <<= 6;
            return result_t::DONE;
        }
    }

    result_t base_t::print (slice_t& buf, size_t bm) const {
        int len = 10;
        uint64_t v = value;
        while (!(v&63) && len>1) {
            v >>= 6;
            len--;
        }
        if (len>buf.size()) return result_t::INCOMPLETE;
        size_t i = len;
        while (i) {
            buf[--i] = INT2CHAR[v&63];
            v >>= 6;
        }
        buf.skip(len);
        return result_t::DONE;
    }

}

#endif
