#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <SDL_image.h>

/*!\brief identifiant du programme GLSL */
//static int _windowWidth = 1200;
static GLuint _pId = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {1500.0, 20.0, 30.0, 1.0};
/*!\brief tableau des identifiants de texture à charger */
static GLuint _tId[4] = {0};
/*!\brief pour les deux astres : terre et lune */
static GLuint _sphere = {0};
static GLuint _cube = 0, _quad = 0;
static GLuint _mode = 0;
GLuint texture1;


void earthF_init(void) {
  int i;
  SDL_Surface * t;
  int vp[4], w, h;

  static char * files[] = {"images/franky_missile.jpg", "images/land_ocean_ice_2048_glossmap.png","images/one.jpg", "images/land_ocean_ice_2048_glossmap.png"};

  _pId  = gl4duCreateProgram("<vs>shaders/earth.vs", "<fs>shaders/earth.fs", NULL);
  glGetIntegerv(GL_VIEWPORT, vp);
  w = vp[2] - vp[0];
  h = vp[3] - vp[1];
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * h / w, 0.5 * h / w, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
  glGenTextures(4, _tId);
  for(i = 0; i < 4; i++) {
    glBindTexture(GL_TEXTURE_2D, _tId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t = IMG_Load(files[i])) != NULL ) {
#ifdef __APPLE__
      int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't open file %s : %s\n", files[i], SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
  }
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
  _quad = gl4dgGenQuadf();
}

void earthF_draw(void) {

   static GLfloat a0 = 0.0;
  static Uint32 t0 = 0, t;
  //SDL_Surface * s = NULL;

  GLfloat dt = 0.0, lumPos[4], *mat;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[1]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  if(_mode == 0)
    gl4duTranslatef(0, 0, -5);
  else
    gl4duTranslatef(0, 0, -14);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);

  glUseProgram(_pId);
  /*
      glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
  gl4duPushMatrix(); {
    gl4duRotatef(5 ,50,50,0);
    gl4duTranslatef(  0,0,-8);
    gl4duScalef(1024,728,0);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  gl4dgDraw(_cube);
*/



//planete terre
 glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
  gl4duPushMatrix(); {
     gl4duRotatef(a0, 0, 1, 0);
    gl4duTranslatef(-2.5, 0, 0);  
    gl4duScalef(0.001, 1.0, 1.0);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  gl4dgDraw(_cube);



//hyper
 /*glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[1]);
  gl4duPushMatrix(); {
     gl4duRotatef(a0, 0, 1, 0);
    gl4duTranslatef(0, 0, 0);  
     gl4duScalef(0.8, 0.8, 0.8);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  gl4dgDraw(_sphere);*/

/*glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[2]);
  gl4duPushMatrix(); {
     gl4duRotatef(a0, 0, 1, 1);
    gl4duTranslatef(3, 0, 0);  
     gl4duScalef(0.001, 0.8, 0.8);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  gl4dgDraw(_cube);*/

/*glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
  gl4duPushMatrix(); {
     gl4duRotatef(a0, 0, 1, 0);
    gl4duTranslatef(2.5, 0, -3);  
     gl4duScalef(0.8, 0.8, 0.8);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  gl4dgDraw(_sphere);*/

  a0 += 360.0 * dt / (6.0);
  gl4dfBlur(0, 0, 0, 1, 0, GL_FALSE);
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel(0, 0, GL_FALSE);
}
