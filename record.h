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
 */

#ifndef __RECORD_H__
#define __RECORD_H__

#define TRUE 1
#define FALSE 0

/**
 * sig_handler - Effectue une tache particuliere des qu'elle recoit un certain signal.
 * @signum: Numero du signal.
 */
void sig_handler(int signum);

/**
 * read_file - Lit et renvoie le contenu du fichier a surveiller.
 * @filename: Fichier a surveiller.
 * @return: Contenu de ce fichier.
 */
char* read_file(char *filename);

/**
 * write_file - Ecrit la taille du contenu du fichier suivit du contenu du fichier dans l'archive.
 * @content: Contenu du fichier a surveiller.
 */
void write_file(char *content);

/**
 * delete_older_from_archive - Supprime le fichier le plus ancien de l'archive.
 * @achivename: Nom de l'archive.
 */
void delete_older_from_archive(char *archivename);

/**
 * add_in_archive - Ajoute le fichier filename dans l'archive archivename.
 * @filename: Nom du fichier a ajouter.
 */
void add_in_archive(char *filename);

/**
 * file_is_modified - Regarde si le fichier a ete modifie ou non.
 * @filename: Nom du fichier.
 * @return: 1 si le fichier a ete modifie, 0 si non.
 */
int file_is_modified(char *filename);

/**
 * record_file - Surveille si un fichier a ete modifie et si oui, l'ajoute dans une archive.
 * @delay: Delai de la surveillance du fichier.
 * @number: Nombre de fichiers maximum dans l'archive.
 * @filename: Nom du fichier a surveiller.
 * @archivename: Nom de l'archive.
 */
void record_file(int delay, int number, char *filename, char *archivename);

#endif
