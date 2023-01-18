// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "NMEAInterface.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handlenmeainterfacecli(SOCKET sockcli, void* pParam)
{
	char databuf[256];
	char tmpbuf[192];
	char checksum[4];
	double latitude = 0.0;
	double longitude = 0.0;
	double heading = 0.0;
	double winddir = 0.0;
	double windspeed = 0.0;
	double sog = 0.0;
	double cog = 0.0;
	int latdeg = 0;
	double latdecmin = 0;
	char NorthOrSouth = 0;
	int longdeg = 0;
	double longdecmin = 0;
	char EastOrWest = 0;
	FILE* file = NULL;
	double tfile = 0.0;
	double t = 0.0;
	CHRONO chrono;

	UNREFERENCED_PARAMETER(pParam);

	StartChrono(&chrono);

	file = fopen("NMEAInterface.csv", "r");

	if (!file)
	{
		printf("NMEAInterface.csv not found.\n");
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
			if (sscanf(buf, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;\n", &tfile, &latitude, &longitude, &heading, &sog, &cog, &winddir, &windspeed) != 8)
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
			memset(databuf, 0, sizeof(databuf));
			memset(tmpbuf, 0, sizeof(tmpbuf));
			//latitude = 48.39;
			//longitude = -4.425;
			GPSLatitudeDecDeg2DegDecMin(latitude, &latdeg, &latdecmin, &NorthOrSouth);
			GPSLongitudeDecDeg2DegDecMin(longitude, &longdeg, &longdecmin, &EastOrWest);
			sprintf(tmpbuf, "$GPGGA,000000,%02d%010.7f,%c,%03d%010.7f,%c,1,7,1.3,0.0,M,,,,", 
				latdeg, latdecmin, NorthOrSouth, longdeg, longdecmin, EastOrWest);
			ComputeChecksumNMEA(tmpbuf, (int)strlen(tmpbuf), checksum);
			sprintf(databuf, "%s%s\r\n", tmpbuf, checksum);
			if (sendall(sockcli, (char*)databuf, (int)strlen(databuf)) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}
			memset(databuf, 0, sizeof(databuf));
			memset(tmpbuf, 0, sizeof(tmpbuf));
			sprintf(tmpbuf, "$GPRMC,000000,A,%02d%010.7f,%c,%03d%010.7f,%c,%06.2f,%06.2f,000000,,,A",
				latdeg, latdecmin, NorthOrSouth, longdeg, longdecmin, EastOrWest, sog, cog);
			ComputeChecksumNMEA(tmpbuf, (int)strlen(tmpbuf), checksum);
			sprintf(databuf, "%s%s\r\n", tmpbuf, checksum);
			if (sendall(sockcli, (char*)databuf, (int)strlen(databuf)) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}
			//heading = 355;
			sprintf(tmpbuf, "$HCHDG,%.1f,0.0,E,0.0,W", heading);
			ComputeChecksumNMEA(tmpbuf, (int)strlen(tmpbuf), checksum);
			sprintf(databuf, "%s%s\r\n", tmpbuf, checksum);
			if (sendall(sockcli, (char*)databuf, (int)strlen(databuf)) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}
			//winddir = 185;
			//windspeed = 10;
			sprintf(tmpbuf, "$WIMDA,30.0000,I,1.0000,B,15.5,C,,,,,,,%.1f,T,%.1f,M,%.1f,N,%.1f,M", 
				winddir, winddir, windspeed*1.94, windspeed);
			ComputeChecksumNMEA(tmpbuf, (int)strlen(tmpbuf), checksum);
			sprintf(databuf, "%s%s\r\n", tmpbuf, checksum);
			if (sendall(sockcli, (char*)databuf, (int)strlen(databuf)) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}
			break;
		}

		mSleep(500);
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

THREAD_PROC_RETURN_VALUE NMEAInterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4001", handlenmeainterfacecli, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the NMEAInterface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
