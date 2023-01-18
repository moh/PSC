// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "ProbeInterface.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handleprobeinterfacecli(SOCKET sockcli, void* pParam)
{
	unsigned char pbdatabuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,NB_DATA_BYTES_PROBE,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char databuf[MAX_BUF_LEN];
	uFloat_Probe value;
	unsigned char crc_h = 0;
	unsigned char crc_l = 0;
	double temperature = 0, pH = 0, conductivity = 0, salinity = 0, turbidity = 0, DOSat = 0, DOMgl = 0, depth = 0, chlorophyll = 0;
	FILE* file = NULL;
	double tfile = 0.0;
	double t = 0.0;
	CHRONO chrono;

	UNREFERENCED_PARAMETER(pParam);

	StartChrono(&chrono);

	file = fopen("ProbeInterface.csv", "r");

	if (!file)
	{
		printf("ProbeInterface.csv not found.\n");
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
			if (sscanf(buf, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;\n", &tfile, &temperature, &pH, 
				&conductivity, &salinity, &turbidity, &DOSat, &DOMgl, &depth, &chlorophyll) != 10)
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

			if (recvall(sockcli, (char*)databuf, 1) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			switch (databuf[0])
			{
			case ADDR_PROBE:
				if (recvall(sockcli, (char*)databuf+1, SENDBUF_LEN_PROBE-1) != EXIT_SUCCESS)
				{
					return EXIT_FAILURE;
				}

				// Check CRC-16.
				CalcCRC16(databuf, SENDBUF_LEN_PROBE-2, &crc_h, &crc_l);
				if ((databuf[SENDBUF_LEN_PROBE-1] != crc_h)|| 
					(databuf[SENDBUF_LEN_PROBE-2] != crc_l))
				{ 
					printf("Bad CRC.\n");
					return EXIT_FAILURE;	
				}
				// Check command.
				if (databuf[1] != READ_COMMAND_PROBE)
				{
					printf("Invalid command.\n");
					return EXIT_FAILURE;
				}
				switch (databuf[2])
				{
				case TEMPERATURE_REG_L_PROBE:
					value.v = (float)temperature;
					mSleep(1500);
					break;
				case PH_REG_L_PROBE:
					value.v = (float)pH;
					mSleep(2500);
					break;
				case CONDUCTIVITY_REG_L_PROBE:
					value.v = (float)conductivity;
					mSleep(2500);
					break;
				case SALINITY_REG_L_PROBE:
					value.v = (float)salinity;
					mSleep(2500);
					break;
				case TURBIDITY_REG_L_PROBE:
					value.v = (float)turbidity;
					mSleep(2500);
					break;
				case DOSAT_REG_L_PROBE:
					value.v = (float)DOSat;
					mSleep(6500);
					break;
				case DOMGL_REG_L_PROBE:
					value.v = (float)DOMgl;
					mSleep(6500);
					break;
				case DEPTH_REG_L_PROBE:
					value.v = (float)depth;
					mSleep(1500);
					break;
				case CHLOROPHYLL_REG_L_PROBE:
					value.v = (float)chlorophyll;
					mSleep(6500);
					break;
				default:
					printf("Invalid data register.\n");
					return EXIT_FAILURE;
				}

				// Set data bytes.
				pbdatabuf[5] = value.c[3];
				pbdatabuf[6] = value.c[2];
				pbdatabuf[3] = value.c[1];
				pbdatabuf[4] = value.c[0];

				// Compute CRC-16.
				CalcCRC16(pbdatabuf, sizeof(pbdatabuf)-2, &crc_h, &crc_l);
				pbdatabuf[sizeof(pbdatabuf)-1] = crc_h;
				pbdatabuf[sizeof(pbdatabuf)-2] = crc_l;

				if (sendall(sockcli, (char*)pbdatabuf, sizeof(pbdatabuf)) != EXIT_SUCCESS)
				{
					return EXIT_FAILURE;
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

THREAD_PROC_RETURN_VALUE ProbeInterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4008", handleprobeinterfacecli, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the ProbeInterface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
