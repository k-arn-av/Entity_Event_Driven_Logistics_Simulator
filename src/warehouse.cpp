#include "warehouse.hpp"
#include "entity_factorymanager.hpp"
#include <print>

std::unique_ptr<package> warehouse::generate_package(const std::string& destination_id) {
    ++generated_package_counter;
    std::string pkg_id = "PKG_" + name + "_" + std::to_string(generated_package_counter);

    std::print("[{}] Generated new package: '{}' -> Destination: '{}'\n", name, pkg_id, destination_id);

    // Assumes package constructor accepts ID and destination
    auto pkg = std::make_unique<package>(pkg_id);
    pkg->setDestinationID(destination_id);
    return pkg;
}

void warehouse::increase_tick(factorymanager* manager, const std::string& default_destination) {
    // 1. Package Generation Step
    if (generator_enabled) {
        ++ticks_since_last_generation;

        if (ticks_since_last_generation >= generation_rate_ticks) {
            ticks_since_last_generation = 0; // Reset counter

            if (!is_full()) {
                auto new_pkg = generate_package(default_destination);

                // If a manager pointer is provided and package has a destination, route directly
                if (manager && !default_destination.empty()) {
                    std::print("[{}] Auto-dispatching generated package '{}' to manager.\n", name, new_pkg->getID());
                    manager->route_package(std::move(new_pkg));
                } else {
                    // Otherwise, store it inside local warehouse inventory
                    receive_package(std::move(new_pkg));
                }
            } else {
                std::print("[{}] Generator skipped: Warehouse capacity reached!\n", name);
            }
        }
    }
}

bool warehouse::receive_package(std::unique_ptr<package> pkg) {
    if (!pkg) {
        std::print("[{}] Error: Attempted to receive null package.\n", name);
        return false;
    }

    if (is_full()) {
        std::print("[{}] Warehouse full! Rejected package: '{}'\n", name, pkg->getID());
        return false;
    }

    std::print("[{}] Stored package: '{}'\n", name, pkg->getID());
    inventory.push_back(std::move(pkg));
    return true;
}

std::unique_ptr<package> warehouse::extract_package(size_t index) {
    if (index >= inventory.size()) {
        std::print("[{}] Error: Invalid inventory index {}\n", name, index);
        return nullptr;
    }

    // Move ownership out of vector and erase the slot
    auto pkg = std::move(inventory[index]);
    inventory.erase(inventory.begin() + index);

    std::print("[{}] Extracted package: '{}'\n", name, pkg->getID());
    return pkg;
}

void warehouse::event(EntityEvent event_type) {
    switch (event_type) {
    case EntityEvent::EMERGENCY_STOP:
        generator_enabled = false;
        std::print("[{}] EMERGENCY STOP! Generator paused and warehouse locked.\n", name);
        break;

        case EntityEvent::PAUSE_WORK:
            generator_enabled = false;
            std::print("[{}] Warehouse operations paused.\n", name);
            break;

        case EntityEvent::RESUME_WORK:
        case EntityEvent::CLEAR_FAULT:
            generator_enabled = true;
            std::print("[{}] Warehouse operations resumed.\n", name);
            break;

        default:
            break;
    }
}