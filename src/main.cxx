#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/PopulationCenter.h"


#include <iostream>
#include <vector>
using namespace sars_cov2_sk;
using namespace std;


int main(int argc, char* argv[])    {
    Person::SetDay(0);

    vector<unsigned int> cities_number_of_citizens = {700000, 400000, 100000, 80000, 50000};
    int population_size = 0;
    for (unsigned int x : cities_number_of_citizens)    {
        population_size += x;
    }

    vector<Person> population;
    for (int i = 0; i < population_size; i++)   {
        if (i % 1000000 == 0)
            cout << i << "//" << population_size << endl;
        population.push_back(Person());
    }

    vector<PopolationCenter> cities;
    int current_person_index = 0;
    for (unsigned int i = 0; i < cities_number_of_citizens.size(); i++) {
        cities.push_back(PopolationCenter(&population, current_person_index, cities_number_of_citizens.at(i), 3.));
    }

    cout << "Population created";
    cin.get();
}

