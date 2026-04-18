#include "transport_catalogue.h"

namespace transport {
	void TransportCatalogue::AddStop(const std::string& name, const geo::Coordinates& cord) {
		assert(!name.empty());
		stops_.push_back({ name,cord });
		stops_name_[stops_.back().name] = &stops_.back();

	}

	const Stop* TransportCatalogue::FindStop(std::string_view name) const {
		auto it = stops_name_.find(name);
		return (it != stops_name_.end()) ? it->second : nullptr;
	}

	const Bus* TransportCatalogue::FindBus(std::string_view name) const {
		auto it = all_route_.find(name);
		return (it != all_route_.end()) ? it->second : nullptr;
	}

	void TransportCatalogue::AddBus(const std::string_view name, const std::vector<const Stop*>& stops, bool circ) {
		buses_.push_back({ std::string(name), stops, circ });
		const Bus* bus = &buses_.back();
		all_route_[bus->name] = bus;

		for (const Stop* stop : stops) {
			stop_to_buses_[stop->name].insert(bus->name);
		}


	}
	const std::unordered_set<std::string_view>* TransportCatalogue::GetBusesForStop(std::string_view stop_name) const {

		auto it_s = stops_name_.find(stop_name);
		if (it_s == stops_name_.end()) {
			return nullptr;
		}

		auto it_b = stop_to_buses_.find(stop_name);

		if (it_b != stop_to_buses_.end()) {
			return &(it_b->second);
		}

		static const std::unordered_set<std::string_view> empty;
		return &empty;

	}
	BusInfo TransportCatalogue::GetBusInfo(std::string_view bus_name) const {

		const Bus* bus = FindBus(bus_name);

		if (bus != nullptr) {
			std::unordered_set<const Stop*> unique_stops;
			unique_stops.insert(bus->route.begin(), bus->route.end());

			double actual_length = 0.0;
			double geo_length = 0.0;

			for (size_t i = 0; i + 1 < bus->route.size(); ++i) {
				actual_length += GetDistance(bus->route[i], bus->route[i + 1]);
				geo_length += geo::ComputeDistance(bus->route[i]->location, bus->route[i + 1]->location);
			}

			double curvature = (geo_length > 0.1) ? (actual_length / geo_length) : 1.0;

			return{ static_cast<int>(bus->route.size()),static_cast<int>(unique_stops.size()), actual_length, curvature };
		}
		return { 0,0,0,0 };
	}
	void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, int distance) {
		distances_[{from, to}] = distance;
	}

	int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const
	{
		auto it = distances_.find({ from,to });

		if (it != distances_.end()) {
			return it->second;
		}
		it = distances_.find({ to, from });
		if (it != distances_.end()) {
			return it->second;
		}
		return 0;
	}
}
