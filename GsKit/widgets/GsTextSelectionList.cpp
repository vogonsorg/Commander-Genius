/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/PointDevice.h>
#include <base/utils/Color.h>

#include "GsTextSelectionList.h"

CGUITextSelectionList::CGUITextSelectionList(const GsRect<float> &rect)  :
    GsControl(rect),
    mScrollbar(this)
{
    GsRect<float> scrollRect(0.0f, 0.0f, 0.1f, 1.0f);
    mScrollbar.setRect(scrollRect);
}


void CGUITextSelectionList::setConfirmButtonEvent(CEvent *ev)
{
	mConfirmEvent.reset(ev);
}

void CGUITextSelectionList::setBackButtonEvent(CEvent *ev)
{
	mBackEvent.reset(ev);
}

bool CGUITextSelectionList::sendEvent(const InputCommand command)
{
	if(command == IC_UP)
	{
        mReleasedSelection--;

        if(mReleasedSelection < 0)
        {
            mReleasedSelection = 0;
            mSelected = false;
            return false;
        }

        if( mReleasedSelection < mScrollbar.scrollPos() )
        {
            mScrollbar.scrollUp();
        }

		return true;
	}
	else if(command == IC_DOWN)
	{
        const int last = static_cast<int>(mItemList.size());

        mReleasedSelection++;

        if(mReleasedSelection >= last)
        {
            mReleasedSelection = last-1;
            mSelected = false;
            return false;
        }

        if( mReleasedSelection >= mScrollbar.lastToShow() - mScrollbar.scrollPos() )
        {
            mScrollbar.scrollDown();
        }

		return true;
	}
	else if(command == IC_STATUS || command == IC_JUMP ||
             command == IC_POGO || command == IC_FIRE || command == IC_RUN)
	{
		if(mConfirmEvent)
			gEventManager.add(mConfirmEvent);
		return true;
	}
	else if(command == IC_BACK)
	{
		if(mBackEvent)
			gEventManager.add(mBackEvent);
		return true;
	}
	else
		return false;

}

void CGUITextSelectionList::addText(const std::string &text)
{    
    mItemList.push_back( item(text) );
}



void CGUITextSelectionList::processLogic()
{
    if(!mEnabled)
    {
        return;
    }

    /// Some logic for colors so we get nice fancy effects
    auto it = mItemList.begin();

    for(int i=0 ; i<mScrollbar.scrollPos() ; it++, i++);

    for ( int line = 0;
          it != mItemList.end() && line<mScrollbar.mLastToShow ;
          it++, line++ )
    {
        // Current line to apply the color
        const int curLinePos = static_cast<int>(line) + mScrollbar.scrollPos();

        if( mPressedSelection == curLinePos )
        {
            it->mBgColor.converge( GsColor(0x95, 0xA5, 0xF1, 0xFF) );
        }
        else if( mReleasedSelection == curLinePos )
        {
            if(mSelected)
            {
                it->mBgColor.converge( GsColor(0xC5, 0xC5, 0xF1, 0xFF) );
            }
            else
            {
                it->mBgColor.converge( GsColor(0xB5, 0xB5, 0xF1, 0xFF) );
            }
        }
#ifndef DISABLE_HOVER
        else if( mHoverSelection == curLinePos )
        {
            it->mBgColor.converge( GsColor(0xC5, 0xC5, 0xC5, 0xFF) );
        }
#endif
    }

/*
    /// Here we check if the mouse-cursor/Touch entry clicked on something!!
    const float bw = gVideoDriver.getGameResolution().dim.x;
    const float bh = gVideoDriver.getGameResolution().dim.y;

    const float fx = mRect.pos.x;
    const float fw = mRect.dim.x;
    const float fy = mRect.pos.y;
    const float fh = mRect.dim.y;

    GsFontLegacy &Font = gGraphics.getFont(mFontID);
    const int pixth = Font.getPixelTextHeight();


    const auto halfBorderHeight = (mBorderHeight/2);

    const float textHeight = (pixth+mBorderHeight);

    const float y_innerbound_min = fy ;
	const float y_innerbound_max = y_innerbound_min +
            static_cast<float>( mItemList.size()*textHeight )/bh;

    const float x_innerbound_min = fx + static_cast<float>(textHeight)/bw;




    GsRect<float> rRect(fx, fy, fw, fh);        

    GsPointingState &pointingState = gPointDevice.mPointingState;

    //processPointingState();

    const GsVec2D<float> mousePos = pointingState.mPos;

    if( rRect.HasPoint(mousePos) )
    {
        // Let scrollbar do its work
        mScrollbar.processLogic();

        if( mousePos.y > fy && mousePos.y < y_innerbound_max )
        {
            int newselection = ( ((mousePos.y-fy)*bh- halfBorderHeight )/textHeight) + mScrollbar.scrollPos();

            if( mousePos.x > x_innerbound_min && mousePos.y > y_innerbound_min)
            {

                if(mHoverTriggers)
                {
                    if(mHovered)
                    {
                        mHoverSelection = newselection;
                        mPressedSelection = newselection;
                        mReleasedSelection = newselection;
                    }
                }
                else*/
/*                {
#ifndef DISABLE_HOVER
                    if(mHovered)
                    {
                        mHoverSelection = newselection;
                    }
#endif
                    if(mPressed)
                    {
                        mPressedSelection = newselection;
                    }
                    if(mReleased)
                    {
                        mReleasedSelection = newselection;
                    }
                }
            }
        }
    }
    else
    {
#ifndef DISABLE_HOVER
        mHoverSelection = -1;
#endif
        mPressedSelection = -1;
    }
    */
}


void CGUITextSelectionList::processPointingStateRel(const GsRect<float> &rect)
{
    const auto absRect = rect.transformed(mRect);
    processPointingState(absRect);

    GsFontLegacy &Font = gGraphics.getFont(mFontID);
    const int pixth = Font.getPixelTextHeight();

    const auto halfBorderHeight = (mBorderHeight/2);

    const float textHeight = (pixth+mBorderHeight);

    const float bw = gVideoDriver.getGameResolution().dim.x;
    const float bh = gVideoDriver.getGameResolution().dim.y;

    const float fx = absRect.pos.x;
    const float fw = absRect.dim.x;
    const float fy = absRect.pos.y;
    const float fh = absRect.dim.y;

    const float y_innerbound_min = fy /*+ static_cast<float>(textHeight)/bh*/;
    const float y_innerbound_max = y_innerbound_min +
            static_cast<float>( mItemList.size()*textHeight )/bh;

    const float x_innerbound_min = fx + static_cast<float>(textHeight)/bw;


    GsRect<float> rRect(fx, fy, fw, fh);

    GsPointingState &pointingState = gPointDevice.mPointingState;

    //processPointingState();

    const GsVec2D<float> mousePos = pointingState.mPos;

    if( rRect.HasPoint(mousePos) )
    {
        // Let scrollbar do its work
        mScrollbar.processLogic();

        if( mousePos.y > fy && mousePos.y < y_innerbound_max )
        {
            int newselection = ( ((mousePos.y-fy)*bh- halfBorderHeight )/textHeight) + mScrollbar.scrollPos();

            if( mousePos.x > x_innerbound_min && mousePos.y > y_innerbound_min)
            {
                {
#ifndef DISABLE_HOVER
                    if(mHovered)
                    {
                        mHoverSelection = newselection;
                    }
#endif
                    if(mPressed)
                    {
                        mPressedSelection = newselection;
                    }
                    if(mReleased)
                    {
                        mReleasedSelection = newselection;
                    }
                }
            }
        }
    }
    else
    {
        mHoverSelection = -1;
        mPressedSelection = -1;
    }


}


void CGUITextSelectionList::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Blit the List surface
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);

    GsRect<Uint16> origRect(displayRect);
    GsRect<Uint16> rect = origRect;
   
    GsColor boxColor(0xDF, 0xDF, 0xDF, 0xFF);

    if(mEnabled)
    {
        boxColor = GsColor(0xFF, 0xFF, 0xFF, 0xFF);
    }

    blitsfc.fillRGBA(rect, boxColor);

	// Now lets draw the text of the list control
    auto &Font = gGraphics.getFont(mFontID);
    const int pixth = Font.getPixelTextHeight();
    const int pixtw = pixth; // NOTE: We assume here, that the height and width are the same. Invalid to galaxy fonts!

	// Move 16 Pixel so we have space for the cursor/twirl to show the selection
    const auto halfBorderHeight = (mBorderHeight/2);
    const int sepHeight = Font.getPixelTextHeight()+mBorderHeight;
    const int xpos = rect.pos.x+16+1;
    const int ypos = rect.pos.y+10;
    unsigned int textlimitWidth = (rect.dim.x-16)/pixtw;

    mScrollbar.mLastToShow = (rect.dim.y/sepHeight)-1;

    rect.dim.y = pixth+mBorderHeight;

    rect.pos.x += 12;
    rect.dim.x -= 12;

    auto it = mItemList.begin();

    for(int i=0 ; i<mScrollbar.scrollPos() ; it++, i++);

    for ( int line = 0;
          it != mItemList.end() && line<mScrollbar.mLastToShow ;
          it++, line++ )
	{
        // Current line to be rendered
        const int curLinePos = static_cast<int>(line) + mScrollbar.scrollPos();

        const auto &theColor = it->mBgColor;
        const auto &theText = it->mText;

        if( mPressedSelection == curLinePos )
        {
            rect.pos.y = ypos+(line*rect.dim.y);
            blitsfc.fillRGBA(rect, theColor);
        }
        else if( mReleasedSelection == curLinePos )
		{
            rect.pos.y = ypos + (line*rect.dim.y);
            blitsfc.fillRGBA(rect, theColor);
		}
#ifndef DISABLE_HOVER
        else if( mHoverSelection == curLinePos )
        {
            rect.pos.y = ypos+(line*sepHeight);
            blitsfc.fillRGBA(rect, theColor);
        }
#endif


        std::string trimmedText = theText;

        // If the text is too large to show, show a part of it. (by trimming)
		if(trimmedText.size() > textlimitWidth)
        {
			trimmedText = trimmedText.substr(0, textlimitWidth);
        }

        Font.drawFont(blitsfc, trimmedText,
                      xpos, ypos+(line*rect.dim.y)+halfBorderHeight, false);
	}

    mScrollbar.mMaxScrollAmt = mItemList.size()-mScrollbar.lastToShow();

    // Do we need a scrollbar?
    if(mScrollbar.mMaxScrollAmt>0)
    {
        mScrollbar.processRender(displayRect);
    }

    // Draw a highlighted over everything
    if( mEnabled && mSelected )
    {
        blitsfc.drawFrameRect(origRect, 2, blitsfc.mapRGB(0xB5, 0xB5, 0xF1) );
    }
}
