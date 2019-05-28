/**
 Class Extensions For Juce
 -------------------------
 Copyright 2019 me-ans@GitHub
 Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_Symbol.h"

namespace ans
{
    using namespace juce;
    
    /**
     Printable adds to any object the capability to print itself to an OutputStream and convert to String.
     This is most useful for console output, logging and debugging, but less so in a deployed product.
     */
    
    class Printable
    {
    public:
        virtual ~Printable () {}
        
        /**
         All derived classes should override this: Render a string representation of the receiver to
         the OutputStream. By default this prints the class name only. This method is deliberately not
         const in order to allow the receiver to do lazy initialisation of properties where needed.
         
         @todo: Make this const and provide encapsulation for lazy evaluation!
         */
        virtual void printOn (OutputStream& s)
        {
            s << className ();
        }
        
        /** Convenience method that returns a String by invoking printOn() */
        virtual String printString ()
        {
            MemoryOutputStream s;
            printOn (s);
            return s.toString ();
        }
        
        /**
         Answer the class name of the receiver, which is used as a default for printing
         objects that don't override printOn(). This name is certainly mangled and
         compiler-dependent, so it should only be used during development!
         */
        virtual String className () const
        {
            return ANS_CLASSNAME;
        }
    };
    
}    // namespace ans

/**
 Extends the << operator for OutputStream to accommodate Printable and other
 basic objects that don't (yet) support this out of the box.
 */

juce::OutputStream& operator<< (juce::OutputStream& s, const ans::Printable& o);
juce::OutputStream& operator<< (juce::OutputStream& s, const ans::Symbol& o);
juce::OutputStream& operator<< (juce::OutputStream& s, const juce::var& o);
