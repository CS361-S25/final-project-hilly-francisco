folder = ''

treatment_postfixes = ['0.0', '0.5', '1.0']
reps = range(10, 21)
header = "uid treatment rep update coop count hist_0\n"

outputFileName = "munged_basic.dat"

outFile = open(outputFileName, 'w')
outFile.write(header)

for t in treatment_postfixes:
    for r in reps:
        fname = folder +"Org_Vals" + str(r) + "SP" + t + ".data"
        uid = t + "_" + str(r)
        curFile = open(fname, 'r')
        for line in curFile:
            if (line[0] != "u"):
                splitline = line.split(',')
                outstring1 = "{} {} {} {} {} {} {}\n".format(uid, t, r, splitline[0], splitline[1], splitline[2], \
                splitline[3])
                outFile.write(outstring1)
        curFile.close()
outFile.close()