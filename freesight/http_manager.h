#pragma once

#include <windows.h>

namespace http_manager
{

	extern int nb_bar;
	extern double last_progress, progress_bar_adv;

	int download_file(_In_opt_ LPUNKNOWN, std::string url, std::string output_file, std::string file_name);
	int progress_bar(void* bar, double t, double d);
}