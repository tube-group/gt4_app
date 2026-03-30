
#include "Tube.h"

std::string trimString(std::string str)
{
	if (str.empty())
	{
		return str;
	}

	int s = str.find_first_not_of(" ");

	if (s < 0)
	{
		str = "";
		return str;;
	}

	int e = str.find_last_not_of(" ");
	str = str.substr(s, e - s + 1);
	return str;
}