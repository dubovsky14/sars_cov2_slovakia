import sys
import pickle

address_input_file = sys.argv[1]
address_output_file = sys.argv[2]

matrix = pickle.load( open(address_input_file, "rb" ))


n_municip = len(matrix)
output_file = open(address_output_file, "w")

for i in range(n_municip):
    for j in range(n_municip):
        output_file.write(str(int(matrix[i][j])) + " ; ")
    output_file.write("\n")

output_file.close()
