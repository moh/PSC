// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Config.h"
#include "SBGInterface.h"

#ifdef _MSC_VER
// Disable some Visual Studio warnings.
#pragma warning(disable : 4127) 
#pragma warning(disable : 4702) 
#endif // _MSC_VER
int handlesbginterfacecli(SOCKET sockcli, void* pParam)
{		
	uInt_SBG serial, hwRev, fwRev, calRev;
	uShort_SBG calYear;
	unsigned char calMonth = 0, calDay = 0;
	uShort_SBG transferCmd;
	uInt_SBG transferSize, transferOffset;
	unsigned char cmdId = 0, classId = 0;
	uShort_SBG errorCode;
	uInt_SBG time_stamp, solution_status;
	uShort_SBG imu_status;
	uFloat_SBG roll0, pitch0, yaw0;
	uFloat_SBG q0, q1, q2, q3;
	double qw = 0, qx = 0, qy = 0, qz = 0;
	uFloat_SBG roll_acc, pitch_acc, yaw_acc;
	uFloat_SBG accx, accy, accz, gyrx, gyry, gyrz, temp, delta_vel_x, delta_vel_y, delta_vel_z, delta_angle_x, delta_angle_y, delta_angle_z;
	int sendbuflen = 0;
	uint8 sendbuf[MAX_NB_BYTES_SBG];
	unsigned char payload[256];
	int len = 0;
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

	file = fopen("SBGInterface.csv", "r");

	if (!file)
	{
		printf("SBGInterface.csv not found.\n");
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
			time_stamp.v = 0;
			roll0.v = (float)fmod_2PI_deg2rad(roll);
			pitch0.v = (float)fmod_2PI_deg2rad(pitch);
			yaw0.v = (float)fmod_2PI_deg2rad(yaw);
			euler2quaternion((double)roll0.v, (double)pitch0.v, (double)yaw0.v, &qw, &qx, &qy, &qz);
			q0.v = (float)qw; q1.v = (float)qx; q2.v = (float)qy; q3.v = (float)qz;
			roll_acc.v = 0;
			pitch_acc.v = 0;
			yaw_acc.v = 0;
			solution_status.v = SBG_ECOM_SOL_MODE_AHRS;

			imu_status.v = (unsigned short)0xFFFF;
			accx.v = (float)0;
			accy.v = (float)0;
			accz.v = (float)0;
			gyrx.v = (float)0;
			gyry.v = (float)0;
			gyrz.v = (float)0;
			temp.v = 0;
			delta_vel_x.v = 0;
			delta_vel_y.v = 0;
			delta_vel_z.v = 0;
			delta_angle_x.v = 0;
			delta_angle_y.v = 0;
			delta_angle_z.v = 0;

			serial.v = 45000000;
			calYear.v = 2019; calMonth = 4, calDay = 22;
			calRev.c[3] = 1; calRev.c[2] = 1;
			hwRev.c[3] = 1; hwRev.c[2] = 1;
			fwRev.c[3] = 1; fwRev.c[2] = 6;

			memset(payload, 0, sizeof(payload));
			strcpy((char*)payload, "ELLIPSE2-A-G4A3-B1");
			offset = 32;
			memcpy(payload+offset, serial.c, 4);
			offset += 4;
			memcpy(payload+offset, calRev.c, 4);
			offset += 4;
			memcpy(payload+offset, calYear.c, 2);
			offset += 2;
			memcpy(payload+offset, &calMonth, 1);
			offset += 1;
			memcpy(payload+offset, &calDay, 1);
			offset += 1;
			memcpy(payload+offset, hwRev.c, 4);
			offset += 4;
			memcpy(payload+offset, fwRev.c, 4);
			offset += 4;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = 58;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_CMD_INFO, SBG_ECOM_CLASS_LOG_CMD_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			cmdId = SBG_ECOM_CMD_EXPORT_SETTINGS; classId = SBG_ECOM_CLASS_LOG_CMD_0;
			errorCode.v = 0;

			offset = 0;
			memcpy(payload+offset, &cmdId, 1);
			offset += 2;
			memcpy(payload+offset, &classId, 1);
			offset += 2;
			memcpy(payload+offset, errorCode.c, 2);
			offset += 2;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = offset;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_CMD_ACK, SBG_ECOM_CLASS_LOG_CMD_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			transferCmd.v = 0x0000;
			transferSize.v = 0;

			offset = 0;
			memcpy(payload+offset, transferCmd.c, 2);
			offset += 2;
			memcpy(payload+offset, transferSize.c, 4);
			offset += 4;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = offset;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_CMD_EXPORT_SETTINGS, SBG_ECOM_CLASS_LOG_CMD_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			cmdId = SBG_ECOM_CMD_EXPORT_SETTINGS; classId = SBG_ECOM_CLASS_LOG_CMD_0;
			errorCode.v = 0;

			offset = 0;
			memcpy(payload+offset, &cmdId, 1);
			offset += 2;
			memcpy(payload+offset, &classId, 1);
			offset += 2;
			memcpy(payload+offset, errorCode.c, 2);
			offset += 2;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = offset;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_CMD_ACK, SBG_ECOM_CLASS_LOG_CMD_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			transferCmd.v = 0x0001;
			transferOffset.v = 0;

			offset = 0;
			memcpy(payload+offset, transferCmd.c, 2);
			offset += 2;
			memcpy(payload+offset, transferOffset.c, 4);
			offset += 4;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = offset;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_CMD_EXPORT_SETTINGS, SBG_ECOM_CLASS_LOG_CMD_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			cmdId = SBG_ECOM_CMD_EXPORT_SETTINGS; classId = SBG_ECOM_CLASS_LOG_CMD_0;
			errorCode.v = 0;

			offset = 0;
			memcpy(payload+offset, &cmdId, 1);
			offset += 2;
			memcpy(payload+offset, &classId, 1);
			offset += 2;
			memcpy(payload+offset, errorCode.c, 2);
			offset += 2;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = offset;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_CMD_ACK, SBG_ECOM_CLASS_LOG_CMD_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			transferCmd.v = 0x0002;

			offset = 0;
			memcpy(payload+offset, transferCmd.c, 2);
			offset += 2;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = offset;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_CMD_EXPORT_SETTINGS, SBG_ECOM_CLASS_LOG_CMD_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			cmdId = SBG_ECOM_CMD_EXPORT_SETTINGS; classId = SBG_ECOM_CLASS_LOG_CMD_0;
			errorCode.v = 0;

			offset = 0;
			memcpy(payload+offset, &cmdId, 1);
			offset += 2;
			memcpy(payload+offset, &classId, 1);
			offset += 2;
			memcpy(payload+offset, errorCode.c, 2);
			offset += 2;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = offset;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_CMD_ACK, SBG_ECOM_CLASS_LOG_CMD_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			offset = 0;
			memcpy(payload+offset, time_stamp.c, 4);
			offset += 4;
			memcpy(payload+offset, roll0.c, 4);
			offset += 4;
			memcpy(payload+offset, pitch0.c, 4);
			offset += 4;
			memcpy(payload+offset, yaw0.c, 4);
			offset += 4;
			memcpy(payload+offset, roll_acc.c, 4);
			offset += 4;
			memcpy(payload+offset, pitch_acc.c, 4);
			offset += 4;
			memcpy(payload+offset, yaw_acc.c, 4);
			offset += 4;
			memcpy(payload+offset, solution_status.c, 4);
			offset += 4;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = 32;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_LOG_EKF_EULER, SBG_ECOM_CLASS_LOG_ECOM_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			offset = 0;
			memcpy(payload+offset, time_stamp.c, 4);
			offset += 4;
			memcpy(payload+offset, q0.c, 4);
			offset += 4;
			memcpy(payload+offset, q1.c, 4);
			offset += 4;
			memcpy(payload+offset, q2.c, 4);
			offset += 4;
			memcpy(payload+offset, q3.c, 4);
			offset += 4;
			memcpy(payload+offset, roll_acc.c, 4);
			offset += 4;
			memcpy(payload+offset, pitch_acc.c, 4);
			offset += 4;
			memcpy(payload+offset, yaw_acc.c, 4);
			offset += 4;
			memcpy(payload+offset, solution_status.c, 4);
			offset += 4;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = 36;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_LOG_EKF_QUAT, SBG_ECOM_CLASS_LOG_ECOM_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
			{
				return EXIT_FAILURE;
			}

			offset = 0;
			memcpy(payload+offset, time_stamp.c, 4);
			offset += 4;
			memcpy(payload+offset, imu_status.c, 2);
			offset += 2;
			memcpy(payload+offset, accx.c, 4);
			offset += 4;
			memcpy(payload+offset, accy.c, 4);
			offset += 4;
			memcpy(payload+offset, accz.c, 4);
			offset += 4;
			memcpy(payload+offset, gyrx.c, 4);
			offset += 4;
			memcpy(payload+offset, gyry.c, 4);
			offset += 4;
			memcpy(payload+offset, gyrz.c, 4);
			offset += 4;
			memcpy(payload+offset, temp.c, 4);
			offset += 4;
			memcpy(payload+offset, delta_vel_x.c, 4);
			offset += 4;
			memcpy(payload+offset, delta_vel_y.c, 4);
			offset += 4;
			memcpy(payload+offset, delta_vel_z.c, 4);
			offset += 4;
			memcpy(payload+offset, delta_angle_x.c, 4);
			offset += 4;
			memcpy(payload+offset, delta_angle_y.c, 4);
			offset += 4;
			memcpy(payload+offset, delta_angle_z.c, 4);
			offset += 4;
			//memset(sendbuf, 0, sizeof(sendbuf));
			len = 58;
			EncodeFrameSBG(sendbuf, &sendbuflen, SBG_ECOM_LOG_IMU_DATA, SBG_ECOM_CLASS_LOG_ECOM_0, payload, len);
			if (sendall(sockcli, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
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

THREAD_PROC_RETURN_VALUE SBGInterfaceThread(void* pParam)
{
	UNREFERENCED_PARAMETER(pParam);

	if (LaunchSingleCliTCPSrv("4007", handlesbginterfacecli, NULL) != EXIT_SUCCESS)
	{
		printf("Error launching the SBGInterface server.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
