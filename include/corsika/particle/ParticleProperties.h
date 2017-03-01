/**
 \file
 Properties of elementary particles
 
 \author Lukas Nellen
 \version $Id$
 \date 03 Apr 2004
 */
#pragma once
#include <corsika/particle/VParticleProperties.h>

namespace corsika
{
    
    /**
     \class ParticleProperties ParticleProperties.h "corsika/ParticleProperties.h"
     
     \brief Provide particle properties that depend only on its type.
     
     \author Lukas Nellen
     \date 03 Apr 2004
     */
    
    struct ParticleProperties : public VParticleProperties
    {
        ParticleProperties(const int theType, const std::string& theName, const double theMass):
        fType(theType), fName(theName), fMass(theMass) {}
        
        /// Get particle type (using PDG particle codes)
        virtual int GetType() const { return fType; }
        
        /// Get particle name
        virtual std::string GetName() const { return fName; }
        
        /// Get particle mass (in Auger units)
        virtual double GetMass() const { return fMass; }
        
    private:
        /// Particle type ID (PDG types)
        int fType;
        /// The particle name
        std::string fName;
        /// particle mass
        double fMass;
        
    };
}
