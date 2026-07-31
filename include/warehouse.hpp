#pragma once
#include "package.hpp"
#include "factoryentity.hpp" // Included for EntityEvent enum
#include <string>
#include <vector>
#include <memory>

class factorymanager; // Forward declaration to route packages via manager

class warehouse{
private:
    std::string name;
    size_t storage_capacity;
    std::vector<std::unique_ptr<package>> inventory;

    // Package Generator Attributes
    bool generator_enabled = true;
    size_t generation_rate_ticks = 5; // Generate 1 package every N ticks
    size_t ticks_since_last_generation = 0;
    size_t generated_package_counter = 0;

    // Private helper to generate new packages
    std::unique_ptr<package> generate_package(const std::string& destination_id);

public:
    warehouse(std::string warehouse_name, size_t capacity, size_t gen_rate = 5): name(std::move(warehouse_name)), storage_capacity(capacity), generation_rate_ticks(gen_rate){}

    // Core simulation tick and event handler
    void increase_tick(factorymanager* manager = nullptr, const std::string& default_destination = "");
    void event(EntityEvent event_type);

    // Storage Management
    bool receive_package(std::unique_ptr<package> pkg);
    std::unique_ptr<package> extract_package(size_t index);

    // Generator Configuration
    void set_generator_enabled(bool enable) {generator_enabled = enable;}
    void set_generation_rate(size_t ticks) {generation_rate_ticks = ticks;}

    // Getters & Utility
    const std::string& get_name() const{ return name;}
    size_t get_inventory_count() const{ return inventory.size();}
    size_t get_capacity() const{ return storage_capacity;}
    bool is_full() const {return inventory.size() >= storage_capacity;}
    bool is_empty() const {return inventory.empty();}
};
