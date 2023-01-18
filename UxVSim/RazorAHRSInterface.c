// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "RazorAHRSInterface.h"

#define MAX_NB_BYTES 4096

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handleclirazorahrs(SOCKET sockcli, void* pParam)
{
	char sendbuf[MAX_NB_BYTES];
	int sendbuflen = 0;
	char cval = 0;
	BOOL bOutputStream = TRUE;
	BOOL bROSMode = FALSE;
	double roll = 0.0;
	double pitch = 0.0;
	double yaw = 0.0;
	FILE* file = NULL;
	double tfile = 0.0;
	double t = 0.0;
	CHRONO chrono;

	UNREFERENCED_PARAMETER(pParam);

	StartChrono(&chrono);

	file = fopen("RazorAHRSInterface.csv", "r");

	if (!file)
	{
		printf("RazorAHRSInterface.csv not found.\n");
		return EXIT_FAILURE;
	}

	if (fseek(file, 0, SEEK_END) != EXIT_SUCCESS)
	{
		printf("fseek() failed.\n");
		return EXIT_FAILURE;
	}

	// Wait for the simulator to put some data in the file.
	mSleep(500);

	for (;;)
	{
		fd_set sock_set;
		int iResult = SOCKET_ERROR;
		struct timeval tv;
		int timeout = 0;
		char buf[MAX_BUF_LEN];

		// Wait for a new line of data in the file.
		memset(buf, 0, sizeof(buf));
		while (fgets(buf, sizeof(buf), file) != NULL)
		{
			if ((buf[0] == 0)||(buf[0] == '\n'))
			{
				memset(buf, 0, sizeof(buf));
				continue;
			}
			if (sscanf(buf, "%lf;%lf;%lf;%lf;\n", &tfile, &roll, &pitch, &yaw) != 4)
			{
				memset(buf, 0, sizeof(buf));
				continue;
			}
			memset(buf, 0, sizeof(buf));
		}
		if (ferror(file))
		{
			printf("ferror() failed.\n");
			return EXIT_FAILURE;
		}
		if (feof(file)) clearerr(file); // On Ubuntu 20.04, this seems necessary, see https://lists.gnu.org/archive/html/info-gnu/2018-08/msg00000.html...

		//printf("%f\n", tfile);

		if (checkavailablebytessocket(sockcli) == EXIT_SUCCESS)
		{
			if (recvall(sockcli, &cval, 1) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}
			if (cval == '#')
			{
				if (recvall(sockcli, &cval, 1) != EXIT_SUCCESS)
				{
					return EXIT_FAILURE;
				}
				switch (cval)
				{
				case 'o':
					if (recvall(sockcli, &cval, 1) != EXIT_SUCCESS)
					{
						return EXIT_FAILURE;
					}
					switch (cval)
					{
					case '0':
						bOutputStream = FALSE;
						break;
					case '1':
						bOutputStream = TRUE;
						break;
					case 'x':
						bROSMode = TRUE;
						break;
					case 't':
						bROSMode = FALSE;
						break;
					default:
						break;
					}
					break;
				case 'p':
					if (sendall(sockcli, (char*)"0\r\n", sizeof("0\r\n")) != EXIT_SUCCESS)
					{
						return EXIT_FAILURE;
					}
					break;
				default:
					break;
				}
			}
		}

		GetTimeElapsedChrono(&chrono, &t);

		tv.tv_sec = (long)(timeout/1000);
		tv.tv_usec = (long)((timeout%1000)*1000);

		// Initialize a fd_set and add the socket to it.
		FD_ZERO(&sock_set);
		FD_SET(sockcli, &sock_set);

		iResult = select((int)sockcli+1, NULL, &sock_set, NULL, &tv);

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
			memset(sendbuf, 0, sizeof(sendbuf));
			if (bROSMode)
			{
				sprintf(sendbuf, "#YPRAG=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n", yaw, pitch, roll, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			}
			else
			{
				sprintf(sendbuf, "#YPR=%.2f,%.2f,%.2f\r\n", yaw, pitch, roll);
			}
			sendbuflen = (int)strlen(sendbuf);
			if (bOutputStream)
			{
				if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
				{
					return EXIT_FAILURE;
				}
			}
			break;
		}

		mSleep(20);
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

THREAD_PROC_RETURN_VALUE RazorAHRSInterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4007", handleclirazorahrs, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the RazorInterface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
