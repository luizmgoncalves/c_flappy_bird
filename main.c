#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define DISPLAY_WIDTH 900
#define DISPLAY_HEIGHT 400

#define BACKGROUND_COLOR al_map_rgba(0, 0, 100, 255)
#define PIPE_COLOR al_map_rgba(255, 255, 0, 255)
#define LETTER_COLOR al_map_rgba(255, 100, 0, 255)
#define PLAYER_COLOR al_map_rgba(255, 255, 255, 255)

#define PIPE_WIDTH 20
#define PIPE_GAP 80
#define PIPE_VEL 1.5

#define PIPES_NUMBER 7

typedef struct {
	float x, y, r, velx, vely;
	int pontuation;
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
		
		if (player->x > pipe->x && !pipe->passed){
			pipe->passed = 1;
			player->pontuation++;
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
	al_draw_filled_rectangle(pipe->x, 0, pipe->x+PIPE_WIDTH, pipe->y1, PIPE_COLOR);
	
	al_draw_filled_rectangle(pipe->x, pipe->y1+PIPE_GAP, pipe->x+PIPE_WIDTH, DISPLAY_HEIGHT, PIPE_COLOR);
}

void update_player(Character *player){
	player->vely +=0.3;
		
	player->y += player->vely;
}

int main(){
	al_init();
	al_install_keyboard();
	al_install_mouse();
	al_init_primitives_addon();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	
	ALLEGRO_FONT *font = al_load_ttf_font("ZacharyBinx.ttf", 20, 0);
	
	ALLEGRO_DISPLAY *display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);


	Character player;
	Pipe pipes[PIPES_NUMBER];
	int running, max_pontuation=0;

	
	
	ALLEGRO_EVENT_QUEUE *mouse_event_queue;
	mouse_event_queue = al_create_event_queue();

	ALLEGRO_EVENT_SOURCE *mouse_source;
	mouse_source = al_get_mouse_event_source();

	al_register_event_source(mouse_event_queue, mouse_source);
	
	
	ALLEGRO_EVENT_QUEUE *display_event_queue;
	display_event_queue = al_create_event_queue();
	al_register_event_source(display_event_queue, al_get_display_event_source(display));
	
	ALLEGRO_EVENT mouse_event;
	ALLEGRO_EVENT display_event;
	
	al_clear_to_color(BACKGROUND_COLOR);
	al_draw_text(font, LETTER_COLOR, DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, ALLEGRO_ALIGN_CENTRE, "Aperte o botao direito do mouse para iniciar o jogo.");
	
	al_flip_display();
	
	for (running=1;running;){
		for (;!al_is_event_queue_empty(mouse_event_queue);){
			al_get_next_event(mouse_event_queue, &mouse_event);
			if (mouse_event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && mouse_event.mouse.button == 2){
				running = 0;
			}
		}
		
		for (;!al_is_event_queue_empty(display_event_queue);){
			al_get_next_event(display_event_queue, &display_event);
			if (display_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				return 0;
		}
		al_rest(0.01);
	}

	
	
	main_loop:;
	
		player.x=DISPLAY_WIDTH/2; player.y=DISPLAY_HEIGHT/2; player.r=6; player.velx=0;player.vely=0; 
		player.pontuation = 0;
		
		for (int i=0; i<PIPES_NUMBER; i++){
			generate_pipe_x(&pipes[i], i);
			generate_pipe_y(&pipes[i]);
			pipes[i].passed = 0;
			if (i==PIPES_NUMBER-1) last_pipe = &pipes[i];
		}
		
		for (running=1;running;){
			for (;!al_is_event_queue_empty(mouse_event_queue);){
				al_get_next_event(mouse_event_queue, &mouse_event);
				
				if (mouse_event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && mouse_event.mouse.button == 1){
					player.vely = -5;
				}
			}
			
			for (;!al_is_event_queue_empty(display_event_queue);){
				al_get_next_event(display_event_queue, &display_event);
				if (display_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
					return 0;
			}
			
			al_clear_to_color(BACKGROUND_COLOR);
			
			al_draw_textf(font, LETTER_COLOR, DISPLAY_WIDTH/2-20, 5, ALLEGRO_ALIGN_CENTRE, "%d", player.pontuation);
			
			update_player(&player);
			update_pipes(&pipes[0], &player);
			
			for (int i=0; i<PIPES_NUMBER; i++){
				draw_pipe(&pipes[i]);
			}
			
			
			if(colide(&pipes[0], &player) || player.y < 0 || player.y > DISPLAY_HEIGHT){
				running = 0;
			}
			
			al_draw_filled_circle((int)player.x, (int)player.y, player.r, PLAYER_COLOR);
			al_flip_display();
			al_rest(0.01);
		}
		
		if (player.pontuation > max_pontuation)
			max_pontuation = player.pontuation;
		
		al_clear_to_color(BACKGROUND_COLOR);
		al_draw_multiline_textf(font, LETTER_COLOR, DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2-20*4, 500, 20, ALLEGRO_ALIGN_CENTRE, "A sua pontuacao final eh de %d pontos.\n\nA pontuacao maxima alcancada foi %d.\n\nClique com o botao direito do mouse para iniciar o jogo novamente.", player.pontuation, max_pontuation);
		
		al_flip_display();
		
		for (running=1;running;){
			for (;!al_is_event_queue_empty(mouse_event_queue);){
				al_get_next_event(mouse_event_queue, &mouse_event);
				if (mouse_event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && mouse_event.mouse.button == 2){
					goto main_loop;
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
