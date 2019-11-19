//
// Created by ilaria on 2019-11-16.
//

#include "measures.h"

void energy(struct Measures &mis, struct H_parameters &Hp, struct Node* Site){

    unsigned int ix, iy, iz, alpha, vec;
    double h_Potential=0., h_Kinetic=0., h_Josephson=0., h_B=0., h_tot=0.;
    double F_A=0;
    double h2=(Hp.h*Hp.h);
    struct O2 S_gauge;

    for(ix=0; ix<Lx; ix++){
        for(iy=0; iy<Ly; iy++){
            for(iz=0; iz<Lz; iz++){
                for(alpha=0; alpha<3; alpha++) {
                    //Potential= (a+ 3/h²)*|Psi_{alpha}(r)|² + b/2*|Psi_{alpha}(r)|⁴
                    h_Potential += (O2norm2(Site[ix + Lx * (iy + iz * Ly)].Psi[alpha]) * ((Hp.a + (3. / h2)) +
                                                                                          (0.5 * Hp.b *
                                                                                           O2norm2(Site[ix + Lx * (iy +
                                                                                                                   iz *
                                                                                                                   Ly)].Psi[alpha]))));
                    //Kinetic= -(1/h²)*\sum_k=1,2,3 |Psi_{alpha}(r)||Psi_{alpha}(r+k)|* cos(theta_{alpha}(r+k) - theta_{alpha}(r) +h*e*A_k(r))
                    for (vec = 0; vec < 3; vec++) {
                        //I can rewrite the kinetic energy in terms of the scalar product between the vector in (r) and the vector in (r+k) rotated according to A
                        S_gauge.t = (Site[nn(ix, iy, iz, vec, 1)].Psi[alpha].t +
                                     (Hp.h * Hp.e * Site[ix + Lx * (iy + iz * Ly)].A[vec]));
                        S_gauge.r = Site[nn(ix, iy, iz, vec, 1)].Psi[alpha].r;
                        polar_to_cartesian(S_gauge);
                        h_Kinetic -= ((1. / h2) * O2prod(Site[ix + Lx * (iy + iz * Ly)].Psi[alpha], S_gauge));
                    }
                }
                for(alpha=0; alpha<3; alpha++) {
                    for(vec=0; vec<3; vec++) {
                        if(vec != alpha) {
                            //Josephson= eta* \sum_beta!=alpha |Psi_{alpha}(r)||Psi_{beys}(r)|* cos(theta_{alpha}(r) - theta_{beta}(r))
                            h_Josephson += (Hp.eta * O2prod(Site[ix + Lx * (iy + iz * Ly)].Psi[alpha], Site[ix + Lx * (iy + iz * Ly)].Psi[vec]));
                            F_A=(Site[ix+ Lx*(iy+ iz*Ly)].A[alpha] + Site[nn(ix, iy, iz, alpha, 1)].A[vec] - Site[nn(ix, iy, iz, vec, 1)].A[alpha] - Site[ix+ Lx*(iy+ iz*Ly)].A[vec]);
                            h_B+=((0.5/h2)*(F_A*F_A));
                        }
                    }

                }
            }
        }
    }

    mis.E_kin=(double) h_Kinetic/N;
    mis.E_pot=(double) h_Potential/N;
    mis.E_Josephson=(double) h_Josephson/N;
    mis.E_B=(double) h_B/N;
    h_tot= h_Potential + h_Kinetic +  h_Josephson +h_B;
    mis.E=(double) h_tot/N;
}

void dual_stiffness(struct Measures &mis, struct H_parameters &Hp, struct Node* Site){

    double qx_min=C_TWO_PI/(Lx*Hp.h);
    double invNorm= 1./((C_TWO_PI)*(C_TWO_PI)*N);
    unsigned int ix, iy, iz;
    double Re_rhoz=0.;
    double Im_rhoz=0.;
    double Dx_Ay=0., Dy_Ax=0.;

    for(ix=0; ix<Lx;ix++){
        for(iy=0; iy<Ly;iy++){
            for(iz=0; iz<Lx;iz++){
                Dx_Ay=(Site[nn(ix, iy, iz, 0, 1)].A[1]- Site[ix + Lx*(iy+ Ly*iz)].A[1])/Hp.h;
                Dy_Ax=(Site[nn(ix, iy, iz, 1, 1)].A[0]- Site[ix + Lx*(iy+ Ly*iz)].A[0])/Hp.h;
                Re_rhoz+=(cos((double)qx_min*ix)*(Dx_Ay -Dy_Ax));
                Im_rhoz+=(sin((double)qx_min*ix)*(Dx_Ay -Dy_Ax));
            }
        }
    }
    mis.d_rhoz=invNorm*((Re_rhoz*Re_rhoz) +(Im_rhoz*Im_rhoz));
}

void magnetization(struct Measures &mis, struct Node* Site){
    //The Ising parameter m(x,y)=+/-1 indicates the chirality of the three phases. If the phases are ordered as: phi_1, phi_2, phi_3 then m=1; otherwise if the order is phi_1, phi_3, phi_2 then m=-1.
    unsigned ix, iy, iz, alpha;
    double *phi_shifted;

    phi_shifted=(double *) calloc(3, sizeof(double));

    for(ix=0; ix<Lx;ix++) {
        for (iy = 0; iy < Ly; iy++) {
            for (iz = 0; iz < Lx; iz++) {
                for(alpha=0; alpha<3; alpha++){
                    phi_shifted[alpha]=Site[ix +Lx*(iy+Ly*iz)].Psi[alpha].t - Site[ix +Lx*(iy+Ly*iz)].Psi[0].t;
                    while(phi_shifted[alpha] >= C_TWO_PI){ phi_shifted[alpha]-= C_TWO_PI;}
                    while(phi_shifted[alpha]< 0){ phi_shifted[alpha]+=C_TWO_PI;}
                }
                if(phi_shifted[1]>=phi_shifted[2]){
                    mis.m+=1;
                }else{
                    mis.m+=(-1);
                }
            }
        }
    }
    mis.m=mis.m/N;
}

void density_psi(struct Measures &mis, struct Node* Site){

    unsigned ix, iy, iz, alpha;

    for(ix=0; ix<Lx;ix++) {
        for (iy = 0; iy < Ly; iy++) {
            for (iz = 0; iz < Lx; iz++) {
                for (alpha = 0; alpha < 3; alpha++) {
                    mis.density_psi[alpha]+=(Site[ix+Lx*(iy +Ly*iz)].Psi[alpha].r);
                }
            }
        }
    }

    for (alpha = 0; alpha < 3; alpha++) {
        mis.density_psi[alpha]/=N;
    }


}



void measures_init(struct Measures &mis){
    //Initialization
    mis.d_rhoz=0.;
    mis.m=0.;
    mis.E=0.;
    mis.E_pot=0.;
    mis.E_kin=0.;
    mis.E_Josephson=0.;
    mis.E_B=0.;
    mis.density_psi=(double *) calloc(3, sizeof(double));
}

void measures_reset(struct Measures &mis){
    //Initialization
    mis.d_rhoz=(double)0.0;
    mis.m=(double)0.0;
    mis.E=(double)0.0;
    mis.E_pot=(double)0.0;
    mis.E_kin=(double)0.0;
    mis.E_Josephson=(double)0.0;
    mis.E_B=(double)0.0;
    memset(mis.density_psi, 0, 3*sizeof(double));
}

