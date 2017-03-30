//
// Created by gritzko on 3/26/17.
//

#ifndef SWARMCPP_FRAME_H
#define SWARMCPP_FRAME_H
#include <stdint.h>
#include <assert.h>
#include "64x64.h"
#include "spec.h"
#include "op.h"

namespace swarm {

    class frame_t {

        const uint8_t *body;
        const uint32_t size;

    public:

        class op_iterator {
            const frame_t  *host;
            uint32_t        offset;
            size_t          op_length;
            uint32_t        pos;
            op_t            op;
            const char     *error_message;

            void error (const char* msg) {
                error_message = msg;
            }
        public:
            op_iterator (const frame_t& host);
            bool operator == (const op_iterator& b) const {
                return offset == b.offset;
            }
            void operator ++ ();
            int stringAt (size_t pos, std::string& to) const;
            int integerAt (size_t pos, int64_t& to) const;
            uint8_t byteAt (size_t pos) const {
                return host->body[offset+pos];
            }
            const op_t& op () const;
        };

        op_iterator begin() const;

        op_iterator end() const;

    };

    class frame_writer {
        std::string flags;
    public:
        frame_writer (std::string coding_flags);

        int append (const frame_t::op_iterator& op);
        int append (const frame_t::op_iterator& op, size_t count);
        int append (const op_t& op);

    };

}

void swarm::frame_t::op_iterator::operator++() {
    offset += op_length;
    op_length = 0;
    if (offset==host->size) return;
    void * at = host->body + offset;
    uint8_t *line;
    slice_t rest;
    // read in prefix len
    const uint8_t plen = line[0];
    const int8_t prefix_length = base_t::char2int8(plen);
    if (prefix_length<0) { error("invalid prefix length"); return; }
    // zero spec
    for(int t=0; t<8; t++)
        if (spec_offs[t]>prefix_length)
            spec[t] = base_t::ZERO;
    // part-zero
    int zero_out = spec_offs[t] - prefix_length;
    op.spec[t] &= base_t::ZERO_MASKS[zero_out];
    // go regen
    uint64_t bookmark = op_t::bookmark(t, zero_out);
    op.scan_continue(rest-1, bookmark);
    // scan values
    value_count = 0;
    at = op.scan_values(at);
//    while (*at!='\n') {
//        spec.scan_value(at, values[value_count++]);
//    }
    //void* nl = memchr(at, '\n', x);
    op_length = nl - at;
}



int swarm::frame_writer::append(const frame_t::op_iterator &i) {
    return append(i.op());
}

int swarm::frame_writer::append(const op_t &new_op) {
    // C-flag 0p ...
    // base_t::operator == (const uint8_t* str)
    int t=0;
    while (t<8 && op.spec[t]==new_op.spec[t]
        t++;
    if (t==8) {
        return -1;
    }
    const uint8_t skip_bytes = op.spec[t].prefixLength(new_op.spec[t]);
    // base_t::matchLength (const uint8_t* str) // 10 for a full match

    // parse/print bookmark: uint64_t
    slice_t buf; // string ??!
    uint64_t bookmark = op::parse_bookmark(t, skip_chars);
    uint64_t progress;
    const char* end = op.print_continue(buf, bookmark, progress); // size? realloc?
    if (end===NULL) {
        buf = realloc(); // new_buf = slice_t.double(buf); 1MB op is OK!
        bookmark = progress;
        op.print_continue(old_end, bookmark, progress);
    }
    // finish that one
    // finish the spec
    // memcpy the value
}

#endif //SWARMCPP_FRAME_H
