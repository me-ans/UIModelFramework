/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_LayoutCursor.h"

namespace ans {
    using namespace juce;

LayoutFrame LayoutCursor::nextCell()
{
    return nextCellWithWidth (cellSize.x);
}

LayoutFrame LayoutCursor::nextCellColumnWise()
{
    if (numberOfRows == 0)
    {
        jassertfalse; // must set number of rows first
        return nextCell();
    }
    LayoutFrame frame = nextFrameAtToHeight (current.x, current.x + cellSize.x);
    current.y += cellSize.y;
    
    if (floor (current.y / cellSize.y) >= numberOfRows)
    {   // column break
        current.y = top;
        current.x += cellSize.x;
    }
    covered.x = jmax (covered.x, current.x + cellSize.x);
    return frame;
}

LayoutFrame LayoutCursor::nextCellForComponent (Component* comp, bool centered)
{
    auto bounds = comp->getBounds();
    auto extent = Point<int>(bounds.getWidth(), bounds.getHeight()) + Point<int>(4, 2);

    if (isConstant(cellSize.y))
        cellSize.y = jmax (cellSize.y, (float)extent.y);

    if (wrapAroundX > 0 && (current.x + extent.x > wrapAroundX))
    {
        newLine();
        if (numberOfIndentCells > 0)
            nextCells (numberOfIndentCells);
    }
    LayoutFrame frame = nextFrameAtToHeight (current.x, current.x + extent.x);
    current.x += extent.x;
    covered.x = jmax (covered.x, current.x);
    return frame;
}

LayoutFrame LayoutCursor::nextCells (int n)
{
    return nextCellWithWidth (cellSize.x * n);
}

LayoutFrame LayoutCursor::nextCellUpTo (float x)
{
    return nextCellWithWidth ( jmax (1.0f, x - current.x - spacing.x) );
}

LayoutFrame LayoutCursor::nextCellWithWidth (float width)
{
    if (wrapAroundX > 0 && (current.x + width > wrapAroundX))
    {
        newLine();
        if (numberOfIndentCells > 0)
            nextCells (numberOfIndentCells);
    }
    LayoutFrame frame = nextFrameAtToHeight (current.x, current.x + width);
    current.x += width;
    covered.x = jmax (covered.x, current.x);
    return frame;
}

LayoutFrame LayoutCursor::nextRow()
{
    LayoutFrame frame = nextFrameAtToHeight (0.0f, rowWidth, cellSize.y);
    newLine();
    covered.y = jmax (covered.y, current.y + cellSize.y);
    return frame;
}

LayoutFrame LayoutCursor::nextRowIndent (float leftIndent, int numberOfRows)
{
    LayoutFrame frame = nextFrameAtToHeight (leftIndent, rowWidth, cellSize.y * numberOfRows);
    for (int i=0; i<numberOfRows; ++i)
        newLine();
    covered.y = jmax (covered.y, current.y + cellSize.y);
    return frame;
}

LayoutFrame LayoutCursor::nextRows (int n)
{
    return nextRowWithHeight (cellSize.y * n);
}

LayoutFrame LayoutCursor::nextRowWithHeight (float height)
{
    LayoutFrame frame = nextFrameAtToHeight (0.0f, rowWidth, height);
    newLineWithHeight (height);
    
    return frame;
}


void LayoutCursor::newLine ()
{
    newLineWithHeight (cellSize.y);
}

void LayoutCursor::newLineWithHeight (float height)
{
    current.y += height;
    top = jmax (top, current.y);
    current.x = 0;
    covered.y = jmax (covered.y, current.y + cellSize.y);
}

LayoutFrame LayoutCursor::nextFrameAtToHeight (float left, float right)
{
    return nextFrameAtToHeight (left, right, cellSize.y);
}

LayoutFrame LayoutCursor::nextFrameAtToHeight (float left, float right, float height)
{
    LayoutFrame frame;
    
    if (left > 1 || right > 1)
    {
        if (isConstant (cellSize.x))
        {
            // constant width & height
            frame.left.offset = left;
            frame.right.offset = right - spacing.x;
            frame.top.offset = current.y + spacing.y;
            frame.bottom.offset = current.y + height - spacing.y;
        } else {
            // fractional width & constant height
            frame.left.fraction = left;
            frame.right.fraction = right;
            frame.right.offset = 0 - spacing.x;
            frame.top.offset = current.y + spacing.y;
            frame.bottom.offset = current.y + height - spacing.y;
        }
    } else {
        if (isConstant (cellSize.y))
        {
            // fractional width & constant height
            frame.left.fraction = left;
            frame.right.fraction = right;
            frame.right.offset = 0 - spacing.x;
            frame.top.offset = current.y + spacing.y;
            frame.bottom.offset = current.y + height - spacing.y;
        } else {
            // both fractional width & height
            frame.top.fraction = current.y;
            frame.bottom.fraction = current.y + height;
            frame.left.fraction = left;
            frame.right.fraction = right;
            frame.right.offset = 0 - spacing.x;
            frame.top.offset = spacing.y;
            frame.bottom.offset = 0 - spacing.y;
        }
    }
    return frame;
}
    
}//
