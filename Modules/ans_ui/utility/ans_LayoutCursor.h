/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_LayoutFrame.h"

namespace ans {
    using namespace juce;

/**
 LayoutCursor provides an incremental stream of LayoutFrames, delivering cells for a grid
 based on rows and columns. Sizes may be constant (points, pixels) or proportional (relative to parent).
 LayoutCursor can be configured for cell size, spacing, wrap-around and other properties.
 It offers a convenient way for building dynamic layouts that are lean and fast.
 */

class LayoutCursor
{
public:
    LayoutCursor () : cellSize ({1.0, 22}) {}
    LayoutCursor (const Point<float>& initialCellSize) : cellSize (initialCellSize) {}
    
    /**
     Changes current cell size. Values <= 1.0 are proportional releative to
     parent frame. Values > 1 are constant in points (pixels) */
    void setCellSize (Point<float> size) { cellSize = size; }

    /**
     Get the current cell size. Values <= 1.0 are proportional releative to
     parent frame. Values > 1 are constant in points (pixels) */
    Point<float> getCellSize() { return cellSize; }
    
    /**
     Changes current cell size. Values <= 1.0 are proportional releative to
     parent frame. Values > 1 are constant in points (pixels) */
    void setCellWidth  (float w) { cellSize.x = w; }
    
    /**
     Changes current cell size. Values <= 1.0 are proportional releative to
     parent frame. Values > 1 are constant in points (pixels) */
    void setCellHeight (float h) { cellSize.y = h; }
    
    /** Changes spacing between cells in the grid */
    void setSpacing (Point<int> p) { spacing = p.toFloat(); }
    
    /** Cell height plus vertical spacing */
    float getRowHeight() { return cellSize.y + spacing.y; }
    
    /** (Optional) Causes the cursor to wrap around to the next row, if this limit would be exceeded when requesting the next cell.
     Values <= 1.0 are proportional releative to parent frame. Values > 1 are constant in points (pixels)*/
    void setWrapAroundAt (float x) { wrapAroundX = x; }
    
    /** (Optional) Number of cenns inserted as an indent after each line wraps around */
    void setNumberOfIndentCells (int n) { numberOfIndentCells = n; }
    
    /** (Optional) Set in advance the number of rows, so we can stream it columns downward first */
    void setNumberOfRows (int n) { numberOfRows = n; }
    
    /** (Optional) Set in advance the width or rows, so cells with proportional size can scale in relation to that.
     Values <= 1.0 are proportional releative to parent frame. Values > 1 are constant in points (pixels) */
    void setRowWidth (float w) { rowWidth = w; }
    
    /** Answer a LayoutFrame covering the next cell, iterating horizontally first */
    LayoutFrame nextCell();
    /** Answer a LayoutFrame covering the next cell, iterating vertically first.
     This requires a total row count be set in advance.*/
    LayoutFrame nextCellColumnWise();
    /** Answer the next LayoutFrame to accommodate the argument with its current bounds */
    LayoutFrame nextCellForComponent (Component* comp, bool centered = false);
    /** Answer a LayoutFrame spanning the next anInteger cells */
    LayoutFrame nextCells (int n);
    /** Answer a new frame filling the current row up to the given point */
    LayoutFrame nextCellUpTo (float x);
    /** Answer a new frame at the end of the current row */
    LayoutFrame nextCellWithWidth (float width);
    /** Answer a LayoutFrame spanning the whole next row */
    LayoutFrame nextRow();
    /** Answer a layoutFrame for a numer of new rows, indented by a portion of space to the left */
    LayoutFrame nextRowIndent (float leftIndent, int numberOfRows);
    /** Answer a LayoutFrame spanning the anInteger next rows */
    LayoutFrame nextRows (int n);
    /** Answer a LayoutFrame for 1/1 width and anInteger fixed points height */
    LayoutFrame nextRowWithHeight (float height);
    
    
    /** Advance to the next line - do not create a new LayoutFrame */
    void newLine ();
    
    /** Advance to the next line, adding a specific vertical spacing */
    void newLineWithHeight (float h);
    
    /** Learn about the total extent covered with cells so far */
    Point<float> getCoveredExtent() { return covered; }
    
    /** Get the current position where the top left edge of next cell would be placed */
    Point<int> getCurrentPosition() { return current.toInt(); }
    
    /** Jump to a new position. Do NOT use fractional values <= 1 here, unless cell size is fractional too. */
    void setCurrentX (float x) { jassert (isFractional(x) == isFractional(cellSize.x)); current.x = x; }
    
    /** Jump to a new position. Do NOT use fractional values <= 1 here, unless cell size is fractional too. */
    void setCurrentY (float y) { jassert (isFractional(y) == isFractional(cellSize.y)); current.y = y; }
    
    /** Add an empty spacing. Do NOT use fractional values <= 1 here, unless cell size is fractional too. */
    void addSpaceX (float x) { jassert (isFractional(x) == isFractional(cellSize.x)); current.x += x; }

    /** Add an empty spacing. Do NOT use fractional values <= 1 here, unless cell size is fractional too. */
    void addSpaceY (float y) { jassert (isFractional(y) == isFractional(cellSize.y)); current.y += y; }
    

    
private:

    bool isConstant (float f)   { return !isFractional (f); }
    bool isFractional (float f) { return f <= 1.0; }
    bool isFractionalWidth ()   { return isFractional (cellSize.x); }
    bool isFractionalHeight ()  { return isFractional (cellSize.y); }

    LayoutFrame nextFrameAtToHeight (float left, float right);
    LayoutFrame nextFrameAtToHeight (float left, float right, float height);
    
    Point<float> cellSize;
    Point<float> spacing = {0, 0};
    Point<float> current = {0, 0};    // incrementally updated
    Point<float> covered = {0, 0};    // total area covered by cells so far
    
    float top = 0;
    float rowWidth = 1.0;
    float wrapAroundX = 0;
    
    int numberOfRows = 0;
    int numberOfIndentCells = 0;
};

}
