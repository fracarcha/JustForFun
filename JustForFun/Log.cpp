/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

// ------------------------------------------ LOG HELPER FUNCTIONS ---------------------------------------------- //

#include <ctime>
#include <iomanip>
#include <sstream>

std::string JFFGetCurrentTime()
{
	// current date/time based on current system, measured since epoch
	time_t now = time(0);

	// converts given time since epoch into calendar time, expressed in local time
	std::tm buf{};

#	if defined(__unix__) // TODO: Not tested in UNIX
		localtime_r(&now, &bt);
#	elif defined(_MSC_VER)
		localtime_s(&buf, &now);
#	else 
		// TODO: This branch isn't tested
		static std::mutex mtx;
		std::lock_guard<std::mutex> lock(mtx);
		bt = *std::localtime(&timer);
#	endif

	// Format calendar time and store it into a string stream
	std::ostringstream oss;
	oss << std::put_time(&buf, "%d-%m-%Y %H:%M:%S");

	return oss.str();
}
