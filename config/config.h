#ifndef CONFIG_H
#define CONFIG_H

#include "../lib/split.h"
#include <yaml-cpp/yaml.h>

class config {

private:

    YAML::Node conf;

    static std::string get_config_path() {
        std::string config_path = std::filesystem::current_path().string().append("/config.yaml");
        return config_path;
    }

    static bool config_exists() {
        return std::filesystem::exists(get_config_path());
    }

public:
    config() = default;

    bool load_config() {

        if (!config_exists()) {
            create_default();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        try {
            conf = YAML::LoadFile(get_config_path());
            return true;
        } catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error: " << re.what() << std::endl;
            create_default();
        }
        catch(const std::exception& ex)
        {
            std::cerr << "Error occurred: " << ex.what() << std::endl;
            create_default();
        }
        catch(...)
        {
            std::cerr << "Unknown failure occurred. Possible memory corruption" << std::endl;
            create_default();
        }
        return false;
    }

    std::string get(const std::string& key, std::string default_value) {
        try {

            std::vector<std::string> keys = split_str(key, ".");
            YAML::Node node = conf;

            for (int i = 0; i < keys.size(); i++) {

                if (node[keys[i]]) {
                    node = node[keys[i]];
                    if (i == keys.size() - 1) {
                        return node.as<std::string>();
                    }
                }
                else {
                    return default_value;
                }

            }
        }
        catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error: " << re.what() << std::endl;
            create_default();
        }
        catch(const std::exception& ex)
        {
            std::cerr << "Error occurred: " << ex.what() << std::endl;
        }
        catch(...)
        {
            std::cerr << "Unknown failure occurred. Possible memory corruption" << std::endl;
            create_default();
        }
        return default_value;
    }

    bool get_bool(const std::string& key, bool default_value) {
        try {

            std::vector<std::string> keys = split_str(key, ".");
            YAML::Node node = conf;

            for (int i = 0; i < keys.size(); i++) {
                if (node[keys[i]].IsDefined()) {
                    node = node[keys[i]];
                    if (i == keys.size() - 1) {
                        load_config();
                        return node.as<bool>();
                    }
                }
                else {
                    return default_value;
                }
            }
        }
        catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error: " << re.what() << std::endl;
            create_default();
        }
        catch(const std::exception& ex)
        {
            std::cerr << "Error occurred: " << ex.what() << std::endl;
        }
        catch(...)
        {
            std::cerr << "Unknown failure occurred. Possible memory corruption" << std::endl;
            create_default();
        }
        return default_value;
    }

    static void create_default() {

        if(config_exists())
            std::filesystem::remove(get_config_path());

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "auto_update";
        out << YAML::Value << "true";
        out << YAML::Key << "options";
        out << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "firewall";
        out << YAML::Value << "true";
        out << YAML::Key << "regedit";
        out << YAML::Value << "true";
        out << YAML::Key << "vpn";
        out << YAML::Value << "false";
        out << YAML::EndMap;

        std::ofstream file(get_config_path());
        file << out.c_str();
        file.close();

        std::cout << "Created default config file" << std::endl;
    }
};

#endif //CONFIG_H
