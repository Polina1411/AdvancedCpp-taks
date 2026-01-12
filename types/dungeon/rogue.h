#pragma once
#include <cassert>
#include <vector>
#include <string>
#include <memory>
#include <list>
#include <deque>
#include <map>
#include <set>
#include "dungeon.h"

#include <stdexcept>

Room* FindFinalRoom(Room* starting_room) {
    std::deque<Room*> rooms_to_check;
    std::set<Room*> checked_roooms;
    std::list<std::string> keys;
    std::set<Door*> closed_doors;
    rooms_to_check.push_back(starting_room);
    while (!rooms_to_check.empty()) {
        auto room = rooms_to_check.front();
        rooms_to_check.pop_front();
        if (room->IsFinal()) {
            return room;
        }
        if (checked_roooms.contains(room)) {
            continue;
        }
        for (auto i = 0; i < room->NumKeys(); ++i) {
            auto key = room->GetKey(i);
            keys.push_back(key);
        }
        for (auto i = 0; i < room->NumDoors(); ++i) {
            auto* door = room->GetDoor(i);
            closed_doors.insert(door);
        }
        for (auto door : closed_doors) {
            for (auto i = keys.begin(); i != keys.end(); ++i) {
                auto key = *i;
                if (door->TryOpen(key)) {
                    break;
                }
            }
            if (door->IsOpen()) {
                auto next = door->GoThrough();
                rooms_to_check.push_back(next);
            }
        }
        checked_roooms.insert(room);
    }
    return nullptr;
}
