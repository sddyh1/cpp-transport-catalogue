#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return { nan, nan };
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return { lat, lng };
}

std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}


std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return { std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1)) };
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}
std::vector<std::pair<int, std::string>> ParseStopDistances(std::string_view desc) {

    std::vector<std::pair<int, std::string>> result;
    auto parts = Split(desc, ',');

    if (parts.size() < 3) return result;

    for (int i = 0; i < parts.size(); ++i) {
        std::string_view str = parts[i];

        size_t m_pos = str.find('m');
        if (m_pos == str.npos) continue;

        int dist = 0;

        try {
            dist = std::stoi(std::string(str.substr(0, m_pos)));
        }
        catch (...) {
            continue;
        }

        size_t to_pos = str.find("to", m_pos + 1);

        if (to_pos == str.npos) continue;
        std::string_view stop_name = str.substr(to_pos + 2);
        stop_name = Trim(stop_name);
        if (!stop_name.empty()) {
            result.emplace_back(dist, std::string(stop_name));
        }

    }
    return result;

}

void InputReader::ApplyCommands([[maybe_unused]] transport::TransportCatalogue& catalogue) const {
    for (const auto& cmd : commands_) {
        if (cmd.command == "Stop") {
            geo::Coordinates coords = ParseCoordinates(cmd.description);
            catalogue.AddStop(cmd.id, coords);
        }
    }

    for (const auto& cmd : commands_) {
        if (cmd.command == "Stop") {
            auto dist = ParseStopDistances(cmd.description);
            auto from = catalogue.FindStop(cmd.id);
            for (const auto& [d, s_n] : dist) {
                auto to = catalogue.FindStop(s_n);
                if (to) catalogue.SetDistance(from, to, d);
            }
        }
    }

    for (const auto& cmd : commands_) {
        if (cmd.command == "Bus") {
            std::vector<std::string_view> stop_names = ParseRoute(cmd.description);
            std::vector<const transport::Stop*> stops_ptrs;
            stops_ptrs.reserve(stop_names.size());
            for (std::string_view name : stop_names) {
                const transport::Stop* stop = catalogue.FindStop(name);
                stops_ptrs.push_back(stop);
            }
            bool is_circular = cmd.description.find('>') != std::string::npos;
            catalogue.AddBus(cmd.id, stops_ptrs, is_circular);
        }
    }
}
