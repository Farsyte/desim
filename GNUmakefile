# This GNUmakefile creates a program called "desim"
# which contains at least some C++ code.
PROG		:= desim

# Select C compiler and flags
CC		:= gcc

# set PRO to -pg for profiling.
PRO             :=

# set COV to -fprofile-arcs -ftest-coverage
# to enable test coverage via gcov
# (NOT TESTED)
COV		:=

# set DBG to -g for debugging.
DBG             := -g

# set OPT to -O0, -O1, -O2, -O3, or -Ofast
# for increasing performace.
OPT             := -O3

STD		:= --std=gnu99
WFLAGS		:= -W -Wall -Wextra -Wpedantic
WFLAGS		+= -Wno-missing-field-initializers

include GNUmakefile.rules

WAITF		:= ${CSRC} ${HSRC} ${INST}
WAITE		:= modify delete

run::		${INST}
	$C ${INST} > ${LOGDIR}run-${PROG}.log 2> ${LOGDIR}run-${PROG}.err

TEST_OBS	:= ${PROG:%=${LOGDIR}run-%.log}
TEST_EXP	:= ${PROG:%=${LOGDIR}run-%.log.expected}
TEST_CMP	:= ${PROG:%=${LOGDIR}run-%.log.difference}

${LOGDIR}run-%.log:	${BINDIR}%
	$< > $@ 2>${LOGDIR}run-$*.err

${LOGDIR}run-%.log.difference:	${LOGDIR}run-%.log
	$Q bin/check.sh '${LOGDIR}run-$*.log.expected' $< $@

# Set up a build rule that I can run in a loop,
# usually with a "make await" between iteraions.

loop::
	$C ${MAKE} cmp

# Set up a "make cmp" rule that always
# runs the program and compares the results.

cmp::
	${RF} '${TEST_OBS}' '${TEST_CMP}'
	$C ${MAKE} '${TEST_OBS}'
	$C ${MAKE} '${TEST_CMP}'

# Add a "make cmp" to the bottom of the "make all" list.
all::
	$C ${MAKE} cmp

# Add removal of the output compare logs to the "make clean" list.
clean::
	${RF} '${TEST_OBS}' '${TEST_CMP}'

# Set up a "make gdb" rule that makes sure
# we are built, and starts GDB.

gdb::		${INST}
	gdb '${INST}'

# Set up a "make format" rule that fixes the
# formatting in all source files.

format::	${INST}
	$E 'indent ...'
	$C bin/indent.sh ${HSRC} ${CSRC} ${BSRC}


world::
	$C $(MAKE) clean
	$C $(MAKE) cmp

logs:
	$C find ${LOGDIR} -empty -o -iname cc-\*.log                      \
		-exec printf '\n%s:\n\n' \{\} \;                        \
		-exec cat \{\} \;
	$C find ${LOGDIR} -empty -o -iname ld-\*.log                      \
		-exec printf '\n%s:\n\n' \{\} \;                        \
		-exec cat \{\} \;
	$C find ${LOGDIR} -empty -o -iname \*.err                         \
		-exec printf '\n%s:\n\n' \{\} \;                        \
		-exec cat \{\} \;
	$C find ${LOGDIR} -empty                                          \
		-o -iname \*.log.difference                             \
		-exec printf '\n%s:\n\n' \{\} \;                        \
		-exec cat \{\} \;
#	$E other log files:
#	$C find ${LOGDIR} -empty                                          \
#		-o -iname cc-\*.log                                     \
#		-o -iname ld-\*.log                                     \
#		-o -iname \*.log                                        \
#		-exec printf '\n%s:\n\n' \{\} \;                        \
#		-exec cat \{\} \;
