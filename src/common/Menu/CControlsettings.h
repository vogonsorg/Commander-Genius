/*
 * CControlsettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CCONTROLSETTINGS_H_
#define CCONTROLSETTINGS_H_

#include "CBaseMenu.h"
#include "StringUtils.h"
#include "gui/CGUIButton.h"
#include "gui/CGUISwitch.h"
#include "engine/CEvent.h"
#include <vector>
#include <map>


// Main Class for Controller Config
class CControlsettings : public CBaseMenu
{
public:
	CControlsettings(const int selectedPlayer);

	void init();

	void release();

private:

	int mSelectedPlayer;

	CGUISwitch *mpTwoButtonSwitch;
};


// Movement config Part
class CControlSettingsMovement : public CBaseMenu
{
public:
	CControlSettingsMovement(const int selectedPlayer);

	void init();
	void release();

private:

	int mSelectedPlayer;

	std::vector<CGUIButton*> mpButtonList;
	std::map<InputCommands, std::string> mCommandName;
};



// Buttons config Part
class CControlSettingsButtons : public CBaseMenu
{
public:
	CControlSettingsButtons(const int selectedPlayer);

	void init();
	void release();

private:

	int mSelectedPlayer;

	std::vector<CGUIButton*> mpButtonList;
	std::map<InputCommands, std::string> mCommandName;
};

// Create the Control-Menu for the indicated Player

struct OpenControlMenuEvent : CEvent
{
	OpenControlMenuEvent(const int option) :
		mNumPlayers(option) {};

	int mNumPlayers;
};

struct OpenButtonsControlMenuEvent : OpenControlMenuEvent
{
	OpenButtonsControlMenuEvent(const int option) :
		OpenControlMenuEvent(option) {};
};

struct OpenMovementControlMenuEvent : OpenControlMenuEvent
{
	OpenMovementControlMenuEvent(const int option) :
		OpenControlMenuEvent(option) {};

};


class CPlayerControl : public CBaseMenu
{

public:
	CPlayerControl() :
		CBaseMenu( CRect<float>(0.25f, 0.35f, 0.5f, 0.3f) )
	{

		for( int i = 1 ; i <= MAX_PLAYERS ; i++ )
		{
			const std::string	playerStr = "Player " + itoa(i);
			mpButtonList.push_back( new CGUIButton( playerStr,
													new OpenControlMenuEvent(i) ) );
			mpMenuDialog->addControl( mpButtonList.back() );
		}

	}

	std::list<CGUIButton*> mpButtonList;
};



#endif /* CCONTROLSETTINGS_H_ */
