/*++
Copyright (c) 2015 Microsoft Corporation

Module Name:

    rlimit.cpp

Abstract:

    Resource limit container.

Author:

    Nikolaj Bjorner (nbjorner) 2015-09-28

Revision History:

--*/
#include "util/rlimit.h"
#include "util/common_msgs.h"
#include "util/mutex.h"


__thread DECLARE_MUTEX(g_rlimit_mux);

void initialize_rlimit() {
    ALLOC_MUTEX(g_rlimit_mux);
}
reslimit::reslimit():
    m_cancel(0),
    m_suspend(false),
    m_count(0),
    m_limit(std::numeric_limits<uint64_t>::max()),
    start_time(0),
    time_out(0)  {
}

void reslimit::start_timer(unsigned to) {
    time_out = to;
    struct timespec st;
    clock_gettime(CLOCK_REALTIME, &st);
    start_time = st.tv_sec * 1000 + st.tv_nsec / 1000000;
}
void reslimit::end_timer() {
    start_time = 0;
}
bool reslimit::timeout() {
    if (start_time == 0) {
        return false;
    } else {
        struct timespec curr_time;
        clock_gettime(CLOCK_REALTIME, &curr_time);
        long end_time = curr_time.tv_sec * 1000 + curr_time.tv_nsec / 1000000;
        return (end_time - start_time) > time_out;
    }
}

void finalize_rlimit() {
    DEALLOC_MUTEX(g_rlimit_mux);
}

uint64_t reslimit::count() const {
    return m_count;
}

bool reslimit::inc() {
    ++m_count;
    return !timeout() && not_canceled();
}

bool reslimit::inc(unsigned offset) {
    m_count += offset;
    return !timeout() && not_canceled();
}

void reslimit::push(unsigned delta_limit) {
    uint64_t new_limit = delta_limit ? delta_limit + m_count : std::numeric_limits<uint64_t>::max();
    if (new_limit <= m_count) {
        new_limit = std::numeric_limits<uint64_t>::max();
    }
    m_limits.push_back(m_limit);
    m_limit = std::min(new_limit, m_limit);
    m_cancel = 0;
}

void reslimit::pop() {
    if (m_count > m_limit) {
        m_count = m_limit;
    }
    m_limit = m_limits.back();
    m_limits.pop_back();
    m_cancel = 0;
}

char const* reslimit::get_cancel_msg() const {
    if (m_cancel > 0) {
        return Z3_CANCELED_MSG;
    }
    else {
        return Z3_MAX_RESOURCE_MSG;
    }
}

void reslimit::push_child(reslimit* r) {
    lock_guard lock(*g_rlimit_mux);
    m_children.push_back(r);    
}

void reslimit::pop_child() {
    lock_guard lock(*g_rlimit_mux);
    m_children.pop_back();    
}

void reslimit::cancel() {
    lock_guard lock(*g_rlimit_mux);
    set_cancel(m_cancel+1);    
}

void reslimit::reset_cancel() {
    lock_guard lock(*g_rlimit_mux);
    set_cancel(0);    
}

void reslimit::inc_cancel() {
    lock_guard lock(*g_rlimit_mux);
    set_cancel(m_cancel+1);    
}

void reslimit::dec_cancel() {
    lock_guard lock(*g_rlimit_mux);
    if (m_cancel > 0) {
        set_cancel(m_cancel-1);
    }
}

void reslimit::set_cancel(unsigned f) {
    m_cancel = f;
    for (unsigned i = 0; i < m_children.size(); ++i) {
        m_children[i]->set_cancel(f);
    }
}
