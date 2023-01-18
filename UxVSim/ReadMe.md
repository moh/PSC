Simulation of the protocol of various devices on a simulated moving boat.

Default configuration (see UxVSim.txt) is to simulate a motorboat (robid parameter set to 0x00000400, with a Pololu Jrk device to control the rudder, a SBG Ellipse AHRS and a NMEA ($GPRMC sentence) GPS), to simulate a sailboat change robid parameter to e.g. 0x00002000. You might want also to change the initial position of the simulated robot (see lat0, long0 parameters) as well as wind, currents and waves conditions (see psi_med, psi_var, beta, Vc_med, Vc_var, psic_med, psic_var, hw_var parameters).

Superseded by most XXXInterface functionalities in https://github.com/ENSTABretagneRobotics/UxVCtrl.
