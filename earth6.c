#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <SDL_image.h>

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {1500.0, 20.0, 30.0, 1.0};
/*!\brief tableau des identifiants de texture à charger */
static GLuint _tId[3] = {0};
static GLuint _tId2[3] = {0};
/*!\brief pour les deux astres : terre et lune */
static GLuint _sphere = {0};

static int _windowWidth = 800;
/*!\brief opened window height */
static int _windowHeight = 600;
/*!\brief Grid geometry Id  */
static GLuint _grid = 0;
/*!\brief grid width */
static int _gridWidth = 129;
/*!\brief grid height */
static int _gridHeight = 129;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
/*!\brief angle, in degrees, for x-axis rotation */
static GLfloat _rx = 90.0f;
/*!\brief angle, in degrees, for y-axis rotation */
static GLfloat _ry = 0.0f;

/*!\brief enum that index keyboard mapping for direction commands */
enum kyes_t {
  KLEFT = 0, KRIGHT, KUP, KDOWN, KSHIFT
};

void earth6_init(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_windowWidth, _windowHeight);
}

static void idle(void) {
  double dt;
  static Uint32 t0 = 0, t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  if(_keys[KLEFT])
    _ry += 180 * dt;
  if(_keys[KRIGHT])
    _ry -= 180 * dt;
  if(_keys[KUP])
    _rx -= 180 * dt;
  if(_keys[KDOWN])
    _rx += 180 * dt;
}
static void resize(int w, int h) {
  _windowWidth = w; 
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 10.0);
  gl4duBindMatrix("modelViewMatrix");
}

static void keydown(int keycode) {
  GLint v[2];
  switch(keycode) {
  case SDLK_LEFT:
    _keys[KLEFT] = GL_TRUE;
    break;
  case SDLK_RIGHT:
    _keys[KRIGHT] = GL_TRUE;
    break;
  case SDLK_UP:
    _keys[KUP] = GL_TRUE;
    break;
  case SDLK_DOWN:
    _keys[KDOWN] = GL_TRUE;
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
    /* when 'w' pressed, toggle between line and filled mode */
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(1.0);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(1.0);
    }
    break;
  default:
    break;
  }
}

static void keyup(int keycode) {
  switch(keycode) {
  case SDLK_LEFT:
    _keys[KLEFT] = GL_FALSE;
    break;
  case SDLK_RIGHT:
    _keys[KRIGHT] = GL_FALSE;
    break;
  case SDLK_UP:
    _keys[KUP] = GL_FALSE;
    break;
  case SDLK_DOWN:
    _keys[KDOWN] = GL_FALSE;
    break;
  default:
    break;
  }
}

void earth6_draw(void)
{
static GLuint _grid = 0;
/*!\brief grid width */
static int _gridWidth = 129;
/*!\brief grid height */
static int _gridHeight = 129;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
/*!\brief angle, in degrees, for x-axis rotation */
static GLfloat _rx = 90.0f;
/*!\brief angle, in degrees, for y-axis rotation */
static GLfloat _ry = 0.0f;  
/* step = x and y max distance between two adjacent vertices */
  GLfloat step[2] = { 2.0 / _gridWidth, 2.0 / _gridHeight};
  /* clears the OpenGL color buffer and depth buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* sets the current program shader to _pId */
  glUseProgram(_pId);
  /* loads the identity matrix in the current GL4Dummies matrix ("modelViewMatrix") */
  gl4duLoadIdentityf();

  /* rotation around x-axis, old y-axis and tranlation */
  gl4duPushMatrix(); {
    gl4duTranslatef(0, 0, -4);
    gl4duRotatef(_ry, 0, cos(_rx * M_PI / 180.0), sin(_rx * M_PI / 180.0));
    gl4duRotatef(_rx, 1, 0, 0);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  /* draws the grid */
  glUniform1ui(glGetUniformLocation(_pId, "frame"), SDL_GetTicks());
  glUniform2fv(glGetUniformLocation(_pId, "step"), 1, step);
  gl4dgDraw(_grid);
}



