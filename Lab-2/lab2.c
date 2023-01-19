#include <stdio.h>
#include <stdlib.h>

extern int var_from_extra;       // ToDo: Replace the <keyword> to import variable from extra.c

char* change_ndx_to_6 = "Nike'sLove";         // TODO: Change NDX from 3 to 6, Hint: think about string literals and pointers 
int static change_bind_to_local = 1;		// TODO: Change BIND from GLOBAL to LOCAL 

int main(){
	int static make_visible = 1; 			// TODO: make it visible in symbol table without moving it to a different line
	printf("\n%d %d %d\n",var_from_extra,make_visible,change_bind_to_local);
	return 0;
}
