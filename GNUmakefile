# This GNUmakefile creates a program called "desim"
# which contains at least some C++ code.
XPROG           := desim

include GNUmakefile.rules

run:		${XINST}
	$C ${XINST}

format:
	bin/clang-format.sh ${HSRC} ${CSRC} ${HHSRC} ${CCSRC}

world::
	$M format
	$M all
