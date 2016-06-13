#ifndef SWARMCPP_64x64_H
#define SWARMCPP_64x64_H
#include <stdint.h>
#include <string>
#include <cstring>

namespace swarm {

    struct base_t {
        // the number itself
        uint64_t value;

        // Constructors

        base_t(uint64_t val64) : value(val64) { }

        base_t(long int val64) : value(val64) { }

        base_t () : value (0) {}

        base_t (const char* str64) : value(0) {
            parser_t parser;
            parser.scan(*this, str64, strlen(str64)+1);
        }

        base_t (std::string str64) : value(0) {
            parser_t parser;
            parser.scan(*this, str64.c_str(), str64.length());
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
            return value >= INFINITY.value;
        }
        bool isInfinity () const {
            return value == INFINITY.value;
        }

        const base_t& operator ++ () {
            value++;
            return *this;
        }

        operator std::string() const {
            char str[11];
            parser_t parser;
            return std::string(str, (size_t)parser.print(*this,str,11));
        }

        // Constants

        static const int CHAR_BITS = 6;
        static const int MAX_CHARS = 10;
        static const int MAX_BITS = CHAR_BITS * MAX_CHARS;
        static const uint64_t CHAR_BIT_MASK;
        static const base_t INFINITY;
        static const base_t INCORRECT;
        static const char INT2CHAR[];
        static const int8_t CHAR2INT[128];

        // perser/serializer type
        struct parser_t {
            int offset;
            parser_t () : offset(0) {}
            // read a base64x64 number from the buffer;
            // @return i<length if read completely or
            // i==length if need more data (possibly)
            int scan(base_t& target, const char *buf, int length);

            // print a base64x64 number to a buffer
            // @return i<length if complete, i==length if needs more space
            int print(const base_t& target, char *buf, int length);
        };

    };

    const uint64_t base_t::CHAR_BIT_MASK = (1 << CHAR_BITS) - 1;
    const base_t base_t::INFINITY = 63L << (6 * 9);
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


    int base_t::parser_t::scan(base_t& target, const char *buf, int length) {
        int i = 0;
        while (i < length && offset < MAX_CHARS) {
            int8_t next = buf[i];
            if (next >= 128) return -1;
            uint64_t num = (uint64_t) CHAR2INT[next];
            if (num==-1)
                return offset > 0 ? i : -1;
            int shift = (MAX_CHARS-offset-1)*CHAR_BITS;
            target.value |= num << shift;
            offset++;
            i++;
        }
        return i;
    }

    int base_t::parser_t::print(const base_t& target, char *buf, int length) {
        int i = 0;
        const uint64_t & value = target.value;
        while (offset < MAX_CHARS && i < length) {
            int shift = (MAX_CHARS - offset - 1) * CHAR_BITS;
            int charVal = int ((value >> shift) & CHAR_BIT_MASK);
            if (charVal==0 && offset>0 && ((value>>shift)<<shift)==value) {
                return i;
            }
            buf[i] = INT2CHAR[charVal];
            i++;
            offset++;
        }
        return i;
    }

}

#endif