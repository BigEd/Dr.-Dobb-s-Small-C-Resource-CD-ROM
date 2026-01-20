#********************************************************
#This is part of the Retargetable Concurrent Small C
#distribution (8051 version)
#Copyright 1997 Andy W. K. Yuen
#********************************************************
#awk program to group and move all literals and other memory
#initialization assembler directives to the end of program
#
BEGIN { FS = "("}
{
#identify and process PCDOES
if ($1 == "TASKS") {
	taskcount++;
	task[taskcount] = $0
	}
else if ($1 == "MNTRS") {
	moncount++;
	mon[moncount] = $0
	}
else if ($1 == "INTRS") {
	intrcount++;
	intr[intrcount] = $0
	print
	}
else if (dataflag) {
	if ($0 == "BEGINDUMP")
		codeflag = 1
	else if ($0 == "ENDSEG(1)")
		dataflag = 0
	else if ($1 == "SHADOW") {
		codeflag = 0
		sum += substr($2, 1, length($2) - 1)
		}

        if (codeflag) {
		codecount++
		code[codecount] = $0
		}
	else {
		datacount++
		data[datacount] = $0
		}
	}
else if ($0 == "TOSEG(1)") {
	dataflag = 1
	datacount++
	data[datacount] = $0
	}
else if ($0 == "END") 
	endmarker = $0
else {
	if ($1 == "MODULENAME")
		name = substr($2, 1, length($2) - 1)
	print
	}
}

END { 
	printf "%s_litsize=%d\n", name, sum > name
	if (codecount != 0) {
		print "\n;;literals and memory initialization assembler directives\n"
		for (i = 1; i <= datacount; i++ ) print data[i] 
		print "\nBEGINLIT"
		for (i = 1; i <= codecount; i++ ) 
			if (code[i] != "BEGINDUMP") print code[i] 
		print "ENDLIT"
		
	}
	printf "%s_tasksize=%d\n", name, taskcount * 6 > name
	if (taskcount != 0) {
		printf "\nBEGINTASK(%d)\n", taskcount
		for (i = 1; i <= taskcount; i++ ) 
			print task[i]
		print "ENDTASK"
	}
	printf "%s_monsize=%d\n", name, moncount * 3 > name
	if (moncount != 0) {
		printf "\nBEGINMON(%d)\n", moncount
		for (i = 1; i <= moncount; i++ ) 
			print mon[i]
		print "ENDMON"
	}
	if (intrcount != 0) {
		for (i = 1; i <= intrcount; i++) {
		   if ($0 != "") {
			split(intr[i], field, ",")
			num = substr(field[2], 1, index(field[2], ")")-1)
			printf "SETVECTOR(%s_%d, %d)\n", "__intr", num, num
		   }
		}
	}
	print endmarker
}

