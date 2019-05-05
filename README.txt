Neil Eichelberger
eichelbe assignment #2

contents:
	oss.c
	user.c
	input.txt
	output.txt
	makefile
	
	notes:
		Git used as version control, use git log to see commit history

		2 processes, user and oss.  oss creates and manages children that exec call to user

		print to output file the child infomation whenever a child is created

		run a clock on the main process oss that acts as event triggers for children

		shared memory is utilized for fork processes and after child exec calls

		print to output file whenever parent terminates
		
		leveraging WNOHANG to recognize child completions

		seconds increase by 1 each iteration

		nano increment established by input file (first line number of file)

		child doesn't start until specified second and nanosecond met

		child doesn't terminate until specified nano (duration + nano clock)

		force terminate all after 10 seconds

	
