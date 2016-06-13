//
// Created by gritzko on 6/13/16.
//
#include "../include/stamp.h"
#include <assert.h>
#include <ctime>

using namespace swarm;
using namespace std;

int main (int argn, char** args) {

    setenv("TZ", "UTC", 1);
    tzset();
    setlocale(LC_TIME, "en_EN.utf8");

    char buf[1024];
    stamp_t::parser_t parser;
    // zero (is-zero: true), named zero
    const char* str0 = "0";
    stamp_t zero(str0);
    assert(zero.isZero());
    assert(zero.isTranscendent());
    assert(!zero.isAbnormal());
    assert(!zero.isError());
    printf("%s\n", string(zero).c_str());

    const char* str0my = "0+my";
    stamp_t myzero(str0my);
    assert(!myzero.isTranscendent());
    assert(myzero.isZero());
    printf("%s\n", string(myzero).c_str());

    const char* str_trans = "transcndnt";
    stamp_t trans(str_trans);
    assert(trans.isTranscendent());
    assert(!trans.isZero());
    printf("%s\n", string(trans).c_str());

    // never, named never
    const char* str_never = "~";
    stamp_t never(str_never);
    assert(never.isTranscendent());
    assert(!never.isZero());
    assert(never.isNever());
    printf("%s\n", string(never).c_str());

    const char* str_nnever = "~+my";
    stamp_t nnever(str_nnever);
    assert(!nnever.isTranscendent());
    assert(!nnever.isZero());
    assert(nnever.isNever());
    printf("%s\n", string(nnever).c_str());

    // parent-of true
    const char* str_parent = "1CQAneD+parnt";
    const char* str_child = "1CQAnek+parntCHILD";
    stamp_t parent(str_parent);
    stamp_t child(str_child);
    assert(parent.isParentOf(child));
    printf("%s\n%s\n", string(parent).c_str(), string(child).c_str());

    // 1 Jan 2010 00:00:01 UTC
    const char* str_epoch1sec = "000001+my";
    stamp_t epoch1sec(str_epoch1sec);
    tm time = epoch1sec.datetime();
    assert(time.tm_year==110);
    assert(time.tm_mon==0);
    assert(time.tm_mday==1);
    assert(time.tm_hour==0);
    assert(time.tm_min==0);
    assert(time.tm_sec==1);
    printf("%s\n", string(epoch1sec).c_str());

    // Fri May 27 20:50:00 UTC 2016: scan and print both formats
    tm ser;
    bzero(&ser, sizeof(ser));
    ser.tm_year = 116;
    ser.tm_mon = 4;
    ser.tm_mday = 27;
    ser.tm_hour = 20;
    ser.tm_min = 50;
    stamp_t stamp_ser(ser, "my");
    char str_ser[1024];
    tm tm_ser = stamp_ser.datetime();
    strftime(str_ser, 1024, "%a %b %d %H:%M:%S UTC %Y", &tm_ser);
    printf("%s\n%s\n", string(stamp_ser).c_str(), str_ser);

    // parse 1CQAn00000+my: the same timestamp untrimmed
    const char* str_pt = "1CQKn00000+my";
    stamp_t pt(str_pt);
    tm time_pt = pt.datetime();
    strftime(str_ser, 1024, "%a %b %d %H:%M:%S UTC %Y", &time_pt);
    printf("%s\n", str_ser);

    // is-abnormal: true (not a timestamp)
    stamp_t abnormal("~abc+author");
    assert(abnormal.isAbnormal());
    printf("%s\n", string(abnormal).c_str());

    // is-transcendent: true (the right half is not an origin)
    stamp_t trans2 ("Object+~3");
    assert(trans2.isTranscendent());
    printf("%s\n", string(trans2).c_str());

    // parse Object+0: zero origin is skipped, transcendent: true
    stamp_t object ("Object+0");
    assert(object.isTranscendent());
    printf("%s\n", string(object).c_str());

    // is-error: true, is-transcendent: false, is-abnormal: true, is-zero: false
    stamp_t error("~~~~~~~~~~+origin");
    assert(error.isError());
    assert(!error.isTranscendent());
    assert(error.isAbnormal());
    assert(!error.isZero());
    printf("%s\n", string(error).c_str());

    return 0;

}