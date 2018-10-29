/*!\file animations.c
 *
 * \brief Votre espace de liberté : c'est ici que vous pouvez ajouter
 * vos fonctions de transition et d'animation avant de les faire
 * référencées dans le tableau _animations du fichier \ref window.c
 *
 * Des squelettes d'animations et de transitions sont fournis pour
 * comprendre le fonctionnement de la bibliothèque. En bonus des
 * exemples dont un fondu en GLSL.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 05, 2014
 */

#include <GL4D/gl4dh.h>
#include "audioHelper.h"
#include "animations.h"
#include <assert.h>
#include <stdlib.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

/*!\brief identifiant de la géométrie QUAD GL4Dummies */
static GLuint _quadId = 0;

void transition_fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state) {
  /* INITIALISEZ VOS VARIABLES */
  int vp[4], i;
  GLint tId;
  static GLuint tex[2], pId;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE TRANSITION (SES VARIABLES <STATIC>s) */
    glGetIntegerv(GL_VIEWPORT, vp);
    glGenTextures(2, tex);
    for(i = 0; i < 2; i++) {
      glBindTexture(GL_TEXTURE_2D, tex[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp[2], vp[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/mix.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tex[0]) {
      glDeleteTextures(2, tex);
      tex[0] = tex[1] = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR LES DEUX ANIMATIONS EN FONCTION DU SON */
    if(a0) a0(state);
    if(a1) a1(state);
    return;
  default: /* GL4DH_DRAW */
    /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tId);
    /* JOUER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[0],  0);
    if(a0) a0(state);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[1],  0);
    if(a1) a1(state);
    /* MIXER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tId,  0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    if(et / (GLfloat)t > 1) {
      fprintf(stderr, "%d-%d -- %f\n", et, t, et / (GLfloat)t);
      exit(0);
    }
    glUniform1f(glGetUniformLocation(pId, "dt"), et / (GLfloat)t);
    glUniform1i(glGetUniformLocation(pId, "tex0"), 0);
    glUniform1i(glGetUniformLocation(pId, "tex1"), 1);
    gl4dgDraw(_quadId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
  }
}

void animation_flash(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Uint16 * s;
  static GLfloat c[4] = {0, 0, 0, 0};
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Uint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    if(l >= 8)
      for(i = 0; i < 4; i++)
	c[i] = s[i] / ((1 << 16) - 1.0);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glClearColor(c[0], c[1], c[2], c[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
}

void animation_vide(int state) {
  switch(state) {
  case GL4DH_DRAW:
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
  default:
    return;
  }
}

void animation_damier(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static GLuint tId = 0, pId;
  static const GLubyte t[16] = {-1,  0,  0, -1, /* rouge */
				 0, -1,  0, -1, /* vert  */
				 0,  0, -1, -1, /* bleu  */
				-1, -1,  0, -1  /* jaune */ };
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
   glGenTextures(1, &tId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, t);
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tId) {
      glDeleteTextures(1, &tId);
      tId = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
  return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glDisable(GL_DEPTH);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tId);
    glUniform1i(glGetUniformLocation(pId, "myTexture"), 0);
    gl4dgDraw(_quadId);
    return;
  }
}

/*void animation_base(int state) {
  switch(state) {
  case GL4DH_INIT:
    base_init();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: // GL4DH_DRAW 
    base_draw();
    return;
  }
}*/


//PLANETE QUI TOURNE 

void animation_earth4(int state) {
  switch(state) {
  case GL4DH_INIT:
    earth4_init();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    earth4_draw();
    return;
  }
}

// missile 

void animation_earthF(int state) {
  switch(state) {
  case GL4DH_INIT:
    earthF_init();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    earthF_draw();
    return;
  }
}

void animation_earthR(int state) {
  switch(state) {
  case GL4DH_INIT:
    earthR_init();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    earthR_draw();
    return;
  }
}


//tunnel

void animation_tunnel(int state) {
  switch(state) {
  case GL4DH_DRAW:
    dessin_tunnel();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default:
    return;
  }
}

//rosace

void animation_rosace(int state) {
  switch(state) {
  case GL4DH_INIT:
    diagInit();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: 
    dessin_rosace();
    return;
  }
}

//sign

void animation_sign(int state) {
  switch(state) {
  case GL4DH_INIT:
    initSign();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: 
    drawSign();
    return;
  }
}

// missile
/*
void animation_missile(int state) {
  switch(state) {
  case GL4DH_INIT:
    initGL_F();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: 
    draw_F();
    return;
  }
}*/


////////////////////////////////////////////////////////



static void copyFromSurface(SDL_Surface * s, int x0, int y0) {
  Uint32 * p, coul, ocoul;
  Uint8 rmix, gmix, bmix;
  double f1, f2;
  int x, y, maxx, maxy;
  assert(s->format->BytesPerPixel == 4); /* pour simplifier, on ne gère que le format RGBA */
  p = s->pixels;
  maxx = MIN(x0 + s->w, gl4dpGetWidth());
  maxy = MIN(y0 + s->h, gl4dpGetHeight());
  for(y = y0; y < maxy; y++) {
    for(x = x0; x < maxx; x++) {
      ocoul = gl4dpGetPixel(x, y);
      coul = p[(s->h - 1 - (y - y0)) * s->w + x - x0]; /* axe y à l'envers */
      f1 = ALPHA(coul) / 255.0; f2 = 1.0 - f1;
      rmix = f1 *   RED(coul) + f2 *   RED(ocoul);
      gmix = f1 * GREEN(coul) + f2 * GREEN(ocoul);
      bmix = f1 *  BLUE(coul) + f2 *  BLUE(ocoul);
      gl4dpSetColor(RGB(rmix, gmix, bmix));
      gl4dpPutPixel(x, y);
    }
  }
}

////////////////////////////////////////////////////////

//DEBUT OPENING 


void roger(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("roger.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


void mugi_one(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("mugiwara.png");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/(999)
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}



////////////////////////////////////////////////////////

//EQUIPAGE MUGIWARA 

//----------LUFFY----------//

void luffy_normal(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("luffy.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/5000,gl4dpGetHeight()/200
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//luffy gear second 

void gear_second(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("gearsecond.png");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/5000,gl4dpGetHeight()/200
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


void luffy_attack(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("pistol.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite, rand()% gl4dpGetWidth(), rand()% gl4dpGetHeight());
    gl4dpUpdateScreen(NULL);
    return;
  }
}



void luffy_shock(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("jet_rafale.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite, gl4dpGetWidth()/999, gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


//luffy gear fourth 
void gear_fourth(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("gearfourth.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/5000,gl4dpGetHeight()/200
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//luffy tankman

void tankman(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("tankman.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/5000,gl4dpGetHeight()/200
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


//luffy snakeman

void snakeman(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("snakeman.png");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/5000,gl4dpGetHeight()/200
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

/****************************************/

//----------ZORO----------//


void zoro_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("zoro.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


void zoro_attack(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("tornado.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite, rand()% gl4dpGetWidth(), rand()% gl4dpGetHeight());
    gl4dpUpdateScreen(NULL);
    return;
  }
}

void zoro_shock(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("zoro_onigiri.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

/****************************************/

//----------SANJI----------//


void sanji_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("sanji.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/(999)
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}




void sanji_attack(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("fire.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite, rand()% gl4dpGetWidth(), rand()% gl4dpGetHeight());
    gl4dpUpdateScreen(NULL);
    return;
  }
}

/****************************************/

//----------JIMBEI----------//


void jimbei_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("jimbei.png");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/(999)
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


//hydro
void water(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("hydro.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite, rand()% gl4dpGetWidth(), rand()% gl4dpGetHeight());
    gl4dpUpdateScreen(NULL);
    return;
  }
}

/****************************************/

//----------USOPP----------//


//usopp
void usopp_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("usopp.png");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/(999)
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//sogeking
void sogeking_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("sogeking.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/(999)
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


//herbe
void herbe(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("herbe.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite, rand()% gl4dpGetWidth(), rand()% gl4dpGetHeight());
    gl4dpUpdateScreen(NULL);
    return;
  }
}

/****************************************/

//----------FRANKY----------//


void franky_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("franky1.gif");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

/****************************************/

//----------RESTE DE L'EQUIPAGE----------//


//robin

void robin_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("robin.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//brook

void brook_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("brook.png");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


//nami

void nami_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("nami.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


//chopper

void chopper_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("chopper.jpg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

////////////////////////////////////////////////////////

//MARINE


////////////////////////////////////////////////////////


//AUTRES PIRATES


//--------------------LAW--------------------//

void law_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("room.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//ajouter ROOM 
//--------------------MARCO--------------------//

void marco_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("marco.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//--------------------DOFLAMINGO--------------------//

void dofla_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("doflamingo.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

////////////////////////////////////////////////////////

//MARINE

//--------------------CORSAIRES AVANT L'ELLIPSE--------------------//

void corsaires_one(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("corsaires.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


//--------------------AOKIJI--------------------//

void ao_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("aokiji.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//--------------------AKAINU 1--------------------//

void ak_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("akainu.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//--------------------AOKIJI VS AKAINU--------------------//

void aovsak_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("aovsak.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//--------------------AKAINU--------------------//

void akainu_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("akainu.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//--------------------CONSEIL DES 5 ÉTOILES--------------------//

////////////////////////////////////////////////////////

//YONKOU

//----------BIG MOM----------//

void equipage_bm(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("equipageBM.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


void bigmom_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("bigmom.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//----------SHANKS----------//


void shanks_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("shanks.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


void equipage_shanks(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("equipageS.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//----------KAIDOU----------//

void equipage_kd(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("equipageK.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

void kaido_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("kaidou.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

//----------BARBE NOIR----------//


void equipage_bn(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("equipageBN.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

void barbenoir_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("blackbeard.jpg");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}



void yonkou_img(int state) {
  // INITIALISEZ VOS VARIABLES 
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    // INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) 
    SDL_Surface * s = IMG_Load("yonkou.png");
	
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  
      SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    // LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s 
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);

      sprite = NULL;
    }
    return;

  case GL4DH_UPDATE_WITH_AUDIO:
    // METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON 
    return;
  default: // GL4DH_DRAW 
    //JOUER L'ANIMATION 
    gl4dpSetScreen(screen_id);
	copyFromSurface(sprite,  gl4dpGetWidth()/999,  gl4dpGetHeight()/999);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

////////////////////////////////////////////////////////


void animation_fin(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static SDL_Surface * sprite = NULL;
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s = IMG_Load("onepiece.jpeg");
    sprite = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    if(sprite) {
      SDL_FreeSurface(sprite);
      sprite = NULL;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    copyFromSurface(sprite, gl4dpGetWidth()/999,gl4dpGetHeight()/(999)
);
    gl4dpUpdateScreen(NULL);
    return;
  }
}


void animationsInit(void) {
  if(!_quadId)
    _quadId = gl4dgGenQuadf();
}




