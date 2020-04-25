import numpy as np
from matplotlib import pyplot

def F(x):
    """
    Number of people in generation x, if the transmission happens only inside households and there was 1 person in 0th generation
    """
    if (x == 0):
        return 1.
    if (x == 1):
        return 0.7058
    if (x == 2):
        return 0.28423
    if (x == 3):
        return 0.06665
    if (x == 4):
        return 0.00891
    if (x == 5):
        return 0.00062
    if (x == 6):
        return 0.00001
    return 0


def R_eff(R_out):
    """
    Calculate global reproduction number (for combination households + outside) if reproduction number for interaction outside households is R_out
    """
    result = ( R_out**7
            + 7*F(1)*R_out**6
            + (6*F(2) + 15*F(1)**2)*R_out**5
            + (5*F(3) + 20*F(2)*F(1) + 10*F(1)**3)*R_out**4
            + (4*F(4) + 12*F(3)*F(1) + 6*F(2)**2 + 12*F(2)*F(1)**2 + F(1)**4)*R_out**3
            + (3*F(5) + 6*F(4)*F(1) + 6*F(3)*F(2) + 3*F(1)*(F(2)**2) + 3*F(3)*(F(1)**2))*R_out**2
            + (2*F(6) + 2*F(5)*F(1) + 2*F(4)*F(2) + F(3)**2)*R_out
            + F(7) )
    return result**(1/7.)


r_out_values = [0., 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.2, 0.3, 0.31, 0.32, 0.33, 0.34, 0.35, 0.36, 0.37, 0.38, 0.39,  0.4, 0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.62, 1.64, 1.66, 1.68, 1.7, 1.8, 1.9, 2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.1]

r_out_values = np.array(r_out_values)

r_total_naive = np.zeros(len(r_out_values),dtype = float)
r_total_gen8  = np.zeros(len(r_out_values),dtype = float)

print ("R_out\tN_meetings\t\t\tR_eff")
for i in range(len(r_out_values)):
    r_total_naive[i] = (r_out_values[i] + F(1))
    r_total_gen8[i]  = R_eff(r_out_values[i])
    n_meetings = r_out_values[i]/(6.43*0.0165)
    print (str(r_out_values[i]) + "\t" + str(n_meetings) + "\t" + str(r_total_gen8[i]))

pyplot.plot(r_out_values, r_total_naive,     color = 'red' , linewidth=1.0)
pyplot.plot(r_out_values, r_total_gen8,     color = 'blue' , linewidth=1.0)
pyplot.grid(True)
pyplot.text(0.1, 3.5, "$R_{\mathrm{out}}$ + $R_{\mathrm{household}}$", size=15, color = 'red')
pyplot.text(0.1, 3, "proper calculation", size=15, color = 'blue')
pyplot.xlabel('$R_{\mathrm{out}}$',    fontsize=13, horizontalalignment='right', x=1.0)
#pyplot.ylim(0.9,2.5)
pyplot.ylabel('$R_{\mathrm{total}}$', fontsize=13, horizontalalignment='left',  x=1.0)
pyplot.savefig("R_eff_vs_R_out.png")

