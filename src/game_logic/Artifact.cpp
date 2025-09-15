//
// Created by mishka on 15.09.2025.
//

#include "Artifact.h"

Artifact::Artifact(const int id, const int cost, const std::string &name, const std::string &description) {
    id_ = id;
    cost_ = cost;
    name_ = name;
    description_ = description;
}
