// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "MESInterface.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handlemesinterfacecli(SOCKET sockcli, void* pParam)
{
	unsigned char databuf[MAX_BUF_LEN];
	double range = 0;
	FILE* file = NULL;
	double tfile = 0.0;
	double t = 0.0;
	CHRONO chrono;

	UNREFERENCED_PARAMETER(pParam);

	StartChrono(&chrono);

	file = fopen("MESInterface.csv", "r");

	if (!file)
	{
		printf("MESInterface.csv not found.\n");
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
			if (sscanf(buf, "%lf;%lf;\n", &tfile, &range) != 2)
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

		GetTimeElapsedChrono(&chrono, &t);

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
			sprintf((char*)databuf, "Range=%.2f\n", 0.0);
			if (sendall(sockcli, (char*)databuf, strlen((char*)databuf)+1) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
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

THREAD_PROC_RETURN_VALUE MESInterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4008", handlemesinterfacecli, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the MESInterface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
