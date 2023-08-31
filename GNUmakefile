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

run::		${INST}
	$C ${INST}

OBSLOG		:= ${PROG:%=${TDIR}%.obs.log}
EXPLOG		:= ${PROG:%=${TDIR}%.exp.log}
CMPLOG		:= ${PROG:%=${TDIR}%.cmp.log}

${TDIR}%.obs.log:	${BDIR}%
	$< > $@

${TDIR}%.cmp.log:	${TDIR}%.obs.log
	$Q bin/check.sh '${TDIR}$*.exp.log' '${TDIR}$*.obs.log'

# Set up a "make rep" rule that I can run repeatedly in another
# window to monitor that everything is good.

rep::
	$C ${MAKE} cmp
	$C git status

# Set up a "make cmp" rule that always
# runs the program and compares the results.

cmp::
	${RF} '${OBSLOG}' '${CMPLOG}'
	$C ${MAKE} '${OBSLOG}'
	$C ${MAKE} '${CMPLOG}'

# Add a "make cmp" to the bottom of the "make all" list.
all::
	$C ${MAKE} cmp

# Add removal of the output compare logs to the "make clean" list.
clean::
	${RF} '${OBSLOG}' '${CMPLOG}'



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
