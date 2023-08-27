# This GNUmakefile creates a program called "desim"
# which contains at least some C++ code.
XPROG           := desim

include GNUmakefile.rules

run::		${XINST}
	$C ${XINST}

# the target I want to repeatedly watch
rep::		${XINST}
	$C ${BDIR}${XPROG} > ${TDIR}${XPROG}.obs.log
	$C diff ${TDIR}${XPROG}.exp.log ${TDIR}${XPROG}.obs.log | tee ${TDIR}${XPROG}.cmp.log
	$E ${XPROG} output looks OK
	$C git status

XOBS		:= ${XPROG:%=${TDIR}%.obs.log}
XEXP		:= ${XPROG:%=${TDIR}%.exp.log}
XCMP		:= ${XPROG:%=${TDIR}%.cmp.log}

${TDIR}%.cmp.log:	${BDIR}%
	$E '$* ...'
	$C ${BDIR}$* > ${TDIR}$*.obs.log
	$Q test -s ${TDIR}$*.exp.log || echo saving output of $* as reference run
	$Q test -s ${TDIR}$*.exp.log || cp ${TDIR}$*.obs.log ${TDIR}$*.exp.log
	$E 'diff ...'
	$C diff ${TDIR}$*.exp.log ${TDIR}$*.obs.log | tee $@

cmp::
	${RF} ${XCMP}
	$M ${XCMP}

clean::
	${RF} ${TDIR}$*.obs.log
	${RF} ${TDIR}$*.cmp.log

format::		${XINST}
	make -k ${XINST} || exit 1
	$E 'clang-format ...'
	$C bin/clang-format.sh ${HSRC} ${CSRC} ${HHSRC} ${CCSRC}

all::		cmp

world::
	$M clean
	$M cmp
