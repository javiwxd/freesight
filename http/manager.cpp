#include "manager.h"

namespace manager {

    int last;
    int total;
    int progress_bar(void* bar, double t, double d);

    int download_file(_In_opt_ LPUNKNOWN, const std::string& url, const std::string& output_file, const std::string& file_name) {

        CURL* curl_download;
        FILE* fp;
        CURLcode res;

        curl_download = curl_easy_init();

        logger::log(std::source_location::current(), "Descargando archivo \"" + file_name + "\"...");
        std::cout << std::endl;

        if (curl_download)
        {
            std::filesystem::create_directories(std::filesystem::path(output_file).parent_path());
            fp = fopen(output_file.c_str(), "wb");

            curl_easy_setopt(curl_download, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl_download, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl_download, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl_download, CURLOPT_NOPROGRESS, FALSE);
            curl_easy_setopt(curl_download, CURLOPT_PROGRESSFUNCTION, progress_bar);
            //curl_easy_setopt(curl_download, CURLOPT_VERBOSE, 1L);

            res = curl_easy_perform(curl_download);

            fclose(fp);
            if (res == CURLE_OK)
            {
                std::cout << std::endl << std::endl;
                logger::log(std::source_location::current(), "Archivo \""+file_name + "\" descargado");

                curl_easy_cleanup(curl_download);
                return 0;
            }
            else
            {
                logger::log_error(std::source_location::current(), "Error al intentar descargar el archivo \"" + file_name+"\". Código de error: " + std::to_string(res));

                curl_easy_cleanup(curl_download);
                return 1;
            }
        }
        curl_easy_cleanup(curl_download);
        return 1;
    }

    int progress_bar(void* bar, double t, double d)
    {

        float perc = ((float)d / (float)t) * 100;
        int rounded = (int)round(perc);

        if (last != rounded) {
            last = rounded;
            if (rounded % 2 == 0) {
                std::cout << ".";

                if (rounded % 5 == 0) {
                    std::cout << rounded;
                }
            }
        }
        return 0;
    }

}