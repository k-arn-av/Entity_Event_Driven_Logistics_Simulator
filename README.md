# C++ Event-Driven Entity Simulator

An object-oriented, event-driven C++23 simulation engine designed to model and manage industrial automated floor devices—such as autonomous robots, storage warehouses, package processing hubs, and service stations—working synchronously under a centralized tick-based management framework.

## System Architecture

```text
                         +------------------------+
                         |        main.cpp        |
                         +-----------+------------+
                                     |
                         +------------------------+
                         | entity_factorymanager  |
                         +-----------+------------+
                                     |
                         +-----------v------------+
                         |     Factoryentity      |  <-- Base Class (Abstract)
                         +-----------+------------+
                                     |
    +-----------------+--------------+---------------+-------------------+
    |                 |                              |                   |
+---v------+   +------v-------+              +-------v------+   +--------v-------+
| autobots |   |  warehouse   |              |processinghub |   | serviceStation |
+----------+   +--------------+              +--------------+   +--------+-------+
                                                                         |
                                                              +----------+----------+
                                                              |                     |
                                                     +--------v--------+   +--------v--------+
                                                     | chargingStation |   |maintenanceStation|
                                                     +-----------------+   +-----------------+
