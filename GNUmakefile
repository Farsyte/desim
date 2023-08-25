# This GNUmakefile creates a program called "desim"
# which contains at least some C++ code.
XPROG           := desim

include GNUmakefile.rules

run::		${XINST}
	$C ${XINST}

XOBS		:= ${XPROG:%=${TDIR}%.observed}
XEXP		:= ${XPROG:%=${TDIR}%.expected}

${XOBS}:	${XINST}
	$C ${XINST} > ${XOBS}
	$E output saved in ${XOBS}

cmp::		${XOBS}
	$Q test -s ${XEXP} || echo this is now the reference run.
	$Q test -s ${XEXP} || cp ${XOBS} ${XEXP}
	$E comparing with ${XEXP} ...
	$C diff ${XEXP} ${XOBS}
	$E comparing with ${XEXP} ... OK.

clean::					; ${RF} ${XOBS}


format::
	$C bin/clang-format.sh ${HSRC} ${CSRC} ${HHSRC} ${CCSRC}

all::
	$M cmp

world::
	$M format
	$M cmp
