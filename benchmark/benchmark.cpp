#include "autobots.hpp"
#include "entity_factorymanager.hpp"
#include "processinghub.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using clock_type = std::chrono::steady_clock;

class null_buffer final : public std::streambuf {
protected:
    int_type overflow(int_type character) override {
        return traits_type::not_eof(character);
    }
};

class silence_cout final {
public:
    silence_cout() : previous_(std::cout.rdbuf(&buffer_)) {}
    ~silence_cout() { std::cout.rdbuf(previous_); }

private:
    null_buffer buffer_;
    std::streambuf* previous_;
};

struct options {
    std::size_t entities = 1000;
    std::size_t ticks = 2000;
    std::size_t events = 2000;
    std::size_t warmup_ticks = 100;
    std::size_t samples = 7;
};

std::size_t parse_size(const char* value, const char* name) {
    char* end = nullptr;
    const unsigned long long parsed = std::strtoull(value, &end, 10);
    if (end == value || *end != '\0' || parsed == 0 ||
        parsed > std::numeric_limits<std::size_t>::max()) {
        throw std::invalid_argument(std::string("invalid ") + name + ": " + value);
    }
    return static_cast<std::size_t>(parsed);
}

options parse_options(int argc, char** argv) {
    options result;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        auto read_value = [&](const char* name) {
            if (index + 1 >= argc) {
                throw std::invalid_argument(std::string("missing value for ") + name);
            }
            return parse_size(argv[++index], name);
        };

        if (argument == "--entities") {
            result.entities = read_value("--entities");
        } else if (argument == "--ticks") {
            result.ticks = read_value("--ticks");
        } else if (argument == "--events") {
            result.events = read_value("--events");
        } else if (argument == "--warmup") {
            result.warmup_ticks = read_value("--warmup");
        } else if (argument == "--samples") {
            result.samples = read_value("--samples");
        } else if (argument == "--help") {
            std::cout << "Usage: entity_simulator_benchmark [options]\n"
                         "  --entities N  Autobots created in the run (default: 1000)\n"
                         "  --ticks N     Timed simulation ticks (default: 2000)\n"
                         "  --events N    Timed event broadcasts (default: 2000)\n"
                         "  --warmup N    Untimed warmup ticks (default: 100)\n"
                         "  --samples N   Repeated timing samples (default: 7)\n";
            std::exit(EXIT_SUCCESS);
        } else {
            throw std::invalid_argument("unknown option: " + argument);
        }
    }
    return result;
}

struct run_result {
    double tick_hz;
    double dispatch_us;
    std::size_t registered_entities;
    std::size_t package_count;
};

factorymanager make_manager(const options& configuration) {
    factorymanager manager;
    manager.set_warehouse(std::make_unique<warehouse>("BenchmarkWarehouse", configuration.entities + 1, 1000000));
    manager.create_entity<processinghub>("BenchmarkHub", [](package&) {});

    for (std::size_t index = 0; index < configuration.entities; ++index) {
        manager.create_entity<autobots>(100.0, 100.0, "BenchmarkBot");
    }
    return manager;
}

run_result measure(const options& configuration) {
    silence_cout silence;
    factorymanager manager = make_manager(configuration);
    std::vector<std::unique_ptr<package>> packages;
    packages.reserve(configuration.entities);

    for (std::size_t index = 0; index < configuration.entities; ++index) {
        auto payload = std::make_unique<package>("BenchmarkPackage");
        payload->setDestinationID("BenchmarkHub_1");
        packages.push_back(std::move(payload));
    }
    for (auto& payload : packages) {
        manager.route_package(std::move(payload));
    }

    for (std::size_t index = 0; index < configuration.warmup_ticks; ++index) {
        manager.tick();
    }

    std::vector<double> tick_times;
    std::vector<double> dispatch_times;
    tick_times.reserve(configuration.samples);
    dispatch_times.reserve(configuration.samples);

    for (std::size_t sample = 0; sample < configuration.samples; ++sample) {
        const auto tick_start = clock_type::now();
        for (std::size_t index = 0; index < configuration.ticks; ++index) {
            manager.tick();
        }
        const auto tick_end = clock_type::now();

        const auto dispatch_start = clock_type::now();
        for (std::size_t index = 0; index < configuration.events; ++index) {
            manager.broadcast_event(EntityEvent::CANCEL_TASK);
        }
        const auto dispatch_end = clock_type::now();

        const double tick_seconds = std::chrono::duration<double>(tick_end - tick_start).count();
        const double dispatch_seconds = std::chrono::duration<double>(dispatch_end - dispatch_start).count();
        tick_times.push_back(tick_seconds / static_cast<double>(configuration.ticks));
        dispatch_times.push_back(dispatch_seconds / static_cast<double>(configuration.events));
    }

    const auto middle = configuration.samples / 2;
    std::nth_element(tick_times.begin(), tick_times.begin() + middle, tick_times.end());
    std::nth_element(dispatch_times.begin(), dispatch_times.begin() + middle, dispatch_times.end());

    return {
        1.0 / tick_times[middle],
        dispatch_times[middle] * 1'000'000.0,
        manager.getentitySize(),
        configuration.entities,
    };
}

} // namespace

int main(int argc, char** argv) {
    try {
        const options configuration = parse_options(argc, argv);
        const run_result result = measure(configuration);

        std::cout << "stable=true\n"
                  << "autobots=" << configuration.entities << "\n"
                  << "registered_entities=" << result.registered_entities << "\n"
                  << "packages_created=" << result.package_count << "\n"
                  << "tick_rate_hz=" << result.tick_hz << "\n"
                  << "dispatch_latency_us=" << result.dispatch_us << "\n"
                  << "samples=" << configuration.samples << "\n"
                  << "ticks_per_sample=" << configuration.ticks << "\n"
                  << "events_per_sample=" << configuration.events << "\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "benchmark failed: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}