#include "stat_reader.h"


void PrintBus(const transport::TransportCatalogue& transport_catalogue, std::string_view name_bus, std::ostream& output) {
    const transport::Bus* bus = transport_catalogue.FindBus(name_bus);
    if (bus != nullptr) {
        transport::BusInfo info_bus = transport_catalogue.GetBusInfo(name_bus);
        output << "Bus " << name_bus << ": "
            << info_bus.route_count << " stops on route, "
            << info_bus.unique_route_count << " unique stops, "
            << std::setprecision(6) << info_bus.route_length << " route length, "
            << std::setprecision(6) << info_bus.curvature << " curvature\n";
    }
    else {
        output << "Bus " << name_bus << ": not found\n";
    }

}
void PrintStop(const transport::TransportCatalogue& catalogue, std::string_view stop_name, std::ostream& output) {
    const transport::Stop* stop = catalogue.FindStop(stop_name);
    if (stop == nullptr) {
        output << "Stop " << stop_name << ": not found\n";
        return;
    }
    const auto& buses = catalogue.GetBusesForStop(stop_name);
    if (buses.empty()) {
        output << "Stop " << stop_name << ": no buses\n";
    }
    else {
        std::vector<std::string_view> sorted(buses.begin(), buses.end());
        std::sort(sorted.begin(), sorted.end());
        output << "Stop " << stop_name << ": buses";
        for (std::string_view bus : sorted) {
            output << " " << bus;
        }
        output << "\n";
    }
}

void ParseAndPrintStat(const transport::TransportCatalogue& transport_catalogue, std::string_view request,
    std::ostream& output) {
    auto space_pos = request.find(" ");
    if (space_pos == std::string_view::npos) return;
    std::string_view command = request.substr(0, space_pos);
    auto it_name = request.find_first_not_of(" ", space_pos + 1);
    std::string_view name = request.substr(it_name);

    if (command == "Bus") {
        PrintBus(transport_catalogue, name, output);
    }
    else if (command == "Stop") {
        PrintStop(transport_catalogue, name, output);
    }

}
