#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;

/*
 * ===== DHCP INFO =====
 */
struct DhcpInfo
{
    string mac;
    string name;
};

unordered_map<string, DhcpInfo> dhcp_map;

/*
 * ===== FLOW STAT =====
 */
struct DeviceStat
{
    uint64_t rx = 0;
    uint64_t tx = 0;
};

unordered_map<string, DeviceStat> last_stats;

/*
 * ANSI COLOR
 */
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

/*
 * 读取 DHCP leases
 */
void load_dhcp()
{
    dhcp_map.clear();

    ifstream file("/tmp/dhcp.leases");
    if (!file.is_open()) return;

    long expire;
    string mac, ip, name, clientid;

    while (file >> expire >> mac >> ip >> name >> clientid)
    {
        dhcp_map[ip] = {mac, name};
    }
}

/*
 * 判断局域网 IP
 */
bool is_lan_ip(const string& ip)
{
    if (ip.rfind("192.168.", 0) == 0) return true;
    if (ip.rfind("10.", 0) == 0) return true;

    if (ip.rfind("172.", 0) == 0)
    {
        size_t p1 = ip.find('.');
        size_t p2 = ip.find('.', p1 + 1);

        if (p2 != string::npos)
        {
            int second = stoi(ip.substr(p1 + 1, p2 - p1 - 1));
            if (second >= 16 && second <= 31)
                return true;
        }
    }
    return false;
}

/*
 * conntrack 扫描
 */
unordered_map<string, DeviceStat> scan_conntrack()
{
    unordered_map<string, DeviceStat> devices;

    ifstream file("/proc/net/nf_conntrack");
    if (!file.is_open()) return devices;

    string line;

    regex src_re(R"(src=([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+))");
    regex bytes_re(R"(bytes=(\d+))");

    while (getline(file, line))
    {
        vector<string> srcs;
        vector<uint64_t> bytes_list;

        auto src_begin = sregex_iterator(line.begin(), line.end(), src_re);
        auto src_end = sregex_iterator();

        for (auto i = src_begin; i != src_end; ++i)
            srcs.push_back((*i)[1]);

        auto bytes_begin = sregex_iterator(line.begin(), line.end(), bytes_re);
        auto bytes_end = sregex_iterator();

        for (auto i = bytes_begin; i != bytes_end; ++i)
            bytes_list.push_back(stoull((*i)[1]));

        if (srcs.size() < 2 || bytes_list.size() < 2)
            continue;

        if (is_lan_ip(srcs[0]))
        {
            devices[srcs[0]].tx += bytes_list[0];
            devices[srcs[0]].rx += bytes_list[1];
        }
        else if (is_lan_ip(srcs[1]))
        {
            devices[srcs[1]].rx += bytes_list[0];
            devices[srcs[1]].tx += bytes_list[1];
        }
    }

    return devices;
}

/*
 * 颜色判断
 */
const char* get_color(double mbps)
{
    if (mbps > 5.0) return RED;
    if (mbps > 1.0) return YELLOW;
    return GREEN;
}

int main()
{
    cout << "FlowMon Started (MB/s + DHCP + MAC)\n";

    while (true)
    {
        load_dhcp();
        auto curr = scan_conntrack();

        cout << "\033[2J\033[H";

        cout << left
             << setw(16) << "IP"
             << setw(18) << "MAC"
             << setw(18) << "NAME"
             << setw(12) << "RX MB/s"
             << setw(12) << "TX MB/s"
             << endl;

        cout << "-------------------------------------------------------------\n";

        for (auto& dev : curr)
        {
            const string& ip = dev.first;

            uint64_t curr_rx = dev.second.rx;
            uint64_t curr_tx = dev.second.tx;

            uint64_t last_rx = last_stats[ip].rx;
            uint64_t last_tx = last_stats[ip].tx;

            uint64_t delta_rx = (curr_rx >= last_rx) ? (curr_rx - last_rx) : curr_rx;
            uint64_t delta_tx = (curr_tx >= last_tx) ? (curr_tx - last_tx) : curr_tx;

            double rx_mbps = delta_rx / 1024.0 / 1024.0;
            double tx_mbps = delta_tx / 1024.0 / 1024.0;

            string mac = "-";
            string name = "-";

            if (dhcp_map.count(ip))
            {
                mac = dhcp_map[ip].mac;
                name = dhcp_map[ip].name;
            }

            const char* color = get_color(max(rx_mbps, tx_mbps));

            cout << color
                 << left
                 << setw(16) << ip
                 << setw(18) << mac
                 << setw(18) << name
                 << setw(12) << fixed << setprecision(2) << rx_mbps
                 << setw(12) << tx_mbps
                 << RESET
                 << endl;

            last_stats[ip] = dev.second;
        }

        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
