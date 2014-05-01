#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "tag.h"

int main(int argc, char *argv[])
{
	int size, fd, r, i, j, nbArg;
	char * affiche = (char *)malloc(9 * sizeof(char));
  	
  	/*ùconst char *file = "/home/infoens/hauspiem/public/asr4/00000.mp3";*/

	u8 test8;
	u16 test16;
	u32 test32;
	id3v2_header_t header;
	id3v2_frame_t frame;
	id3v2_frame_t * tabFrame;
	
	nbArg=0;

	fd = open("/home/infoens/hauspiem/public/asr4/00000.mp3", O_RDONLY, 0666);
	if(fd == -1){printf("erreur\n");}
	/* lecture d'un entier stocké sur un octet */
	r = read_u8(fd, &test8);
	/*si la lecture s'est bien passee on affiche le resulat*/
	if(r == 1){
		printf("read_u8 : %x\n",test8);
	}else{
		printf("Erreur lecture\n");
	}
	/* lecture d'un entier stocké sur deux octets en format big endian */
	r = read_u16(fd, &test16);
	if(r == 1){
		printf("read_u16 : %x\n", test16);
	}else{
		printf("Erreur lecture\n");
	}
	/* lecture d'un entier stocké sur quatre octets en format big endian */
	r = read_u32(fd, &test32);
	if(r == 1){
		printf("read_u32 : %x\n", test32);
	}else{
		printf("Erreur lecture\n");
	}
	/*  conversion d'un entier de 4 octets (format little endian) en un autre entier dont les bits 7 de chaque
		octet auront été ignorés */
	size = 98689;
	printf("convert_size : %x\n",convert_size(size));
	/* lecture d''une chaîne de caractère de taille size octets depuis le ﬁchier fd selon l’encodage encoding (0 : ascii, 1 : unicode)*/
  	fd = open("muse", O_RDONLY);
	if(fd == -1){
		perror ("/monfichier");
	}
	read_string(fd, affiche, 4, 1);
	printf("read_string : %s\n", affiche);
	fd = open("/home/infoens/hauspiem/public/asr4/00000.mp3", O_RDONLY);
	if(fd == -1){
		perror ("/monfichier");
	}
	r = tag_read_id3_header(fd, &header);
	if(r == 0){
		printf("tag_read_id3_header, Taille : %d\n", header.size);	
	}
	r = tag_read_one_frame(fd,&frame);
	if(r == 0){
		printf("tag_read_one_frame, ID : %s\n", frame.id);
		printf("tag_read_one_frame, Taille : %d\n", frame.size);
		printf("tag_read_one_frame, Text : %s\n", frame.text);
	}
	printf("\n");
	printf("--\n");
	printf("Lecture complete d'une frame : \n");
	if(nbArg<argc){printf("Aucun argument passé en paramètre\n");}
	for(i=1; i<argc; i++){
		printf("Musique : %s\n", argv[i]);
  		tabFrame = tag_get_frames(argv[i], &size);
  		if(tabFrame != NULL){
  			for(j = 0 ; j<size ; j++){
  				printf("%s \n", tabFrame[j].text);
  			}
  		}else{
  			printf("Erreur lecture\n");
  		}
  	}
	return 1;
}

