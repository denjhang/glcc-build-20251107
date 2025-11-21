
# Compile with:
# $ glink TSTcontrol.s -o TSTcontrol.gt1 --rom=dev7 --map=64k --gt1-exec-address=main --entry=main --frags

def scope():

    def FILL():
        # Simplified FILL implementation
        ST(T2)         # Store target address low byte
        ST(T2+1)       # Store target address high byte
        ST(T3)         # Store fill value
        ST(T3+1)       # Store size
        LDWI(0x8000)   # System call address
        SYS(0x80)      # Execute system call

    def code_main():
        nohop()
        org(0x200)

        bytes(0,48,3,51,12,60,15,63)
        label('main')
        MOVQW(8,R9)
        MOVIW(0x800,T2)
        label('.loop')
        LDXW(R9,v('main')-9);ST(T3)
        LDWI(0x7814);FILL()
        ADDSV(0x14,T2)
        DBNE(R9,'.loop')
        HALT()

    module(name = 'TSTcontrol.s',
           code = [('EXPORT', 'main'),
                   ('CODE', 'main', code_main) ] )

        
scope()


# Local Variables:
# mode: python
# indent-tabs-mode: ()
# End:
