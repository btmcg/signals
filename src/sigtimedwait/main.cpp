#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <ctime>
#include <iostream>


int main()
{
    sigset_t sigset;

    assert(::sigemptyset( &sigset ) != -1);

    assert(::sigaddset(&sigset, SIGINT) != -1);
    assert(::sigaddset(&sigset, SIGTERM) != -1);
    assert(::sigaddset(&sigset, SIGHUP) != -1);
    assert(::sigaddset(&sigset, SIGUSR1) != -1);
    assert(::sigaddset(&sigset, SIGUSR2) != -1);

    // block the signals so that we can handle them in sigtimedwait.
    if (::sigprocmask(SIG_BLOCK, &sigset, nullptr) == -1) {
        std::cerr << "sigprocmask: " << std::strerror(errno) << std::endl;
        return 1;
    }

    // set our timeout value to one second
    const timespec timeout = {1, 0};

    siginfo_t siginfo;

    // Main loop
    while (true) {
        // wait for our signal
        if (::sigtimedwait(&sigset, &siginfo, &timeout) == -1) {
            // no signal occurred
            if (errno == EAGAIN) continue;

            std::cerr << "sigprocmask: " << std::strerror(errno) << std::endl;
            return 1;
        }

        switch (siginfo.si_signo) {
            // exit on these signals
            case SIGINT:
            case SIGTERM:
                std::cout << "Caught signal: " << ::strsignal(siginfo.si_signo) << std::endl;
                return 0;

            // just report these signals
            case SIGHUP:
            case SIGUSR1:
            case SIGUSR2:
                std::cout << "Caught signal: " << ::strsignal(siginfo.si_signo) << std::endl;
                break;

            default:
                std::cout << "wtf?" << std::endl;
                return 1;
        }

    } // while (true)

    return 0;

} // main()
