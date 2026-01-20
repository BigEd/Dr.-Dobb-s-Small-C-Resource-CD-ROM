#********************************************************
#This is part of the Retargetable Concurrent Small C
#distribution (8051 version)
#Copyright 1997 Andy W. K. Yuen
#********************************************************
#awk program to output symbol definitions for specifying
#placement of constants in code and xdata segments
#this program is intended to be invoked with the command
#line option:
#	-v file=input_file_name
#
BEGIN { FS = "=" 
	#read in the config file
	while (getline line < file > 0) {
		if (split(line, param, "=") == 2) {
			if (param[1] == "CLITBEG") cstart = param[2]
			else if (param[1] == "DVARBEG") dstart = param[2]
		}
	}
}
{
if (NF == 2) {
	split($1, name, "_")
	#record previous module name
	if (thismod != name[1]) {
		lastmod = thismod
		thismod = name[1]
		}

	#handle literals
	if (name[2] == "litsize") {
		if (litflag) {
			count++
			text[count] = sprintf("%s_lit_at=%s_lit_at+%s_litsize",
				name[1], lastmod, lastmod)
			count++
			text[count] = sprintf("%s_var_at=%s_var_at+%s_litsize",
				name[1], lastmod, lastmod)
			total += $2
			}
		else {
			count++
			text[count] = sprintf("%s_lit_at=%s", name[1], cstart)
			count++
			text[count] = sprintf("%s_var_at=%s", name[1], dstart)
			total += $2
			}
		litflag = 1
	}

	else if (name[2] == "tasksize") {
		if (taskflag) {
			count++
			text[count] = sprintf("%s_task_at=%s_task_at+%s_tasksize", 
				name[1], lastmod, lastmod)
			task_size += $2
			}
		else {
			count++
			text[count] = sprintf("%s_task_at=task_start", 
				name[1])
			task_size += $2
			}
		taskflag = 1
	}
	else if (name[2] == "monsize") {
		if (monflag) {
			count++
			text[count] = sprintf("%s_mon_at=%s_mon_at+%s_monsize", 
				name[1], lastmod, lastmod)
			mon_size += $2
			}
		else {
			count++
			text[count] = sprintf("%s_mon_at=mon_start", 
				name[1])
			mon_size += $2
			}
		monflag = 1
	}
	print $0
}
}

END { 
	printf "cvar_size=%d\n",total
	printf "mon_size=%d\n", mon_size
	printf "task_size=%d\n", task_size
	print "mon_start=CLITBEG+cvar_size"
	print "task_start=mon_start+mon_size"
	if (count != 0) {
		for (i = 1; i <= count; i++ ) print text[i] 
		}
}

