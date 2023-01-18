// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "PololuInterface.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handlepololuinterfacecli(SOCKET sockcli, void* pParam)
{
	unsigned char databuf[MAX_BUF_LEN];
	int target = 0;
	int pw = 0;
	double angle = 0.0;
	FILE* file = NULL;
	double t = 0.0;
	CHRONO chrono;

	UNREFERENCED_PARAMETER(pParam);

	StartChrono(&chrono);

	file = fopen("PololuInterface.csv", "a");

	if (!file)
	{
		printf("PololuInterface.csv not found.\n");
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
			if (recvall(sockcli, (char*)databuf, 1) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}
			if ((databuf[0] & 0xE0) == 0xC0) // Set Target High Resolution.
			{
				//memset(databuf, 0, sizeof(databuf));
				if (recvall(sockcli, (char*)databuf+1, 1) != EXIT_SUCCESS)
				{
					return EXIT_FAILURE;
				}
				// Convert pulse width (in us) into angle (in rad).
				target = ((databuf[1] & 0x7F)<<5)|(databuf[0] & 0x3F);
				pw = target*(2000-1000)/4095+1000;
				angle = (pw-1500)*((M_PI/4.0)-(-M_PI/4.0))/(2000-1000);
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
			else if ((databuf[0]) == 0xA7) // Get Scaling Feedback.
			{
				memset(databuf, 0, sizeof(databuf));
				databuf[0] = (unsigned char)(0xC0 + (target & 0x1F)); // Command byte holds the lower 5 bits of target.
				databuf[1] = (unsigned char)((target >> 5) & 0x7F); // Data byte holds the upper 7 bits of target.
				if (sendall(sockcli, (char*)databuf, 2) != EXIT_SUCCESS)
				{
					return EXIT_FAILURE;
				}
			}
			else if ((databuf[0]) == 0xB3) // Get Error Flags Halting.
			{
				memset(databuf, 0, sizeof(databuf));
				databuf[0] = 0;
				databuf[1] = 0;
				if (sendall(sockcli, (char*)databuf, 2) != EXIT_SUCCESS)
				{
					return EXIT_FAILURE;
				}
			}
			else
			{
				memset(databuf, 0, sizeof(databuf));
				databuf[0] = 0;
				databuf[1] = 0;
				if (sendall(sockcli, (char*)databuf, 2) != EXIT_SUCCESS)
				{
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

THREAD_PROC_RETURN_VALUE PololuInterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4004", handlepololuinterfacecli, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the PololuInterface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
