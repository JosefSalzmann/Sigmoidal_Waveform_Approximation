import subprocess
import os
import argparse

parser = argparse.ArgumentParser(description="")
parser.add_argument("-p", help="path to folder")
parser.add_argument("-c", help="path to circuit")
args = parser.parse_args()

folder_path = args.p
circuit_path = args.c

circuit_dirs = []
target_dir = "/media/sf_SPICE/ISCAS85_Benchmarking/" + folder_path

for (dirpath, dirnames, filenames) in os.walk(target_dir):
    if dirnames[0].__contains__("circuit_"):
        circuit_dirs = dirnames
        break

for dir in circuit_dirs:
    command = "cp " + target_dir + "/" + dir + "/*.csv ./benchmarking/"
    print(command)
    code = subprocess.call(command, shell=True)
    command = "./main -c " + circuit_path + " >> SigmoidSim_log.txt"
    print(command)
    code = subprocess.call(command, shell=True)
    command = "mv output.txt SigmoidSim_prediction.txt"
    print(command)
    code = subprocess.call(command, shell=True)

    command = "cp SigmoidSim_prediction.txt " + \
        target_dir + "/" + dir
    print(command)
    code = subprocess.call(command, shell=True)
    command = "cp SigmoidSim_log.txt " + \
        target_dir + "/" + dir
    print(command)
    code = subprocess.call(command, shell=True)

    command = "rm SigmoidSim_log.txt SigmoidSim_prediction.txt ./benchmarking/*.csv"
    print(command)
    code = subprocess.call(command, shell=True)
