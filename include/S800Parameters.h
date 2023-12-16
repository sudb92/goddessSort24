/*! \file S800Parameters.h
    \brief Parameter and function definitions for S800 analysis.
    
    This provides the variables, parameters and functions required for
    S800 analysis, based on the original S800.h SpecTcl source code from
    NSCL. 

    Author: H. Crawford

    Date: January 2011
*/

#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#include "S800Definitions.h"
#include "GRETINA.h"

#include "TObject.h"
#include "TMath.h"
#include "TVector3.h"
#include "TString.h"
#include <cstring>
#include <vector>
#include <RConfigure.h>

#define DEBUGS800 0
#define MAXNSCALER 260

class S800Full;
class S800Scaler;
using namespace std;
static const unsigned short tppacmapx[64] =
  { 30, 31, 28, 29,   26, 27, 24, 25,   22, 23, 20, 21,   18, 19, 16, 17,
    14, 15, 12, 13,   10, 11,  8,  9,    6,  7,  4,  5,    2,  3,  0,  1,
    33, 32, 35, 34,   37, 36, 39, 38,   41, 40, 43, 42,   45, 44, 47, 46,
    49, 48, 51, 50,   53, 52, 55, 54,   57, 56, 59, 58,   61, 60, 63, 62 };

static const unsigned short tppacmapy[64] =
  { 0 ,  1,  2,  3,    4,  5,  6,  7,    8,  9, 10, 11,   12, 13, 14, 15,
    16, 17, 18, 19,   20, 21, 22, 23,   24, 25, 26, 27,   28, 29, 30, 31,
    63, 62, 61, 60,   59, 58, 57, 56,   55, 54, 53, 52,   51, 50, 49, 48,
    47, 46, 45, 44,   43, 42, 41, 40,   39, 38, 37, 36,   35, 34, 33, 32 };

/*! \class S800ImMap
    \brief Class for implementation of an intermediate image inverse 
    map for the S800.

    Class for use of an inverse map based on information from the 
    intermediate image detectors of the analysis line/S800.  Access is
    by s800->im.track.map.<..>.
*/

class S800ImMap : public TObject {
 private:
  S800Full* m_top;

 public:
  UShort_t maxcoefficient[S800_II_TRACK_PARAMETERS];
  UShort_t order[S800_II_TRACK_PARAMETERS][S800_II_TRACK_COEFFICIENTS];
  UShort_t exponent[S800_II_TRACK_PARAMETERS][S800_II_TRACK_PARAMETERS][S800_II_TRACK_COEFFICIENTS];
  Double_t coefficient[S800_II_TRACK_PARAMETERS][S800_II_TRACK_COEFFICIENTS];

 public:
  /* Variables */
  double maxcoefficients;
  double maxparameters;
  double maxorder;

  TString imMapFilename;

 public:
  void Initialize(S800Full *s800);
  /*! \fn void Initialize(S800Full *s800);
      \brief Initialization for S800 intermediate image inverse map parameters.
      \param s800 Instance of the parent class, S800Full
      \return No return -- void

      Initializes the intermediate image inverse map parameters to 0, and 
      then loads the inverse map with the member function 
      LoadInverseMap(TString filename) and the class member variable 
      imMapFilename.
  */

  void LoadInverseMap(TString filename);
  /*! \fn void LoadInverseMap(TString filename)
      \brief Loads S800 intermediate image inverse map from standard format
      generated by S800 map server.
      \param filename TString name of the inverse map file.
      \return No return -- void

      Loads the intermediate image inverse map for the S800 analysis, from
      the provided filename.  Assumes the standard inverse map format as
      given by the online S800 map server.
  */

  void Reset();
  /*! \fn void Reset()
      \brief Resets the intermediate image inverse map parameters to 0.
      \param None -- resets class member variables.
      \return No return -- void

      Resets all S800 intermediate image inverse map parameters to 0.
  */

  Bool_t WasLoaded() {
    if (maxorder == 0) { return false; }
    return true;
  }
  /*! \fn Bool_t WasLoaded()
      \brief Checks if intermediate image inverse map has been loaded.
      \param None -- checks based on class member variable maxorder.
      \return Bool_t value of 0 (false) if map is not loaded, and 1 
      (true) if it is.
      
      Function checks the value of class member variable maxorder
      to determine if the S800 intermediate image inverse map has been
      loaded or not.
  */

  double Calculate(int calcorder, int parameter, double *input);
  /*! \fn double Calculate(int calcorder, int parameter, double *input) 
      \brief Calculates the intermediate image inverse map parameters.
      \param calcorder Int_t value representing the order of the inverse map
      to be calculated
      \param parameter Int_t parameter number to be calculated
      \param input Double_t input values for map calculation
      \return Double_t Returns summed value of map parameters.

      Function calculates the intermediate image map parameters based
      on the input from the inverse map file, and up to the order specified
      in the function arguments.
  */

  ClassDef(S800ImMap, 1);
};

/*! \class S800Map
    \brief Class for implementation of an S800 inverse map.

    Class for use of an inverse map based on information from the 
    S800 fp detectors.  Access is by s800->fp.track.map.<..>.
*/

class S800Map : public TObject {
 private:
  S800Full* m_top;

 public:
  UShort_t maxcoefficient[S800_TRACK_PARAMETERS];
  UShort_t order[S800_TRACK_PARAMETERS][S800_TRACK_COEFFICIENTS];
  UShort_t exponent[S800_TRACK_PARAMETERS][S800_TRACK_PARAMETERS][S800_TRACK_COEFFICIENTS];
  Double_t coefficient[S800_TRACK_PARAMETERS][S800_TRACK_COEFFICIENTS];
  
 public:
  /* Variables */
  double maxcoefficients;
  double maxparameters;
  double maxorder;
  
  TString mapFilename;

 public:
  void Initialize(S800Full *s800);
  void LoadInverseMap(TString filename);
  void Reset();
  Bool_t WasLoaded() 
  { if (maxorder == 0) return false;
    return true;
  }
  double Calculate(int calcorder, int parameter, double *input);
  
  ClassDef(S800Map, 1);
};

/*! \class S800FpCrdcPads
    \brief Class for raw S800 focal plane CRDC detector pad information

    
    A CRDC contains an anode and 256 cathode pads.  Signal pads are 
    sampled and read out.  For each pad we have to 'integrate' the signal,
    obtaining a 'projection' of 256 pads.  In this projection we have 
    to figure out the position information.  
    
    Up to now only >>> S800_FP_CRDC_PACKET <<< implemented. 
    
    Unpacking of a S800_FP_CRDC_RAW_PACKET:                 
    - highest bit controls if (sampletime, channel) (on)  
      or (sampletime, channelbase) (off) 
    - crdc channel is channelbase*64 + channel            
    - sampletime 0xxx xxxx xx00 0000,                     
    
      channel    0000 0000 00xx xxxx,                     
    
      sample     0000 00xx xxxx xxxx,                     
      
      chanbase   0000 xx00 0000 0000                      
                                                            
      It is not necessary to hold a 256-512 array for each    
      event.  We need the (x) projection m_projX, for faster  
      processing we remember the range [m_minChn...m_maxChn]  
      where we have data.  m_threshold is a value obtained    
      from the data stream.                                   
      
      Projected pad data is calibrated via m_caltable.        
      (MaxValue of m_projX = MaxValue of sample = 0x03FF.) 

      This CRDC pad class provides the unpacking routines for
      raw CRDC data, pad calibration, and picks out the CRDC data
      that must be remembered for further processing.
      
      Access is by s800->fp.crdcX.pad.<..>.
*/

/* Forward declaration */
class S800FpCrdcCalc;

class S800FpCrdcPads : public TObject {
 private:
  S800Full* m_top;

  int m_width;
  double m_dprojX[S800_FP_CRDC_CHANNELS]; /* For classic SpecTcl method */
  int m_caltable[S800_FP_CRDC_CHANNELS][0x0400];
  
  UShort_t* UnpackRawDataFastClassic(UShort_t *p);
  UShort_t* UnpackRawDataFast(UShort_t *p);
  
 public:
  UShort_t m_minChn;
  UShort_t m_maxChn; /* Actually max+1 (easier loops) */
  
  /* In theory, should use list rather than vector, especially for
     deleting members which might occur.  In reality, a big drop in
     performance occurs when changing to list.  Maybe memory handling 
     is more efficient for vector... */
  
  vector <int> m_padlist;
  vector <int>::iterator m_padlist_iter;

  UShort_t m_samples[S800_FP_CRDC_CHANNELS];

  /* Parameters */
  double raw[S800_FP_CRDC_CHANNELS];
  double cal[S800_FP_CRDC_CHANNELS];
  int samples[S800_FP_CRDC_CHANNELS];
  double samplemax[S800_FP_CRDC_CHANNELS];
  double samplemin[S800_FP_CRDC_CHANNELS];
  double delta[S800_FP_CRDC_CHANNELS];
  
  /* Internal use values */
  int m_samplemax[S800_FP_CRDC_CHANNELS];
  int m_samplemin[S800_FP_CRDC_CHANNELS];
  
  void CalcDelta(int center, S800FpCrdcCalc* calc);
  
  /* Variables */
  double ped[S800_FP_CRDC_CHANNELS];
  double minsamples[S800_FP_CRDC_CHANNELS];
  double m_thresh[S800_FP_CRDC_CHANNELS];
  double slope[S800_FP_CRDC_CHANNELS];
  double offset[S800_FP_CRDC_CHANNELS];
  
 public:
  void Initialize(S800Full *s800); 
  void Reset();
  UShort_t* UnpackRawData(UShort_t *p);
  UShort_t* UnpackRawDataClassic(UShort_t *p);
  void AverageCal(int integration);
  void BuildLookUp();

  ClassDef(S800FpCrdcPads, 1);

};

/**************************************************************/
/* CRDC calculated parameters s800->fp.crdcX.calc.<..>        */
/**************************************************************/

/* Forward declaration */
class S800FpCrdc;

class S800FpCrdcCalc : public TObject {
 private:
  S800Full* m_top;

  bool m_badpad[S800_FP_CRDC_CHANNELS];
  int m_saturationvalue;
  void FindPadMax();
  void CalcSkew(int left, int right, double center);
  
 public:
  /* Parameters */
  double padmax;
  double padsum;
  double padused;
  
  double maxpad[S800_FP_CRDC_CHANNELS];
  double maxpad0[S800_FP_CRDC_CHANNELS];
  
  double padcalc[S800_FP_CRDC_CHANNELS];
  double saturationWidth;
  
  double x_gravity;

  /* Variables */
  double gravity_width;
  double fit_width;
  double method;
  double saturation;
  double m_saturation;
  
  double badpads;
  double badpad[99];
  
  S800FpCrdcPads* m_pad;
  S800FpCrdc* m_parent;

 public:
  void Initialize(S800Full *s800, S800FpCrdcPads *pad, S800FpCrdc *fpcrdc);
  void Reset();
  void CalculateGravity();
  void CalculateGravityClassic();
  void CheckBadPads();
  
  bool isbadpad(int chn) { return m_badpad[chn]; }

  ClassDef(S800FpCrdcCalc, 1);

};
  
/**************************************************************/
/* CRDC Parameters s800->fp.crdcX.<..>                        */
/*                                                            */
/*    Unpacking of a S800_FP_CRDC_RAW_PACKET:                 */
/*      - highest bit controls if (sampletime, channel) (on)  */
/*                         or (sampletime, channelbase) (off) */
/*      - crdc channel is channelbase*64 + channel            */
/*      - sampletime 0xxx xxxx xx00 0000,                     */
/*        channel    0000 0000 00xx xxxx,                     */
/*        sample     0000 00xx xxxx xxxx,                     */
/*        chanbase   0000 xx00 0000 0000                      */
/*                                                            */
/*    In general this data contains sampled signals of the    */
/*    CRDC pads.  We can represent this data as a matrix      */
/*    x:CRDC-chn vs. y-Sample(t).  From the projection on     */
/*    x-axis we calculate center-of-mass of distribution and  */
/*    get the position of the event.                          */
/*                                                            */
/*    + S800_FP_CRDC_ANODE_PACKET (Len(=4), Tag, anode, tac)  */
/**************************************************************/

class S800FpCrdc : public TObject {
 private:
  S800Full* m_top;
  
 public:
  S800FpCrdcPads pad;
  S800FpCrdcCalc calc;

  /* Parameters */
  double x; /*! CRDC x position, in mm */
  double y; /*! CRDC y position, in mm */
  double tac;
  double anode;

  double evtlength;

  /* Variables */
  double x_offset; /*! CRDC x calibration offset, in mm */
  double x_slope; /*! CRDC x calibration slope, in mm/pad */
  double y_offset; /*! CRDC y calibration offset, in mm */
  double y_slope; /*! CRDC y calibration slope, in mm/ns */
  double xy_offset;
  double xy_slope;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  void CalculatePosition();
  UShort_t* Unpack(UShort_t *p);

  ClassDef(S800FpCrdc, 1);

};
  
/**************************************************************/
/* Tracking PPAC s800->im.track.tppacX.<..>                   */
/**************************************************************/

class S800Tppac : public TObject {
 private:
  S800Full* m_top;

  bool m_badxstrip[64];
  bool m_badystrip[64];

 public:
  /* Parameters */
  double xstrip[64];
  double ystrip[64];
  double xsum;
  double ysum;
  double xmult;
  double ymult;

  double x;
  double y;
  
  /* Variables */
  double badxstrips;
  double badxstrip[20];
  double badystrips;
  double badystrip[20];
  double width;
  double xcenter;
  double ycenter;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  void CheckBadStrips();
  void Calculate(int xleft, int xright, int yleft, int yright);
  
  ClassDef(S800Tppac, 1);
};
  
/**************************************************************/
/* Tracking PPAC s800->im.track.<..>                          */
/*                                                            */
/*    The data has the same structure as CRDC_RAW_PACKET.     */
/*    We have to do the same projection, but this 256 chn     */
/*    data contains 4*64 chn for four detectors.              */
/*                                                            */
/*    I need a channel mapping for xstrips (det = even), and  */
/*    ystrips (det = odd) given by tppacmap? [64]             */
/*                                                            */
/*    S800_II_TRACK_RAW_PACKET type will be processed         */
/**************************************************************/  

class S800Track : public TObject {
 private:
  S800Full* m_top;

  int m_thresh;
  int m_projX [4][S800_II_TRACK_CHANNELS/4];
  int m_minChn[4]; /* x(det1) y(det1) x(det2) y(det2) */
  int m_maxChn[4];
  int m_width;

 public:
  S800Tppac tppac1;
  S800Tppac tppac2;

  /* Parameters */
  double xim; /*!< dispersive direction position at S800 image, in m */
  double yim; /*!< non-dispersive direction position at S800 image, in m */
  double aim; /*!< dispersive angle at S800 image, in radians */
  double bim; /*!< dispersive angle at S800 image, in radians */
  double dim;
  double xta;
  double yta;
  double ata;
  double bta;

  /* Variables */
  double order;

  S800ImMap map;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* UnpackRawData(UShort_t *p);
  void CalculateTracking();

  ClassDef(S800Track, 1);
};

/**************************************************************/
/* FPIonChamber s800->fp.ic.<..>                              */
/*                                                            */
/*    S800_FP_IC_PACKET                                       */
/*    Packet style: (SubLen, S800_FP_IC_ENERGY_PACKET, data)  */
/*    Following 2nd packet is not in data stream:             */
/*            (SubLen, S800_FP_IC_TIME_PACKET, 2 tac data)    */
/*    therefore omitted!                                      */
/*                                                            */
/*    Understanding:                                          */
/*       The data contains up to S800_FP_IC_CHANNELS entries */
/*       for the S800_FP_IC_CHANNELS wires and 2 timing data */
/*       tac1, and tac2.  The charge (?) data has to be       */
/*       calibrated, a sum/S800_FP_IC_CHANNELS is calculated */
/*       in sum and calibrated again in de.                   */
/*                                                            */
/*    --------------------                                    */
/*    NEW                                                     */
/*    Calibration of de:                                      */
/*       Sum (strongly) depends on crdc x and y position.     */
/*       x dependence is exponential (strong path length      */
/*          dependent charge loss), y dependence linear       */
/*       Variable de_x0tilt, de_xtilt, and de_ytilt describe  */
/*          the correction, which is:                         */
/*            de += sum*ytilt*crdc.y                          */
/*            de *= exp((de_x0tilt-crdc.x)*de_xtilt),         */
/*                 if crdc.x < de_x0tilt, otherwise unchanged */
/*       After this, linear calibration is applied to de.     */
/*                                                            */
/**************************************************************/  

class S800FpIonChamber : public TObject {
 private:
  S800Full* m_top;

  int m_activeChannels; /* buildcaltable figures this out */

 public:
  UShort_t caltable_cal[S800_FP_IC_CHANNELS][0x0fff]; /* raw c[0..0x0fff] */
  UShort_t caltable_de[0x0fff];

  double caltable_xtilt[0x0fff];
  double caltable_ytilt[0x0fff];
  /* Remember, here indices are a crdc1.x/y value...crdc raw data
     is < 0x0400, therefore 0x0fff should be safe. */

 public:
  /* Parameters */
  double raw[S800_FP_IC_CHANNELS];
  double cal[S800_FP_IC_CHANNELS];
  
  double tac1;
  double tac2;
  double sum;
  double de;

  /* Variables */
  double slope[S800_FP_IC_CHANNELS];
  double offset[S800_FP_IC_CHANNELS];
  double active[S800_FP_IC_CHANNELS];
  
  double de_x0tilt;
  double de_xtilt;
  double de_ytilt;
  double de_slope;
  double de_offset;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p);
  void BuildLookUp();
  void CalibrateIcSum();

  ClassDef(S800FpIonChamber, 1);
};

/**************************************************************/
/* Scintillator s800->fp.eX.<..>                              */
/*                                                            */
/*    S800_FP_SCINT_PACKET                                    */
/*                                                            */
/*    Each scintillator consists of two PMTs.  Here a         */
/*    scintillator event eX.time eX.de is valid as soon as    */
/*    one valid PMT time is available (this means also valid  */
/*    for s800.invalid bit pattern).  Invalid, if             */
/*    trigger.s800 is missing.                                */
/*                                                            */
/*    The user can test:  eX.pos = -2000 if one PMT time is   */
/*                                       missing (invalid)    */
/*                        eX.pos = -2001 if one PMT energy is */
/*                                       0, both times valid  */
/*                                                            */
/**************************************************************/

class S800FpScintillator : public TObject {
 private:
  S800Full* m_top;

 public:
  /* Parameters */
  double de_up;
  double de_down;
  double time_up;
  double time_down;
  double de;
  double time;
  double pos;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  void Calculate();

  ClassDef(S800FpScintillator, 1);
};

/**************************************************************/
/* Tracking s800->fp.track.<..>                               */
/**************************************************************/

class S800FpTrack : public TObject {
 private:
  S800Full* m_top;

 public:
  /* Parameters */
  double xfp; /*!< Focal plane dispersive direction position, in m */
  double afp; /*!< FP dispersive angle, in radians */
  double yfp; /*!< FP non-dispersive direction, in m */
  double bfp; /*!< FP non-dispersive angle, in radians */
  double ata; /*!< Dispersive angle at the target, in radians */
  double yta; /*!< Non-dispersive position at the target, in m */
  double bta; /*!< Non-dispersive angle at the target, in radians */
  double dta; /*!< Fractional energy at the target, (E-E0)/E0, in parts */
  double azita; /*!< Phi polar angle at target, in radians */
  double scatter; /*!< Theta polar angle at target, in mradians */
  double energy; /*!< Energy at target, in MeV */
  double ptot; /*!< Total momentum, in MeV/c */
  double ppar; /*!< Parallel momentum, in MeV/c */
  double ptra; /*!< Transverse momentum, in MeV/c */
  
  /* Corrected angles for yta/dta correlation */
  double ata_cor;
  double bta_cor;
  double azita_cor;
  double scatter_cor;
  
  /* Variables */
  double anglea; /*!< Dispersive angle offset, in radians */
  double angleb; /*!< Non-dispersive angle offset, in radians */
  double brho; /*!< Brho of the S800, i.e. rigidity of a particle 
		 following a central trajectory */
  double mass;
  double charge;
  double order;
  double zfp; /*!< Displacement of FP position from CRDC1, 
		in the beam direction, in m */
  double gecorr;
  
  double anglea_cor;
  double angleb_cor;
  double ata_dtacor;
  double bta_ytacor;

  S800Map map;
  double beta0, deltabeta;
  
 public:
  void Initialize(S800Full *s800);
  void Reset();
  void CalculateTracking();

  ClassDef(S800FpTrack, 1);
};

/**************************************************************/
/* Hodoscope s800->fp.hodo.<..>                               */
/**************************************************************/

class S800FpHodo : public TObject {
 private:
  S800Full* m_top;
  
 public:
  /* Parameters */
  double raw[32];
  double cal[32];
  double max;
  double sum;
  double mult;

  /* Variables */
  double slope[32];
  double offset[32];
  double xCorr[32];
  
 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p);
  void CorrectX();
  
  ClassDef(S800FpHodo, 1);
};
  
/**************************************************************/
/* Focal Plane s800->fp.<..>                                  */
/*                                                            */
/*    Scintillators e1-e3 are in one data packet, and thus    */
/*    are unpacked by this class.                             */
/*                                                            */
/*    Zero suppressed data of energies AND times (if both     */
/*    have no data, then suppressed).                         */
/*    | Detector channel | SCINT channel | TIMES channel |    */
/*    |      E1_UP       |       0       |       0       |    */
/*    |     E1_DOWN      |       1       |       1       |    */
/*    |      E2_UP       |       2       |       2       |    */
/*    |     E2_DOWN      |       3       |       3       |    */
/*    |      E3_UP       |       4       |       4       |    */
/*    |     E3_DOWN      |       5       |       5       |    */
/*                                                            */
/*    If trigger.s800 is NOT valid, whole scintillator        */
/*    becomes invalid.  fp.eX.time and pos only set if both   */
/*    PMTs gave valid data.  If PMT time = 0 is read, PMT     */
/*    time parameter stays invalid.                           */
/*                                                            */
/*    For user scintillators are named e1, e2, and e3.  For   */
/*    'easier' bookkeeping in ScintUnpack() method, there is  */
/*    a helper array m_[] instead.                            */
/**************************************************************/

class S800FocalPlane : public TObject {
 private:
  S800Full* m_top;

  S800FpScintillator* m_e[3]; /* Internal for ScintUnpack() */

 public:
  S800FpScintillator e1;
#ifdef S800_LINK_E2
  S800FpScintillator e2;
#endif
#ifdef S800_LINK_E3
  S800FpScintillator e3;
#endif

  S800FpIonChamber ic;
  S800FpCrdc crdc1;
  S800FpCrdc crdc2;
  
  S800FpTrack track;
  S800FpHodo hodo;
  
  /* Variables */
  double gap;
  
 public:
  void Initialize(S800Full *s800);
  void Reset();
  void LoadValues();
  UShort_t* ScintUnpack(UShort_t *p);

  ClassDef(S800FocalPlane, 1);
};

/***************************************************************/
/* PPAC struct (e05109 - 2 classic PPACs @ intermediate image) */ 
/*     s800->im.ppacX.<..>                                     */
/* Length, S800_II_PPAC_PACKET, 4 wrds 1st PPAC, 2nd PPAC      */       
/***************************************************************/

class S800ClassicPpac : public TObject {
 private:
  S800Full* m_top;

 public:
  /* Parameters */
  double up; 
  double down;
  double left;
  double right;

  double x; /* mm */
  double y; /* mm */

  /* Variables */
  double z;
  double u_offset;
  double u_slope;
  double d_offset;
  double d_slope;
  double l_offset;
  double l_slope;
  double r_offset;
  double r_slope;
  double x_offset;
  double x_slope;
  double y_offset;
  double y_slope;
  
 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p, int ChnOffSet);

  ClassDef(S800ClassicPpac, 1);
};

/**************************************************************/
/* Intermediate image s800->im.<..>                           */
/**************************************************************/

class S800Image : public TObject {
 private:
  S800Full* m_top;
  
 public:
#ifdef S800_LINK_IMAGE_TRACK
#ifdef S800_LINK_IMAGE_PPAC
#error Tppac OR ppac, not both...
#endif
  S800Track track;
#endif
  
#ifdef S800_LINK_IMAGE_PPAC
  S800ClassicPpac ppac1;
  S800ClassicPpac ppac2;
#endif
  
  /* Parameters */
  double x;
  double theta;
  double y;
  double phi;
  double dp;
  
  /* Variables */
  double z;
  double gap;
  double dpphi;

  bool trigger;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  void Calculate();
  
  ClassDef(S800Image, 1);
};

/**************************************************************/
/* Timestamp s800->trigger.timestamp.<..>                     */
/*                                                            */
/*  Timestamp for e01018 GRETINA test                         */
/**************************************************************/

class S800TimeStamp : public TObject {
 private:
  S800Full* m_top;

  double m_prevStamp;
  
 public:
  /* Parameters */
  double stampHigh;
  double stampMid;
  double stampLow;
  double stampLowest;
  double diff;
  long long int timestamp;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p);

  ClassDef(S800TimeStamp, 1);
};

class S800EventNumber : public TObject {
 private:
  S800Full* m_top;
  
 public:
  /* Parameters */
  long long int eventNum;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p);
  
  ClassDef(S800EventNumber, 1);
};

/**************************************************************/
/* Trigger s800->trigger.<..>                                 */
/*                                                            */
/*  S800_TRIGGER_PACKET                                       */
/*   | Detector channel | TIMES channel |                     */
/*   |   S800 source    |      8        |                     */
/*   | External1 source |      9        |                     */
/*   | External2 source |     10        |                     */
/*   | Secondary source |     11        |                     */
/*                                                            */
/*  Assumed:  maximum 4 data words, channel coded in 4        */
/*            highest bits                                    */
/*  Unpacker is called first in an event                      */
/*  Unpacker initialized -> InvalidBitReg with                */ 
/*                          S800_INVALID_NOFEVENTS           */
/**************************************************************/

class S800Trigger : public TObject {
 private:
  S800Full* m_top;
 
  double m_prevStamp;
  
 public:
  /* Parameters */
  double reg;
  double s800;
  double external1;
  double external2;
  double secondary;
#ifdef S800_LINK_TIMESTAMP
  S800TimeStamp timestamp;
#endif
  
 public:
  void Initialize(S800Full *m_s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p);
  
  ClassDef(S800Trigger, 1);
};

/****************************************************************/
/* Time of Flight s800->tof.<..>                                */
/*                                                              */
/*  S800_TOF_PACKET                                             */
/*     | Detector Channel | TIMES channel |                     */
/*     |       RF         |      12       |                     */
/*     |      OBJ         |      13       |                     */
/*     |      XFP         |      14       |                     */
/*     |     DIAOR        |      15       |                     */
/*     |      DIA1        |       4       |                     */
/*     |      DIA2        |       5       |                     */
/*     |      DIA3        |       6       |                     */
/*     |      DIA4        |       7       |                     */
/*                                                              */
/*  If trigger.s800 is NOT valid all these values become        */
/*  invalid, otherwise they are corrected with trigger.s800     */
/*                                                              */
/*  DIAMOND:                                                    */
/*  New parameter and variables for diamond                     */
/*    Problem: For incoming PID one needs 'raw' diamond         */
/*             timing, not corrected with S800 TOF which is     */
/*             dependent on reaction channel.  Only S800 TOF    */
/*             corrected data (dia1-4) was available.  On the   */
/*             other hand one needs this corrected data for     */
/*             pid in ic-tof plot.  Furthermore all four        */
/*             diamond times differ (mainly) in offset.         */
/*    Therefore: dia(1-4) is now raw data (minus                */
/*               s800.trigger.s800 and offset corrected)        */
/*               dia(1-4)RF is dia(1-4) corrected with offset   */
/*               against RF (diaX-RF)                           */
/*               diaRF is one of the dia(1-4)RF (one has only   */
/*               to handle one time, not four)                  */
/*               !! diaRF is -1000 if more than one diamond !!  */
/*               dia1Cor-dia4Cor is S800 corrected data         */
/*               diaCor is one of the DiaXCor                   */
/*               !! diaCor is -1000 if more than one diamond !! */  
/*    Variables:                                                */
/*       diaXoffset is offset correction for diaXCor and diaXRF */
/*       dia2ndCor is additional diaCor vs. aft correction      */
/*       i.e. [diaCor = (diaCor-dia2ndCorOffset)*afp*dia2ndCor] */
/*                                                              */
/****************************************************************/

class S800TimeOfFlight : public TObject {
 private:
  S800Full* m_top;
 
 public:
  /* Parameters */
  double rf;
  double obj;
  double xfp;
  double xfp_obj;
  double rfe1;
  double obje1;
  double xfpe1;
#ifdef S800_LINK_E2
  double obje2;
  double xfpe2;
#endif
  
#ifdef S800_LINK_TOFTAC
  double tac_obj;
  double tac_obje1;
  double tac_xfp;
  double tac_xfpe1;
#endif
  
  /* MESYTEC TDC -- Added properly to ROOT analysis package October 2017 */
  double mesypat;
  double mesye1up;
  double mesye1down;
  double mesyobj;
  double mesyxfp;
  double mesyrf;
  double mesycrdc1;
  double mesycrdc2;
  double mesyref;
  double mesyobje1;
  double mesyxfpe1;
  double mesyrfe1;
  double mesyhitsum;

  int mesyrefchn;
  double mselobj; double mselxfp; double mselrf;
  double mselref;

  int mesyhit[S800MESY_MAXCHN];
  double mesymindiff[S800MESY_MAXCHN];
  vector<double> me1up, me1down;
  vector<double> mobj, mxfp,  mrf;
  vector<double> mcrdc1,  mcrdc2; 
  vector<double> mhoth; 
  vector<double> mref; 
  vector<double> mobje1, mxfpe1, mrfe1;

#ifdef S800_LINK_DIAMOND
  double diaor;
  double dia1;
  double dia2;
  double dia3;
  double dia4;
  double dia1RF;
  double dia2RF;
  double dia3RF;
  double dia4RF;
  double diaRF;
  double dia1Cor;
  double dia2Cor;
  double dia3Cor;
  double dia4Cor;
  double diaCor;
#endif
  
  /* Variables */
  double obj_shift;
  double rfCorrection;
  double rfe1Correction;
  double mrfCorrection;
  double mrfe1Correction;
  double objCorrection;
  double mobjCorrection;
  double diaCorrection;
  double diax;
  double dia2ndCor;
  double dia2ndCorOffset;
  double dia1offset;
  double dia2offset;
  double dia3offset;
  double dia4offset;
  double xfpCorrection;
  double mxfpCorrection;
  double obje1Correction;
  double mobje1Correction;
  double xfpe1Correction;
  double mxfpe1Correction;
  double obje2Correction;
  double xfpe2Correction;
  double im2objCorrection;
  
  double tacobjCorrection;
  double tacobje1Correction;
  double tacxfpCorrection;
  double tacxfpe1Correction;
  
 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p);
  UShort_t* UnpackMesy(UShort_t *p);
  void CalculateTOF();
  void CalculateMesyTOF();

  ClassDef(S800TimeOfFlight, 1);
};

/**************************************************************/
/* PIN s800->ta.pinX.<..> OR s800->ob.pin.<..>                */
/**************************************************************/

class S800Pin : public TObject {
 private:
  S800Full* m_top;

 public:
  /* Parameters */
  double e;
  double t;
  double ecorr;
  double dE;
  
  /* Variables */
  double e_offset; 
  double e_slope;
  
  double pin_cor;
  
 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p);
  void PinCorrect();

  ClassDef(S800Pin, 1);
};

/**************************************************************/
/* LaBr detectors                                             */
/**************************************************************/

class S800LaBr : public TObject {
 private:
  S800Full* m_top;

 public:
  /* Parameters */
  double energy[16];
  double time[16];
  
 public:
  void Initialize(S800Full *s800);
  void Reset();

  ClassDef(S800LaBr, 1);
};

/**************************************************************/
/* Object chamber                                             */
/**************************************************************/

class S800Object : public TObject {
 private:
  S800Full* m_top;
  
 public:
#ifdef S800_LINK_OBJECT_PIN
  S800Pin pin;
#endif

 public:
  void Initialize(S800Full *s800);
  void Reset();

  ClassDef(S800Object, 1);
};

/**** NEED TO FIGURE OUT S800 MAP THING ******/

/**************************************************************/
/* PPAC class                                                 */
/**************************************************************/

class S800Ppac : public TObject {
 private:
  S800Full* m_top;

  Bool_t hasdata;
  
 public:
  /* Parameters */
  double up;
  double down;
  double left;
  double right;
  double a;
  double time;
  double sumx;
  double sumy;
  double x;
  double y;

  /* Variables */
  double z;
  double u_offset;
  double u_slope;
  double d_offset;
  double d_slope;
  double l_offset;
  double l_slope;
  double r_offset;
  double r_slope;
  double x_offset;
  double x_slope;
  double y_offset;
  double y_slope;

 public:
  void Initialize(S800Full *s800);
  void Reset();
  UShort_t* Unpack(UShort_t *p);
  void Calibrate();
  void CalculatePosition();

  ClassDef(S800Ppac, 1);
};

/***** MISSING ANOTHER MAP FUNCTION THING ****/

/**************************************************************/
/*  Target chamber s800->ta.<..>                              */
/**************************************************************/

class S800Target : public TObject {
 private:
  S800Full* m_top;

 public:
#ifdef S800_LINK_TARGET_PPAC
  S800Ppac ppac2;
#endif

#ifdef S800_LINK_TARGET_PIN
  S800Pin pin1;
  S800Pin pin2;
#endif

#ifdef S800_LINK_TARGET
  /* Parameter */
  double E;
#endif

 public:
  void Initialize(S800Full *s800);
  void Reset();
  void Calculate();

  ClassDef(S800Target, 1);
};

/**************************************************************/
/*  The whole S800                                            */
/**************************************************************/

struct s800Phys {
  int32_t type;
  float crdc1_x;
  float crdc1_y;
  float crdc2_x;
  float crdc2_y;
  float ic_sum;
  float tof_xfp;
  float tof_obj;
  float rf;
  int32_t trigger;
  float ic_de;
  float tof_xfpe1;
  float tof_obje1;
  float tof_rfe1;
  float ata;
  float bta;
  float dta;
  float yta;
};

class S800Physics : public TObject {
 public:
  s800Phys s800Ph;
  
 public:
  S800Physics() { ; }
  ~S800Physics() { ; }
  void Reset();
  
 private:
  ClassDef(S800Physics, 1);
};

class S800Full : public TObject {

 public:

  UShort_t s800data[8192];
  int InvalidBitReg;
  
  /* Variables */
  bool debugMode;
  bool evtreset;

  /* Parameters */
  int invalid;
  double evtlength;
  double evtcount;
  
  Double_t evtNumber;
  Double_t lastEvtNumber;
  Double_t evtReset;
  Int_t reportEvtIncrease;
  
  S800LaBr labr;
  S800FocalPlane fp;
  S800Target target;
  S800Image im;
  S800Object obj;
  S800Trigger trigger;
  S800EventNumber evtnum;
#ifndef S800_LINK_TIMESTAMP
  S800TimeStamp ts;
#endif
  S800TimeOfFlight tof;

  S800Physics phys;

 public:
  S800Full();
  
 public:
  void Initialize();
  void Reset();
  void DoInvalidBitReg();
  void getAndProcessS800(FILE *inf, Int_t length);
  Double_t GetNextValue(FILE *file);
  void InitializeS800Variables(TString inputFilename);
  void UpdateS800RunVariables(TString filename);
  Float_t getDoppler(TVector3 xyz, Float_t beta, GRETINAVariables *gVar);
  void getPhysics(FILE *inf);

 private:

  ClassDef(S800Full, 3);
};

/**************************************************************/
/*  S800 Scaler Data                                          */
/**************************************************************/

class S800Scaler : public TObject {
  
 public:
  ULong64_t start, end;
  ULong64_t timestamp;
  long long int globalTS;
  ULong64_t nScaler;
  
  vector<TString> Id;
  ULong64_t value[MAXNSCALER];
  ULong64_t valueOld[MAXNSCALER];
  ULong64_t rate[MAXNSCALER];

  UShort_t auxData[8192];

 public:
  S800Scaler();
  
 public:
  void Initialize();
  void InitializeS800ScalerParameters(const char* inputFilename);
  void Reset();
  Bool_t getAndProcessS800Aux(FILE *inf, Int_t length, long long int ts);
  
 private:

  ClassDef(S800Scaler, 1);
};

#endif /* For #ifndef __PARAMETERS_H */
