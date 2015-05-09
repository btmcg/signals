#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <unistd.h>


int main( int argc, char* argv[] )
{
    sigset_t sigset;

    // Empty the signal set.
    ::sigemptyset( &sigset );

    // Add the signals we want to handle.
    assert( ::sigaddset( &sigset, SIGINT ) != -1 );
    assert( ::sigaddset( &sigset, SIGTERM ) != -1 );
    assert( ::sigaddset( &sigset, SIGHUP ) != -1 );
    assert( ::sigaddset( &sigset, SIGUSR1) != -1 );
    assert( ::sigaddset( &sigset, SIGUSR2) != -1 );

    // Block the signals so that we can handle them in sigtimedwait.
    if ( ::sigprocmask( SIG_BLOCK, &sigset, nullptr ) == -1 )
    {
        std::cerr << "sigprocmask: " << std::strerror( errno ) << std::endl;
        return 1;
    }

    // Get the signalfd file descriptor.
    const int signal_fd = ::signalfd( -1, &sigset, SFD_NONBLOCK );
    if ( signal_fd == -1 )
    {
        std::cerr << "signalfd: " << std::strerror( errno ) << std::endl;
        return 1;
    }


    // Get epoll file descriptor.
    const int epoll_fd = ::epoll_create1( 0 );
    if ( epoll_fd == -1 )
    {
        std::cerr << "epoll_create1: " << std::strerror( errno ) << std::endl;
        return 1;
    }

    // Add signal_fd to our epoll control.
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = signal_fd;
    if ( ::epoll_ctl( epoll_fd, EPOLL_CTL_ADD, signal_fd, &ev ) == -1 )
    {
        std::cerr << "epoll_ctl: " << std::strerror( errno ) << std::endl;
        return 1;
    }


    // Container for our epoll events.
    epoll_event events[1];

    // Main loop
    for ( ; ; )
    {
        // Block in epoll for 1 second at a time.
        const int num_events = ::epoll_wait( epoll_fd, events, 1, 1000 );
        if ( num_events == -1 )
        {
            std::cerr << "epoll_wait: " << std::strerror( errno ) << std::endl;
            return 1;
        }

        for ( int i = 0; i < num_events; ++i )
        {
            // We received a signal.
            if ( events[i].data.fd == signal_fd )
            {
                signalfd_siginfo info;
                const int bytes = ::read( signal_fd, &info, sizeof( info ) );
                if ( bytes == -1 )
                {
                    std::cerr << "read: " << std::strerror( errno ) << std::endl;
                    return 1;
                }

                if ( bytes != sizeof( info ) )
                {
                    std::cerr << "read wrong number of bytes" << std::endl;
                    return 1;
                }


                switch ( info.ssi_signo )
                {
                    // Exit on these signals.
                    case SIGINT:
                    case SIGTERM:
                        std::cout << "Caught signal: " << ::strsignal( info.ssi_signo ) << std::endl;
                        return 0;

                    // Just report these signals.
                    case SIGHUP:
                    case SIGUSR1:
                    case SIGUSR2:
                        std::cout << "Caught signal: " << ::strsignal( info.ssi_signo ) << std::endl;
                        break;

                    default:
                        std::cout << "wtf?" << std::endl;
                        return 1;
                }
            }
        } // for each event

    } // for ( ; ; )

    return 0;

} // main()
