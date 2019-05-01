/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "JuceHeader.h"

namespace ans {
    using namespace juce;
    
/**
 SourceOutputStream supports indenting.
 */
class SourceOutputStream : public MemoryOutputStream
{
public:
    SourceOutputStream () :
        MemoryOutputStream (2048),
        spaces (4),
        indent (0)
    {
    }
    
    void incrementIndent()
    {
        indent++;
        setNewLineString (nl());
    }
    
    void decrementIndent()
    {
        indent--;
        setNewLineString (nl());
    }
 
private:
    
    String nl ()
    {
        String s;
        s << "\n";
        for (int i=0; i<(spaces*indent); ++i)
            s << " ";
        return s;
    }
    
    int spaces;
    int indent;
};

}
