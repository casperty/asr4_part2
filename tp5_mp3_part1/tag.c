#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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


