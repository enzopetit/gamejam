#include "audio/MusicManager.hpp"
#include <fstream>
#include <regex>

namespace { std::string extractStringValue(const std::string& content, const std::string& key){ std::regex re("\\\""+key+"\\\"\\s*:\\s*\\\"([^\\\"]+)\\\""); std::smatch m; if (std::regex_search(content,m,re)) return m[1].str(); return {}; }
std::string extractObjectBlock(const std::string& content, const std::string& key){ std::string needle = "\""+key+"\""; size_t k=content.find(needle); if (k==std::string::npos) return {}; size_t b=content.find('{',k); if (b==std::string::npos) return {}; int d=0; for (size_t i=b;i<content.size();++i){ if(content[i]=='{') d++; else if(content[i]=='}'){ d--; if(!d) return content.substr(b, i-b+1);} } return {}; }
}

bool MusicManager::loadFromJson(const std::string& path) {
    std::ifstream file(path); if (!file.is_open()) return false;
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    beginTrack = extractStringValue(content, "begin"); perMinute.clear();
    std::string block = extractObjectBlock(content, "per_minute");
    if (!block.empty()) { std::regex re("\\\"(\\d+)\\\"\\s*:\\s*\\\"([^\\\"]+)\\\""); for (auto it=std::sregex_iterator(block.begin(), block.end(), re), end=std::sregex_iterator(); it!=end; ++it){ int minute = std::stoi((*it)[1].str()); perMinute[minute] = (*it)[2].str(); } }
    ready = !perMinute.empty(); return ready;
}

