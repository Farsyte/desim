# This GNUmakefile creates a program called "desim"
# which contains at least some C++ code.
PROG		:= desim

# Select C compiler and flags
CC		:= gcc
DBG             := -g
OPT             := -O2
STD		:= --std=gnu99
WFLAGS		:= -W -Wall -Wextra -Wpedantic
WFLAGS		+= -Wno-missing-field-initializers

include GNUmakefile.rules

WAITF		:= ${CSRC} ${HSRC} ${INST}
WAITE		:= modify delete

run::		${INST}
	$C ${INST} > ${LOGD}run-${PROG}.log 2> ${LOGD}run-${PROG}.err

TEST_OBS	:= ${PROG:%=${LOGD}run-%.log}
TEST_EXP	:= ${PROG:%=${LOGD}run-%.log.expected}
TEST_CMP	:= ${PROG:%=${LOGD}run-%.log.difference}

${LOGD}run-%.log:	${BDIR}%
	$< > $@ 2>${LOGD}run-$*.err

${LOGD}run-%.log.difference:	${LOGD}run-%.log
	$Q bin/check.sh '${LOGD}run-$*.log.expected' $< $@

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

format::
	$E 'indent ...'
	$C bin/indent.sh ${HSRC} ${CSRC}


world::
	$C $(MAKE) clean
	$C $(MAKE) cmp

logs:
	$C find ${LOGD} -empty -o -iname cc-\*.log                      \
		-exec printf '\n%s:\n\n' \{\} \;                        \
		-exec cat \{\} \;
	$C find ${LOGD} -empty -o -iname ld-\*.log                      \
		-exec printf '\n%s:\n\n' \{\} \;                        \
		-exec cat \{\} \;
	$C find ${LOGD} -empty -o -iname \*.err                         \
		-exec printf '\n%s:\n\n' \{\} \;                        \
		-exec cat \{\} \;
	$C find ${LOGD} -empty                                          \
		-o -iname \*.log.difference                             \
		-exec printf '\n%s:\n\n' \{\} \;                        \
		-exec cat \{\} \;
#	$E other log files:
#	$C find ${LOGD} -empty                                          \
#		-o -iname cc-\*.log                                     \
#		-o -iname ld-\*.log                                     \
#		-o -iname \*.log                                        \
#		-exec printf '\n%s:\n\n' \{\} \;                        \
#		-exec cat \{\} \;
