#pragma once

#include <iosfwd>
#include <string_view>
#include <iomanip>
#include <string>
#include <algorithm>
#include "transport_catalogue.h"

void PrintBus(const transport::TransportCatalogue& transport_catalogue, std::string_view name_bus, std::ostream& output);
void PrintStop(const transport::TransportCatalogue& catalogue, std::string_view stop_name, std::ostream& output);
void ParseAndPrintStat(const transport::TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
