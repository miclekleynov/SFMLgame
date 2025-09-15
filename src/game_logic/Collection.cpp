//
// Created by mishka on 15.09.2025.
//

#include "Collection.h"

Collection::Collection(const std::vector<Artifact> &artifacts) {
    artifacts_ = artifacts;
}

Artifact & Collection::get_artifact_by_id(const int id) {
    for (Artifact& artifact : artifacts_)
    {
        if (artifact.read_id() == id)
        {
            return artifact;
        }
    }
    throw std::runtime_error("Artifact with id " + std::to_string(id) + " not found in collection");
}

void Collection::add_artifact(const Artifact &artifact) {
    artifacts_.push_back(artifact);
}

void Collection::remove_artifact_by_id(const int id) {
    for (auto it = artifacts_.begin(); it != artifacts_.end(); /* пусто */)
    {
        if (it->read_id() == id)
        {
            artifacts_.erase(it);
            return;
        }
        ++it;
    }
}


