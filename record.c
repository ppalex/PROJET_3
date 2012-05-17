/* 
 * record -- Enregistre dans une archive les N dernieres versions d'un fichier. 
 *           Pour cela, record s'executera en tâche de fond et observera toutes 
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
#include "record.h"

int files_in_archive=0;
int last_modification;

int filesize=0;

int a;
int f;

void sig_handler(int signum) {
	signal(signum, SIG_IGN);
	printf("\n***END***\n");
	// Fermeture des fichiers
	if(close(a)==-1) {
		perror("close");
		exit(EXIT_FAILURE);
	}
	if(close(f)==-1) {
		perror("close");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_FAILURE);
}

char* read_file(char *filename) {
	struct stat file_stat;	
	stat(filename, &file_stat);
	filesize = (long)file_stat.st_size; 
	char *buffer;
	buffer = (char*)malloc(sizeof(char)*filesize);
	if(read(f, (void *)buffer, (size_t)filesize)==-1){
		perror("read");
		exit(EXIT_FAILURE);	
	}		
	char *content = buffer;
	free(buffer);
	return content;
}

void write_file(char* content) {
	/*if(write(a, (void *)filesize, sizeof(int))==-1) {
		perror("write size");
		exit(EXIT_FAILURE);	
	}*/
	if(write(a, (void *)content, strlen(content))==-1) {
		perror("write content");
		exit(EXIT_FAILURE);	
	}
	printf("TEST\n");
	
}

void delete_older_from_archive() {
	// TO DO
}

void add_in_archive(char *filename) {
	char *temp = read_file(filename);
	write_file(temp);
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
	/*FILE *test = fopen(archivename, "r");
	if(test!=NULL){
		printf("Archive already exist, please choose another name\n");
		fclose(test);
		exit(EXIT_FAILURE);
	}
	
	else{*/
	int fd;

	fd=open(archivename,O_WRONLY|O_CREAT|O_EXCL|O_TRUNC, S_IRUSR|S_IWUSR);
	if(fd==-1) {
		perror("open archive with rights");
		exit(EXIT_FAILURE);
	}
	close(fd);

	// Boucle infinie qui se lance toutes les "delay" secondes
	while(TRUE) {
		if(file_is_modified(filename)) {
			// Ouverture de l'archive et du fichier a surveiller
			a=open(archivename,O_RDWR);

			f=open(filename, O_RDONLY);
			
			if(a==-1) {
				perror("open archive");
				exit(EXIT_FAILURE);
			}
			if(f==-1) {
				perror("open file");
				exit(EXIT_FAILURE);
			}
			
			// Lock des deux fichiers
			if(flock(a,LOCK_EX)==-1) {
				perror("flock - LOCK_EX");
				exit(EXIT_FAILURE);
			}
			if(flock(f,LOCK_EX)==-1) {
				perror("flock - LOCK_EX");
				exit(EXIT_FAILURE);
			}
			
			if(files_in_archive<=number) {
				// Ajout du fichier dans l'archive
				add_in_archive(filename);
			}
			else {
				// Suppression du premier fichier de l'archive
				delete_older_from_archive();
				// Ajout du fichier dans l'archive
				add_in_archive(filename);
			}
			
			// Liberation des locks
			if (flock(a,LOCK_UN)==-1) {
				perror("flock - LOCK_UN");
				exit(EXIT_FAILURE);
			}
			if (flock(f,LOCK_UN)==-1) {
				perror("flock - LOCK_UN");
				exit(EXIT_FAILURE);
			}
			
			// Fermeture des fichiers
			if(close(a)==-1) {
				perror("close archive");
				exit(EXIT_FAILURE);
			}
			if(close(f)==-1) {
				perror("close file");
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
		if(number==0) // Nombres de fichiers a mettre dans l'archive par défaut = 10
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
