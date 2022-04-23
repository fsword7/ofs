// elp-mpp02.cpp - ELP/MPP02 - Lunar ephermral data
//
// Author:  Timothy Stark
// Date:    Apr 23, 2022

#include "main/core.h"
#include "ephem/elp-mpp02.h"
#include "universe/astro.h"

ELP2000Orbit::ELP2000Orbit(dataMode mode)
{
    setupParameters(mode);
    setupCoefficients();
}

void ELP2000Orbit::setupParameters(dataMode mode)
{
	switch (mode)
	{
	case elpUseLLR:
        params.Dw1_0   = -0.10525;
        params.Dw2_0   =  0.16826;
        params.Dw3_0   = -0.10760;
        params.Deart_0 = -0.04012;
        params.Dperi   = -0.04854;
        params.Dw1_1   = -0.32311;
        params.Dgam    =  0.00069;
        params.De      =  0.00005;
        params.Deart_1 =  0.01442;
        params.Dep     =  0.00226;
        params.Dw2_1   =  0.08017;
        params.Dw3_1   = -0.04317;
        params.Dw1_2   = -0.03794;
        params.Dw1_3   =  0.0;
        params.Dw1_4   =  0.0;
        params.Dw2_2   =  0.0;
        params.Dw2_3   =  0.0;
        params.Dw3_2   =  0.0;
        params.Dw3_3   =  0.0;
		break;

	case elpUseDE406:
        params.Dw1_0   = -0.07008;
        params.Dw2_0   =  0.20794;
        params.Dw3_0   = -0.07215;
        params.Deart_0 = -0.00033;
        params.Dperi   = -0.00749;
        params.Dw1_1   = -0.35106;
        params.Dgam    =  0.00085;
        params.De      = -0.00006;
        params.Deart_1 =  0.00732;
        params.Dep     =  0.00224;
        params.Dw2_1   =  0.08017;
        params.Dw3_1   = -0.04317;
        params.Dw1_2   = -0.03743;
        params.Dw1_3   = -0.00018865;
        params.Dw1_4   = -0.00001024;
        params.Dw2_2   =  0.00470602;
        params.Dw2_3   = -0.00025213;
        params.Dw3_2   = -0.00261070;
        params.Dw3_3   = -0.00010712;
		break;
	}

	// derived parameters
	const double am = 0.074801329;
	const double alpha = 0.002571881;
	const double dtsm = 2.0 * alpha / ( 3.0 * am);
	const double xa = 2.0 * alpha / 3.0;
	const double sec = pi / 648000.0; // arcsecs -> radians

	double bp[5][2] = {
		{ 0.311079095, -0.103837907},
		{-0.004482398,  0.000668287},
		{-0.001102485, -0.001298072},
		{ 0.001056062, -0.000178028},
		{ 0.000050928, -0.000037342}
	};

	double w11 = (1732559343.73604 + params.Dw1_1)*sec;
	double w21 = (14643420.3171 + params.Dw2_1)*sec;
	double w31 = (-6967919.5383 + params.Dw3_1)*sec;
	double x2 = w21/w11;
	double x3 = w31/w11;
	double y2 = am*bp[0][0] + xa*bp[4][0];
	double y3 = am*bp[0][1] + xa*bp[4][1];
	double d21 = x2-y2;
	double d22 = w11*bp[1][0];
	double d23 = w11*bp[2][0];
	double d24 = w11*bp[3][0];
	double d25 = y2/am;
	double d31 = x3-y3;
	double d32 = w11*bp[1][1];
	double d33 = w11*bp[2][1];
	double d34 = w11*bp[3][1];
	double d35 = y3/am;

	params.Cw2_1 = d21*params.Dw1_1 + d25*params.Deart_1 + d22*params.Dgam +
				   d23*params.De + d24*params.Dep;
	params.Cw3_1 = d31*params.Dw1_1 + d35*params.Deart_1 + d32*params.Dgam +
				   d33*params.De + d34*params.Dep;

	// factors multipled by B1-B5 for longitude and latitude
	double delnu_nu = (0.55604 + params.Dw1_1)*sec/w11;
	double dele = (0.01789 + params.De)*sec;
	double delg = (-0.08066 + params.Dgam)*sec;
	double delnp_nu = (-0.06424 + params.Deart_1)*sec/w11;
	double delep = (-0.12879 + params.Dep)*sec;

	// factors multipled by B1-B5 for longitude and latitude
	facs.fB1 = -am*delnu_nu + delnp_nu;
	facs.fB2 = delg;
	facs.fB3 = dele;
	facs.fB4 = delep;
	facs.fB5 = -xa*delnu_nu + dtsm*delnp_nu;
	// factor multiplie A_i for distance
	facs.fA = 1.0 - 2.0/3.0*delnu_nu;
}

void ELP2000Orbit::loadMainProblemFile(const fs::path &fname,
    int &n, int **&iMain, double *&aMain, double fA, elpFACS &facs)
{
	double A, B1, B2, B3, B4, B5, B6;
	std::ifstream file(fname, std::ios::in);

	if (!file) {
		// std::cerr << fmt::sprintf("ELP2000Orbit: File '%s': %s\n",
		// 		fname, strerror(errno));
		return;
	}

	file >> n;

	iMain = new int *[n];
	aMain = new double[n];
	for (int idx = 0; idx < n; idx++)
		iMain[idx] = new int[4];

	for (int idx = 0; idx < n; idx++) {
		if (file.eof()) {
			// std::cerr << fmt::sprintf("ELP2000Orbit: File '%s': reached EOF before reading all data!\n",
			// 		fname);
			return;
		}

		file >> iMain[idx][0] >> iMain[idx][1] >> iMain[idx][2] >> iMain[idx][3]
			 >> A >> B1 >> B2 >> B3 >> B4 >> B5 >> B6;
		aMain[idx] = (fA * A) + (facs.fB1 * B1) + (facs.fB2 * B2) + (facs.fB3 * B3) +
			(facs.fB4 * B4) + (facs.fB5 * B5);
	}

	file.close();
}

void ELP2000Orbit::loadPerturbationFile(const fs::path &fname,
	int &n, int **&iPert, double *&aPert, double *&phase)
{
	std::ifstream file(fname, std::ios::in);

	if (!file) {
		// std::cerr << fmt::sprintf("ELP2000Orbit: File '%s': %s\n",
		// 		fname, strerror(errno));
		return;
	}

	file >> n;

	iPert = new int *[n];
	aPert = new double[n];
	phase = new double[n];
	for (int idx = 0; idx < n; idx++)
		iPert[idx] = new int[13];

	for (int idx = 0; idx < n; idx++) {
		if (file.eof()) {
			// std::cerr << fmt::sprintf("ELP2000Orbit: File '%s': reached EOF before reading all data!\n",
			// 		fname);
			return;
		}

		file >> iPert[idx][0]  >> iPert[idx][1] >> iPert[idx][2] >> iPert[idx][3]
			 >> iPert[idx][4]  >> iPert[idx][5] >> iPert[idx][6] >> iPert[idx][7]
			 >> iPert[idx][8]  >> iPert[idx][9] >> iPert[idx][10] >> iPert[idx][11]
			 >> iPert[idx][12] >> aPert[idx] >> phase[idx];
	}

	file.close();
}

void ELP2000Orbit::setupCoefficients()
{
	// Main problem
	loadMainProblemFile("ephem/elp_main.long",
        coefs.n_main_long, coefs.i_main_long, coefs.A_main_long, 1.0, facs);
	loadMainProblemFile("ephem/elp_main.lat",
        coefs.n_main_lat, coefs.i_main_lat, coefs.A_main_lat, 1.0, facs);
	loadMainProblemFile("ephem/elp_main.dist",
        coefs.n_main_dist, coefs.i_main_dist, coefs.A_main_dist, facs.fA, facs);

	// perturbation, longitude
    loadPerturbationFile("ephem/elp_pert.longT0",
        coefs.n_pert_longT0, coefs.i_pert_longT0, coefs.A_pert_longT0, coefs.ph_pert_longT0);
	loadPerturbationFile("ephem/elp_pert.longT1",
        coefs.n_pert_longT1, coefs.i_pert_longT1, coefs.A_pert_longT1, coefs.ph_pert_longT1);
	loadPerturbationFile("ephem/elp_pert.longT2",
        coefs.n_pert_longT2, coefs.i_pert_longT2, coefs.A_pert_longT2, coefs.ph_pert_longT2);
	loadPerturbationFile("ephem/elp_pert.longT3",
        coefs.n_pert_longT3, coefs.i_pert_longT3, coefs.A_pert_longT3, coefs.ph_pert_longT3);

	// perturbation, latitude
	loadPerturbationFile("ephem/elp_pert.latT0",
        coefs.n_pert_latT0, coefs.i_pert_latT0, coefs.A_pert_latT0, coefs.ph_pert_latT0);
	loadPerturbationFile("ephem/elp_pert.latT1",
        coefs.n_pert_latT1, coefs.i_pert_latT1, coefs.A_pert_latT1, coefs.ph_pert_latT1);
	loadPerturbationFile("ephem/elp_pert.latT2",
        coefs.n_pert_latT2, coefs.i_pert_latT2, coefs.A_pert_latT2, coefs.ph_pert_latT2);

	// perturbation, distance
	loadPerturbationFile("ephem/elp_pert.distT0",
        coefs.n_pert_distT0, coefs.i_pert_distT0, coefs.A_pert_distT0, coefs.ph_pert_distT0);
	loadPerturbationFile("ephem/elp_pert.distT1",
        coefs.n_pert_distT1, coefs.i_pert_distT1, coefs.A_pert_distT1, coefs.ph_pert_distT1);
	loadPerturbationFile("ephem/elp_pert.distT2",
        coefs.n_pert_distT2, coefs.i_pert_distT2, coefs.A_pert_distT2, coefs.ph_pert_distT2);
	loadPerturbationFile("ephem/elp_pert.distT3",
        coefs.n_pert_distT3, coefs.i_pert_distT3, coefs.A_pert_distT3, coefs.ph_pert_distT3);
}

void ELP2000Orbit::computeArguments(double T, elpArgs &args) const
{
    const double deg = pi/180.0; // degrees -> radians
    const double sec = pi/648000.0; // arcsecs -> radians
    double T2 = T*T;
    double T3 = T*T2;
    double T4 = T2*T2;
    double w10 = (-142.0 + 18.0/60.0 +(59.95571 + params.Dw1_0)/3600.0)*deg;
    double w11 = mod2pi((1732559343.73604 + params.Dw1_1)*T*sec);
    double w12 = mod2pi((-6.8084 + params.Dw1_2)*T2*sec);
    double w13 = mod2pi((0.006604 + params.Dw1_3)*T3*sec);
    double w14 = mod2pi((-3.169e-5 + params.Dw1_4)*T4*sec);
    double w20 = (83.0 + 21.0/60.0 + (11.67475 + params.Dw2_0)/3600.0)*deg;
    double w21 = mod2pi((14643420.3171 + params.Dw2_1 + params.Cw2_1)*T*sec);
    double w22 = mod2pi((-38.2631 + params.Dw2_2)*T2*sec);
    double w23 = mod2pi((-0.045047+ params.Dw2_3)*T3*sec);
    double w24 = mod2pi(0.00021301*T4*sec);
    double w30 = (125.0 + 2.0/60.0 + (40.39816 + params.Dw3_0)/3600.0)*deg;
    double w31 = mod2pi((-6967919.5383 + params.Dw3_1 + params.Cw3_1)*T*sec);
    double w32 = mod2pi((6.359 + params.Dw3_2)*T2*sec);
    double w33 = mod2pi((0.007625 + params.Dw3_3)*T3*sec);
    double w34 = mod2pi(-3.586e-5*T4*sec);
    double Ea0 = (100.0 + 27.0/60.0 + (59.13885 + params.Deart_0)/3600.0)*deg;
    double Ea1 = mod2pi((129597742.293 + params.Deart_1)*T*sec);
    double Ea2 = mod2pi(-0.0202*T2*sec);
    double Ea3 = mod2pi(9e-6*T3*sec);
    double Ea4 = mod2pi(1.5e-7*T4*sec);
    double p0 = (102.0 + 56.0/60.0 + (14.45766 + params.Dperi)/3600.0)*deg;
    double p1 = mod2pi(1161.24342*T*sec);
    double p2 = mod2pi(0.529265*T2*sec);
    double p3 = mod2pi(-1.1814e-4*T3*sec);
    double p4 = mod2pi(1.1379e-5*T4*sec);

    double Me = (-108.0 + 15.0/60.0 + 3.216919/3600.0)*deg;
    Me += mod2pi(538101628.66888*T*sec);
    double Ve = (-179.0 + 58.0/60.0 + 44.758419/3600.0)*deg;
    Ve += mod2pi(210664136.45777*T*sec);
    double EM = (100.0 + 27.0/60.0 + 59.13885/3600.0)*deg;
    EM += mod2pi(129597742.293*T*sec);
    double Ma = (-5.0 + 26.0/60.0 + 3.642778/3600.0)*deg;
    Ma += mod2pi(68905077.65936*T*sec);
    double Ju = (34.0 + 21.0/60.0 + 5.379392/3600.0)*deg;
    Ju += mod2pi(10925660.57335*T*sec);
    double Sa = (50.0 + 4.0/60.0 + 38.902495/3600.0)*deg;
    Sa += mod2pi(4399609.33632*T*sec);
    double Ur = (-46.0 + 3.0/60.0 + 4.354234/3600.0)*deg;
    Ur += mod2pi(1542482.57845*T*sec);
    double Ne = (-56.0 + 20.0/60.0 + 56.808371/3600.0)*deg;
    Ne += mod2pi(786547.897*T*sec);

    double W1 = w10+w11+w12+w13+w14;
    double W2 = w20+w21+w22+w23+w24;
    double W3 = w30+w31+w32+w33+w34;
    double Ea = Ea0+Ea1+Ea2+Ea3+Ea4;
    double pomp = p0+p1+p2+p3+p4;

    // Mean longitude of the Moon
    args.W1 = mod2pi(W1);
    // Arguments of Delaunay
    args.D = mod2pi(W1-Ea + pi);
    args.F = mod2pi(W1-W3);
    args.L = mod2pi(W1-W2);
    args.Lp = mod2pi(Ea-pomp);

    // zeta
    args.zeta = mod2pi(W1 + 0.02438029560881907*T);

    // Planetary arguments (mean longitudes and mean motions)
    args.Me = mod2pi(Me);
    args.Ve = mod2pi(Ve);
    args.EM = mod2pi(EM);
    args.Ma = mod2pi(Ma);
    args.Ju = mod2pi(Ju);
    args.Sa = mod2pi(Sa);
    args.Ur = mod2pi(Ur);
    args.Ne = mod2pi(Ne);
}

double ELP2000Orbit::computeMainSum(int n, int **iMain, double *aMain,
		elpArgs &args, int dist) const
{
    double sum = 0.0;
    double phase;

    if (dist==0) {
       // sine series
       for (int idx = 0; idx < n; idx++) {
          phase = iMain[idx][0]*args.D + iMain[idx][1]*args.F + iMain[idx][2]*args.L +
                  iMain[idx][3]*args.Lp;
          sum += aMain[idx]*sin(phase);
       }
    } else {
       // cosine series
       for (int idx = 0; idx < n; idx++) {
          phase = iMain[idx][0]*args.D + iMain[idx][1]*args.F + iMain[idx][2]*args.L +
                  iMain[idx][3]*args.Lp;
          sum += aMain[idx]*cos(phase);
       }
    }

    return sum;

}

double ELP2000Orbit::computePerturbationSum(int n, int **iPert, double *aPert,
		double *phPert, elpArgs &args) const
{
    double sum = 0.0;
    double phase;

    for (int idx = 0; idx < n; idx++) {
       phase = phPert[idx] + iPert[idx][0]*args.D + iPert[idx][1]*args.F +
               iPert[idx][2]*args.L + iPert[idx][3]*args.Lp + iPert[idx][4]*args.Me +
               iPert[idx][5]*args.Ve + iPert[idx][6]*args.EM + iPert[idx][7]*args.Ma +
               iPert[idx][8]*args.Ju + iPert[idx][9]*args.Sa + iPert[idx][10]*args.Ur +
               iPert[idx][11]*args.Ne + iPert[idx][12]*args.zeta;
       sum += aPert[idx]*sin(phase);
    }

    return sum;
}

vec3d_t ELP2000Orbit::calculatePosition(double jd) const
{
	// Julian time since EPOCH J2000.0
	double T  = (jd - 2451545.0) / 36525.0;
	double T2 = T * T;
	double T3 = T2 * T;
	double T4 = T3 * T;
	double T5 = T4 * T;

	elpArgs args;

	computeArguments(T, args);

	// Sum the ELP/MPP02 series
	// main problem series
	double main_long = computeMainSum(coefs.n_main_long, coefs.i_main_long,
								  coefs.A_main_long, args, 0);
	double main_lat = computeMainSum(coefs.n_main_lat, coefs.i_main_lat,
								  coefs.A_main_lat, args, 0);
	double main_dist = computeMainSum(coefs.n_main_dist, coefs.i_main_dist,
								  coefs.A_main_dist, args, 1);
	// perturbation, longitude
	double pert_longT0 = computePerturbationSum(coefs.n_pert_longT0, coefs.i_pert_longT0,
											coefs.A_pert_longT0, coefs.ph_pert_longT0, args);
	double pert_longT1 = computePerturbationSum(coefs.n_pert_longT1, coefs.i_pert_longT1,
											coefs.A_pert_longT1, coefs.ph_pert_longT1, args);
	double pert_longT2 = computePerturbationSum(coefs.n_pert_longT2, coefs.i_pert_longT2,
											coefs.A_pert_longT2, coefs.ph_pert_longT2, args);
	double pert_longT3 = computePerturbationSum(coefs.n_pert_longT3, coefs.i_pert_longT3,
											coefs.A_pert_longT3, coefs.ph_pert_longT3, args);
	// perturbation, latitude
	double pert_latT0 = computePerturbationSum(coefs.n_pert_latT0, coefs.i_pert_latT0,
											coefs.A_pert_latT0, coefs.ph_pert_latT0, args);
	double pert_latT1 = computePerturbationSum(coefs.n_pert_latT1, coefs.i_pert_latT1,
											coefs.A_pert_latT1, coefs.ph_pert_latT1, args);
	double pert_latT2 = computePerturbationSum(coefs.n_pert_latT2, coefs.i_pert_latT2,
											coefs.A_pert_latT2, coefs.ph_pert_latT2, args);
	// perturbation, distance
	double pert_distT0 = computePerturbationSum(coefs.n_pert_distT0, coefs.i_pert_distT0,
											coefs.A_pert_distT0, coefs.ph_pert_distT0, args);
	double pert_distT1 = computePerturbationSum(coefs.n_pert_distT1, coefs.i_pert_distT1,
											coefs.A_pert_distT1, coefs.ph_pert_distT1, args);
	double pert_distT2 = computePerturbationSum(coefs.n_pert_distT2, coefs.i_pert_distT2,
											coefs.A_pert_distT2, coefs.ph_pert_distT2, args);
	double pert_distT3 = computePerturbationSum(coefs.n_pert_distT3, coefs.i_pert_distT3,
											coefs.A_pert_distT3, coefs.ph_pert_distT3, args);


	// Moon's longitude, latitude and distance
	double longM = args.W1 + main_long + pert_longT0 + mod2pi(pert_longT1*T) +
				 mod2pi(pert_longT2*T2) + mod2pi(pert_longT3*T3);
	double latM  = main_lat + pert_latT0 + mod2pi(pert_latT1*T) + mod2pi(pert_latT2*T2);
	const double ra0 = 384747.961370173/384747.980674318;
	double r = ra0*(main_dist +  pert_distT0 + pert_distT1*T + pert_distT2*T2 + pert_distT3*T3);

	longM = longM + pi;
	latM  = latM - (pi/2.0);
	
//	cout << fmt::sprintf("Longtitude: %lf  Latitude: %lf  Distance: %lf\n",
//		glm::degrees(longM), glm::degrees(latM), r);

	return vec3d_t( sin(latM) * cos(longM) * r,
					cos(latM) * r,
					sin(latM) * -sin(longM) * r);

//	// Precession matrix
//	double P = 0.10180391e-4*T + 0.47020439e-6*T2 - 0.5417367e-9*T3
//			 - 0.2507948e-11*T4 + 0.463486e-14*T5;
//	double Q = -0.113469002e-3*T + 0.12372674e-6*T2 + 0.12654170e-8*T3
//			 - 0.1371808e-11*T4 - 0.320334e-14*T5;
//	double sq = sqrt(1 - P*P - Q*Q);
//	double p11 = 1 - 2*P*P;
//	double p12 = 2*P*Q;
//	double p13 = 2*P*sq;
//	double p21 = 2*P*Q;
//	double p22 = 1-2*Q*Q;
//	double p23 = -2*Q*sq;
//	double p31 = -2*P*sq;
//	double p32 = 2*Q*sq;
//	double p33 = 1 - 2*P*P - 2*Q*Q;
//
//	// Finally, components of position vector wrt J2000.0 mean ecliptic and equinox
//	return vec3d_t( p11*pos.x + p12*pos.y + p13*pos.z,
//					p21*pos.x + p22*pos.y + p23*pos.z,
//					p31*pos.x + p32*pos.y + p33*pos.z);
}
