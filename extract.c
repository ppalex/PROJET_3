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

int a;

void sig_handler(int signum) {
	signal(signum, SIG_IGN);
	printf("\nGoodbye\n");
	// Fermeture du fichier archive
	close(a);
	exit(EXIT_FAILURE);
}

void extract_file(int indice, char *archivename) {
	printf("EXTRACT\n");
	int temporary;
	temporary=open("README1", O_WRONLY|O_CREAT|O_EXCL|O_APPEND, S_IRUSR|S_IWUSR);
	if(temporary==-1) {
		perror("create temporary file");
		exit(EXIT_FAILURE);
	}
	
	// Ouverture de l'archive
	a=open(archivename, O_RDONLY);
	if(a==-1) {
		perror("open archive");
		exit(EXIT_FAILURE);
	}
	
	// Lock de l'archive
	if(flock(a,LOCK_EX)==-1) {
		perror("flock - LOCK_EX");
		exit(EXIT_FAILURE);
	}
	
	int i = 0;
	while((err=read(a, (void *)&x, sizeof(int)))>0) {
		if(err==-1) {
			perror("read int");
			exit(EXIT_FAILURE);
		}
		
		char *buffer;
		buffer=(char *)malloc((sizeof(char)*x)-1);
		if(buffer==NULL) {
			free(buffer);
			exit(EXIT_FAILURE);
		}
		if(read(a, (void *)buffer, (size_t)x-1)==-1) {
			perror("read content");
			exit(EXIT_FAILURE);	
		}
		
		if(i==indice) {
			if(write(temporary, (void *)buffer, strlen(buffer))==-1) {
				perror("write content");
				exit(EXIT_FAILURE);	
			}
		}
		free(buffer);
		i++;
	}
	if(i>indice) {
		printf("File with indice %d doesn't exist in archive\n", indice);
		unlink("README1");
		if(close(a)==-1) {
			perror("close archive");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_FAILURE);
	}
	
	// Liberation du lock
	if (flock(a,LOCK_UN)==-1) {
		perror("flock - LOCK_UN");
		exit(EXIT_FAILURE);
	}
	
	// Fermeture des fichiers
	if(close(a)==-1) {
		perror("close archive");
		exit(EXIT_FAILURE);
	}
	if(close(temporary)==-1) {
		perror("close temporary");
		exit(EXIT_FAILURE);
	}
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
