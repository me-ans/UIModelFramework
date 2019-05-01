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
 LayoutFrame offers a very flexible yet lean (and fast) solution for denoting
 rectangles relative to a parent rectangle that scale dynamically. They allow for
 a concise description of layouts, doing without the complexity of other approaches.
 */
struct LayoutFrame
{
    struct Side
    {
        Side () :
            fraction (0),
            offset (0),
            limits (0, INT_MAX),
            relative (false)
        {}
        
        Side (float f, int o) :
            fraction (f),
            offset (o),
            limits (0, INT_MAX),
            relative (false)
        {}
        
        /** This constructor interprets f <= 1.0 as fractional, or offset else */
        Side (float f) :
            fraction (f <= 1.0 ? f : 0),
            offset   (f >  1.0 ? f : 0),
            limits (0, INT_MAX),
            relative (false)
        {}
        
        /** Construct a Side that is off points relative to opposing side */
        static Side off (int distance)
        {
            Side s;
            s.offset = distance;
            s.relative = true;
            return s;
        }
        
        static Side fromString (const String& source)
        {
            Side s;
            s.parse(source);
            return s;
        }
        
        /** Maps a single side relative to a parent rectangle according to the side's specs */
        int relativeToParent (int parent, int opposite) const;
                
        String print() const;
        void parse (const String& source);
        
        bool isNotDefault() const { return fraction != 0.0 || offset != 0 || relative; }
        bool isRelative() const { return relative; }
        
        bool operator== (const Side& other) const;
        bool operator!= (const Side& other) const { return !(*this == other); }
        
        
        float fraction;     // proportional scale of the corresponding parent
        int offset;         // absolute offset from that (negative/positive)
        Range<int> limits;  // absolute limits on the final value
        bool relative;      // if true, expression is relative to opposing side
    };
    
    LayoutFrame () :
        left   (0.0, 0),
        top    (0.0, 0),
        right  (0.0, 0),
        bottom (0.0, 0)
    {}
    
    /** Frame resolves to a constant rectangle */
    LayoutFrame (const Rectangle<int>& rect) :
        left   (0, rect.getTopLeft().x),
        top    (0, rect.getTopLeft().y),
        right  (0, rect.getBottomRight().x),
        bottom (0, rect.getBottomRight().y)
    {}
    
    /**
    LayoutFrame (Side leftSide, Side topSide, Side rightSide, Side bottomSide) :
        left   (leftSide),
        top    (topSide),
        right  (rightSide),
        bottom (bottomSide)
    {}
    */
    LayoutFrame (const String& leftSide, const String& topSide, const String& rightSide, const String& bottomSide) :
        left   (Side::fromString(leftSide)),
        top    (Side::fromString(topSide)),
        right  (Side::fromString(rightSide)),
        bottom (Side::fromString(bottomSide))
    {}

    LayoutFrame (float leftFraction,   int leftOffset,
                 float topFraction,    int topOffset,
                 float rightFraction,  int rightOffset,
                 float bottomFraction, int bottomOffset) :
        left   (leftFraction, leftOffset),
        top    (topFraction, topOffset),
        right  (rightFraction, rightOffset),
        bottom (bottomFraction, bottomOffset)
    {}
    
    LayoutFrame (float leftF, float topF, float rightF, float bottomF) :
        left   (leftF),
        top    (topF),
        right  (rightF),
        bottom (bottomF)
    {}
    
    /** Creates a LayoutFrame that maps to the entire area of the parent */
    static LayoutFrame entire();
    
    bool hasRelativeWidth ()  const { return right.isRelative(); }
    bool hasRelativeHeight()  const { return bottom.isRelative(); }
    
    bool isNotDefault()   const
    {
        return left.isNotDefault() || top.isNotDefault() || right.isNotDefault() || bottom.isNotDefault();
    }
    
    bool operator== (const LayoutFrame& other) const;
    bool operator!= (const LayoutFrame& other) const { return !(*this == other); }
    
    /** Maps the rectangle relative to a parent rectangle according to the frame's specs */
    Rectangle<int> rectangleRelativeTo (const Rectangle<int>& parent) const;
    
    /** Returns a version of this frame with a fixed width. */
    LayoutFrame withFixedWidth  (int w) const { auto f = LayoutFrame (*this); f.right = Side::off(w); return f; }
    
    /** Returns a version of this frame with a fixed height. */
    LayoutFrame withFixedHeight (int h) const { auto f = LayoutFrame (*this); f.bottom = Side::off(h); return f; }
    
    /** Returns a version of this frame with one side modified. */
    LayoutFrame withLeft (Side s) const { auto f = LayoutFrame (*this); f.left = s; return f; }
    
    /** Returns a version of this frame with one side modified. */
    LayoutFrame withTop (Side s) const { auto f = LayoutFrame (*this); f.top = s; return f; }
    
    /** Returns a version of this frame with one side modified. */
    LayoutFrame withRight (Side s) const { auto f = LayoutFrame (*this); f.right = s; return f; }
    
    /** Returns a version of this frame with one side modified. */
    LayoutFrame withBottom (Side s) const { auto f = LayoutFrame (*this); f.bottom = s; return f; }
    
    /** Returns a version of this rectangle with the given amount removed from its left-hand edge. */
    LayoutFrame withTrimmedLeft (int amount) const   { return withLeft ({left.fraction, left.offset + amount}); }
    
    /** Returns a version of this rectangle with the given amount removed from its right-hand edge. */
    LayoutFrame withTrimmedRight (int amount) const  { return withRight ({right.fraction, right.offset - amount}); }
    
    /** Returns a version of this rectangle with the given amount removed from its top edge. */
    LayoutFrame withTrimmedTop (int amount) const    { return withTop ({top.fraction, top.offset + amount}); }
    
    /** Returns a version of this rectangle with the given amount removed from its bottom edge. */
    LayoutFrame withTrimmedBottom (int amount) const { return withBottom ({bottom.fraction, bottom.offset - amount}); }
    
    /** For use in C++ expressions */
    String generateSourceCPP () const;
    
    // Member Variables
    Side left, top, right, bottom;
};

}
