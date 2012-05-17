/* 
 * record -- Enregistre dans une archive les N dernieres versions d'un fichier. 
 *           Pour cela, record s'executera en t�che de fond et observera toutes 
 *           les t secondes les changements d'etat eventuels d'un fichier. 
 *           Des qu'un changement est detecte par record, le fichier actuel est 
 *           ajoute a l'archive (si l'archive contient deja N versions du fichier, 
 *           la plus ancienne est automatiquement supprimee).
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
#include "record.h"

int files_in_archive=0;
int last_modification;

int filesize=0;

int file;
int archive;

void sig_handler(int signum) {
	signal(signum, SIG_IGN);
	printf("\n***END***\n");
	// Fermeture des fichiers
	if(close(archive)==-1) {
		perror("close");
		exit(EXIT_FAILURE);
	}
	if(close(file)==-1) {
		perror("close");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_FAILURE);
}

char* read_file(char *filename) {
	struct stat file_stat;	
	stat(filename, &file_stat);
	filesize=(long)file_stat.st_size; 
	char *buffer;
	buffer=(char *)malloc(sizeof(char)*filesize);
	read(file, *buffer, (size_t)filesize);
	char *contents = buffer;
	free(buffer);
	return contents;
}

void write_file() {

}

int delete_older_from_archive() {
	// TO DO
}

int add_in_archive() {
	// TO DO
}

int file_is_modified(char *filename) {
	struct stat file_stat;	
	int st=stat(filename, &file_stat);	
	if(st==-1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}

	if(files_in_archive==0) {
		files_in_archive++;
		last_modification=(int)file_stat.st_mtime;
		return TRUE;
	}
	
	else {		
		if((int)file_stat.st_mtime>last_modification) {
			files_in_archive++;
			last_modification=(int)file_stat.st_mtime;
			return TRUE;
		}
	
		else {
			return FALSE;
		}
	}
}

void record_file(int delay, int number, char *filename, char *archivename) {
	FILE *test = fopen(archivename, "r");
	if(test!=NULL){
		printf("Archive already exist, please choose another name");
		fclose(test);
		exit(EXIT_FAILURE);
	}
	
	// Boucle infinie qui se lance toutes les "delay" secondes
	while(TRUE) {
		if(file_is_modified(filename)) {
			// Ouverture de l'archive et du fichier a surveiller
			archive=open(archivename, O_RDWR);
			file=open(filename, O_RDONLY);
			if(archive==-1) {
				perror("open");
				exit(EXIT_FAILURE);
			}
			if(file==-1) {
				perror("open");
				exit(EXIT_FAILURE);
			}
			
			// Lock des deux fichiers
			if(flock(archive,LOCK_EX)==-1) {
				perror("flock - LOCK_EX");
				exit(EXIT_FAILURE);
			}
			if(flock(file,LOCK_EX)==-1) {
				perror("flock - LOCK_EX");
				exit(EXIT_FAILURE);
			}
			
			if(files_in_archive<=number) {
				// Ajout du fichier dans l'archive
				add_in_archive();
			}
			else {
				// Suppression du premier fichier de l'archive
				delete_older_from_archive();
				// Ajout du fichier dans l'archive
				add_in_archive();
			}
			
			// Liberation des locks
			if (flock(archive,LOCK_UN)==-1) {
				perror("flock - LOCK_UN");
				exit(EXIT_FAILURE);
			}
			if (flock(file,LOCK_UN)==-1) {
				perror("flock - LOCK_UN");
				exit(EXIT_FAILURE);
			}
			
			// Fermeture des fichiers
			if(close(archive)==-1) {
				perror("close");
				exit(EXIT_FAILURE);
			}
			if(close(file)==-1) {
				perror("close");
				exit(EXIT_FAILURE);
			}
		}
		sleep(delay);
	}
}


int main(int argc, char *argv[]) {
	if(argc==5 || argc==7) {
		// Gestion des parametres lors de l'execution du programme
		char *filename, *archivename;
		int opt, index, delay=0, number=0;
		while((opt=getopt(argc,argv,"t:n:"))!=-1) {
			switch(opt) {
				case 't':
					delay=atoi(optarg);
					break;
				case 'n':
					number=atoi(optarg);
					break;
				case '?':
					if(optopt=='n')
						printf("Option -%c requires an argument.\n", optopt);
					else if (isprint (optopt))
						printf("Unknown option '-%c'.\n", optopt);
					else
						printf("Unknown option character '\\x%x'.\n", optopt);
					exit(EXIT_FAILURE);
				default:
					exit(EXIT_FAILURE);
			}
		}
		if(delay==0) // Valeur du delay par defaut = 60 secondes
			delay=60;
		if(number==0) // Nombres de fichiers a mettre dans l'archive par d�faut = 10
			number=10;
		for(index=optind; index<argc; index++) {
			filename=argv[index];
			index++;
			archivename=argv[index];
		}

		// Traitement des signaux d'arret du programme
		signal(SIGINT, sig_handler);
		signal(SIGHUP, sig_handler);
		signal(SIGQUIT, sig_handler);
		signal(SIGTERM, sig_handler);
		signal(SIGTSTP, sig_handler);
		
		// Lancement du programme
		record_file(delay, number, filename, archivename);

		return EXIT_SUCCESS;
	}

	else {
		printf("Wrong number of arguments\n");
		return EXIT_FAILURE;
	}
}
