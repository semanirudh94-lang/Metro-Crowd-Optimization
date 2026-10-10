#pragma once
#include <string>
#include <vector>
#include "Common.h"
#include "Station.h"
#include "Passenger.h"
#include "CrowdManager.h"
#include "RouteOptimizer.h"

class MetroSystem {
private:
    std::vector<Station> stations;   // must be declared before crowd
    std::vector<Edge> edges;
    CrowdManager crowd;
    RouteOptimizer optimizer;

    void printRoute(const std::string& title, const RouteResult& r) const;

public:
    MetroSystem();

    bool loadStations(const std::string& file);
    bool loadNetwork(const std::string& file);

    int findStationId(const std::string& name) const;   // -1 if not found
    void printStations() const;
    void handlePassenger(const Passenger& p);
};
