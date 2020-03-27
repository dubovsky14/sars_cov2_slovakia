#include "../sars_cov2_sk/Person.h"
#include <iostream>
#include <vector>
using namespace sars_cov2_sk;
using namespace std;


int main(int argc, char* argv[])    {
    Person::SetDay(0);
    vector<Person> population;
    const int population_size = 5500000;
    for (int i = 0; i < population_size; i++)   {
        if (i % 1000000 == 0)
            cout << i << "//" << population_size << endl;
        population.push_back(Person());
    }

    cout << "Population created";
    cin.get();
}

