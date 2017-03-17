#include <corsika/GaisserHillasParameter.h>
#include <cmath>
#include <sstream>
using namespace std;
using namespace corsika;

static void log(const std::string& mess)
{
    std::cout << mess << std::endl;
}
#define WARNING(mess) log(mess);

GaisserHillasParameter::GaisserHillasParameter():
    fXZero(0.0),
    fXZeroError(0.0),
    fA(0.0),
    fAError(0.0),
    fB(0.0),
    fBError(0.0),
    fC(0.0),
    fCError(0.0)
{
}

double GaisserHillasParameter::Eval(const double depth) const
{
    if (depth <= fXZero)
        return 0;
    
    const double tmp = fXMax - fXZero;
    const double lambda = fA + depth*fB + depth*depth*fC;
    
    return fNMax * pow((depth - fXZero) / tmp, tmp / lambda) *
    exp((fXMax - depth) / lambda);
}

void GaisserHillasParameter::Dump(ostream& os) const
{
    static const double g = 1;
    static const double cm = 1;
    static const double cm2 = cm*cm;
    os << "GH function with 6 parameters" << '\n'
    << "  X_0   = " << fXZero/g*cm2 << " g/cm^2 (sigma = "
    << fXZeroError/g*cm2 << ")\n"
    << "  X_max = " << fXMax/g*cm2 << " g/cm^2 (sigma = "
    << fXMaxError/g*cm2 << ")\n"
    << "  N_max = " << fNMax << " (sigma = " << fNMaxError << ")\n"
    << "  a     = " << fA/g*cm2 << " (sigma = " << fAError/g*cm2 << ")\n"
    << "  b     = " << fB << " (sigma = " << fBError << ")\n"
    << "  c     = " << fC*g/cm2 << " (sigma = " << fCError*g*cm2 << ")\n";
}

double GaisserHillasParameter::GetIntegral() const
{
    WARNING("not implemented!");
    return 0.;
}

double GaisserHillasParameter::GetIntegralError() const
{
    WARNING("not implemented!");
    return 0.;
}
