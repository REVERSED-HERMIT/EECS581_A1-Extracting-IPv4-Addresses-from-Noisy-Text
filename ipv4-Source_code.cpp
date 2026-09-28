//Code was produced by GPT-6 Astra Medium, and monitored, tested, reviewed, and analyzed by Abdulaziz Arab

#include <iostream>
#include <string>

namespace {
bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isTokenCharacter(char c) {
    return isDigit(c) || c == '.' || c == ':';
}

// Parse one numeric component without any string-to-number conversion.
bool parseNumber(const std::string& str, std::size_t& pos,
                 std::size_t end, unsigned int maxDigits,
                 unsigned int maxValue, unsigned int& value) {
    const std::size_t start = pos;
    value = 0;
    while (pos < end && isDigit(str[pos])) {
        // Check the length before accumulating, preventing overflow even
        // for input containing an arbitrarily long run of digits.
        if (pos - start >= maxDigits) return false;
        if (pos > start && str[start] == '0') return false;
        value = value * 10 + static_cast<unsigned int>(str[pos] - '0');
        if (value > maxValue) return false;
        ++pos;
    }
    return pos > start;
}

bool parseCandidate(const std::string& str, std::size_t start,
                    std::size_t end, unsigned long& address, int& port) {
    std::size_t pos = start;
    address = 0;
    port = -1;
    for (int octetIndex = 0; octetIndex < 4; ++octetIndex) {
        unsigned int octet = 0;
        if (!parseNumber(str, pos, end, 3, 255, octet)) return false;
        address = address * 256UL + octet;
        if (octetIndex < 3) {
            if (pos == end || str[pos] != '.') return false;
            ++pos;
        }
    }
    if (pos < end) {
        if (str[pos] != ':') return false;
        ++pos;
        unsigned int parsedPort = 0;
        if (!parseNumber(str, pos, end, 5, 65535, parsedPort)) return false;
        port = static_cast<int>(parsedPort);
    }
    return pos == end;
}
} // namespace

// Policy for multiple valid candidates: return the first from left to right.
// Failed candidates are skipped in full, never searched for valid substrings.
bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort) {
    outAddress = 0;
    outPort = -1;
    std::size_t pos = 0;
    while (pos < str.size()) {
        if (!isTokenCharacter(str[pos])) {
            ++pos;
            continue;
        }
        const std::size_t start = pos;
        while (pos < str.size() && isTokenCharacter(str[pos])) ++pos;
        unsigned long address = 0;
        int port = -1;
        if (parseCandidate(str, start, pos, address, port)) {
            outAddress = address;
            outPort = port;
            return true;
        }
    }
    return false;
}

int main() {
    std::string line;
    while (true) {
        std::cout << "Enter a string (or 'END' to quit): ";
        if (!std::getline(std::cin, line)) break;
        if (line == "END") {
            std::cout << "Program terminated.\n";
            break;
        }
        unsigned long address = 0;
        int port = -1;
        if (!extractIPv4(line, address, port)) {
            std::cout << "Invalid input: no valid IPv4 address found\n";
            continue;
        }
        std::cout << "Extracted IPv4 address: "
                  << ((address >> 24) & 255UL) << '.'
                  << ((address >> 16) & 255UL) << '.'
                  << ((address >> 8) & 255UL) << '.'
                  << (address & 255UL)
                  << " (decimal value: " << address << ", port: ";
        if (port == -1) std::cout << "none";
        else std::cout << port;
        std::cout << ")\n";
    }
    return 0;
}
