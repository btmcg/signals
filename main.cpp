#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <ctime>
#include <iostream>


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

    // Set our timeout value.
    const timespec timeout = { 1, 0 };    // one second

    // We want to know what signal was sent.
    siginfo_t siginfo;

    // Main loop
    for ( ; ; )
    {
        // Wait for our signal
        if ( ::sigtimedwait( &sigset, &siginfo, &timeout ) == -1 )
        {
            // No signal occurred.
            if ( errno == EAGAIN ) continue;

            std::cerr << "sigprocmask: " << std::strerror( errno ) << std::endl;
            return 1;
        }

        switch ( siginfo.si_signo )
        {
            // Exit on these signals.
            case SIGINT:
            case SIGTERM:
                std::cout << "Caught signal: " << ::strsignal( siginfo.si_signo ) << std::endl;
                return 0;

            // Just report these signals.
            case SIGHUP:
            case SIGUSR1:
            case SIGUSR2:
                std::cout << "Caught signal: " << ::strsignal( siginfo.si_signo ) << std::endl;
                break;

            default:
                std::cout << "wtf?" << std::endl;
                return 1;
        }

    } // for ( ; ; )

    return 0;

} // main()
