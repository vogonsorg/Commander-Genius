/*
 * CGUIText.cpp
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 */

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>
#include <base/utils/StringUtils.h>

#include "GsText.h"

#include <graphics/cgttf.h>

CGUIText::CGUIText(const std::string &text,
                   const GsRect<float> &rect) :
GsWidget(rect)
{
    setText(text);
}

CGUIText::CGUIText(const std::string &text,
         const GsRect<float> &rect,
         const int fontId) :
GsWidget(rect),
mFontId(fontId)
{
    setText(text);
}



void CGUIText::setText(const std::string& text)
{    
    if(!mTextVec.empty())
        mTextVec.clear();

	// Split up the text in lines
    mTextDim.dim.x = 0;

	// TODO: I think there is a more elegant way to achieve this!
	std::string buf = "";
	for( size_t i=0 ; i<text.size() ; i++ )
	{
        if( endofLine(text.substr(i)) )
		{            
            if( mTextDim.dim.x < buf.size() )
                mTextDim.dim.x = static_cast<unsigned int>(buf.size());

            mTextVec.push_back(buf);
			buf.clear();
		}
		else
        {
			buf += text[i];
        }
	}

	size_t pos = 0;
	if(!buf.empty())
	{
		while( (pos = buf.find('\n')) != std::string::npos )
			buf.erase(pos,1);
	}
    mTextVec.push_back(buf);

    if( mTextDim.dim.x < buf.size() )
        mTextDim.dim.x = static_cast<unsigned int>(buf.size());

    mTextDim.dim.y = static_cast<unsigned int>(mTextVec.size());

    mTextChanged = (mText != text);
    mText = text;
}

void CGUIText::setTextColor(const GsColor &color)
{
    mTextColor = color;
}

void CGUIText::processLogic()
{
    // Horizontal scrolling.
    // If Max is zero, nothing need to be scrolled
    if(mScrollPosMax <= 0)
        return;

    // Check if scroll position touches the edges
    if(mScrollPos <= 0)
    {
        mScrollDir = ScrollDir::RIGHT;
        mScrollPos = 0;
    }
    else if(mScrollPos >= mScrollPosMax)
    {
        mScrollDir = ScrollDir::LEFT;
        mScrollPos = mScrollPosMax;
    }

    // Scroll into the direction
    if(mScrollDir == ScrollDir::LEFT)
    {
        mScrollPos -= mScrollVel;
    }
    else if(mScrollDir == ScrollDir::RIGHT)
    {
        mScrollPos += mScrollVel;
    }
}

void CGUIText::updateTTFTextSfc(const GsRect<float> &displayRect)
{
    const int reqFontSize = int(displayRect.dim.y*0.75f);

    auto &textSfcVec = mTextSfcVecByColor[mTextColor];

    bool needUpdate =
            (mFontSize != reqFontSize) || (mTextChanged);

    if( textSfcVec.empty() )
        needUpdate = true;

    if(needUpdate)
    {
        mFontSize = reqFontSize;

        mTrueTypeFont.openFromMem(gCgTtf, reqFontSize);

        const auto numTexLines = mTextVec.size();
        textSfcVec.resize(numTexLines);

        for(unsigned int idx = 0 ; idx<numTexLines ; idx++)
        {
            mTrueTypeFont.render(textSfcVec[idx], mTextVec[idx], mTextColor);
        }

        mTextChanged = false;
    }
}

void CGUIText::updateLegacyTextSfc(const GsRect<float> &displayRect)
{
    const int reqFontSize = int(displayRect.dim.y*0.75f);

    bool needUpdate =
            (mFontSize != reqFontSize) || (mTextChanged);

    auto &curTextSfcVec = mTextSfcVecByColor[mTextColor];

    if( curTextSfcVec.empty() )
        needUpdate = true;

    const auto numTexLines = mTextVec.size();

    if(needUpdate)
    {                
        mFontSize = reqFontSize;                

        auto lRect = displayRect.SDLRect();
        lRect.x = lRect.y = 0;

        if(mFontId < 0)
            return;

        if(gGraphics.getNumLegacyFonts() <= mFontId)
           return;

        // Now lets draw the text of the list control
        auto &Font = gGraphics.getFontLegacy( mFontId );

        const auto letterHeight = int(Font.getPixelTextHeight());
        const auto textWidth = int(Font.calcPixelTextWidth(mText));

        lRect.h = letterHeight;
        lRect.w = textWidth;

        for(auto &textSfcVecPair : mTextSfcVecByColor)
        {
            const auto color = textSfcVecPair.first;
            auto &textSfcVec = textSfcVecPair.second;

            textSfcVec.resize(numTexLines);

            for(unsigned int idx = 0 ; idx<numTexLines ; idx++)
            {
                auto &theText = mTextVec[idx];
                auto &textSfc = textSfcVec[idx];

                textSfc.createRGBSurface(lRect);

                Font.drawFont(textSfc, theText,
                              0, 0,
                              false);

                if(Font.tintingAllowed())
                    textSfc.tintColor( color );
            }
        }

        mTextChanged = false;

        for(unsigned int idx = 0 ; idx<numTexLines ; idx++)
        {
            auto &theText = mTextVec[idx];
            const auto textWidth = int(Font.calcPixelTextWidth(theText));

            // The tolerance is the amount of pixels at least of difference to consider
            // for scrolling. We consider a tolerance so strange jittery are avoided for text
            // that nearly fits
            const int tol = 8;

            // The first text item decides wheter scrolling takes place
            if(textWidth > int(lRect.w) + tol) // tolerance
            {
                mScrollPosMax = textWidth - lRect.w;
            }
            else
            {
                mScrollPosMax = 0;
            }
        }

    }

}

void CGUIText::processRender(const GsRect<float> &RectDispCoordFloat)
{
    processRender(RectDispCoordFloat, RectDispCoordFloat);
}

void CGUIText::processRender(const GsRect<float> &backRect,
                             const GsRect<float> &frontRect)
{
    // Transform this object display coordinates
    auto objBackRect = backRect.transformed(getRect());
    auto objFrontRect = objBackRect.clipped(frontRect);

    auto &blit = gVideoDriver.gameSfc();

    auto &textSfcVec = mTextSfcVecByColor[mTextColor];

    unsigned int totTextSfcH = 0;

    if(mFontId < 0)
    {        
        updateTTFTextSfc(objBackRect);
    }
    else
    {
        updateLegacyTextSfc(objBackRect);
    }


    GsVec2D<unsigned int> totTextSfcDim(0,0);

    // Calculate total text dimension in case we need to center
    if(mHCentered)
    {
        for(auto &textSfc : textSfcVec)
        {
            if(textSfc.empty())
                break;

            totTextSfcDim.x =
                    std::max(static_cast<unsigned int>(textSfc.width()),
                             static_cast<unsigned int>(totTextSfcDim.x));
            const auto txtHeight = textSfc.height();
            totTextSfcDim.y += txtHeight;
        }
    }

    // Time to render
    for(auto &textSfc : textSfcVec)
    {
        if(textSfc.empty())
            break;

        GsRect<float> blitPos = objFrontRect;

        if(mHCentered)
        {
            blitPos.pos = blitPos.pos + (blitPos.dim-totTextSfcDim)/2;
        }

        auto textrect = blitPos;

        textrect.pos.y = (objBackRect.pos.y < objFrontRect.pos.y) ?
                          objBackRect.dim.y - objFrontRect.dim.y : 0;
        textrect.pos.x = (objBackRect.pos.x < objFrontRect.pos.x) ?
                          objBackRect.dim.x - objFrontRect.dim.x : 0;

        blitPos.pos.y += totTextSfcH;

        textSfc.blitTo(blit,
                       textrect.SDLRect(),
                       blitPos.SDLRect());
        totTextSfcH += textSfc.height();
    }

}

