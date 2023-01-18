// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#ifndef CONFIG_H
#define CONFIG_H

#include "Globals.h"

inline int LoadConfig()
{
	FILE* file = NULL;
	char line[MAX_BUF_LEN];

	// Default values.
	robid = SAILBOAT2_ROBID;
	bEnableSimulator = 1;
	bEmbeddedCom = 1;
	bResetFiles = 1;
	bRealTime = 1;

	loop_sleep_time = 20; // Time step (in ms).

	// Reference GPS position (in decimal degrees).
	//lat0 = 48.3920; long0 = -4.4280; // Coordonnées GPS de la sortie du port du Moulin Blanc.
	lat0 = 48.3904; long0 = -4.4245;	

	// State variables.
	x = 0.0; y = 0.0; theta = 3*M_PI/8; v = 0.5; omega = -0.02; phi = 0.02; phiPoint = 0.0; 
	deltag = 0.0; deltavmax = 0.0;

	// Longueur totale de la coque 3.65 m.
	// Largeur totale de la coque 0.86 m.

	// 130 -> 3.65
	// 70 -> rg+rv=2
	// 170 -> longueur du mat=4.77
	// 150*50 -> longueur*largeur grande voile=4.21*1.40
	// 80*30 -> longueur*largeur petite voile=2.25*0.84
	// 80 -> distance centre de poussée à G=2.25

	// Parameters.
	m = 200.0; // Mass (en kg).
	Jx = 12.5; Jz = 50.0; // Inertial moments.
	rg = 1.5; // Distance from the rudder to G (in m).
	rv = 0.5; // Distance from the mast to G (in m).
	l = 0.5; // Distance from the im483i center of pressure to the mast (in m).
	alphatheta = 125.0; // Yaw friction coefficient.
	alphaf = 20.0; // Forward friction coefficient.
	alphag = 75.0; // Rudder force coefficient.
	alphav = 30.0; // Sail force coefficient.
	alphaphi = 40.0; // Roll friction coefficient.
	alphaw = 50.0; // Perturbations due to waves coefficient.
	hv = 2.0; // Distance from the im483i center of pressure to G (in m).
	leq = 0.2; // Length of the equivalent pendulum in roll (in m).

	deltavminreal = 0.30; // Min equivalent opening angle of the real im483i (in rad).
	deltavmaxreal = 1.20; // Max equivalent opening angle of the real im483i (en rad).

	// Wind.
	V_med = 7.0; // Wind speed (in m/s).
	V_var = 3.0; // Wind speed max variation (in m/s).
	psi_med = 3*M_PI/2; // Wind direction (in rad).
	psi_var = M_PI/8.0; // Wind direction max variation (in rad).
	beta = 0.01; // Current due to the wind (fraction of the wind speed).

	// Current.
	Vc_med = 0.1; // Current speed (in m/s).
	Vc_var = 0.1; // Current speed max variation (in m/s).
	psic_med = 0.0*M_PI/2; // Current direction (in rad).
	psic_var = 1.0*M_PI/8.0; // Current direction max variation (in rad).

	// Waves.
	hw_var = 0.75; // Amplitude of the waves (in m).

	// Measurement errors.
	gps_error = 0.5; // GPS error (in m).
	mt_error = 0.1; // Compass error (in rad).

	// Load data from a file.
	file = fopen("UxVSim.txt", "r");
	if (file != NULL)
	{
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%i", &robid) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%d", &bEnableSimulator) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%d", &bEmbeddedCom) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%d", &bResetFiles) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%d", &bRealTime) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%d", &loop_sleep_time) != 1) printf("Invalid configuration file.\n");

		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &lat0) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &long0) != 1) printf("Invalid configuration file.\n");
		// State variables.
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &x) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &y) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &theta) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &v) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &omega) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &phi) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &phiPoint) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &deltag) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &deltavmax) != 1) printf("Invalid configuration file.\n");
		// Parameters.
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &m) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &Jx) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &Jz) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &rg) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &rv) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &l) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &alphatheta) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &alphaf) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &alphag) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &alphav) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &alphaphi) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &alphaw) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &hv) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &leq) != 1) printf("Invalid configuration file.\n");

		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &deltavminreal) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &deltavmaxreal) != 1) printf("Invalid configuration file.\n");
		// Wind.
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &V_med) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &V_var) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &psi_med) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &psi_var) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &beta) != 1) printf("Invalid configuration file.\n");
		// Current.
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &Vc_med) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &Vc_var) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &psic_med) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &psic_var) != 1) printf("Invalid configuration file.\n");
		// Waves.	
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &hw_var) != 1) printf("Invalid configuration file.\n");
		// Measurement errors.	
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &gps_error) != 1) printf("Invalid configuration file.\n");
		if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
		if (sscanf(line, "%lf", &mt_error) != 1) printf("Invalid configuration file.\n");

		if (fclose(file) != EXIT_SUCCESS) printf("fclose() failed.\n");
	}
	else
	{
		printf("Configuration file not found.\n");
	}

	return EXIT_SUCCESS;
}

#endif // CONFIG_H
