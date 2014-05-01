#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"
#include "ligne_commande.h"


int main(int argc, char *argv[]){
	while(1){
		affiche_prompt();
		execute_ligne_commande();	
	}

	return 0;
}
