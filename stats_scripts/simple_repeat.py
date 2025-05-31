#a script to run several replicates of several treatments locally

directory = "Example/"
seeds = range(10, 21)
start_probs = [0.0, 0.5, 1.0]

import subprocess

def cmd(command):
    '''This wait causes all executions to run in sieries.                          
    For parralelization, remove .wait() and instead delay the                      
    R script calls unitl all neccesary data is created.'''
    return subprocess.Popen(command, shell=True).wait()

def silent_cmd(command):
    '''This wait causes all executions to run in sieries.                          
    For parralelization, remove .wait() and instead delay the                      
    R script calls unitl all neccesary data is created.'''
    return subprocess.Popen(command, shell=True, stdout=subprocess.PIPE).wait()

print("Copying MySettings.cfg to "+directory)
cmd("cp MySettings.cfg "+directory)

for a in seeds:
    for b in start_probs:
        command_str = './plot_lab -SEED '+str(a)+' -START_PROB '+str(b)+' -FILE_PATH '+directory+' -FILE_NAME SP'+str(b)+'.data'
        
        print(command_str)
        cmd(command_str)