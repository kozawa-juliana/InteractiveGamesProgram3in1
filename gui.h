#ifndef _GUI_H
#define _GUI_H
#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "errors.h"

#define GUI_QUIT	-10
typedef SDL_Rect * img_s;

typedef struct image
{
	SDL_Surface * image_file; /* source image file */
	img_s enabled_image; /* take an "enabled" pic from a spritesheet */
	img_s selected_image; /* take an "enabled" pic from a spritesheet */
}image_struct;
typedef image_struct* image_s;

typedef struct list_node
{
	struct list_node *prev; /* prev node in list */
	struct list_node *next; /* next node in list */
	struct control *control; /* info */
}node_s;

typedef struct list
{
	int size;
	node_s *first;
	node_s *last;
}list_s;

typedef struct control control_s;

struct control
{
	int id; /* data to identify a button */
	int type; /* 0- panel, 1- leaf, 2 - window */
	/* size of control */
	int width;
	int height;
	/* place */
	int offset_x;
	int offset_y;
	image_s image; /* image */
	list_s *lst;	/* child list */
	int (*action)(control_s* control); /* CB to call when click */
};


/* add child to control. */
int add_child(control_s *ctr, control_s *child);
/* init img_s, return NULL on error */
img_s init_image(int x, int y, int w, int h);
/*  create image, return NULL on error */
image_s create_image(SDL_Surface* source, img_s img_e, img_s img_s);
/* init window. return NULL on error. */
control_s *create_window(int w, int h);
/* init panel, return NULL on error */
control_s *create_panel(int x, int y, int w, int h, image_s image);
/*  create button, return NULL on error */
control_s *create_button(int x, int y, int w, int h, image_s image, int (*action)(control_s* control), int id);
/* create label, return NULL on error */
control_s *create_label(int x, int y, int w, int h, img_s img);
/* get window and display it. return -1 on error */
int display_tree(control_s *root, char * name);
/* free UI tree */
int free_tree(control_s *root);
/* scan tree to find where is the mouse */
control_s *scan_tree(control_s *root, int x, int y);
/* load file */
SDL_Surface * load_img(char * name);
/* free image file */
void free_img_file(SDL_Surface * image);
/* free image_s */
void free_image(image_s image);
/* init SDL */
int gui_init();
/* free SDL */
int gui_free();
/*  wait for event, and take care. */
int handle_events(control_s *window, char* name);

#endif /* _GUI_H */
