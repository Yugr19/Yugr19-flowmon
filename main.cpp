#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <regex>

using namespace std;

struct FlowInfo {
    unsigned long long bytes = 0;
};

unordered_map<string, unsigned long long> last_bytes;

/*
 * 从 /proc/net/nf_conntrack 提取 IP + bytes
 */
unordered_map<string, unsigned long long> parse_conntrack() {
    ifstream file("/proc/net/nf_conntrack");
    unordered_map<string, unsigned long long> result;

    if (!file.is_open()) return result;

    string line;

    regex ip_regex(R"(src=([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+))");
    regex bytes_regex(R"(bytes=(\d+))");

    while (getline(file, line)) {

        smatch ip_match;
        smatch bytes_match;

        string ip;
        unsigned long long bytes = 0;

        if (regex_search(line, ip_match, ip_regex)) {
            ip = ip_match[1];
        } else {
            continue;
        }

        if (regex_search(line, bytes_match, bytes_regex)) {
            bytes = stoull(bytes_match[1]);
        }

        result[ip] += bytes;
    }

    return result;
}

int main() {
    cout << "FlowMon IP-level speed started..." << endl;

    while (true) {

        auto curr = parse_conntrack();

        cout << "---- Device Speed ----" << endl;

        for (auto &p : curr) {

            string ip = p.first;
            unsigned long long bytes_now = p.second;

            unsigned long long bytes_last = last_bytes[ip];

            unsigned long long delta = 0;

            if (bytes_now > bytes_last)
                delta = bytes_now - bytes_last;

            double kbps = delta / 1024.0;

            cout << ip << " → " << kbps << " KB/s" << endl;

            last_bytes[ip] = bytes_now;
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
}