import sys
import matplotlib
from matplotlib import pyplot
import json
import numpy as np

results_file = sys.argv[1]

with open(results_file) as json_file:
    data = json.load(json_file)
    days_array = None

    infected_total      = np.asarray(data["data"]["infected"], dtype=int)
    #exposed_total       = np.asarray(data["data"]["exposed"], dtype=int)
    #symptomatic_total   = np.asarray(data["data"]["symptomatic"], dtype=int)
    #asymptomatic_total  = np.asarray(data["data"]["asymptomatic"], dtype=int)
    hospitalized_total  = np.asarray(data["data"]["hospitalized"], dtype=int)
    critical_total      = np.asarray(data["data"]["critical_care"], dtype=int)
    dead_total          = np.asarray(data["data"]["dead"], dtype=int)
    immune_total        = np.asarray(data["data"]["immune"], dtype=int)
    new_cases_total     = np.asarray(data["data"]["new_cases"], dtype=int)
    unaffected_total    = np.asarray(data["data"]["susceptible"], dtype=int)

    infected_total      = infected_total    .sum(axis = 1)
    #exposed_total       = exposed_total     .sum(axis = 1)
    #symptomatic_total   = symptomatic_total .sum(axis = 1)
    #asymptomatic_total  = asymptomatic_total.sum(axis = 1)
    hospitalized_total  = hospitalized_total.sum(axis = 1)
    critical_total      = critical_total    .sum(axis = 1)
    dead_total          = dead_total        .sum(axis = 1)
    immune_total        = immune_total      .sum(axis = 1)
    new_cases_total     = new_cases_total   .sum(axis = 1)
    unaffected_total    = unaffected_total  .sum(axis = 1)

    print("day \t\t#infected\thosp.\t\tcritical\tdead\t\timmune\t\tsusceptible\tnew_cases")
    for i in range(len(infected_total)):
        print ( str(i)                          + "\t\t" +
                str(infected_total[i])          + "\t\t" +
                #str(exposed_total[i])           + "\t\t" +
                #str(asymptomatic_total[i])      + "\t\t" +
                #str(symptomatic_total[i])       + "\t\t" +
                str(hospitalized_total[i])      + "\t\t" +
                str(critical_total[i])          + "\t\t" +
                str(dead_total[i])              + "\t\t" +
                str(immune_total[i])            + "\t\t" +
                str(unaffected_total[i])        + "\t\t" +
                str(new_cases_total[i])         + "\t\t"
               )


