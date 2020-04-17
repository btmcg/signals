#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstddef> // std::size_t
#include <cstring>
#include <ctime>
#include <iostream>


int main(int, char**)
{
    sigset_t sigset;
    ::sigemptyset(&sigset);

    assert(::sigaddset(&sigset, SIGINT) != -1);
    assert(::sigaddset(&sigset, SIGTERM) != -1);
    assert(::sigaddset(&sigset, SIGHUP) != -1);
    assert(::sigaddset(&sigset, SIGUSR1) != -1);
    assert(::sigaddset(&sigset, SIGUSR2) != -1);

    // block the signals so that we can handle them in sigtimedwait
    if (::sigprocmask(SIG_BLOCK, &sigset, nullptr) == -1) {
        std::cerr << "sigprocmask: " << std::strerror(errno) << std::endl;
        return 1;
    }

    // get the signalfd file descriptor
    int const signal_fd = ::signalfd(-1, &sigset, SFD_NONBLOCK);
    if (signal_fd == -1) {
        std::cerr << "signalfd: " << std::strerror(errno) << std::endl;
        return 1;
    }

    // get epoll file descriptor
    int const epoll_fd = ::epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "epoll_create1: " << std::strerror(errno) << std::endl;
        return 1;
    }

    // add signal_fd to our epoll control
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = signal_fd;
    if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, signal_fd, &ev) == -1) {
        std::cerr << "epoll_ctl: " << std::strerror(errno) << std::endl;
        return 1;
    }

    // container for our epoll events
    epoll_event events[1];

    // Main loop
    while (true) {
        // block in epoll for 1 second at a time
        int const num_events = ::epoll_wait(epoll_fd, events, 1, 1000);
        if (num_events == -1) {
            std::cerr << "epoll_wait: " << std::strerror(errno) << std::endl;
            return 1;
        }

        for (int i = 0; i < num_events; ++i) {
            // we received a signal
            if (events[i].data.fd == signal_fd) {
                signalfd_siginfo info;
                ::ssize_t const bytes = ::read(signal_fd, &info, sizeof(info));
                if (bytes == -1) {
                    std::cerr << "read: " << std::strerror(errno) << std::endl;
                    return 1;
                }

                if (static_cast<std::size_t>(bytes) != sizeof(info)) {
                    std::cerr << "read wrong number of bytes" << std::endl;
                    return 1;
                }

                switch (info.ssi_signo) {
                // exit on these signals.
                case SIGINT:
                case SIGTERM:
                    std::cout << "Caught signal: " << ::strsignal(static_cast<int>(info.ssi_signo))
                              << std::endl;
                    return 0;

                // just report these signals
                case SIGHUP:
                case SIGUSR1:
                case SIGUSR2:
                    std::cout << "Caught signal: " << ::strsignal(static_cast<int>(info.ssi_signo))
                              << std::endl;
                    break;

                default:
                    std::cout << "wtf?" << std::endl;
                    return 1;
                }
            } // received sig
        } // for each event
    } // while (true)

    return 0;

} // main()
