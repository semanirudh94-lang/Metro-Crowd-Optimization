#include "MetroSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

MetroSystem::MetroSystem() : crowd(stations) {}

// File format, one station per line:  id name capacity currentLoad
// Lines starting with # are ignored.
bool MetroSystem::loadStations(const std::string& file) {
    std::ifstream in(file);
    if (!in) {
        std::cout << "Could not open " << file << "\n";
        return false;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        int id, capacity, load;
        std::string name;
        if (ss >> id >> name >> capacity >> load) {
            stations.emplace_back(id, name, capacity, load);
        }
    }
    return !stations.empty();
}

// File format, one connection per line:  from to time
// Each line is added in both directions.
bool MetroSystem::loadNetwork(const std::string& file) {
    std::ifstream in(file);
    if (!in) {
        std::cout << "Could not open " << file << "\n";
        return false;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        int a, b;
        double t;
        if (ss >> a >> b >> t) {
            edges.push_back({a, b, t});
            edges.push_back({b, a, t});
        }
    }
    return !edges.empty();
}

int MetroSystem::findStationId(const std::string& name) const {
    for (const Station& s : stations) {
        if (s.getName() == name) return s.getId();
    }
    return -1;
}

void MetroSystem::printStations() const {
    crowd.printLoads();
}

void MetroSystem::printRoute(const std::string& title, const RouteResult& r) const {
    std::cout << title << ": ";
    if (!r.found) {
        std::cout << "no route found\n";
        return;
    }
    for (size_t i = 0; i < r.path.size(); i++) {
        std::cout << stations[r.path[i]].getName();
        if (i + 1 < r.path.size()) std::cout << " -> ";
    }
    std::cout << "  (time " << std::fixed << std::setprecision(1) << r.totalTime
              << " min, crowd cost " << r.totalCost << ")\n";
}

void MetroSystem::handlePassenger(const Passenger& p) {
    RouteResult shortest = optimizer.findBestRoute(stations, edges,
                                                   p.getSource(), p.getDestination(), false);
    RouteResult best = optimizer.findBestRoute(stations, edges,
                                               p.getSource(), p.getDestination(), true);

    std::cout << "\n";
    printRoute("Shortest route (ignores crowd)", shortest);
    printRoute("Recommended route (crowd-aware)", best);

    if (!best.found) return;

    crowd.applyRoute(best.path, p.getCount());
    std::cout << "\nUpdated station load after " << p.getCount() << " passengers:\n";
    crowd.printLoads();
    std::cout << "\n";
    crowd.printAlerts();
}
