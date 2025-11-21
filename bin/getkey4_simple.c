#include <stdio.h>
#include <gigatron/sys.h>

int main()
{	
	printf("Gigatron Key Test\n");
	
	while(1){
		if(buttonState==0xff) printf("no key is pressed\n");
		else printf("%x\n",buttonState);
	}
	return 0;
}
