#pragma once
#include <iostream>

namespace corsika
{

  /**
     \class GaisserHillasParameter
     \brief Gaisser-Hillas with 6 parameters (CORSIKA)

     \date Mo Jul 25 15:18:15 CEST 2005
     \author Ralf Ulrich, Javier Gonzalez
     \version $Id$
  */

  class GaisserHillasParameter {

  public:
    GaisserHillasParameter();
    virtual ~GaisserHillasParameter() { }

    double GetXMax() const { return fXMax; }
    double GetXMaxError() const { return fXMaxError; }
    double GetNMax() const { return fNMax; }
    double GetNMaxError() const { return fNMaxError; }
    double GetNMaxXMaxCorrelation() const { return fRhoNMaxXMax; }

    double GetChiSquare() const { return fChiSqr; }
    unsigned int GetNdof() const { return fNdof; }

    bool IsdEdXProfile() const { return fIsEnergyDeposit; }

    void SetXMax(const double xMax, const double error)
    { fXMax = xMax; fXMaxError = error; }

    void SetNMax(const double nMax, const double error, const bool isEnergyDeposit = false)
    { fNMax = nMax; fNMaxError = error; fIsEnergyDeposit = isEnergyDeposit; }

    void SetNMaxXMaxCorrelation(const double rho)
    { fRhoNMaxXMax = rho; }

    void SetChiSquare(const double chi, const unsigned int ndof)
    { fChiSqr = chi; fNdof = ndof; }

    double operator()(const double depth) const
    { return this->Eval(depth); }

    double GetXZero() const { return fXZero; }
    double GetXZeroError() const { return fXZeroError; }
    double GetA() const { return fA; }
    double GetAError() const { return fAError; }
    double GetB() const { return fB; }
    double GetBError() const { return fBError; }
    double GetC() const { return fC; }
    double GetCError() const { return fCError; }

    void SetXZero(const double xZero, const double error)
    { fXZero = xZero; fXZeroError = error; }

    void SetA(const double a, const double error)
    { fA = a; fAError = error; }

    void SetB(const double b, const double error)
    { fB = b; fBError = error; }

    void SetC(const double c, const double error)
    { fC = c; fCError = error; }

    double Eval(const double depth) const;
    void Dump(std::ostream& os = std::cout) const;
    double GetIntegral() const;
    double GetIntegralError() const;

  private:
    double fXMax;
    double fXMaxError;
    double fNMax;
    double fNMaxError;
    double fRhoNMaxXMax;
    double fChiSqr;
    unsigned int fNdof;

    double fGammaIntegral;
    double fGammaError;

    bool fIsEnergyDeposit;
    double fXZero;
    double fXZeroError;
    double fA;
    double fAError;
    double fB;
    double fBError;
    double fC;
    double fCError;

  };

}
