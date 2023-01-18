// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#ifndef PROBE_H
#define PROBE_H

#include "OSNet.h"
#include "OSMisc.h"

#define IP_ADDR_PROBE "172.20.5.4"
#define TCP_PORT_PROBE "4008"

#define SENDBUF_LEN_PROBE 8
#define RECVBUF_LEN_PROBE 9

#define ADDR_PROBE 0x01
#define READ_COMMAND_PROBE 0x03
#define NB_REGISTERS_READ_H_PROBE 0x00
#define NB_REGISTERS_READ_L_PROBE 0x01

#define NB_DATA_BYTES_PROBE 4

#define TEMPERATURE_REG_L_PROBE 0x00
#define PH_REG_L_PROBE 0x06
#define CONDUCTIVITY_REG_L_PROBE 0x0A
#define SALINITY_REG_L_PROBE 0x10
#define TURBIDITY_REG_L_PROBE 0xA6
#define DOSAT_REG_L_PROBE 0xB2
#define DOMGL_REG_L_PROBE 0xB4
#define DEPTH_REG_L_PROBE 0x22
#define CHLOROPHYLL_REG_L_PROBE 0xD0
#define NOSLEEP_REG_L_PROBE 0x0C

// In ms.
#define STARTUP_DELAY_PROBE 4000
#define SHUTDOWN_DELAY_PROBE 4000
#define WAKEUP_DELAY_PROBE 500
#define SLEEP_DELAY_PROBE 12000
#define MAX_MEASUREMENT_DELAY_PROBE 6500

union usShort_Probe
{
	short v;
	unsigned char c[2];
};
typedef union usShort_Probe usShort_Probe;

union uShort_Probe
{
	unsigned short v;
	unsigned char c[2];
};
typedef union uShort_Probe uShort_Probe;

union usInt_Probe
{
	int v;
	unsigned char c[4];
};
typedef union usInt_Probe usInt_Probe;

union uInt_Probe
{
	unsigned int v;
	unsigned char c[4];
};
typedef union uInt_Probe uInt_Probe;

union uFloat_Probe
{
	float v;
	unsigned char c[4];
};
typedef union uFloat_Probe uFloat_Probe;

union uDouble_Probe
{
	double v;
	unsigned char c[8];
};
typedef union uDouble_Probe uDouble_Probe;

struct PROBE
{
	SOCKET s;
	char address[MAX_BUF_LEN];
	char port[MAX_BUF_LEN];
};
typedef struct PROBE PROBE;


//unsigned char temperatureBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0x00,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x84,0x0A};//,0xC4,0x0B};//,0x84,0x0A};
//unsigned char pHBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0x06,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x64,0x0B};//,0x24,0x0A};
//unsigned char conductivitySpBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0x0A,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0xA4,0x08};//,0xE4,0x09};
//unsigned char salinityBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0x10,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x85,0xCF};//,0xC5,0xCE};
//unsigned char turbidityBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0xA6,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x64,0x29};//,0x24,0x28};
//unsigned char DOSatBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0xB2,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x24,0x2D};//,0x64,0x2C};
//unsigned char DOMgLBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0xB4,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0xC4,0x2C};//,0x84,0x2D};
//unsigned char depthBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0x22,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x24,0x00};//,0x64,0x01};
//unsigned char chlorophyllBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0xD0,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x85,0xF3};//,0xC5,0xF2};
//unsigned char noSleepBuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0x0C,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x44,0x09};//,0x04,0x08};

inline int InitProbe(PROBE* pProbe)
{
	FILE* file = NULL;
	char line[MAX_BUF_LEN];

	// Default values.
	memset(line, 0, sizeof(line));
	memset(pProbe->address, 0, sizeof(pProbe->address));
	sprintf(pProbe->address, IP_ADDR_PROBE);
	memset(pProbe->port, 0, sizeof(pProbe->port));
	sprintf(pProbe->port, TCP_PORT_PROBE);

	// Load data from a file.
	file = fopen("Probe.txt", "r");
	if (file != NULL)
	{
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%32s", pProbe->address) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%32s", pProbe->port) != 1) printf("Invalid configuration file.\n");
		if (fclose(file) != EXIT_SUCCESS) printf("fclose() failed.\n");
	}
	else
	{
		printf("Configuration file not found.\n");
	}

	// Connect to server.
	if (inittcpcli(&pProbe->s, pProbe->address, pProbe->port) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

inline int WakeUpProbe(PROBE* pProbe)
{
	// Wake up byte.
	if (sendall(pProbe->s, "\r", 1) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	// Time to wake up.
	mSleep(WAKEUP_DELAY_PROBE);

	return EXIT_SUCCESS;
}

inline int GetValueProbe(PROBE* pProbe, int data_reg_l, double* pValue)
{
	unsigned char sendbuf[] = {ADDR_PROBE,READ_COMMAND_PROBE,0x00,0x00,NB_REGISTERS_READ_H_PROBE,NB_REGISTERS_READ_L_PROBE,0x00,0x00};
	int sendbuflen = SENDBUF_LEN_PROBE;
	char recvbuf[MAX_BUF_LEN];
	int recvbuflen = 0;
	uFloat_Probe value;
	unsigned char crc_h = 0;
	unsigned char crc_l = 0;

	// Set data register address (lowest byte).
	sendbuf[3] = (unsigned char)data_reg_l;
	// Compute CRC-16.
	CalcCRC16(sendbuf, SENDBUF_LEN_PROBE-2, &crc_h, &crc_l);
	sendbuf[SENDBUF_LEN_PROBE-1] = crc_h;
	sendbuf[SENDBUF_LEN_PROBE-2] = crc_l;

	// Data request.
	if (sendall(pProbe->s, (char*)sendbuf, sendbuflen) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	// Wait for the data to be ready.
	//switch (data_reg_l)
	//{
	//case TEMPERATURE_REG_L_PROBE:
	//	mSleep(1500);
	//	break;
	//case PH_REG_L_PROBE:
	//	mSleep(2500);
	//	break;
	//case CONDUCTIVITY_REG_L_PROBE:
	//	mSleep(2500);
	//	break;
	//case SALINITY_REG_L_PROBE:
	//	mSleep(2500);
	//	break;
	//case TURBIDITY_REG_L_PROBE:
	//	mSleep(2500);
	//	break;
	//case DOSAT_REG_L_PROBE:
	//	mSleep(6500);
	//	break;
	//case DOMGL_REG_L_PROBE:
	//	mSleep(6500);
	//	break;
	//case DEPTH_REG_L_PROBE:
	//	mSleep(1500);
	//	break;
	//case CHLOROPHYLL_REG_L_PROBE:
	//	mSleep(6500);
	//	break;
	//case NOSLEEP_REG_L_PROBE:
	//	mSleep(1500);
	//	break;
	//default :
	//	printf("Invalid register address.\n");
	//	return EXIT_FAILURE;
	//}

	mSleep(MAX_MEASUREMENT_DELAY_PROBE);

	//if (waitforsocket(pProbe->s, MAX_MEASUREMENT_DELAY_PROBE+2000) != EXIT_SUCCESS)
	//{
	//	return EXIT_FAILURE;
	//}

	// Prepare the buffer that should receive data from the server.
	memset(recvbuf, 0, sizeof(recvbuf));
	recvbuflen = RECVBUF_LEN_PROBE;

	if (recvall(pProbe->s, recvbuf, recvbuflen) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	// Display and analyze received data.
	//printf("Received : \"%s\"\n", recvbuf);

	// Check CRC-16.
	CalcCRC16((unsigned char*)recvbuf, RECVBUF_LEN_PROBE-2, &crc_h, &crc_l);
	if (((unsigned char)recvbuf[RECVBUF_LEN_PROBE-1] != crc_h)|| 
		((unsigned char)recvbuf[RECVBUF_LEN_PROBE-2] != crc_l))
	{ 
		printf("Bad CRC.\n");
		return EXIT_FAILURE;	
	}
	// Check slave address.
	if ((unsigned char)recvbuf[0] != ADDR_PROBE)
	{
		printf("Invalid device address.\n");
		return EXIT_FAILURE;
	}
	// Check command.
	if ((unsigned char)recvbuf[1] != READ_COMMAND_PROBE)
	{
		printf("Invalid command.\n");
		return EXIT_FAILURE;
	}
	// Check number of data bytes.
	if ((unsigned char)recvbuf[2] != NB_DATA_BYTES_PROBE)
	{
		printf("Unhandled number of data bytes.\n");
		return EXIT_FAILURE;
	}

	// Data bytes conversion to float.
	value.c[3] = (unsigned char)recvbuf[5];
	value.c[2] = (unsigned char)recvbuf[6];
	value.c[1] = (unsigned char)recvbuf[3];
	value.c[0] = (unsigned char)recvbuf[4];

	*pValue = (double)value.v;

	return EXIT_SUCCESS;
}

inline int GetDefaultValuesProbe(PROBE* pProbe, double* pTemperature, double* ppH, double* pConductivity, 
						  double* pSalinity, double* pTurbidity, double* pDOSat, double* pDOMgL, 
						  double* pDepth, double* pChlorophyll)
{
	if (WakeUpProbe(pProbe) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, TEMPERATURE_REG_L_PROBE, pTemperature) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, PH_REG_L_PROBE, ppH) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, CONDUCTIVITY_REG_L_PROBE, pConductivity) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, SALINITY_REG_L_PROBE, pSalinity) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, TURBIDITY_REG_L_PROBE, pTurbidity) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, DOSAT_REG_L_PROBE, pDOSat) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, DOMGL_REG_L_PROBE, pDOMgL) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, DEPTH_REG_L_PROBE, pDepth) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if (GetValueProbe(pProbe, CHLOROPHYLL_REG_L_PROBE, pChlorophyll) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

inline int ReleaseProbe(PROBE* pProbe)
{
	// Disconnect from server.
	if (releasetcpcli(pProbe->s) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif // PROBE_H
