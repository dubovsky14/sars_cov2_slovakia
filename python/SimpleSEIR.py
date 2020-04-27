S = 5000000.-100.
E = 100.
I = 0.
R = 0.
D = 0.

total = (S+E+I+R+D)

T_inc = 5.1
T_inf = 6.5

mortality = 0.000

R0 = 6.5*0.0165*18.3

N_day_pieces = 10
dt = 1./N_day_pieces



print("day \t\t#infected\texposed\t\tasymptomatic\tsymptomatic\thosp.\t\tcritical\tdead\t\timmune\t\tsusceptible")
for day in range (0,1000) :
    print (str(int(day)) + "\t\t" + str(int(E+I)) + "\t\t" + str(int(E)) + "\t\t" + str(0) + "\t\t" + str(int(I)) + "\t\t0\t\t0\t\t" + str(int(D)) + "\t\t" + str(int(R))+ "\t\t" + str(int(S)) )

    for i in  range(0, N_day_pieces):
        dSdt = (-R0/T_inf)*I*S/total
        dEdt = (R0/T_inf)*I*S/total - E/T_inc
        dIdt = E/T_inc - I/T_inf
        dRdt = (1-mortality)*I/T_inf
        dDdt = mortality*I/T_inf

        S += dSdt*dt
        E += dEdt*dt
        I += dIdt*dt
        R += dRdt*dt
        D += dDdt*dt

    if (int(E+I) == 0):
        break


