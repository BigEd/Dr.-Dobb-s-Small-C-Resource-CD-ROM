#********************************************************
#This is part of the Retargetable Concurrent Small C
#distribution (8051 version)
#Copyright 1997 Andy W. K. Yuen
#********************************************************
#program to create a header which consists of all library
#source files that need to be included for a RCSC program
#this program is intended to be invoked with the command
#line option:
#	 -v path=rcsc_pathname
#
BEGIN{
	file = sprintf("%s\\file.idx", path)
	while ((getline line[++linecnt] < file) > 0) {
		split(line[linecnt], name, ":")
		symbol[linecnt] = name[1]
	}
}

{

if (length($3) != 0) {
	for (i = 1; i <= linecnt; i++) {
		if ($3 == symbol[i]) {
			count = split(line[i], name, ":")
			for (j = 2; j <= count; j++) {
				printf "#include \"%s\\%s\"\n", path, name[j]
				}
			break;
			}
		}
	}
}
