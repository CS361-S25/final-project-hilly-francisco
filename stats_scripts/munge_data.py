folder = ''

reps = range(10, 21)
header = "rep update prey_seen prey_consumed predator_vision predator_vision_height predator_vision_width attack_efficiency\n"

outputFileName = "munged_basic.dat"

outFile = open(outputFileName, 'w')
outFile.write(header)

for r in reps:
        fname = folder +"Org_Vals" + str(r) + "SP" + str(r) + ".data"
        print("Looking for file:", fname)
        curFile = open(fname, 'r')
        for line in curFile:
            if (line[0] != "u"):
                splitline = line.split(',')
                outstring1 = "{} {} {} {} {} {} {} {}\n".format(
                    r,
                    splitline[0],  # update
                    splitline[1],  # prey_seen
                    splitline[2],  # prey_consumed
                    splitline[3],  # predator_vision
                    splitline[4],  # predator_vision_height
                    splitline[5],  # predator_vision_width
                    splitline[6]   # attack efficiency
                )
                outFile.write(outstring1)
        curFile.close()
outFile.close()