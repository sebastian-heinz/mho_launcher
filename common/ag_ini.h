#ifndef MHO_AG_INI_H
#define MHO_AG_INI_H

#include <fstream>
#include <map>
#include <string>

inline std::map<std::string, std::string> ag_ini_read(const std::wstring &path) {
    std::map<std::string, std::string> out;
    std::ifstream f(path.c_str());
    if (!f.is_open()) return out;

    auto trim = [](std::string &s) {
        size_t a = s.find_first_not_of(" \t");
        size_t b = s.find_last_not_of(" \t\r");
        if (a == std::string::npos) { s.clear(); return; }
        s = s.substr(a, b - a + 1);
    };

    std::string line;
    while (std::getline(f, line)) {
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        char c = line[start];
        if (c == '#' || c == ';' || c == '[') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        trim(key);
        trim(val);
        if (!key.empty()) out[key] = val;
    }
    return out;
}

inline bool ag_ini_create_if_missing(const std::wstring &path, const char *contents) {
    std::ifstream test(path.c_str());
    if (test.is_open()) return false;
    std::ofstream f(path.c_str());
    if (!f.is_open()) return false;
    f << contents;
    return true;
}

inline int ag_ini_get_int(const std::map<std::string, std::string> &m, const std::string &key, int def) {
    auto it = m.find(key);
    if (it == m.end() || it->second.empty()) return def;
    try { return std::stoi(it->second); } catch (...) { return def; }
}

static const char AG_MHO_INI_DEFAULTS[] =
    "# ag_mho.ini - MHO launcher / injector config\n"
    "\n"
    "# Inject ag_mho.dll into MHOClient.exe (0 = disabled, 1 = enabled)\n"
    "inject_dll = 0\n"
    "\n"
    "# Log client_log messages captured by the mhoclient hook (0 = disabled, 1 = enabled)\n"
    "log_client_logs = 0\n"
    "\n"
    "# Log encrypted network traffic (TPDU encrypt/decrypt input + output buffers) in protocalhandler (0 = disabled, 1 = enabled)\n"
    "log_encrypted_network = 0\n";

#endif
