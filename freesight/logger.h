#pragma once

#include <Windows.h>
#include <iostream>

namespace logger {
	
	std::string get_date_with_seconds_formated() {

		time_t rawtime;
		struct tm timeinfo;
		char buffer[80];


		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);

		strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
		std::string str(buffer);

		return str;
	}
	
	void log_ok(std::string line)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 07);
		std::cout << get_date_with_seconds_formated();
		std::cout << " [ ";

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);

		std::cout << "OK";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 07);
		std::cout << " ] -- " << line << std::endl;
	}

	void log_error(std::string line)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 07);
		std::cout << get_date_with_seconds_formated();
		std::cout << " [ ";

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 04);

		std::cout << "ERROR";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 07);
		std::cout << " ] -- " << line << std::endl;
	}

	void log_info(std::string line)
	{
		printf("\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 07);
		std::cout << " [ ";

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 233);

		std::cout << "INFO";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 07);
		std::cout << " ] -- " << line << std::endl;
		printf("\n");
	}
}