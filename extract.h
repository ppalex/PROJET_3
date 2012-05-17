/* 
 * extract -- Extrait une des versions du fichier se trouvant dans l'archive.
 *
 * Prieto Pantoja Alexandre (NOMA: 5238-10-00)
 * Thibert Simon (NOMA: 4772-10-00)
 *
 * grp14
 *
 */

#ifndef __EXTRACT_H__
#define __EXTRACT_H__

/**
 * sig_handler - Effectue une tache particuliere des qu'elle recoit un certain signal.
 * @signum: Numero du signal.
 */
void sig_handler(int signum);

/**
 * extract_file - Extrait le fichier d'indice i de l'archive archivename.
 * @i: Indice du fichier a recuperer.
 * @archivename: Nom de l'archive.
 */
void extract_file(int i, char *archivename);

#endif

