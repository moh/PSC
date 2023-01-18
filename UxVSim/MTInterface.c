// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "MTInterface.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handlemtinterfacecli(SOCKET sockcli, void* pParam)
{
	unsigned char gotoconfigackbuf[] = {PREAMBLE_MT,ADDR_MT,GO_TO_CONFIG_ACK_MID,0x00,0x00};
	unsigned char configurationbuf[123] = {PREAMBLE_MT,ADDR_MT,CONFIGURATION_MID,118};
	unsigned char gotomeasurementackbuf[] = {PREAMBLE_MT,ADDR_MT,GO_TO_MEASUREMENT_ACK_MID,0x00,0x00};
	unsigned char mtdatabuf[17] = {PREAMBLE_MT,ADDR_MT,MTDATA_MID,12};
	unsigned char databuf[MAX_BUF_LEN];
	uFloat_MT uf;
	int offset = 0;
	double roll = 0.0;
	double pitch = 0.0;
	double yaw = 0.0;
	FILE* file = NULL;
	double tfile = 0.0;
	double t = 0.0;
	CHRONO chrono;

	UNREFERENCED_PARAMETER(pParam);

	StartChrono(&chrono);

	file = fopen("MTInterface.csv", "r");

	if (!file)
	{
		printf("MTInterface.csv not found.\n");
		return EXIT_FAILURE;
	}

	if (fseek(file, 0, SEEK_END) != EXIT_SUCCESS)
	{
		printf("fseek() failed.\n");
		return EXIT_FAILURE;
	}

	// Wait for the simulator to put some data in the file.
	mSleep(500);

	// Wait for GO_TO_CONFIG_MID.
	if (recvall(sockcli, (char*)databuf, 5) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	SetMTChecksum(gotoconfigackbuf, sizeof(gotoconfigackbuf));
	if (sendall(sockcli, (char*)gotoconfigackbuf, sizeof(gotoconfigackbuf)) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	// Wait for REQ_CONFIGURATION_MID.
	if (recvall(sockcli, (char*)databuf, 5) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	memset(configurationbuf+4, 0, sizeof(configurationbuf)-4);
	offset = 104+4;
	configurationbuf[offset+1] = 0x04;
	configurationbuf[offset+0] = 0x00;
	configurationbuf[offset+5] = 0x04;
	configurationbuf[offset+4] = 0x00;
	configurationbuf[offset+3] = 0x00;
	configurationbuf[offset+2] = 0x00;
	offset += 6;

	SetMTChecksum(configurationbuf, sizeof(configurationbuf));
	if (sendall(sockcli, (char*)configurationbuf, sizeof(configurationbuf)) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	// Wait for GO_TO_MEASUREMENT_MID.
	if (recvall(sockcli, (char*)databuf, 5) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	SetMTChecksum(gotomeasurementackbuf, sizeof(gotomeasurementackbuf));
	if (sendall(sockcli, (char*)gotomeasurementackbuf, sizeof(gotomeasurementackbuf)) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

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
			memset(mtdatabuf+4, 0, sizeof(mtdatabuf)-4);
			uf.v = (float)roll;
			offset = 4;
			mtdatabuf[offset+0] = uf.c[3];
			mtdatabuf[offset+1] = uf.c[2];
			mtdatabuf[offset+2] = uf.c[1];
			mtdatabuf[offset+3] = uf.c[0];
			offset += 4;
			uf.v = (float)pitch;
			mtdatabuf[offset+0] = uf.c[3];
			mtdatabuf[offset+1] = uf.c[2];
			mtdatabuf[offset+2] = uf.c[1];
			mtdatabuf[offset+3] = uf.c[0];
			offset += 4;
			uf.v = (float)yaw;
			mtdatabuf[offset+0] = uf.c[3];
			mtdatabuf[offset+1] = uf.c[2];
			mtdatabuf[offset+2] = uf.c[1];
			mtdatabuf[offset+3] = uf.c[0];
			offset += 4;

			SetMTChecksum(mtdatabuf, sizeof(mtdatabuf));
			if (sendall(sockcli, (char*)mtdatabuf, sizeof(mtdatabuf)) != EXIT_SUCCESS)
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

THREAD_PROC_RETURN_VALUE MTInterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4007", handlemtinterfacecli, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the MTInterface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
