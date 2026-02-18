#include "my_config.hpp"

#if RICKY_WIN

#include "thread.hpp"
#include "my_exception.hpp"

#include <Windows.h>

namespace my::plat::thread {

struct ThreadHandle {
    HANDLE handle{nullptr};
    bool joinable{false};
    void (*entry)(void*){nullptr};
    void* arg{nullptr};
};

namespace {

DWORD WINAPI thread_entry(LPVOID arg) {
    auto* th = static_cast<ThreadHandle*>(arg);
    if (th && th->entry) {
        th->entry(th->arg);
    }
    return 0;
}

} // namespace

ThreadHandle* create(void (*entry)(void*), void* arg) {
    if (entry == nullptr) {
        throw argument_exception("Invalid thread entry");
    }
    auto* th = new ThreadHandle{};
    th->entry = entry;
    th->arg = arg;
    th->handle = ::CreateThread(nullptr, 0, &thread_entry, th, 0, nullptr);
    if (th->handle == nullptr) {
        delete th;
        throw system_exception("Failed to create thread");
    }
    th->joinable = true;
    return th;
}

void join(ThreadHandle* thread) {
    if (thread == nullptr || !thread->joinable) {
        return;
    }
    ::WaitForSingleObject(thread->handle, INFINITE);
    ::CloseHandle(thread->handle);
    thread->handle = nullptr;
    thread->joinable = false;
    delete thread;
}

void detach(ThreadHandle* thread) {
    if (thread == nullptr || !thread->joinable) {
        return;
    }
    ::CloseHandle(thread->handle);
    thread->handle = nullptr;
    thread->joinable = false;
    delete thread;
}

bool joinable(ThreadHandle* thread) {
    return thread != nullptr && thread->joinable;
}

void sleep_ms(const u32 ms) {
    ::Sleep(ms);
}

} // namespace my::plat::thread

#endif // RICKY_WIN
