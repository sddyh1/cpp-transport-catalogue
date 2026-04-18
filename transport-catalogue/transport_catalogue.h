#pragma once
#include <string>
#include "geo.h"
#include <deque>
#include <unordered_map>
#include <cassert>
#include <vector>
#include <unordered_set>
#include <string_view>
#include <set>
#include <functional>
namespace transport {

	struct Stop
	{
		std::string name;
		geo::Coordinates location;
	};
	struct Bus
	{
		std::string name;
		std::vector<const Stop*> route;
		bool is_circular;
	};
	struct BusInfo
	{
		int route_count;
		int unique_route_count;
		double route_length;
		double curvature;
	};

	struct PairHasher {
		size_t operator()(const std::pair<const Stop*, const Stop*>& p) const {
			size_t h1 = std::hash<const void*>{}(p.first);
			size_t h2 = std::hash<const void*>{}(p.second);
			return h1 ^ (h2 << 1);
		}

	};

	class TransportCatalogue {
	public:
		void AddStop(const std::string& name, const geo::Coordinates& cord);
		const Stop* FindStop(std::string_view name)const;
		const Bus* FindBus(std::string_view name) const;
		void AddBus(std::string_view name, const std::vector<const Stop*>& stops, bool circ);
		const std::unordered_set<std::string_view>* GetBusesForStop(std::string_view stop_name) const;
		BusInfo GetBusInfo(std::string_view stop_name) const;

		void SetDistance(const Stop* from, const Stop* to, int distance);
		int GetDistance(const Stop* from, const Stop* to) const;

	private:
		std::deque<Stop> stops_;
		std::deque<Bus> buses_;

		std::unordered_map<std::string_view, const Stop*> stops_name_;
		std::unordered_map<std::string_view, const Bus*> all_route_;
		std::unordered_map<std::string_view, std::unordered_set<std::string_view>> stop_to_buses_;
		std::unordered_map<std::pair<const Stop*, const Stop*>, int, PairHasher> distances_;

	};
}

