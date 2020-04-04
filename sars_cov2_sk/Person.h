#ifndef PERSON_H
#define PERSON_H

#include<vector>

namespace sars_cov2_sk	{
    enum seir_status    {enum_susceptible, enum_exposed, enum_infective, enum_immune, enum_dead};
    enum symptoms       {enum_asymptomatic, enum_mild_symptons, enum_serious_symptoms, enum_critical};
	class Person	{
		private:
            // five possible values: enum_susceptible, enum_exposed, enum_infective, enum_immune, enum_dead;
            seir_status m_seir_status;

            int     m_age_category;

            float   m_health_state;   // from zero to one. Higher = more healthy
            bool    m_has_symptoms;
            bool    m_in_quarantine;
            bool    m_needs_hospitalization;
            bool    m_is_hospitalized;
            bool    m_had_positive_test;
            int     m_day_of_infection; // the day when the person got infected
            std::vector<sars_cov2_sk::Person *>   m_list_of_contacts; // list of met people since got infected

            int     m_infective_period;

            static int s_day_index;

		public:
            Person();

            // Infect the person if not immune or ill. If immune or ill, do nothing.
            void Infect();

            void AddContact(sars_cov2_sk::Person *person);

            // Simulate evolution of healt state in a day (appearance of symptoms, need (not) to be hoslitalized)
            // Does not include interaction with other people
            void Evolve();

            void Heal();

            void Kill();

            void PutToQuarantine();

            void ReleseFromToQuarantine();

            void Hospitalize();

            void SetTestResult(bool test_result)    {m_had_positive_test = test_result;};

            // Variable characteristic for the person. It's from interval (0,1), where higher value means better health condition.
            // Those with low health_state needs to be hospitalized and some of them die
            float HealthState()     const   {return m_health_state;};

            bool IsIll()            const   {return m_seir_status == enum_exposed || m_seir_status == enum_infective;};
            bool IsUnaffected()     const   {return m_seir_status == enum_susceptible;};
            bool HasSymptoms()      const   {return m_has_symptoms;};
            bool IsImmune()         const   {return m_seir_status == enum_immune;};
            bool IsInfective()      const   {return m_seir_status == enum_infective;};
            bool InQuarantine()     const   {return m_in_quarantine;};
            bool NeedsHospitalization() const   {return m_needs_hospitalization;};
            bool IsHospitalized()       const   {return m_is_hospitalized;};
            bool IsDead()           const   {return m_seir_status == enum_dead;};
            bool IsNewCase()        const;
            bool PositivelyTesed()  const {return m_had_positive_test;};
            const std::vector<sars_cov2_sk::Person *> *GetListOfContacts() const {return  &m_list_of_contacts;};

            void ForgetContacts();

            // Temporary solution (will be move to a separate singleton class).
            // The class person needs to know the date in order to evolve status of people (hospitalization, appearance of symptoms ...)
            static void SetDay(int day);

            // Simulate meeting of two people with a given transmission rate of virus.
            // But we do not remember all people we met, so only a random part of these meetings is saved for later tracking
            static void Meet(   sars_cov2_sk::Person *person1, sars_cov2_sk::Person *person2, 
                                float transmission_probability, float probability_to_remember);

            static int GetNumberOfInfectedPersonsInPopulation(const std::vector<sars_cov2_sk::Person *> &population);

            static int GetNumberOfInfectedPersonsInPopulation(const std::vector<sars_cov2_sk::Person> &population);

            static int GetDay() {return s_day_index;};

            // Generates random integers with the same distribution as the age categories distribution in Slovakia
            static int GenerateRandomAgeCategory();
	};
}

#endif
