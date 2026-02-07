include ./makefile_common

# Project Target
EXE_SBDP-Test       = ./bin/SBDP-Test
DIR_SBDP-Test       = ./SBDP-Test
DEP_SBDP-Test       = 

# Default Target
all: $(EXE_SBDP-Test)

$(EXE_SBDP-Test): $(DEP_SBDP-Test)
	$(MAKE) -C $(DIR_SBDP-Test)

# Clean Rule.
clean:
	$(MAKE) -C $(DIR_SBDP-Test)       clean
