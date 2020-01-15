///
/// @file 
/// @brief Test of Runge-Kutta solver for series of ODEs
/// @author Bob Hirosky
/// @date 31 Dec 2019 
/// 
/// Use the Rk4 solver for coupled ODEs to solve for projectile 
/// motion with air resistance
///
/// Definition of our variables
/// x    = time <br>
/// y[0] = position along i axis  ; f_ri = dri/dt => velocity along i axis  <br>
/// y[1] = velocity along i axis  ; f_vi = dvi/dt => acceleration along i axis <br>
/// y[2] = position along j axis  ; f_rj = drj/dt => velocity along j axis <br>
/// y[3] = velocity along j axis  ; f_vj = dvj/dt => acceleration along j axis <br>


#include "runge-kutta/RKn.hpp"
#include "TROOT.h"
#include "TApplication.h"
#include "TLegend.h"
#include "TFile.h"
#include "TStyle.h"
#include "TGClient.h"
#include "TF1.h"
#include "TCanvas.h"
#include <iostream>

using namespace std;
const double b=0.25;
const double a=0.5;
const double F_0=2.0;
const double omega=2.4;
const double gam=0.1;
const double m=1.0;     ///< mass of object [kg], nb simple projectile motion does not depend on the mass


// functions to describe simple projectile motion
// here use use ri,rj,rk to define directions to prevent confusion with
// standard ODE notation, where x=independent variable, \vec y=dependent variable(s)


/// \brief Change in position along \f$\hat i\f$ axis
/// \param[in] x independent variable
/// \param[in] y dependent variables
double f_ri(double x, const vector<double> &y){ 
  (void) x;   // prevent unused variable warning
  return y[1];
}

/// \brief Change in velocity along  \f$\hat i\f$ axis
/// \param[in] x independent variable
/// \param[in] y dependent variables
double f_vi(double x, const vector<double> &y){ 
  (void) x;
  return (-gam*y[1] + 2*a*y[0] - 4*b*pow(y[0],3) + F_0*cos(omega*x) ) / m;
  // return 0;  // if no air, no forces/acceleration along i direction in this problem
}


/// \brief Stopping condition
/// \param[in] x independent variable
/// \param[in] y dependent variables
///
/// Returns 0(1) to flag continuation(termination) of calculation 
double f_stop(double x, const vector<double> &y){
  (void) x;
  if (x>100) return 1;  // stop calulation if the current step takes height to negative value
  return 0;  // continue calculation
}
/// \brief Use RK4 method to describe simple projectile motion.
int main(int argc, char **argv){
  TApplication theApp("App", &argc, argv); // init ROOT App for displays

  // ******************************************************************************
  // ** this block is useful for supporting both high and std resolution screens **
  UInt_t dh = gClient->GetDisplayHeight()/2;   // fix plot to 1/2 screen height  
  //UInt_t dw = gClient->GetDisplayWidth();
  UInt_t dw = 1.1*dh;
  // ******************************************************************************
  

  // *** test 2: Use RK4SolveN to calculate simple projectile motion
  vector<pfunc_t> v_fun(2);   // 4 element vector of function pointers
  v_fun[0]=f_ri;
  v_fun[1]=f_vi;
  vector<double> y0(2);
  // initial conditions are starting position, velocity and angle, equivalently ri,rj,vi,vj
  y0[0]=0.5;   // init position on i-axis
  y0[1]=0;  // init velocity along i axis
  auto tgN = RK4SolveN(v_fun, y0, 10000, 0, 100, f_stop);
  TCanvas *c2 = new TCanvas("c2","ODE solutions 2",dw,dh);
  tgN[0].Draw("AL");
  c2->Draw();

  y0[0] = 0.5001;
  y0[1] = 0;
  auto tgN2 = RK4SolveN(v_fun, y0, 10000, 0, 100, f_stop);
  tgN2[0].Draw("L");
  tgN2[0].SetLineColor(kRed);

  // save our graphs
  TFile *tf=new TFile("RKnDemo.root","recreate");
  for (unsigned i=0; i<v_fun.size(); i++){
    tgN[i].Write();
  }
  tf->Close();

  
  cout << "Press ^c to exit" << endl;
  theApp.SetIdleTimer(30,".q");  // set up a failsafe timer to end the program  
  theApp.Run();
}

