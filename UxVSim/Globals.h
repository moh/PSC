// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#ifndef GLOBALS_H
#define GLOBALS_H

#include "OSNet.h"
#include "OSMisc.h"

#define MOTORBOAT_ROBID               0x00000400
#define SPEBOT_ROBID                  0x00000800
#define VAIMOS_ROBID                  0x00002000
#define SAILBOAT_ROBID                0x00004000
#define SAILBOAT2_ROBID               0x00008000

// Other.
extern int bEnableSimulator, bEmbeddedCom, bResetFiles, bRealTime;
extern int loop_sleep_time;
extern double lat0, long0;	

// State variables.
extern double x, y, theta, v, vx, vy, omega, phi, phiPoint, 
deltag, deltavmax;

// Parameters.
extern int robid;
extern double m, Jx, Jz, rg, rv, l,
alphatheta, alphaf, alphag, alphav, alphaphi, alphaw, 
hv, leq, deltavminreal, deltavmaxreal;

// Wind.
extern double V, V_med, V_var, psi, psi_med, psi_var, beta;

// Current.
extern double Vc, Vc_med, Vc_var, psic, psic_med, psic_var;

// Waves.
extern double hw, hw_var;

// Measurement errors.
extern double gps_error, mt_error;

extern FILE* logsimfile;
extern char logsimfilename[MAX_BUF_LEN];

#endif // GLOBALS_H
