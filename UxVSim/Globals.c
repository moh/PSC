// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "Globals.h"

// Other.
int bEnableSimulator = 0, bEmbeddedCom = 0, bResetFiles = 0, bRealTime = 0;
int loop_sleep_time = 0;
double lat0 = 0, long0 = 0;	

// State variables.
double x = 0, y = 0, theta = 0, v = 0, vx = 0, vy = 0, omega = 0, phi = 0, phiPoint = 0, 
deltag = 0, deltavmax = 0;

// Parameters.
int robid;
double m = 0, Jx = 0, Jz = 0, rg = 0, rv = 0, l = 0,
alphatheta = 0, alphaf = 0, alphag = 0, alphav = 0, alphaphi = 0, alphaw = 0, 
hv = 0, leq = 0, deltavminreal = 0, deltavmaxreal = 0;

// Wind.
double V = 0, V_med = 0, V_var = 0, psi = 0, psi_med = 0, psi_var = 0, beta = 0;

// Current.
double Vc = 0, Vc_med = 0, Vc_var = 0, psic = 0, psic_med = 0, psic_var = 0;

// Waves.
double hw = 0, hw_var = 0;

// Measurement errors.
double gps_error = 0, mt_error = 0;

FILE* logsimfile = NULL;
char logsimfilename[MAX_BUF_LEN];
