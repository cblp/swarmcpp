#ifndef SWARMCPP_SLICE_H
#define SWARMCPP_SLICE_H

#include <cassert>
#include <string>

namespace swarm {

    namespace detail {

        template <bool constancy, typename T> struct AddConstIf;
        template <typename T> struct AddConstIf<false, T> { using type = T; };
        template <typename T> struct AddConstIf<true,  T> { using type = const T; };

        template <bool constancy> struct slice_base_t {

            using Char = typename AddConstIf<constancy, char>::type;

            // fields

            Char * from;
            const char * till;

            // ctors

            slice_base_t(Char * str, size_t len): from(str), till(str + len) {}

            // properties

            bool isEmpty() const { return from == till; }
            size_t size() const {
                assert(till >= from);
                return till - from;
            }

            // element access

            Char & operator [] (size_t at) {
                assert(from + at >= from);
                assert(from + at < till);
                return from[at];
            }

            Char front() const { return (*this)[0]; }

            // skipping

            /// may overflow!
            void skip(size_t len) { from += len; }

            /// skip without overflow
            void skipVoid(size_t bytes) {
                from = std::min(from + bytes, till);
            }

            /// skip an character without overflow
            void skip() {
                skipVoid(1);
            }

            /// skip to a specified value
            void skipTo(char value) {
                Char * const pos =
                    static_cast<Char *>(memchr(from, value, till - from));
                if (pos)
                    from = pos;
            }

        };

    } // namespace swarm::detail

struct const_slice_t: detail::slice_base_t<true> {

    const_slice_t (const char * str) : slice_base_t(str, strlen(str) + 1) {}

    const_slice_t slice (size_t f, size_t t) const {
        // TODO checks
        return const_slice_t(from+f, t-f);
    }

    size_t seek (char c) const {
        const char* at = (const char*) memchr(from, c, size());
        return at ? at - from : SIZE_MAX;
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
        char ret = isEmpty() ? (char)0 : *from;
        skip();
        return ret;
    }*/

};

struct slice_t: detail::slice_base_t<false> {
    using slice_base_t::slice_base_t;

    void putChar (char c) {
        assert(!isEmpty());
        *from = c;
        from++;
    }
};

enum class result_t {
    DONE = 0,
    INCOMPLETE = 1,
    BAD_INPUT = -1
};

} // namespace swarm

#endif //SWARMCPP_SLICE_H
