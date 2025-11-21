#VCPUv5

# ======== ('CODE', 'main', code0)
def code0():
	label('main');
	_PROLOGUE(4,2,0x0); # save=None
	LDWI('.2');STW(R8);
	CALLI('printf');
	LDI(0);
	label('.1');
	_EPILOGUE(4,2,0x0,saveAC=True);
# ======== ('DATA', '.2', code1, 0, 1)
def code1():
	label('.2');
	bytes(72,101,108,108,111,44,32,71);
	bytes(105,103,97,116,114,111,110,33);
	bytes(10,0);
# ======== (epilog)
code=[
	('EXPORT', 'main'),
	('CODE', 'main', code0),
	('IMPORT', 'printf'),
	('DATA', '.2', code1, 0, 1) ]
module(code=code, name='hello.c', cpu=5);

# Local Variables:
# mode: python
# indent-tabs-mode: t
# End:
