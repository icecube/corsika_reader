/**
 \file
 Exceptions in CORSIKA io
 
 \author Lukas Nellen
 \version $Id$
 \date 19 Nov 2003
 */

#pragma once
#include <exception>
#include <string>

namespace corsika
{
    /**
     \class CorsikaIOException CorsikaIOException.h "io/CorsikaIOException.h"
     \brief Base for exceptions in the CORSIKA reader
     \ingroup corsika exceptions
     */
    
    struct IOException : public std::exception
    {
        /// Construct CORISKA IO exception with message
        IOException(std::string message) : fMessage(message) { }
        
        /// Virtual destructor for CorsikaIOException
        virtual ~IOException() throw() { }
        
        const char* what() const throw() { return fMessage.c_str(); }
        
        /// Retrieve verbose exception name
        virtual std::string GetExceptionName() const { return "Corsika IO exception"; }
    private:
        std::string fMessage;
    };
}
