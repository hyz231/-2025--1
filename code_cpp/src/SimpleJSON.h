#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <cctype>

// Very small JSON helper tailored for this project.
// Supports serializing basic objects and arrays produced/consumed by this project.

inline std::string escapeJsonString(const std::string &input) {
    std::ostringstream ss;
    for (auto c : input) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c << std::dec << std::setfill(' ');
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}

// Note: this parser is intentionally minimal and expects JSON produced by the serializer.
inline std::string unquote(const std::string &s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        // naive unescape for common sequences
        std::string out;
        for (size_t i = 1; i+1 < s.size(); ++i) {
            char c = s[i];
            if (c == '\\' && i+1 < s.size()-1) {
                char n = s[++i];
                switch (n) {
                    case '"': out.push_back('"'); break;
                    case '\\': out.push_back('\\'); break;
                    case 'n': out.push_back('\n'); break;
                    case 'r': out.push_back('\r'); break;
                    case 't': out.push_back('\t'); break;
                    default: out.push_back(n); break;
                }
            } else out.push_back(c);
        }
        return out;
    }
    return s;
}
