#ifndef NETWORK_BASE_H
#define NETWORK_BASE_H

#include<vector>
#include<string>

namespace sars_cov2_sk	{
    class NetworkBase  {
        protected:
            float m_rewiring;
            int   m_connections;

            void Check() const;

        public:
            NetworkBase();

            void SetRewireing(float rewiring)   {m_rewiring = rewiring;};

            void SetConnectionsPerPerson(int connections) {m_connections = connections;};

            virtual std::vector<std::vector<unsigned int>> CreateNetwork(unsigned int persons, bool shuffle = false) const;


    };

    template <class T>
    std::vector<std::vector <T>> CreateNetwork(const std::vector<T> &input_vector, const std::vector<std::vector<unsigned int>> &network) {
        if (input_vector.size() != network.size())  {
            throw "Inconsistent inputs in CreateNetwork function!";
        }
        std::vector<std::vector <T>> result;
        for (unsigned int person = 0; person < network.size(); person++)    {
            std::vector<T> contacts;
            for (unsigned int connection : network.at(person))  {
                contacts.push_back(input_vector.at(connection));
            }
            result.push_back(contacts);
        }
        return std::move(result);
    }
};
#endif
