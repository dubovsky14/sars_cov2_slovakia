#include "../sars_cov2_sk/NetworkBase.h"

#include "../sars_cov2_sk/HelperFunctions.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace sars_cov2_sk;

NetworkBase::NetworkBase()      {
    m_rewiring      = -1;
    m_connections   = -1;
}

void NetworkBase::Check() const   {
    if (m_rewiring < 0) {
        throw "Please set rewiring parameter of the network!";
    }
    if (m_connections < 0) {
        throw "Please set number of connetions in the network!";
    }
}

// Agorithm described here:
// https://networkx.github.io/documentation/stable/reference/generated/networkx.generators.random_graphs.watts_strogatz_graph.html
vector< vector<unsigned int>> NetworkBase::CreateNetwork(unsigned int persons) const {
    Check();
    const int connections_temp = m_connections < int(persons) ? m_connections : int(persons);
    vector< vector<unsigned int>> result;
    for (unsigned int i = 0; i < persons; i++)  {
        result.push_back(vector<unsigned int>());
    }

    // Create connections to neighbours
    bool start_from_odd = false;
    int step = 1;
    for (int i_connections = 0; i_connections < connections_temp; i_connections++) {
        for (unsigned int i_person = int(start_from_odd); i_person < result.size(); i_person += 2)  {
            const unsigned int connect_to = int(i_person + step) < connections_temp ? (i_person + step) : (i_person + step - connections_temp);
            result.at(i_person).push_back(connect_to);
        }
        if (start_from_odd) {
            step++;
        }
        start_from_odd = !start_from_odd;
    }

    // Randomly drop m_rewiring part of the connections
    for (unsigned int person = 0; person < result.size(); person++)    {
        for (unsigned int connection = 0; connection < result.at(person).size(); connection++)   {
            if (RandomUniform() < m_rewiring)   {
                unsigned int new_connection = persons*RandomUniform();
                while (new_connection == person || new_connection >= persons)   {
                    new_connection = persons*RandomUniform();
                }
                result.at(person).at(connection) = new_connection;
            }
        }
    }

    return std::move(result);
}