#ifndef COVIDTEST_H
#define COVIDTEST_H

#include "../sars_cov2_sk/Person.h"

#include<string>
#include<iostream>
#include<fstream>

namespace sars_cov2_sk	{

    class Person;

    class CovidTest  {
        private:
            float m_reliability;

            int m_yesterday;
            int m_today;

            std::vector<sars_cov2_sk::Person *> m_positively_tested_yesterday;
            std::vector<sars_cov2_sk::Person *> m_positively_tested_today;

        public:
            CovidTest(float test_reliability);
            
            bool Test(Person *person) const;

            int TestPeople(const std::vector<Person *> &persons, float fraction);

            int TestPeople(std::vector<Person> *persons, float fraction);

            int TestContactOfPositivesFromYesterday();

            void PutToCarantinePositivelyTestedFromYesterday();
    };
};
#endif