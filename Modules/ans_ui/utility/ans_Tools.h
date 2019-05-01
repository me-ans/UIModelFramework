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

struct ArrayTools
{
    template <typename ElementClass, typename ArrayClass, typename LAMBDA>
    static ElementClass* find (const ArrayClass& array, LAMBDA&& match)
    {
        for (ElementClass* each : array)
            if (match (each))
                return each;
        return nullptr;
    }
    
    template <typename ElementClass, typename ArrayClass, typename LAMBDA1, typename LAMBDA2>
    static ElementClass* find (const ArrayClass& array, LAMBDA1&& match, LAMBDA2&& absent)
    {
        for (ElementClass* each : array)
            if (match (each))
                return each;
        return absent();
    }
};


}
