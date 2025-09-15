//
// Created by mishka on 15.09.2025.
//

#ifndef ARTIFACT_H
#define ARTIFACT_H

#include <iostream>

class Artifact
{
    int id_;
    int cost_;
    std::string name_;
    std::string description_;

public:

    Artifact(const int id, const int cost, const std::string& name, const std::string& description);

    int read_cost() const
    {
        return  cost_;
    }

    int read_id() const
    {
        return id_;
    }

    std::string read_name() const
    {
        return name_;
    }

    std::string read_description() const
    {
        return description_;
    }

};



#endif //ARTIFACT_H
