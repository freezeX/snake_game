// TODO: Complete this file
#include "game.h"
#include "screen.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX(x,y) (x>y? x:y)
#define MIN(x,y) (x>y? y:x)
#define LIM(x,y,z) (MAX(y,MIN(x,z)))


struct Points {
  int x;
  int y;
  int dir_x;
  int dir_y;
  struct Points* next;
};

typedef struct Points Points;


struct game_t{
  Points* snake;
  Points* foods;
  Points* obstacles;
  Points* bonus;// two kind of bonus
  int xmax;
  int ymax;
  screen_t * scr;
  int move_x;
  int move_y;
  int curr_score;
  int high_score;
  clock_t start;
};
typedef struct game_t game_t;

//check whether the points have same place
static
bool same_place(Points* cell1, Points* cell2) {
  return cell1->x == cell2->x && cell1->y == cell2->y;
}

//linked list node remove
static 
bool list_remove(Points* elt, Points** list,game_t * g) {
  Points *currP, *prevP;
  prevP = NULL;

  for (currP = *list;
      currP != NULL;
      prevP = currP, currP = currP->next) {
    if (same_place(currP, elt)) {
      if (prevP == NULL) {
        *list = currP->next;
      } else {
        prevP->next = currP->next;
      }
      screen_set(g->scr,currP->x,currP->y,' ');
      free(currP);
      return true;
    }
  }
  return false;
}

//check whether the list contain the points
static 
bool list_contains(Points* cell, Points* list) {
  Points* s = list;
  while (s) {
    if (same_place(s, cell)) {
      return true;
    }
    s = s->next;
  }
  return false;
}

static
Points* create_point(int x, int y) {
  Points* cell = malloc(sizeof(*cell));
  cell->x = x;
  cell->y = y;
  cell->next = NULL;
  return cell;
}

static
Points* create_random_point(int xmax, int ymax) {
  return create_point(rand() % (xmax-2) +1, rand() % (ymax-2)+1);//avoid generate cell on the boundary
}

// create a linked list of two nodes representing the snake
static
Points* create_snake(game_t * g) {
  Points* a = create_point(1,2);
  Points* b = create_point(1,1);
  screen_set(g->scr,a->x,a->y,SCREEN_CHAR_SNAKE);
  screen_set(g->scr,b->x,b->y,SCREEN_CHAR_SNAKE);
  a->next = b;
  return a;
}

static
void add_new_food(game_t* g) {
  Points* new_food;
  do {
    new_food = create_random_point(g->xmax, g->ymax);
  } while(list_contains(new_food, g->foods) || list_contains(new_food, g->snake));
  new_food->next = g->foods;
  g->foods = new_food;
  screen_set(g->scr,new_food->x,new_food->y,'f');
}
static
void add_new_obstacle(game_t* g) {
  Points* new_obstacle;
  do {
    new_obstacle = create_random_point(g->xmax, g->ymax);
  } while(list_contains(new_obstacle, g->obstacles));
  new_obstacle->next = g->obstacles;
  g->obstacles = new_obstacle;
  screen_set(g->scr,new_obstacle->x,new_obstacle->y,'o');
}

static 
void add_bonus(game_t* g){
  Points* new_bonus;
  do {
    new_bonus = create_random_point(g->xmax, g->ymax);
  } while(list_contains(new_bonus, g->bonus));
  new_bonus->next = g->bonus;
  g->bonus = new_bonus;
  screen_set(g->scr,new_bonus->x,new_bonus->y,'b');
}

/// Called to initialize internal game structures.
/// Your game should not use global variables, but should associate all state
/// with the pointer returned by game_init. This pointer will be passed to the
/// opther game functions.
///
/// A pointer to the screen to use for output/input (see screen.h) is passed.
/// The game does not own this pointer, i.e. it should not free it when the
/// game is destroyed.
game_t * game_init (screen_t * scr){

  screen_message(scr,"Snake Game, # reprsents snake, o reprsents obstacles, f reprsents food, b is bonus");
  screen_clear(scr);
  game_t * g = malloc(sizeof(game_t));
  g->scr = scr;
  g->snake = create_snake(g);
  g->xmax = screen_getmaxx(scr);
  g->ymax = screen_getmaxy(scr);
  g->curr_score = 0;
  g->high_score = 0;
  g->foods = NULL;
  g->start = clock();
  for (int i = 0;i<6; i++){
    add_new_food(g);
    add_new_obstacle(g);
    add_bonus(g);
  }
  g->move_x = 0;
  g->move_y = 1;

  return g;
}

static 
void free_point(Points* l){
  if (l->next != NULL){
      free_point(l->next);
  }
  free(l);
}


/// Called when the game is over. Free any internal state here.
void game_done (game_t * g){
  if (g->obstacles) free_point(g->obstacles);
  if (g->foods) free_point(g->foods);
  if (g->snake) free_point(g->snake);
  if (g->bonus) free_point(g->bonus);
  free(g);

  return;
}



/// Called when a key was pressed; hor and ver indicate which key according to
/// the table below:
///  
///   key     hor    ver
///  left     -1     0
///  right     1     0
///  up        0    -1
///  down      0     1
///
/// Return 0 if delay before calling game_tick again should remain unmodified
/// If non-zero, the return
/// value is taken as the delay before game_tick should be called again
/// (ignoring any remaining delay that remained before the key was pressed)
unsigned long game_key(game_t * g, int hor, int ver){
  g->move_x =hor;
  g->move_y =ver;
  return 0;
}


static 
void move_around_food(game_t * g){
  Points* food = g->foods;
  while(food !=NULL){
    int r = rand() %1000;
    if (r<20){//use random to check whether to move
      screen_set(g->scr,food->x,food->y,' ');

      //new position of food
      food->x = food->x + rand() %3 -1;
      food->x = LIM(food->x,1,g->xmax-2);//limit of boudary
      food->y = food->y + rand() %3 -1;
      food->y = LIM(food->y,1,g->ymax-2);

      screen_set(g->scr,food->x,food->y,'f');
    }
    food = food->next;
  }
}

//cut the tail of snake
static
void cut_tail(game_t * g){
  Points* end = g->snake;
  int i=2;
  while(end->next->next) {
      end = end->next;
      i++;
  }
  if (i>=3){
    screen_set(g->scr,end->next->x,end->next->y,' ');
    free(end->next);
    end->next = NULL;
  }
  
  
}

/// Called at regular intervals; The return code should be how long (in ms) 
/// to sleep until this function should be called again, or 0 if the game is
/// over.
unsigned long game_tick (game_t * g){

  //changing speed by check elapsed time
  clock_t curr_time =clock();
  int elap=  ((int) (curr_time - g->start)) / CLOCKS_PER_SEC;
  unsigned long delay = 200- elap*20/10000;
  delay = MIN(delay,100);


  move_around_food(g);
  int new_x = g->snake->x + g->move_x;
  int new_y = g->snake->y + g->move_y;
  Points* new;
  if (new_x <= 0 || new_y <= 0 || new_x >= g->xmax-1 ||new_y >= g->ymax-1) {
      return 0; //run out of boundary
  }else {
      new = create_point(new_x, new_y);
  }

  if (g->snake->next && same_place(new, g->snake->next)) {//backward move 
      new->next = NULL;
      free(new);
      new = create_point(g->snake->x+g->snake->dir_x, g->snake->y + g->snake->dir_y);
      new->dir_x = g->snake->dir_x;
      new->dir_y = g->snake->dir_y;
  }else{
      new->dir_x = g-> move_x;
      new->dir_y = g->move_y;
  }
  if (list_contains(new, g->snake)) {//collision to itself
      return 0;
  }

  if (list_contains(new, g->obstacles)) {//collision to obstacles
      return 0;
  }

  screen_set(g->scr,new->x,new->y,SCREEN_CHAR_SNAKE);

  if (list_contains(new, g->foods)) {
  
      new->next = g->snake;
      g->snake = new;
      list_remove(new, &(g->foods),g);//remove consumed food
      screen_set(g->scr,new->x,new->y,SCREEN_CHAR_SNAKE);
      add_new_food(g);
      g->curr_score+=1;
      screen_update_score(g->scr, g->curr_score, g->high_score);
      return delay;
  }

  if (list_contains(new, g->bonus)) { //meet bonus
      list_remove(new, &(g->bonus),g);
      screen_set(g->scr,new->x,new->y,SCREEN_CHAR_SNAKE);
      cut_tail(g);
      if (rand()%2 ==0){
        for (int k = 0;k<3;k++) add_new_food(g);
      }else{
        cut_tail(g);
      }
  }
  // connect the new head to the rest of the snake;
  new->next = g->snake;
  g->snake = new;

  Points* end = g->snake;
  while(end->next->next) {
      end = end->next;
  }
  screen_set(g->scr,end->next->x,end->next->y,' ');
  free(end->next);
  end->next = NULL;
  
  
  screen_refresh(g->scr);

  return delay;
}

