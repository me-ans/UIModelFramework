/**
 Experimental UI Framework
 
 A proof-of-concept for a declarative UI framework based on generated code.
 Please regard the license terms accompanying this project.
 Copyright 2019 me-ans@GitHub
 */

#include "CustomExample.h"



void CustomExample::populateCustomComponent (UIComposite* comp)
{
    LayoutCursor cursor;
    cursor.setCellSize ({0.1, 0.2});
    cursor.setSpacing ({4,4});
    cursor.setWrapAroundAt (1.0);
    
    for (int i=0; i<20; ++i)
    {
        static const Array<Colour> colours =
            { Colours::orange, Colours::brown, Colours::azure, Colours::goldenrod, Colours::forestgreen };
        
        auto swatch = std::make_unique<UIComposite> (comp->getUIInstance(), String(i));
        swatch->setBackgroundColour (colours[i % colours.size()]);
        
        comp->addComponent (std::move(swatch), cursor.nextCells (i % 3 + 1));
        if (i % 5 == 0)
        {
            cursor.newLine();
            cursor.setCellHeight (cursor.getCellSize().y * 0.8);
        }
    }
}
