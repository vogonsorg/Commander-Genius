/*
 * options.h
 *
 *  Created on: 17.10.2009
 *      Author: gerstrong
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>

enum e_OptionKeyword
{ OPT_FULLYAUTOMATIC, OPT_SUPERPOGO,
	OPT_ALLOWPKING, OPT_KEYSTACK,
	OPT_ANALOGJOYSTICK,
	OPT_LVLREPLAYABILITY, OPT_RISEBONUS,
	OPT_SWITCHSCORES, OPT_IMPPOGO,
	NUM_OPTIONS
};

struct stOption
{
	std::string menuname;
	std::string name;
	char value;
};

#endif /* OPTIONS_H_ */
