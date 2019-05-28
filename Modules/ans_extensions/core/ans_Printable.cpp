/**
 Class Extensions For Juce
 -------------------------
 Copyright 2019 me-ans@GitHub
 Please regard the license terms accompanying this Juce module.
 */

#include "ans_Printable.h"

juce::OutputStream& operator<< (juce::OutputStream& s, const ans::Printable& o)
{
    // Cast const away here, as some implementations of printOn() may need
    // to do lazy evaluations of members that are required only for printing
    
    const_cast<Printable&>(o).printOn(s);
    return s;
}

juce::OutputStream& operator<< (juce::OutputStream& s, const ans::Symbol& o)
{
    s << o.toString();
    return s;
}

juce::OutputStream& operator<< (juce::OutputStream& s, const juce::var& o)
{
    s << o.toString();
    return s;
}

