/* 
 * extract -- Extrait une des versions du fichier se trouvant dans l'archive.
 *
 * Prieto Pantoja Alexandre (NOMA: 5238-10-00)
 * Thibert Simon (NOMA: 4772-10-00)
 *
 * grp14
 *
 * Note : Les specifications n'ont pas ete recopiees.
 *
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/file.h>
#include "extract.h"

int archive;

void sig_handler(int signum) {
	signal(signum, SIG_IGN);
	printf("\n***END***\n");
	// Fermeture du fichier archive
	if(close(archive)==-1) {
		perror("close");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_FAILURE);
}

void extract_file(int indice, char *archivename) {
	// TO DO
	//archive = open();
}

int main(int argc, char *argv[]) {
	if(argc==4) {
		// Gestion des parametres lors de l'execution du programme
		char *archivename;
		int opt, index, indice;
		while((opt=getopt(argc,argv,"n:"))!=-1) {
			switch(opt) {
				case 'n':
					indice=atoi(optarg);
					break;
				case '?':
					if(optopt=='n')
						printf("Option -%c requires an argument.\n", optopt);
					else if (isprint(optopt))
						printf("Unknown option '-%c'.\n", optopt);
					else
						printf("Unknown option character '\\x%x'.\n", optopt);
					return EXIT_FAILURE;
				default:
					return EXIT_FAILURE;
			}
		}
		for(index=optind; index<argc; index++)
			archivename=argv[index];

		// Traitement des signaux d'arret du programme
		signal(SIGINT, sig_handler);
		signal(SIGHUP, sig_handler);
		signal(SIGQUIT, sig_handler);
		signal(SIGTERM, sig_handler);
		signal(SIGTSTP, sig_handler);	

		// Lancement du programme
		extract_file(indice, archivename);
		
		return EXIT_SUCCESS;
	}
	else {
		printf("Wrong number of arguments\n");
		return EXIT_FAILURE;
	}
}
