#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "tag.h"

int read_u8(int fd, u8 *val)
{
	int r = read(fd, val,1);
		/* si r egal 1 retourne 1 sinon retourne 0*/
		return r == 1?1:0; 
}

int read_u16(int fd, u16 *val)
{
	int tmp;
	int tmp2;
	int r = read(fd, val,2);
	if(r != 2){
		return 0;
	}
	else{
		tmp = *val & 0xFF;
		tmp2 = *val & 0xFF00;
		*val = (tmp << 8) + (tmp2 >> 8);
	return 1;
	}
}

int read_u32(int fd, u32 *val)
{
	int tmp1, tmp2, tmp3, tmp4;
	int r = read(fd, val,4);
	if(r != 4){
		return 0;
	}
	else{
		tmp1 = *val & 0xFF000000;
		tmp2 = *val & 0xFF0000;
		tmp3 = *val & 0xFF00;
		tmp4 = *val & 0xFF;
		*val = (tmp4 << 24) + (tmp3 << 8) + (tmp2 >> 8) + (tmp1 >>24);
		return 1;
	}
}

u32 convert_size(u32 size){
	int taille = (size & 0x0000007F) + ((size & 0x00007F00)>>1) +((size & 0x007F0000)>>2) + ((size & 0x7F000000)>>3);
	return taille;
}

char * read_string(int fd, char *to, int size, int encoding){
  int res, i, bom;
  u8 carac;
  u16 carac16;
  if(encoding == 0){
  
  if(to == NULL){
    to = (char *)malloc((size)* sizeof(char));
  }
  
  	for(i = 0; i<size; i++){
   	 	res = read_u8(fd, &carac);
    	if(res == 0){
    	  return NULL;
   	  }
    	to[i]=carac;
  	}
  	to[i]='\0';
  }else{
  
  size = size -2;
    if(to == NULL){
      to = (char *)malloc((size)* sizeof(char));

    }
  	if(read_u16(fd, &carac16)){
    	  if(carac16 == 0xFFFE){
    	  	bom = 1;
    	  }else{
    	  	bom = 2;
    	  }
   	}
  	for(i = 0; i<(size)/2; i++){	  
   	  if(bom == 1){
   	  	if(read_u8(fd, &carac) == 0){
   	  		return NULL;
   	  	}
   	  	to[i] = carac;
   	  	if(read_u8(fd, &carac) == 0){
   	  		return NULL;
   	  	}
   	  }else{
   	  	if(read_u8(fd, &carac) == 0){
   	  		return NULL;
   	  	}
				if(read_u8(fd, &carac) == 0){
   	  		return NULL;
   	  	}
				to[i] = carac;
   	  }
  	}
  	to[i]='\0';
  }
  return to;
}

int tag_read_id3_header(int fd, id3v2_header_t *header){
	if(read_string(fd, (*header).ID3, 3, 0)==0)return -1;
	(*header).ID3[3]='\0';
	if(read_u16(fd, &((*header).version))==0)return -1;
	if((*header).version>0x0300)return -1;
	if(read_u8(fd, &((*header).flags))==0)return -1;
	if(read_u32(fd, &((*header).size))==0)return -1;
	return 0;
}

int tag_read_one_frame(int fd, id3v2_frame_t *frame){
	u8 encodage;
	
	if(read_string(fd, (*frame).id, 4, 0) == NULL)return -1;
	frame->id[4]='\0';
	
	
	if(frame->id[0]!='T'|| strcmp(frame->id, "TXXX")==0)return -1;
	if(read_u32(fd, &(frame->size))==0)return -1;
	if(read_u16(fd, &(frame->flags))==0)return -1;
	if(read_u8(fd, &encodage)==0)return -1;

	
	frame->text=read_string(fd, NULL, frame->size-1, encodage);
	if(frame->text==NULL)return -1; 
	return 0;
}
id3v2_frame_t *tag_get_frames(const char *file, int *frame_array_size){
	id3v2_header_t header;
	id3v2_frame_t * tmp;
 	id3v2_frame_t * tabFrame = malloc(sizeof(id3v2_frame_t));
	int fd = open(file, O_RDONLY), test, cpt;
	test = tag_read_id3_header(fd, &header);
  	if(test != 0)return NULL;
  	test=cpt=0;
  	while(test == 0){
  		test = tag_read_one_frame(fd,&tabFrame[cpt]);
  		cpt++;
  		/*allocation*/
		
  		tmp = realloc(&tabFrame[0], (cpt + 1) * sizeof(id3v2_frame_t));
		if(tmp!=NULL) {
			tabFrame=tmp;
		}
		else{
			return NULL;
		}
  		*frame_array_size = cpt;
  	}
 	*frame_array_size = cpt-1;
 	tabFrame = realloc(&tabFrame[0], *frame_array_size * sizeof(id3v2_frame_t));
 	return tabFrame;
}
void tag_free(id3v2_frame_t *frames, int frame_count){
	int i;
	for(i = 0 ; i<frame_count ; i++){
  		free(frames[i].text);
	}
  free(frames);
}

const char *get_file_extension(const char *file){
	char *p="";
	int i, j, taille=0;
	char tmp=file[strlen(file)-1];
	for(i=strlen(file)-1;(tmp!='.')||(i==0);i--){
		tmp=file[i];
		taille++;	
	}
	i=i+2;
	p=malloc(taille*sizeof(char));
	for(j=0;j<taille-1;j++){
		p[j]=file[i];
		i++;
	}
	p[j]='\0';
	return p;


}

void clean_string(char *s){
	unsigned int i=0;
	s[i]=toupper(s[i]);
	for(i=1;i<strlen(s);i++){
		s[i]=tolower(s[i]);
		if(s[i]=='/')
			s[i]='-';
	}

}

int get_file_info(const char *source_file, sort_info_t *info){
	int i, size=0;
	id3v2_frame_t *frame;
	frame=tag_get_frames(source_file, &size);
	if(frame == NULL)return -1;
	info->artiste="\0";
	info->album="\0";
	info->genre="\0";
	info->titre="\0";
	info->numero="\0";
	for(i=0;i<size;i++){
		if(strcmp(frame[i].id, "TPE1")==0){
			clean_string(info->artiste=frame[i].text);
		} else if(strcmp(frame[i].id, "TALB")==0)
			clean_string(info->album=frame[i].text);
		else if(strcmp(frame[i].id, "TCON")==0)
			clean_string(info->genre=frame[i].text);
		else if(strcmp(frame[i].id, "TIT2")==0) 
			clean_string(info->titre=frame[i].text);
		else if(strcmp(frame[i].id, "TRCK")==0) 
			clean_string(info->numero=frame[i].text);
	}
	return 0;
}

const char *get_artist_folder(char *buffer, int size, const char *root_folder, const sort_info_t *info){
	int taille=strlen(root_folder);
	char *tmp=malloc(2);
	if(*tmp==-1)return NULL;

	if(taille>=size)return NULL;
	strcat(buffer, root_folder);

	taille+=9;
	if(taille>=size)return NULL;
	strcat(buffer, "By Artist");
	
	taille+=2;
	if(taille>=size)return NULL;
	strcat(buffer, "/");
	tmp[0]=(info->artiste)[0];
	tmp[1]='\0';
	strcat(buffer, tmp);

	taille+=1+strlen(info->artiste);
	if(taille>=size)return NULL;
	strcat(buffer, "/");
	strcat(buffer, info->artiste);

	taille+=1+strlen(info->artiste)+3+strlen(info->album)+1;
	if(taille>=size)return NULL;
	strcat(buffer, "/");
	strcat(buffer, info->artiste);
	strcat(buffer, " - ");
	strcat(buffer, info->album);
	strcat(buffer, "/");
	
	/*printf("Artiste_folder : %s\n", buffer);*/

	return buffer;

}

const char *get_genre_folder(char *buffer, int size, const char *root_folder, const sort_info_t *info){
	int taille=strlen(root_folder);
	if(taille>=size)return NULL;
	strcat(buffer, root_folder);

	taille+=8;
	if(taille>=size)return NULL;
	strcat(buffer, "By Genre");
	
	taille+=1+strlen(info->genre);
	if(taille>=size)return NULL;
	strcat(buffer, "/");
	strcat(buffer, info->genre);

	taille+=1+strlen(info->artiste);
	if(taille>=size)return NULL;
	strcat(buffer, "/");
	strcat(buffer, info->artiste);

	taille+=1+strlen(info->artiste)+3+strlen(info->album)+1;
	if(taille>=size)return NULL;
	strcat(buffer, "/");
	strcat(buffer, info->artiste);
	strcat(buffer, " - ");
	strcat(buffer, info->album);
	strcat(buffer, "/");

	/*printf("Genre_folder : %s\n", buffer);*/

	return buffer;
}

int check_and_create_folder(const char *path){
	mkdir(path, 0750);
	return access(path,  F_OK | R_OK | X_OK);
}

int create_tree(const char *fullpath){
	int fini;
	unsigned int i;
	char *rep;
	fini=-1;
	rep=malloc(strlen(fullpath));	
	i=1;
	rep[0]='/';
	while(fini==-1){
		while(i<strlen(fullpath) && fullpath[i]!='/'){
			rep[i]= fullpath[i];
			i++;
		}
		rep[i]= '\0';
		if(check_and_create_folder(rep)==-1)
			return -1;
		rep[i]='/';
		i++;
		if(i>=strlen(fullpath))
			fini=0;
		
	}
	return 0;
}		
			
const char *get_file_name(char *buffer, int size, const sort_info_t *info, const char *ext){
	
	int taille=1+strlen(info->artiste)+3+strlen(info->album)+3+strlen(info->numero)+1+strlen(info->titre)+1+strlen(ext);
	
	if(taille>=size)return NULL;
	sprintf(buffer, "%s%s%s%s%s%s%s%s%s", info->artiste, " - ", info->album, " - ", info->numero, ".", info->titre, ".", ext);
	return buffer;
}

int verif_info(sort_info_t *info){
	if(strcmp(info->artiste,"\0")==0){
		printf("Pas d'artiste\n");
		return -1;
	}
	if(strcmp(info->album,"\0")==0){
		printf("Pas d'album\n");
		return -1;
	}
	if(strcmp(info->numero,"\0")==0){
	printf("Pas de numero\n");
		return -1;
	}
	if(strcmp(info->titre,"\0")==0){
	printf("Pas de titre\n");
		return -1;
	}
	if(strcmp(info->genre,"\0")==0){
	printf("Pas de genre\n");
		return -1;
	}
	return 0;
}

int sort_file(const char *root_folder, const char *source_file){
	
	sort_info_t info;
	char *genre_folder;
	char *artist_folder;
	char *tmp_root_folder;
	char *new_file_name;
	const char *new_file_folder;
	
	if((genre_folder=malloc(200))==NULL)return -1;
	if((artist_folder=malloc(200))==NULL)return -1;
	if((tmp_root_folder=malloc(200))==NULL)return -1;
	if((new_file_name=malloc(100))==NULL)return -1;
	if((new_file_folder=malloc(200))==NULL)return -1;
	*tmp_root_folder=*root_folder;
	
	if(get_file_info(source_file, &info)==-1)return -1;
	
	if(verif_info(&info)==-1)return -1;

	get_genre_folder(genre_folder, 200, root_folder, &info);
	create_tree(genre_folder);
	
	*tmp_root_folder=*root_folder;

	get_artist_folder(artist_folder, 200, root_folder, &info);
	create_tree(artist_folder);

	new_file_folder=get_file_name(new_file_name, 100, &info, get_file_extension(source_file));
	
	
	sprintf(tmp_root_folder, source_file);
	sprintf(genre_folder, "%s%s", genre_folder, new_file_folder);
	sprintf(artist_folder, "%s%s", artist_folder, new_file_folder);
	
	if(link(tmp_root_folder,  genre_folder)==-1)return -1;
	if(link(tmp_root_folder, artist_folder)==-1)return -1;
	printf("tmp_root_folder : %s\n", tmp_root_folder);
	if(unlink(tmp_root_folder)==-1)return-1;
	return 0;
}
	
int sort_dir(const char *root_folder, const char *source_dir){
	struct stat buf;
	DIR *dir;
	struct dirent *file;
	char *source_file;
	char *fullpath;
	int cpt=0;
	
	
	if((source_file=malloc(200))==NULL)return -1;
	if((fullpath=malloc(200))==NULL)return -1;
	printf("42\n");
	dir=opendir(source_dir);
	printf("43\n");
	while((file=readdir(dir))!=NULL){
		source_file=file->d_name;
		if(stat(source_file, &buf)==0){
			printf("Mode : %d", buf.st_mode);
			if(S_ISDIR(buf.st_mode)){
				printf("Dossier détecté dans le sort_dir\n");
				sprintf(fullpath, "%s%s%s", root_folder, "/", source_dir);
				sort_dir(fullpath, source_file);
			} else if(S_ISREG(buf.st_mode)){
				sort_file(root_folder, source_file);
				printf("Fichier traité dans le sort_dir : %s\n", source_file);
			} else {
				printf("Ce n'est ni un fichier, ni un dossier\n");
			}		
		} else {
			printf("Problème de fichier : %s\n", source_file);
			perror("");
		}
	}
	return 0;
}urn -1;
	if(link(tmp_root_folder, artist_folder)==-1)return -1;
	printf("tmp_root_folder : %s\n", tmp_root_folder);
	if(unlink(tmp_root_folder)==-1)return-1;
	return 0;
}
