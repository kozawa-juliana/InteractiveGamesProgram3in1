#include <string.h>
#include <math.h>
#include <stdio.h>
#include "gui.h"



SDL_Surface *screen;
SDL_Event event;

control_s* last_mouse = NULL;
int mouse_down = 0;

#define LEAF	1
#define WINDOW	2
#define IS_LEAF(control) 	(control->type & LEAF)
#define IS_WINDOW(control) 	(control->type & WINDOW)
#define MIN(x,y)	((x)<(y)?(x):(y))
#define MIN3(x,y,z)	MIN((x),MIN((y),(z)))

/* init screen */
SDL_Surface *create_screen(SDL_Surface *screen, control_s *window)
{
	screen = SDL_SetVideoMode(window->width, window->height, 32, SDL_SWSURFACE);
	return screen;
}

/* init rect */
int init_rect(SDL_Rect *rect, int x, int y, int w, int h)
{
	if(rect != NULL)
	{
		rect->x = x;
		rect->y = y;
		rect->w = w;
		rect->h = h;
		return 0;
	}
	return -1;//init rectangle failed
}

/* return list */
list_s *create_list()
{
	list_s* lst;
	if((lst = (list_s*)calloc(1, sizeof(list_s))) == NULL)
		printf(ALLOC_ERROR);
	return lst;
}

/* add control to list */
int add_to_list(list_s *lst, control_s *ctrl)
{
	node_s  *node;
	/* alloc memory for node */
	node = (node_s *)calloc(1, sizeof(node_s));
	if(node == NULL)
	{
		printf(ALLOC_ERROR);
		return -1;
	}
	/* put ctrl in node->control */
	node->control = ctrl;
	/* add node to list */
	if(lst->last == NULL)
	{
		lst->first = node;
		lst->last = node;
	}
	else
	{
		lst->last->next = node;
		node->prev = lst->last;
		lst->last = node;
	}
	lst->last->next = NULL;
	lst->size++;
	return 0;
}

/**
 * add child to control.
 * ctr can't be a leaf, child can't be window
 * return -1 or -2 on error
 */
int add_child(control_s *ctr, control_s *child)
{
	if(ctr == NULL || child == NULL || IS_LEAF(ctr)|| IS_WINDOW(child))
	{
		/* child can't be a window, ctr can't be leaf or null */
		return -1;
	}
	if(ctr->lst == NULL){
		if((ctr->lst = create_list()) == NULL)
			return -2; /* alloc error */
	}
	if(add_to_list(ctr->lst, child) < 0)
	{
		return -2; /* add to list failed */
	}
	return 0;
}
/**
 * init img_s, return NULL on error
 */
img_s init_image(int x, int y, int w, int h)
{
	img_s image = (img_s)calloc(1, sizeof(img_s));
	if(image == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	if(init_rect(image, x, y, w, h) < 0)
	{
		return NULL;
	}
	return image;
}
/**
 * create image, return NULL on error
 */
image_s create_image(SDL_Surface *source, img_s img_e, img_s img_s){
	image_s image = (image_s)calloc(1, sizeof(image_struct));
	if(image == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	image->image_file = source;
	image->enabled_image = img_e;
	image->selected_image = img_s;
	return image;
}

/**
 * init window. return NULL on error.
 */
control_s *create_window(int w, int h)
{
	control_s *window = (control_s *)calloc(1, sizeof(control_s));
	if(window == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	window->type =WINDOW;
	window->width = w;
	window->height = h;
	return window;
}

/**
 * init panel, return NULL on error
 */
control_s *create_panel(int x, int y, int w, int h, image_s image)
{
	control_s *panel = (control_s *)calloc(1, sizeof(control_s));
	if(panel == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	panel->type = 0;
	panel->offset_x = x;
	panel->offset_y = y;
	panel->width = w;
	panel->height = h;
	panel->image = image;
	return panel;
}

/**
 * create button, return NULL on error
 */
control_s *create_button(int x, int y, int w, int h, image_s image, int (*action)(control_s* control), int id)//SDL_Surface source, img_s en_img, img_s sel_img, int (*action)(int id, control_s* control))
{
	control_s *button;
	button = (control_s*)malloc(sizeof(control_s));
	if(button == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	button->lst = NULL;
	button->type = LEAF;
	button->offset_x = x;
	button->offset_y = y;
	button->width = w;
	button->height = h;
	button->image = image;
	button->id = id;
	button->action = action; /* CB function */
	return button;
}

/**
 * create label, return NULL on error
 */
control_s *create_label(int x, int y, int w, int h, img_s img)
{
	control_s *label = (control_s*)calloc(1, sizeof(control_s));
	if(label == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	label->type = LEAF;
	label->offset_x = x;
	label->offset_y = y;
	label->width = w;
	label->height = h;
	label->image->enabled_image = img;
	return label;
}
/**
 * draw pic, return -1 on error
 */
int apply_surface(int x, int y, image_s pic, SDL_Surface *destination)
{
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	return SDL_BlitSurface(pic->image_file, pic->enabled_image, destination, &offset);
}

/**
 * draw pic for selected pic, return -1 on error
 */
int apply_selected_surface(int x, int y, image_s pic, SDL_Surface *destination)
{
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	return SDL_BlitSurface(pic->image_file, pic->selected_image, destination, &offset);
}

/**
 * draw UI tree, return -1 on error.
 */
int display_tree_recursive(control_s *root, int x , int y, int w, int h, control_s* selected){

	int old_h, old_w;
	if(root == NULL)
		return 0;
	if(!IS_WINDOW(root)){
		if(root->image == NULL){
			printf("ERROR: NULL image\n");
			return -1;
		}
		old_h= root->image->enabled_image->h;
		old_w= root->image->enabled_image->w;
		/* changing size according to parent width and height */
		if(w>=0){
			root->image->enabled_image->w = MIN3(root->image->enabled_image->w,root->width, w- root->offset_x );
			root->image->enabled_image->h = MIN3(root->image->enabled_image->h, root->height, h- root->offset_y );
		}
		/* draw root */
		if(root != selected){
			if(apply_surface(x + root->offset_x, y + root->offset_y, root->image, screen) != 0)
			{
				printf("ERROR: failed to blit image: %s\n", SDL_GetError());
				SDL_FreeSurface(root->image->image_file);
				return -1;
			}
		}else{
			if(apply_selected_surface(x + root->offset_x, y + root->offset_y, root->image, screen) != 0)
			{
				printf("ERROR: failed to blit image: %s\n", SDL_GetError());
				SDL_FreeSurface(root->image->image_file);
				return -1;
			}
		}
		/* return sizes */
		root->image->enabled_image->w = old_w;
		root->image->enabled_image->h = old_h;
	}
	/* if root as no childs */
	if(root->lst == NULL)
	{
		return 0;
	}
	/* call recusively for every child */
	node_s* node= root->lst->first;
	while(node != NULL)
	{
		if(display_tree_recursive(node->control, x+root->offset_x, y+ root->offset_y, root->width, root->height, selected)< 0)
		{
			return -1;
		}
		node = node-> next;
	}
	return 0;
}

/**
 * get window and display it. mark the button the mouse is on it. return -1 on error
 */
int display_tree_and_mark_selected(control_s *root, char * name, control_s * selected ){
	int return_value;
	SDL_WM_SetCaption(name, name);/* set window name */
	/* root must be window */
	if(!IS_WINDOW(root))
		return -1;
	SDL_FreeSurface(screen);
	screen = create_screen(screen, root);
	if (screen == NULL)
	{
		printf("Error in screen initialization\n");
		return -1;
	}
	/* display tree */
	 if((return_value =display_tree_recursive(root, 0, 0,-1, -1, selected))<0)
		 return return_value;
	 /* flip screen */
	 if(SDL_Flip(screen)< 0){
		printf("Error: SDL Flip failed: %s\n", SDL_GetError());
		return -1;
	 }
	 return 0;
}


/**
 * get window and display it. return -1 on error
 */
int display_tree(control_s *root, char * name){
	return display_tree_and_mark_selected(root, name, NULL);
}

/**
 * free node list
 */
int free_list(node_s* node){
	if(node == NULL)
		return 0;
	free_tree(node->control);
	free_list(node->next);
	if(node !=NULL)
		free(node);
	return 0;
}

/**
 * free UI tree
 */
int free_tree(control_s *root){
	if(root == NULL)
	{
		return 0;
	}
	if(root->lst != NULL){
		node_s* node= root->lst->first;
		free_list(node);
		if(root->lst != NULL)
			free(root->lst);
	}
	last_mouse = NULL;
	free(root);
	root = NULL;
	return 0;
}

/**
 * scan tree to find where is the mouse
 * return NULL if mouse is not on leaf.
 */
control_s *scan_tree(control_s *root, int x, int y)
{
	/* root is NULL or the mouse is not on root */
	if(root == NULL|| x<=0 || y<=0 || root->width < x || root->height < y)
	{
		return NULL;
	}
	/* root is leaf */
	if(IS_LEAF(root))
	{
		/* return root */
		return root;
	}
	/* root has no childs */
	if(root->lst == NULL)
	{
		return NULL;
	}
	/* call recursivly for every child */
	node_s *node= root->lst->last;
	control_s *control;
	while(node != NULL)
	{
		if((control = scan_tree(node->control, x- node->control->offset_x, y - node->control->offset_y)) != NULL)
		{
			return control;
		}
		node = node->prev;
	}
	return NULL;
}
/**
 * init SDL
 */
int gui_init(){
	if(SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		printf("ERROR: SDL init failed\n");
		return 1;
	}
	return 0;
}

/**
 * free SDL
 */
int gui_free(){
	SDL_Quit();
	return 0;
}

/**
 * load file
 */
SDL_Surface * load_img(char * name){
	return SDL_LoadBMP(name);
}

/**
 * free image file
 */
void free_img_file(SDL_Surface * image){
	SDL_FreeSurface(image);
}

/**
 * free image_s
 */
void free_image(image_s image){
	if(image == NULL)
		return;
	if(image->enabled_image == image->selected_image && image->selected_image != NULL){
		free(image->enabled_image);
		return;
	}
	if(image->enabled_image != NULL)
		free(image->enabled_image);
	if(image->selected_image != NULL)
		free(image->selected_image);
	free(image);
}

/**
 * wait for event, and take care.
 */
int handle_events(control_s *window, char* name){
	int x = event.motion.x;
	int y = event.motion.y;
	/* wait for event */
	while(SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			return GUI_QUIT;
		}
	}
	control_s *control;
	control = scan_tree(window, x, y);
	/* if mouse down first time, call button func */
	if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && control != NULL && mouse_down == 0){
		mouse_down = 1;
		if(control->action != NULL){
			return control->action(control); 
		}
	/* if mouse up, make mouse down 0 */
	} else if(event.type == SDL_MOUSEBUTTONUP){
		mouse_down = 0;
	} else {
		/* mark selected button */
		if(control != last_mouse){
			last_mouse = control;
			display_tree_and_mark_selected(window, name, last_mouse);
		}
	}
	return 0;
}
