// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "SSC32Interface.h"

#define MIN_ANGLE_RUDDER (-M_PI/4.0)
#define MAX_ANGLE_RUDDER (M_PI/4.0)

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handlessc32interfacecli(SOCKET sockcli, void* pParam)
{
	char databuf[MAX_BUF_LEN];
	int pw = 0;
	double angle = 0.0;
	FILE* file = NULL;
	double t = 0.0;
	CHRONO chrono;

	UNREFERENCED_PARAMETER(pParam);

	StartChrono(&chrono);

	file = fopen("SSC32Interface.csv", "a");

	if (!file)
	{
		printf("SSC32Interface.csv not found.\n");
		return EXIT_FAILURE;
	}

	// Synchronization.
	mSleep(500);

	for (;;)
	{
		fd_set sock_set;
		int iResult = SOCKET_ERROR;
		struct timeval tv;
		int timeout = 0;

		tv.tv_sec = (long)(timeout/1000);
		tv.tv_usec = (long)((timeout%1000)*1000);

		// Initialize a fd_set and add the socket to it.
		FD_ZERO(&sock_set); 
		FD_SET(sockcli, &sock_set);

		iResult = select((int)sockcli+1, &sock_set, NULL, NULL, &tv);

		// Remove the socket from the set.
		// No need to use FD_ISSET() here, as we only have one socket the return value of select() is 
		// sufficient to know what happened.
		FD_CLR(sockcli, &sock_set); 

		switch (iResult)
		{
		case SOCKET_ERROR:
			//printf("select() failed.\n");
			return EXIT_FAILURE;
		case 0:
			// The timeout on select() occured.
			//printf("select() timed out.\n");
			break;
		default:
			memset(databuf, 0, sizeof(databuf));

			if (recvuntil(sockcli, databuf, '\r', sizeof(databuf)) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}
			if (sscanf(databuf, "#0P%d\r", &pw) == 1)
			{
				// Convert ssc32 pulse width (in us) into angle (in rad).
				angle = (pw-DEFAULT_MID_PW_SSC32)*(MAX_ANGLE_RUDDER-MIN_ANGLE_RUDDER)/(DEFAULT_MAX_PW_SSC32-DEFAULT_MIN_PW_SSC32);
				GetTimeElapsedChrono(&chrono, &t);
				if (fprintf(file, "%f;%f;\n", t, angle) <= 0)
				{
					printf("fprintf() failed.\n");
					return EXIT_FAILURE;
				}
				if (fflush(file) != EXIT_SUCCESS)
				{
					printf("fflush() failed.\n");
					return EXIT_FAILURE;
				}
			}
			break;
		}

		mSleep(10);
	}

	if (fclose(file) != EXIT_SUCCESS) 
	{
		printf("fclose() failed.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
#ifdef _MSC_VER
// Restore the Visual Studio warnings previously disabled.
#pragma warning(default : 4127) 
#pragma warning(default : 4702) 
#endif // _MSC_VER

THREAD_PROC_RETURN_VALUE SSC32InterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4004", handlessc32interfacecli, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the SSC32Interface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
