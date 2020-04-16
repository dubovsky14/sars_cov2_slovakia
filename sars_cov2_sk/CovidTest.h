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
            int m_number_of_tests_used_today;
            int m_number_of_available_tests_daily;

        public:
            CovidTest(float test_reliability);

            void SetNumberOfAvailableTestsDaily(int tests) {m_number_of_available_tests_daily = tests;};

            // true = positive test result, false = negative
            bool Test(Person *person);

            // Test people in input vector. Only symptomatic with health state < fraction will be tested
            int TestPeople(const std::vector<Person *> &persons, float fraction);

            // Test people in input vector. Only symptomatic with health state < fraction will be tested
            int TestPeople(std::vector<Person> *persons, float fraction);

            int TestContactOfPositivesFromYesterday();

            void PutToCarantinePositivelyTestedFromYesterday();
    };
};
#endif