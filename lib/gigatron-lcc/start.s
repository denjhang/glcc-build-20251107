#VCPUv5

# ======== ('CODE', '_start', code0)
def code0():
	label('_start');
	_PROLOGUE(4,0,0x0); # save=None
	CALLI('main');
	label('.1');
	_EPILOGUE(4,0,0x0);
# ======== (epilog)
code=[
	('EXPORT', '_start'),
	('CODE', '_start', code0),
	('IMPORT', 'main') ]
module(code=code, name='start.c', cpu=5);

# Local Variables:
# mode: python
# indent-tabs-mode: t
# End:
