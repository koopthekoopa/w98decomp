ifndef W98_DECOMP_DIR
$(error Error getting decompilation path. Try running "InstallEnvironment.bat")
endif

NAME = autorun
SDK = VC40
OUT = EXE

OPTIMIZE = GLOBAL_SMALL

TYPE = DISC
DISC_LOCATION = cdsample/autorun

LIBRARIES = advapi32.lib gdi32.lib kernel32.lib shell32.lib user32.lib winmm.lib

SOURCES =	autorun.obj \
			utils.obj

include $(W98_DECOMP_TOOLS_DIR)\buildrules


