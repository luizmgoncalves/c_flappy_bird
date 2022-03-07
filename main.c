#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#define COLOR_BITS 8
#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 400

#define PIPE_WIDTH 20
#define PIPE_GAP 80
#define PIPE_VEL 1.5

#define PIPES_NUMBER 7

typedef struct {
	float x, y, r, velx, vely, pontuation;
} Character;

typedef struct {
	float x, y1, passed;
} Pipe;

void generate_pipe_y(Pipe *pipe){
	int r = rand()%(DISPLAY_HEIGHT-PIPE_GAP);
	pipe->y1 = r;
}

Pipe *last_pipe;

int colide(Pipe *pipe_i, Character *player){
	Pipe *pipe;
	for (int i=0; i<PIPES_NUMBER; i++){
		pipe = pipe_i+i;
		
		if (player->x + player->r > pipe->x && player->x + player->r < pipe->x+PIPE_WIDTH){
			if(player->y + player->r > pipe->y1+PIPE_GAP || player->y-player->r < pipe->y1){
				return 1;
			}
		}
		
		if (player->x - player->r > pipe->x && player->x - player->r < pipe->x+PIPE_WIDTH){
			if(player->y + player->r > pipe->y1+PIPE_GAP || player->y-player->r < pipe->y1){
				return 1;
			}
		}
	}
	return 0;
}

void update_pipes(Pipe *pipe_i, Character *player){
	Pipe *pipe;
	int rotate = 0;
	
	for (int i=0; i<PIPES_NUMBER; i++){
		pipe = pipe_i+i;
		
		al_draw_filled_rectangle(pipe->x, 0, pipe->x+PIPE_WIDTH, pipe->y1, al_map_rgba(255, 100, 0, 255));
		al_draw_filled_rectangle(pipe->x, pipe->y1+PIPE_GAP, pipe->x+PIPE_WIDTH, DISPLAY_HEIGHT, al_map_rgba(255, 100, 0, 255));
		
		if (player->x > pipe->x && !pipe->passed){
			pipe->passed = 1;
			player->pontuation++;
			printf("%.0f\n", player->pontuation);
		}
		
		if (pipe->x < -PIPE_WIDTH){
			pipe->x = last_pipe->x + (float)DISPLAY_WIDTH/(PIPES_NUMBER-1);
			pipe->passed = 0;
			generate_pipe_y(pipe);
			last_pipe = pipe;
			rotate = 1;
		}
		
		pipe->x -= PIPE_VEL;
	}
}

void generate_pipe_x(Pipe *pipe, int index){
	pipe->x = DISPLAY_WIDTH/3*2+index*(float)DISPLAY_WIDTH/(PIPES_NUMBER-1);
}

void draw_pipe(Pipe *pipe){
	al_draw_filled_rectangle(pipe->x, 0, pipe->x+PIPE_WIDTH, pipe->y1, al_map_rgba(255, 255, 0, 255));
	
	al_draw_filled_rectangle(pipe->x, pipe->y1+PIPE_GAP, pipe->x+PIPE_WIDTH, DISPLAY_HEIGHT, al_map_rgba(255, 255, 0, 255));
}

void update_player(Character *player){
	al_draw_filled_circle((int)player->x, (int)player->y, (int)player->r, al_map_rgba(255, 100, 0, 255));
		
	player->vely +=0.3;
		
	player->y += player->vely;
}

int main(){
	al_init();
	al_install_keyboard();
	al_install_mouse();
	al_init_primitives_addon();
	
	
	ALLEGRO_DISPLAY *display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);


	Character player;
	
	Pipe pipes[PIPES_NUMBER];
	
	int running;

	
	
	ALLEGRO_EVENT_QUEUE *mouse_event_queue;
	mouse_event_queue = al_create_event_queue();

	ALLEGRO_EVENT_SOURCE *mouse_source;
	mouse_source = al_get_mouse_event_source();

	al_register_event_source(mouse_event_queue, mouse_source);
	
	
	ALLEGRO_EVENT_QUEUE *display_event_queue;
	display_event_queue = al_create_event_queue();
	al_register_event_source(display_event_queue, al_get_display_event_source(display));
	
	
	ALLEGRO_EVENT_QUEUE *keyb_event_queue;
	keyb_event_queue = al_create_event_queue();

	ALLEGRO_EVENT_SOURCE *keyb_source;
	keyb_source = al_get_keyboard_event_source();

	al_register_event_source(keyb_event_queue, keyb_source);

	al_clear_to_color(al_map_rgba(255, 100, 0, 255));
	
	ALLEGRO_EVENT mouse_event;
	ALLEGRO_EVENT keyboard_event;
	ALLEGRO_EVENT display_event;
	
	
	inicio:;
	
	player.x=DISPLAY_WIDTH/2; player.y=DISPLAY_HEIGHT/2; player.r=6; player.velx=0;player.vely=0; 
	player.pontuation = 0;
	
	for (int i=0; i<PIPES_NUMBER; i++){
		generate_pipe_x(&pipes[i], i);
		generate_pipe_y(&pipes[i]);
		pipes[i].passed = 0;
		if (i==PIPES_NUMBER-1) last_pipe = &pipes[i];
		al_rest(0.0001);
	}
	
	for (running=1;running;){
		for (;!al_is_event_queue_empty(mouse_event_queue);){
			al_get_next_event(mouse_event_queue, &mouse_event);
			
			if (mouse_event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && mouse_event.mouse.button == 1){
				player.vely = -5;
			}
		}
		
		for (;!al_is_event_queue_empty(keyb_event_queue);){
			al_get_next_event(keyb_event_queue, &keyboard_event);
		
			if (keyboard_event.type == ALLEGRO_EVENT_KEY_DOWN && keyboard_event.keyboard.keycode == ALLEGRO_KEY_Q){
				running=0;
			}
		}
		
		for (;!al_is_event_queue_empty(display_event_queue);){
			al_get_next_event(display_event_queue, &display_event);
			if (display_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				return 0;
		}
		
		update_player(&player);
		update_pipes(&pipes[0], &player);
		
		for (int i=0; i<PIPES_NUMBER; i++){
			draw_pipe(&pipes[i]);
		}
		
		
		if(colide(&pipes[0], &player) || player.y < 0 || player.y > DISPLAY_HEIGHT){
			running = 0;
		}
		
		
		al_draw_filled_circle((int)player.x, (int)player.y, player.r, al_map_rgba(0, 0, 0, 255));
		al_flip_display();
		al_rest(0.01);
	}
	
	
	al_clear_to_color(al_map_rgba(255, 100, 0, 255));
	al_flip_display();
	printf("A sua pontuacao final eh de %.0f pontos.\n", player.pontuation);
	
	for (running=1;running;){
		for (;!al_is_event_queue_empty(mouse_event_queue);){
			al_get_next_event(mouse_event_queue, &mouse_event);
			if (mouse_event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && mouse_event.mouse.button == 2){
				goto inicio;
			}
		}
		
		for (;!al_is_event_queue_empty(keyb_event_queue);){
			al_get_next_event(keyb_event_queue, &keyboard_event);
		
			if (keyboard_event.type == ALLEGRO_EVENT_KEY_DOWN && keyboard_event.keyboard.keycode == ALLEGRO_KEY_R){
				goto inicio;
			}
		}
		
		for (;!al_is_event_queue_empty(display_event_queue);){
			al_get_next_event(display_event_queue, &display_event);
			if (display_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				return 0;
		}
		al_rest(0.1);
	}

	return 0;
}
