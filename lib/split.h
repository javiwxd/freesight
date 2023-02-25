#ifndef SPLIT_H
#define SPLIT_H

inline std::vector<std::string> split_str(const std::string& str, const std::string& deli = ".")
{
    int start = 0;
    int end = str.find(deli);

    std::vector<std::string> output = std::vector<std::string>();

    while (end != -1) {
        output.push_back(str.substr(start, end - start));
        start = end + deli.size();
        end = str.find(deli, start);
    }
    output.push_back(str.substr(start, end - start));
    return output;
}

#endif //SPLIT_H
