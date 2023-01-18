// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "OntrakInterface.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handleontrakcli(SOCKET sockcli, void* pParam)
{
	char sidatabuf[MAX_BUF_LEN];
	char databuf[MAX_BUF_LEN];
	char szDev[2];
	int dev = 0;
	int channel = 0;
	int K[8];
	int PA[8];
	FILE* file = NULL;
	//double t = 0.0;
	CHRONO chrono;

	UNREFERENCED_PARAMETER(pParam);

	StartChrono(&chrono);

	file = fopen("OntrakInterface.csv", "a");

	if (!file)
	{
		printf("OntrakInterface.csv not found.\n");
		return EXIT_FAILURE;
	}

	// Synchronization.
	mSleep(500);

	memset(K, 0, sizeof(K));
	memset(PA, 0, sizeof(PA));

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

			memset(szDev, 0, sizeof(szDev));
			memcpy(szDev, databuf, 1);
			dev = atoi(szDev);
			switch (dev)
			{
			case 0:
				if (sscanf(databuf+1, "RD%d\r", &channel) == 1)
				{
					sprintf(sidatabuf, "%04d ", (int)(1024+128*rand()/(double)RAND_MAX));
					if (sendall(sockcli, sidatabuf, strlen(sidatabuf)) != EXIT_SUCCESS)
					{
						return EXIT_FAILURE;
					}
				} 
				else if (sscanf(databuf+1, "RESPA%d\r", &channel) == 1)
				{
					PA[channel] = 0;
				}
				else if (sscanf(databuf+1, "SETPA%d\r", &channel) == 1)
				{
					PA[channel] = 1;
				}
				else if (sscanf(databuf+1, "RPA%d\r", &channel) == 1)
				{
					sprintf(sidatabuf, "%01d ", PA[channel]);
					if (sendall(sockcli, sidatabuf, strlen(sidatabuf)) != EXIT_SUCCESS)
					{
						return EXIT_FAILURE;
					}
				}
				break;
			case 1:
				if (sscanf(databuf+1, "RK%d\r", &channel) == 1)
				{
					K[channel] = 0;
				}
				else if (sscanf(databuf+1, "SK%d\r", &channel) == 1)
				{
					K[channel] = 1;
				}
				else if (sscanf(databuf+1, "RPK%d\r", &channel) == 1)
				{
					sprintf(sidatabuf, "%01d ", K[channel]);
					if (sendall(sockcli, sidatabuf, strlen(sidatabuf)) != EXIT_SUCCESS)
					{
						return EXIT_FAILURE;
					}
				}
				else if (sscanf(databuf+1, "RPA%d\r", &channel) == 1)
				{
					sprintf(sidatabuf, "%01d ", 1);
					if (sendall(sockcli, sidatabuf, strlen(sidatabuf)) != EXIT_SUCCESS)
					{
						return EXIT_FAILURE;
					}
				}
				break;
			default:
				break;
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

THREAD_PROC_RETURN_VALUE OntrakInterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4002", handleontrakcli, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the OntrakInterface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
