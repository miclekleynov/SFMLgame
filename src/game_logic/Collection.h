//
// Created by mishka on 15.09.2025.
//

#ifndef COLLECTION_H
#define COLLECTION_H
#include <vector>
#include "Artifact.h"


class Collection {
private:
    std::vector<Artifact> artifacts_;

public:
    Collection(const std::vector<Artifact>& artifacts);

    Artifact& get_artifact_by_id(const int id);

    void add_artifact(const Artifact& artifact);

    void remove_artifact_by_id(const int id);

    std::vector<Artifact> read_artifacts() const {
        return artifacts_;
    };
};



#endif //COLLECTION_H
