import sys
import matplotlib
from matplotlib import pyplot
import json
import numpy as np

results_file = sys.argv[1]

with open(results_file) as json_file:
    data = json.load(json_file)
    days_array = None

    days_array          = np.asarray(data["global"]["simulation_days"])
    infected_total      = np.asarray(data["global"]["infected"])
    exposed_total       = np.asarray(data["global"]["exposed"])
    symptomatic_total   = np.asarray(data["global"]["symptomatic"])
    asymptomatic_total  = np.asarray(data["global"]["asymptomatic"])
    hospitalized_total  = np.asarray(data["global"]["hospitalized"])
    critical_total      = np.asarray(data["global"]["critical"])
    dead_total          = np.asarray(data["global"]["dead"])
    immune_total        = np.asarray(data["global"]["immune"])
    new_cases_total     = np.asarray(data["global"]["new_cases"])
    unaffected_total    = np.asarray(data["global"]["unaffected"])
    new_cases_total     = np.asarray(data["global"]["new_cases"])

    print("day \t\t#infected\texposed\t\tasymptomatic\tsymptomatic\thosp.\t\tcritical\tdead\t\timmune\t\tsusceptible\tnew_cases")
    for i in range(len(infected_total)):
        print ( str(days_array[i])              + "\t\t" +
                str(infected_total[i])          + "\t\t" +
                str(exposed_total[i])           + "\t\t" +
                str(asymptomatic_total[i])      + "\t\t" +
                str(symptomatic_total[i])       + "\t\t" +
                str(hospitalized_total[i])      + "\t\t" +
                str(critical_total[i])          + "\t\t" +
                str(dead_total[i])              + "\t\t" +
                str(immune_total[i])            + "\t\t" +
                str(unaffected_total[i])        + "\t\t" +
                str(new_cases_total[i])            + "\t\t"
               )


