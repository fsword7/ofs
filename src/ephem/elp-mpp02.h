// elp-mpp02.h - ELP/MPP02 - Lunar ephermral data
//
// Author:  Timothy Stark
// Date:    Apr 23, 2022

#pragma once

#include "ephem/orbit.h"

class ELP2000Orbit : public CachingOrbit
{
public:
    enum dataMode
    {
        elpUseLLR,
        elpUseDE406
    };

    // parameters for adjusting the lunar and planetary arguments
    struct elpParams {
        // parameters adjusted to fit data
        double  Dw1_0, Dw2_0, Dw3_0, Deart_0, Dperi, Dw1_1, Dgam, De, Deart_1, Dep,
                Dw2_1, Dw3_1, Dw1_2, Dw1_3, Dw1_4, Dw2_2, Dw2_3, Dw3_2, Dw3_3;

        // parameters derived from the previous parameters
        double  Cw2_1, Cw3_1;
    };

    // Factors multiplied by B1-B5 for longitude and latitude
    struct elpFACS {
        double fA, fB1, fB2, fB3, fB4, fB5;
    };

    struct elpData {
        // Main problem
        int n_main_long, n_main_lat, n_main_dist;
        int **i_main_long, **i_main_lat, **i_main_dist;
        double *A_main_long, *A_main_lat, *A_main_dist;

        // Perturbation, longitude
        int n_pert_longT0, n_pert_longT1, n_pert_longT2, n_pert_longT3;
        int **i_pert_longT0, **i_pert_longT1, **i_pert_longT2, **i_pert_longT3;
        double *A_pert_longT0, *A_pert_longT1, *A_pert_longT2, *A_pert_longT3;
        double *ph_pert_longT0, *ph_pert_longT1, *ph_pert_longT2, *ph_pert_longT3;

        // Perturbation, latitude
        int n_pert_latT0, n_pert_latT1, n_pert_latT2;
        int **i_pert_latT0, **i_pert_latT1, **i_pert_latT2;
        double *A_pert_latT0, *A_pert_latT1, *A_pert_latT2;
        double *ph_pert_latT0, *ph_pert_latT1, *ph_pert_latT2;

        // Perturbation, distance
        int n_pert_distT0, n_pert_distT1, n_pert_distT2, n_pert_distT3;
        int **i_pert_distT0, **i_pert_distT1, **i_pert_distT2, **i_pert_distT3;
        double *A_pert_distT0, *A_pert_distT1, *A_pert_distT2, *A_pert_distT3;
        double *ph_pert_distT0, *ph_pert_distT1, *ph_pert_distT2, *ph_pert_distT3;
    };

    // Arguments for ELP/MPP02 series
    struct elpArgs {
        double W1, D, F, L, Lp, zeta, Me, Ve, EM, Ma, Ju, Sa, Ur, Ne;
    };

    ELP2000Orbit(dataMode mode);
    virtual ~ELP2000Orbit() = default;

    vec3d_t calculatePosition(double tjd) const override;
    // vec3d_t calculateVelocity(double tjd) const override;

    double getPeriod() const			{ return period; }
    double getBoundingRadius() const	{ return boundingRadius; }

private:
    void setupParameters(dataMode mode);
    void setupCoefficients();

    void loadMainProblemFile(const fs::path &fname,
        int &n, int **&iMain, double *&aMain, double fA, elpFACS &facs);
    void loadPerturbationFile(const fs::path &fname,
        int &n, int **&iPert, double *&aPert, double *&phase);

    void computeArguments(double T, elpArgs &args) const;
    double computeMainSum(int n, int **iMain, double *aMain,
        elpArgs &args, int dist) const;
    double computePerturbationSum(int n, int **iPert, double *aPert,
        double *phPert, elpArgs &args) const;

protected:
    double period;
    double boundingRadius;

    elpParams params;
    elpFACS facs;
    elpData coefs;
};