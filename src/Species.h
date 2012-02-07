////////////////////////////////////////////////////////////////////////////////
//
// Species.h:
//
////////////////////////////////////////////////////////////////////////////////
// $Id: Species.h,v 1.6 2010/08/26 17:44:16 draeger1 Exp $

#ifndef SPECIES_H
#define SPECIES_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <complex>
using namespace std;
#include "Context.h"
#include "Basis.h"

class Species {
  private:
  
  const Context& ctxt_;
  int nlm_;             // number of non-local projectors: 
  int ndft_;
  
  vector<vector<double> > vps_spl_, phi_spl_;
  vector<double>          gspl_, vlocg_, vlocg_spl;
  vector<vector<double> > vnlg_, vnlg_spl;
  vector<double> wsg_;  // wsg_[l] Kleinman-Bylander weight 1/<phi|delta_V|phi>
  
  vector<double> rps_;  // radial linear mesh (same for all l)
  
  string name_;         // name used to refer to species in current application
  string uri_;          // uri of the resource defining the pseudopotential

  string symbol_;
  int atomic_number_;
  double mass_;        // mass in a.m.u (Carbon = 12.0)
  
  string description_; // description of the pseudopotential
  int zval_;           // valence charge
  int lmax_;           // largest angular momentum
  int llocal_;         // angular momentum taken as local
  int nquad_;          // number of semi-local quadrature points
  double rquad_;       // end of semi-local quadrature interval
  double deltar_;      // mesh spacing for potentials and wavefunctions
  vector<vector<double> > vps_;  // potentials for each l
  vector<vector<double> > phi_;  // atomic wavefunctions for each l
  double rcps_;        // cutoff radius of gaussian pseudocharge
  int initsize_;       // potential grid size read from file
  double hubbard_u_;   // Hubbard U for DFT+U
  int hubbard_l_;      // angular momentum of (localized) orbitals for Hubbard correction
  double hubbard_alpha_;   // Hubbard alpha for DFT+U (used in auto-detection of U)
  vector<double> phig_, phig_spl_;  // atomic orbitals of Hubbard l in reciprocal space
  vector<double> phir_;  // temporary storage array for Hubbard orbitals
  bool nlcc_;            // true if potential has non-linear core correction
  
  // ultrasoft potential terms
  bool usoft_;                             // true if potential is ultrasoft
  int nbeta_;                              // number of projectors
  int nbetalm_;                            // total number of lm combinations
  int betalmax_;                           // max value of l 
  vector<int> betalm_l_;                   // value of l for given lm index
  vector<int> betalm_m_;                   // value of m for given lm index
  vector<int> betaind_;                    // index of beta fn for given lm index
  vector<vector<double> > dzero_;          // D_nm^0
  vector<double> rinner_;                  // rinner:  outer radii of Qnm^L(r) expansion
  vector<vector<vector<double> > > qfcoeff_; // coefficients in Qnm^L(r) expansion
  vector<vector<int> > qfunind_;           // index of qfun for given nm
  vector<int> qfunl1_;                     // angular momentum l1 for given qfung
  vector<int> qfunl2_;                     // angular momentum l2 for given qfung
  vector<int> qfunb1_;                     // beta index n for given qfung
  vector<int> qfunb2_;                     // beta index m for given qfung
  int nqfun_;                              // number of input charge augmentation functions Q_ij(r)
  int betarsize_;                          // number of linear grid points in betar_
  vector<int> betal_;                      // angular momentum of each beta fn
  vector<vector<double> > betar_;          // ultrasoft projectors, real space
  vector<vector<double> > betag_;          // ultrasoft projectors, reciprocal space
  vector<vector<double> > betag_spl_;      // ultrasoft projectors spline
  vector<vector<double> > qfunr_;          // augmentation charge function
  vector<vector<vector<double> > > qfung_;     // augmentation charge function
  vector<vector<vector<double> > > qfung_spl_; // augmentation charge function
  vector<vector<double> > dmat_;           // nonlocal proj coeffs
  int nqtot_;                              // number of Q_ij allowed by Clebsch-Gordan symmetry
  vector<int> qnm_lm1_;                    // lm1 allowed by C-G symmetry
  vector<int> qnm_lm2_;                    // lm2 allowed by C-G symmetry
  vector<int> ncgcoeff_;                   // num. of C-G coeffs for this lm1, lm2
  vector<vector<double> > cgcoeff_;        // Clebsch-Gordan coeffs
  vector<vector<int> > cgltot_;            // ltot of Clebsch-Gordan coeffs
  vector<vector<int> > cgmtot_;            // mtot of Clebsch-Gordan coeffs
  void bessel_trans(int l, vector<double> &fnr, vector<double> &fng); // spherical Bessel transform

  vector<double> rhor_nlcc_; // non-linear core correction, radial fn. in real space
  vector<double> rhog_nlcc_; // non-linear core correction, radial fn. in reciprocal space
  vector<double> rhog_nlcc_spl_;
  
  public:

  bool fix_rcps;  // if true, Ewald radius will be kept at user-supplied value and 
                  // not chosen automatically by AtomSet.set_rcps

  Species(const Context& ctxt, string name);
  
  const Context& context(void) const { return ctxt_; }
  const string& name(void) const { return name_; }
  const string& symbol(void) const { return symbol_; }
  const string& description(void) const { return description_; }
  const string& uri(void) const { return uri_; }
  int atomic_number(void) const { return atomic_number_; }
  int zval(void) const { return zval_; }
  double mass(void) const { return mass_; }
  int lmax(void) const { return lmax_; }
  int llocal(void) const { return llocal_; }
  int nquad(void) const { return nquad_; }
  double rquad(void) const { return rquad_; }
  double deltar(void) const { return deltar_; }
  double rcps(void) const { return rcps_; }
  
  // number of non-local projectors sum_(l!=llocal) (2*l+1)
  int nlm(void) { return nlm_; }
  
  bool non_local(void) { return lmax_ > 0; };
  double eself(void)
  { return zval_ * zval_ / ( sqrt ( 2.0 * M_PI ) * rcps_ ); };

  void vpsr(int l, double r, double &v);               // Vps(l,r) in r space
  void dvpsr(int l, double r, double &v, double &dv);  // Vps and dVps/dr 
  void vlocg(double q, double &v);                     // Vloc(g)
  void dvlocg(double q, double &v, double &dv);        // Vloc(g) and dVloc/dg
  void vnlg(int l, double q, double &v);               // Vnl(l,g)
  void dvnlg(int l, double q, double &v, double &dv);  // Vnl(l,g) and dVnl/dg
  void phig(double q, double &v);                      // phi(g) at hubbard_l
  void dphig(double q, double &v, double &dv);         // phi(g), dphi/dg at hubbard_l
  int nbeta(void) { return nbeta_; }                   // nbeta for this species
  int nbetalm(void) { return nbetalm_; }               // nbetalm for this species
  int betal(int b) { return betal_[b]; }               // l for given beta fn.
  int betalm_l(int lm) { return betalm_l_[lm]; }       // l for given beta lm index
  int betalm_m(int lm) { return betalm_m_[lm]; }       // m for given beta lm index
  int qnm_lm1(int qind) { return qnm_lm1_[qind]; }     // lm1 for given qind
  int qnm_lm2(int qind) { return qnm_lm2_[qind]; }     // lm2 for given qind
  void betag(int b, double q, double &v);              // beta_b(g) 
  void dbetag(int b, double q, double &v, double &dv); // beta_b(g) and dbeta_b/dg
  double qfung(int q, int ltot, double g);             // Q_nm^L(g)
  double rhog_nlcc(double g);
  double rhopsg(double q);        // pseudocharge in g space
  double wsg(int l) { return wsg_[l]; };
  double rcut_loc(double epsilon); // radius beyond which potential is local
  void set_hubbard_u(double uval, int lval);
  double hubbard_u(void) { return hubbard_u_; }
  int hubbard_l(void) { return hubbard_l_; }
  double hubbard_alpha(void) { return hubbard_alpha_; }
  void calc_qnmg(Basis* basis_, vector<complex<double> > &qtotg, vector<double> &qaug);
  int nqfun(void) { return nqfun_; }
  int nqtot(void) { return nqtot_; }
  void qind_to_beta(int qind, int &b1, int &b2);
  void qind_to_betalm(int qind, int &lm1, int &lm2);
  double dzero(int qind);  // return D_nm^0 for qind=n,m
  double ylm(int l, int m, double gx, double gy, double gz);  // spherical harmonics
  bool ultrasoft(void) { return usoft_; }
  bool nlcc(void) { return nlcc_; }
  
  const vector<vector<double> >& vps(void) const { return vps_; }
  const vector<vector<double> >& phi(void) const { return phi_; }
  
  bool initialize(double rcps);
  void info(ostream& os);
  void printsys(ostream& os) const;
  
  friend class SpeciesReader;
  friend class SpeciesHandler;
  
};
ostream& operator << ( ostream &os, Species &a );
class SpeciesInitException
{
  public:
  string msg;
  SpeciesInitException(string s) : msg(s) {}
};
#endif