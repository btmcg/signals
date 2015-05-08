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
    ::sigaddset( &sigset, SIGINT );
    ::sigaddset( &sigset, SIGTERM );
    ::sigaddset( &sigset, SIGHUP );
    ::sigaddset( &sigset, SIGUSR1 );
    ::sigaddset( &sigset, SIGUSR2 );

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
            case SIGINT:
            case SIGTERM:
                std::cout << "Caught signal: " << ::strsignal( siginfo.si_signo ) << std::endl;
                return 0;

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
