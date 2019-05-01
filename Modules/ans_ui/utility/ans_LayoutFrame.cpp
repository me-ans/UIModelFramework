/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_LayoutFrame.h"

namespace ans {
    using namespace juce;

namespace SideHelpers
{
    struct Parser
    {
        Parser (const String& source) :
            input(source),
            position(0),
            size((int)source.getNumBytesAsUTF8())
        {}
        
        bool atEnd() { return position > size; }
        bool isNumeric (char c) { return (c >= '0' && c <= '9') || c == '+' || c == '-'; }
        
        inline char peek() { return input[position]; }
        inline char next() { position++; return input[position-1]; }
        
        bool readNumber (int& n)
        {
            int a = position;
            while ( isNumeric (peek()) )
                next();
            if (position > a)
            {
                n = input.substring (a, position).getIntValue();
                return true;
            }
            return false;
        }
        
        const String input;
        int position;
        int size;
    };
}


int LayoutFrame::Side::relativeToParent (int length, int opposite) const
{
    if (relative)
        return opposite + limits.clipValue ((fraction * length) + offset);
    else
        return limits.clipValue ((fraction * length) + offset);
}

Rectangle<int> LayoutFrame::rectangleRelativeTo (const Rectangle<int>& parent) const
{
    int x1 = left.relativeToParent  (parent.getWidth(), 0);
    int y1 = top.relativeToParent   (parent.getHeight(), 0);
    int x2 = right.relativeToParent (parent.getWidth(), x1);
    int y2 = bottom.relativeToParent(parent.getHeight(), y1);
    
    // Prevent defunct rects
    x2 = jmax (x1, x2);
    y2 = jmax (y1, y2);
    
    return Rectangle<int> (Point<int>(x1,y1), Point<int>(x2,y2));
}

String LayoutFrame::Side::print() const
{
    String out;
    LayoutFrame::Side standard;
    
    if (fraction != standard.fraction)
        out << roundToInt(fraction * 100) << "%";

    if (offset != standard.offset || fraction == standard.fraction)
        out << (out.isNotEmpty() ? " ":"") << (offset < 0 ? (out.isEmpty() ? "-" : "- ") : (out.isEmpty() ? "" : "+ ")) << abs(offset);

    if (limits != standard.limits)
        out << (out.isNotEmpty() ? " ":"") << "[" << limits.getStart() << ".." << limits.getEnd() << "]";
    
    if (relative)
        out << " relative";
    
    return out;
}

void LayoutFrame::Side::parse (const String& source)
{
    // "-100"
    // "30% + 100"
    // "50% - 450 [100..1200]"
    // "22 relative"
    // "12%-2 relative"
    
    offset = 0;
    fraction = 0;
    relative = false;
    limits = {0, INT_MAX};
    
    SideHelpers::Parser p (source.removeCharacters(" "));
    int n;
    if (p.readNumber (n))
    {
        if (p.peek() == '%')
        {
            fraction = 0.01f * n;
            p.next();
        } else
            offset = n;
        
        if (p.readNumber (n))
            offset = n;

        if (p.peek() == '[')
        {
            p.next();
            if (p.readNumber(n)) limits.setStart(n);
            while (p.peek() == '.') p.next();
            if (p.readNumber(n)) limits.setEnd(n);
        }
    }
    relative = source.indexOfIgnoreCase ("rel") > 0;
}

bool LayoutFrame::Side::operator== (const Side& other) const
{
    return fraction == other.fraction
        && offset == other.offset
        && limits == other.limits
        && relative == other.relative;
}

bool LayoutFrame::operator== (const LayoutFrame& other) const
{
    return left == other.left
        && top == other.top
        && right == other.right
        && bottom == other.bottom;
}

LayoutFrame LayoutFrame::entire ()
{
    return LayoutFrame (0.0, 0,
                        0.0, 0,
                        1.0, 0,
                        1.0, 0);
}

String LayoutFrame::generateSourceCPP() const
{
    if (LayoutFrame::entire() == *this)
        return "LayoutFrame::entire()";
        
    String out;
    out << "LayoutFrame("
        << left.print().quoted() << ", "
        << top.print().quoted() << ", "
        << right.print().quoted() << ", "
        << bottom.print().quoted() << ")";
    
    return out;
}

}
