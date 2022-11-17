#include <Windows.h>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <filesystem>

#pragma warning(disable : 4996)

namespace http_manager {

	int nb_bar;
	double last_progress, progress_bar_adv;
	int progress_bar(void* bar, double t, double d);

	int download_file(_In_opt_ LPUNKNOWN, std::string url, std::string output_file, std::string file_name) {

		CURL* curl_download;
		FILE* fp;
		CURLcode res;

		curl_download = curl_easy_init();

		if (curl_download)
		{

			std::filesystem::create_directories(std::filesystem::path(output_file).parent_path());
			fp = fopen(output_file.c_str(), "wb");

			curl_easy_setopt(curl_download, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl_download, CURLOPT_WRITEFUNCTION, NULL);
			curl_easy_setopt(curl_download, CURLOPT_WRITEDATA, fp);
			curl_easy_setopt(curl_download, CURLOPT_NOPROGRESS, FALSE);
			//curl_easy_setopt(curl_download, CURLOPT_PROGRESSFUNCTION, progress_bar);

			res = curl_easy_perform(curl_download);

			fclose(fp);
			if (res == CURLE_OK)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				std::cout << std::endl << std::endl << " -- " << file_name << " ha sido descargado" << std::endl;

				curl_easy_cleanup(curl_download);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				return 0;
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				std::cout << std::endl << std::endl << " -- Ha ocurrido un error al intentar descargar el archivo: " << res << std::endl;

				curl_easy_cleanup(curl_download);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				return 1;
			}
			curl_easy_cleanup(curl_download);
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		return 0;
	}

	int progress_bar(void* bar, double t, double d)
	{
		if (last_progress != round(d / t * 100))
		{
			nb_bar = 25;
			progress_bar_adv = round(d / t * nb_bar);

			std::cout << "\r ";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 160);
			std::cout << " Progreso : [ ";

			if (round(d / t * 100) < 10)
			{
				std::cout << "0" << round(d / t * 100) << " %]";
			}
			else
			{
				std::cout << round(d / t * 100) << " %] ";
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			std::cout << " [";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			for (int i = 0; i <= progress_bar_adv; i++)
			{
				if (i > 30) {
					//Error
					std::cout << std::endl << " -- Buffer overflow: " << i << ">30" << std::endl;
					exit(1);
				}
				std::cout << "#";
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			for (int i = 0; i < nb_bar - progress_bar_adv; i++)
			{
				std::cout << ".";
			}

			std::cout << "]";
			last_progress = round(d / t * 100);
		}
		return 0;
	}

}