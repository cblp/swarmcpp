#ifndef SWARMCPP_SLICE_H
#define SWARMCPP_SLICE_H
#include <stdint.h>
#include <algorithm>
#include <cstring>
#include <cassert>


struct const_slice_t {

    const char *from, *till;

    const_slice_t (const char* str, size_t len) : from(str), till(str+len) {}
    const_slice_t (const char* str) : const_slice_t(str, strlen(str)+1) {}

    bool empty () const {
        return from==till;
    }
    void skip () {
        skipVoid(1);
    }
    size_t size () const {
        return till-from;
    }

    void skipVoid(size_t bytes) {
        from = std::min(from+bytes, till);
    }
    void skip (size_t bytes) {

    }
    void skipTo(const char value) {
        const void* pos = memchr(from, value, till-from);
    }

    char operator * () {
        return *from;
    }

    char operator [] (size_t pos) {
        assert(from+pos<till);
        return from[pos];
    }

    /*bool printNumber (double num) {
        if (size()<20) return false;
        size_t ln = (size_t) snprintf(from, 20, "%f", num);
        skip(ln);
        return true;
    }
    bool scanNumber (double& num) {
        sscanf(from, "%lf", &num);
    }


    bookmark_t scan64x64 (uint64_t& num, int bookmark) {
        int pos = 0;
    }
    bookmark_t scan64x64 (uint64_t& num) {
        return scan64x64(num, 0);
    }

    char scanChar () {
        char ret = empty() ? (char)0 : *from;
        skip();
        return ret;
    }*/


};

struct slice_t {
    char * from;
    const char* till;

    slice_t (char* to, size_t len) : from(to), till(to+len) {}

    size_t size() { return till-from; }
    bool empty () { return from<till; }
    void putChar (char c) {
        assert(!empty());
        *from = c;
        from++;
    }
    void skip (size_t len) {
        from += len;
    }
    char& operator [] (size_t at) {
        assert(from+at<till);
        return from[at];
    }
    char& operator * () {
        return *from;
    } // TODO asserts
};

enum result_t {
    DONE = 0,
    INCOMPLETE = 1,
    BAD_INPUT = -1
};

#endif //SWARMCPP_SLICE_H
