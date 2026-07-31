#include "package.hpp"
#include <random>
#include <iostream>


// random id generator engine 
std::string package::generateID(){

    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static thread_local std::mt19937 engine(std::random_device{}());
    static thread_local std::uniform_int_distribution<std::size_t> dist(0, sizeof(charset) - 2);

    std::string result;
    result.reserve(16);
    for (int i = 0; i < 10; ++i) {
        result += charset[dist(engine)];
    }
    return result;
}

size_t package::calculate_priority(Content_type type){
    switch (type) {
        case Content_type::MEDICINE_OR_CHEMICALS:
            return 5; // Highest priority
        case Content_type::PROCESSED_FOOD:
            return 4;
        case Content_type::PROCESSED_DRINK:
            return 3;
        case Content_type::FLUID_NON_FOOD:
            return 2;
        case Content_type::SOLID_NON_FOOD:
        default:
            return 1; // Lowest priority
        }
}

std::string package::getSummary() const{
    return "[" + package_id + "] " + package_name + " | Weight: " + std::to_string(weight) + "kg";
}





