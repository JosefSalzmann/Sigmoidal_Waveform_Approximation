import subprocess
import os
import argparse

parser = argparse.ArgumentParser(description="")
parser.add_argument("-p", help="path to folder")
args = parser.parse_args()

folder_path = args.p

circuit_dirs = []
target_dir = "/media/sf_SPICE/Benchmarking/" + folder_path

for (dirpath, dirnames, filenames) in os.walk(target_dir):
    if dirnames[0].__contains__("circuit_"):
        circuit_dirs = dirnames
        break

for dir in circuit_dirs:
    command = "cp " + target_dir + "/" + dir + "/InA.csv ./benchmarking/InA.csv"
    print(command)
    code = subprocess.call(command, shell=True)
    command = "./main -c benchmarking/benchmarking_circuit.txt"
    print(command)
    code = subprocess.call(command, shell=True)
    f = open("benchmarking/InA.csv", "r")
    ina_csv_string = f.read()
    f.close()
    ina_csv_string = "OA_0\n" + ina_csv_string + "\n\n"
    f = open("tool_prediction.txt", "w+")
    f.write(ina_csv_string)
    f.close()
    command = "cat output.txt >> tool_prediction.txt"
    print(command)
    code = subprocess.call(command, shell=True)
    command = "cp tool_prediction.txt " + \
        target_dir + "/" + dir + "/tool_prediction.txt"
    print(command)
    code = subprocess.call(command, shell=True)
