#include <stdlib.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4duw_SDL2.h>
#include "animations.h"
#include "audioHelper.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C. */
static void init(void);
static void quit(void);
static void resize(int w, int h);
static void keydown(int keycode);

/*!\brief tableau contenant les animations sous la forme de timeline,
 * ce tableau se termine toujours par l'élémént {0, NULL, NULL,
 * NULL} */

static GL4DHanime _animations[] = {

//DEBUT

{ 13000, animation_earth4, NULL,NULL },


{ 5000,roger,animation_flash,NULL },
{ 5000, animation_tunnel, NULL,transition_fondu},
{ 3000, mugi_one, NULL,NULL},




// EQUIPAGE CHAPEAU DE PAILLE
{ 3000,luffy_normal, NULL,transition_fondu },
{ 500, gear_second, NULL,NULL },
{ 1000, luffy_attack, animation_flash, transition_fondu },
{ 1000, luffy_shock, animation_flash, transition_fondu },
{ 500, gear_fourth, NULL,NULL },
{ 500, tankman, NULL,NULL },
{ 1500, snakeman, NULL,NULL },

{ 2000,zoro_img,NULL,NULL },
{ 2000, zoro_attack, animation_flash, transition_fondu },
{ 2000, zoro_shock, animation_flash, transition_fondu },

{ 2000, sanji_img, NULL, NULL },
{ 3000, sanji_attack, animation_flash, NULL },

{ 2000, jimbei_img, NULL, NULL },
{ 3000, water, NULL, NULL },



{ 2000, usopp_img, NULL, NULL },
{ 2000, sogeking_img, NULL, NULL },
{ 3000, herbe, animation_flash, NULL },

{ 2000,franky_img,animation_flash,NULL },
{ 1000, animation_earthF, NULL,NULL },

{ 1000, robin_img, NULL, NULL },
{ 1000, brook_img, NULL, NULL },
{ 1000, nami_img, NULL, NULL },
{ 2000, chopper_img, NULL, transition_fondu },


//MARINE 

{ 1500, corsaires_one, NULL, NULL },
{ 1500, ao_img, NULL, NULL},
{ 1500, ak_img, NULL, NULL},
{ 2000, aovsak_img, NULL,NULL },
{ 3000, akainu_img, NULL, NULL},



//AUTRES PIRATES
{ 1000, law_img, NULL,NULL },
{ 3000, animation_earthR, NULL,NULL },
{ 1000, marco_img,NULL,NULL},
{ 1000, dofla_img, NULL, NULL},
{ 5000, animation_rosace, NULL,NULL},


//YONKOUS
{ 750, equipage_bm, NULL,NULL },
{ 1500, bigmom_img, NULL,transition_fondu },
{ 750, equipage_shanks, NULL ,NULL},
{ 1500, shanks_img, NULL ,transition_fondu},
{ 750, equipage_kd, NULL ,NULL},
{ 1500, kaido_img, NULL ,transition_fondu},
{ 750, equipage_bn, NULL,NULL },
{ 1500, barbenoir_img, NULL,transition_fondu },
{ 3000, yonkou_img, NULL, NULL },

//FIN
{ 4000, animation_fin, NULL, transition_fondu },
{ 5000, animation_sign, NULL,transition_fondu},

{ 0, NULL, NULL, NULL } /* Toujours laisser à la fin */
};

/*!\brief dimensions initiales de la fenêtre */
static GLfloat _dim[] = {1280, 583};
//static GLfloat _dim[] = {1506, 753};
/*!\brief fonction principale : initialise la fenêtre, OpenGL, audio
 * et lance la boucle principale (infinie).
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Opening One Piece", 
			 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			 _dim[0], _dim[1],
			 SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(gl4dhDraw);

  ahInitAudio("OnePiece15.mp3");
  gl4duwMainLoop();
  return 0;
}

/*!\brief Cette fonction initialise les paramètres et éléments OpenGL
 * ainsi que les divers données et fonctionnalités liées à la gestion
 * des animations.
 */
static void init(void) {
  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
  gl4dhInit(_animations, _dim[0], _dim[1], animationsInit);
  resize(_dim[0], _dim[1]);
}

/*!\brief paramétre la vue (viewPort) OpenGL en fonction des
 * dimensions de la fenêtre.
 * \param w largeur de la fenêtre.
 * \param w hauteur de la fenêtre.
 */
static void resize(int w, int h) {
  _dim[0] = w; _dim[1] = h;
  glViewport(0, 0, _dim[0], _dim[1]);
}

/*!\brief permet de gérer les évènements clavier-down.
 * \param keycode code de la touche pressée.
 */
static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  default: break;
  }
}

/*!\brief appelée à la sortie du programme (atexit).
 */
static void quit(void) {
  ahClean();
  gl4duClean(GL4DU_ALL);
}
