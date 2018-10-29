/*!\file window.c
 *
 * \brief introduction aux textures (plus ce qui a �t� vu avant) 
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr 
 * \date February 15 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
/*static void init(void);
static void draw(void);
static void quit(void);
*/
/*!\brief largeur et hauteur de la fen�tre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du Vertex Array Object */
static GLuint _vao = 0;
/*!\brief identifiant des Vertex Buffer Objects */
static GLuint _buffer[2] = { 0 };
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant d'une texture */
static GLuint _texId = 0;

/*!\brief cr�� la fen�tre d'affichage, initialise GL et les donn�es,
 * affecte les fonctions d'�v�nements et lance la boucle principale
 * d'affichage.
 */



/*!\brief initialise les param�tres OpenGL et les donn�es. 
 *
 * Exercice (corrig� en 1.1) : utiliser une fonctionnalit� SDL2 ou
 * SDL2_image ou autres pour charger une image � partir d'un fichier
 * et l'appliquer avec interpolation lin�aire au quadrilat�re.
 *
 * Exercice (corrig� en 1.2) : remplacer la texture ci-dessous par un
 * damier noir et blanc de 50x50 cases en utilisant tr�s peu de
 * donn�es. Puis (corrig� en 1.3) remplacer le vao cr�� manuellement
 * par le quadrilat�re g�n�rable par gl4dgGenQuadf et modifier le
 * shader pour n'afficher que la texture car les couleurs n'existent
 * pas dans ce qui est g�n�r� par les fonctions gl4dgGenXXXXX o� : (1)
 * la premi�re donn�e est un vec3 coordonn�e 3D ; (2) la seconde est
 * un vec3 vecteur normal 3D ; (3) la troisi�me est un vec2 coordonn�e
 * de texture 2D normalis�e. Ce dernier point ne permet plus de
 * r�p�ter la texture, corriger le probl�me en trouvant une solution
 * (corrig� en 1.4). Enfin (corrig� en 1.5), placer le quadrilat�re
 * horizontalement et agrandisser-le d'un facteur au moins 50. Tenter
 * de faire du MipMapping ou de l'anisotropic filtering.
 */
 void initSign(void) {
  /* indices pour r�aliser le maillage des g�om�trie, envoy�s dans le
   * VBO ELEMENT_ARRAY_BUFFER */
  GLuint idata[] = {
    /* un quadrilat�re en triangle strip (d'o� inversion entre 3 et 2) */
    0, 1, 3, 2
  };
  /* donn�es-sommets envoy�e dans le VBO ARRAY_BUFFER */
  GLfloat data[] = {
    /* un sommet est compos� d'une coordonn�e 3D, d'une couleur 3D et
     * d'une coordonn�e de texture 2D */
    /* sommet  0 */ -1, -1, 0, 1, 0, 0, 0, 0,
    /* sommet  1 */  1, -1, 0, 0, 1, 0, 1, 0,
    /* sommet  2 */  1,  1, 0, 0, 0, 1, 1, 1,
    /* sommet  3 */ -1,  1, 0, 1, 0, 0, 0, 1
  };
  const GLuint R = RGB(255, 0, 0), B = RGB(255, 255, 255), N = 0;
  GLuint tex[] = {
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, 
    B, N, N, B, N, N, B, N, B, B, B, N, B, R, B, B, B, B, R, B, N, N, B, B, 
    B, N, B, N, B, N, B, N, B, B, N, B, B, R, R, B, B, B, R, B, N, B, N, B, 
    B, N, B, B, B, N, B, N, B, N, B, B, B, R, B, R, B, B, R, B, N, B, N, B, 
    B, N, B, B, B, N, B, N, N, B, B, B, B, R, B, B, R, B, R, B, N, B, N, B, 
    B, N, B, B, B, N, B, N, B, N, B, B, B, R, B, B, B, R, R, B, N, B, N, B, 
    B, N, B, B, B, N, B, N, B, B, N, B, B, R, B, B, B, B, R, B, N, N, B, B, 
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B
  };
  /* G�n�ration d'un identifiant de VAO */
  glGenVertexArrays(1, &_vao);
  /* Lier le VAO-machine-GL � l'identifiant VAO g�n�r� */
  glBindVertexArray(_vao);
  /* Activation des 3 premiers indices d'attribut de sommet */
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  /* G�n�ration de deux identifiants de VBO un pour ARRAY_BUFFER, un
   * pour ELEMENT_ARRAY_BUFFER */
  glGenBuffers(2, _buffer);
  /* Lier le VBO-ARRAY_BUFFER � l'identifiant du premier VBO g�n�r� */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[0]);
  /* Transfert des donn�es VBO-ARRAY_BUFFER */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Param�trage 3 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof *data, (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof *data, (const void *)(3 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof *data, (const void *)(6 * sizeof *data));
  /* Lier le VBO-ELEMENT_ARRAY_BUFFER � l'identifiant du second VBO g�n�r� */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer[1]);
  /* Transfert des donn�es d'indices VBO-ELEMENT_ARRAY_BUFFER */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof idata, idata, GL_STATIC_DRAW);
  /* d�-lier le VAO puis les VAO */
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  /* g�n�rer un identifiant de texture */
  glGenTextures(1, &_texId);
  /* lier l'identifiant de texture comme texture 2D (1D ou 3D
   * possibles) */
  glBindTexture(GL_TEXTURE_2D, _texId);
  /* param�trer la texture, voir la doc de la fonction glTexParameter
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  /* envoi de la donn�e texture depuis la RAM CPU vers la RAM GPU voir
   * la doc de glTexImage2D (voir aussi glTexImage1D et glTexImage3D)
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 24, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
  /* d�-lier la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Cr�ation du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation de la texture 2D */
  glEnable(GL_TEXTURE_2D);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derri�re. */
  glEnable(GL_DEPTH_TEST);
  /* Cr�ation des matrices GL4Dummies, une pour la projection, une
   * pour la mod�lisation et une pour la vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
  /* on active la matrice de projection cr��e pr�c�demment */
  gl4duBindMatrix("projectionMatrix");
  /* la matrice en cours re�oit identit� (matrice avec des 1 sur la
   * diagonale et que des 0 sur le reste) */
  gl4duLoadIdentityf();
  /* on multiplie la matrice en cours par une matrice de projection
   * orthographique ou perspective */
  /* d�commenter pour orthographique gl4duOrthof(-1, 1, -1, 1, 0, 100); */
  gl4duFrustumf(-1, 1, -1, 1, 2, 100);
  /* dans quelle partie de l'�cran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
 void drawSign(void) {
  /* une variable d'angle, maintenant elle passe ne degr�s */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur, nous rajoutons aussi le buffer
   * de profondeur afin de bien rendre les fragments qui sont devant
   * au dessus de ceux qui sont derri�re. Commenter le
   * "|GL_DEPTH_BUFFER_BIT" pour voir la diff�rence. Nous avons ajout�
   * l'activation du test de profondeur dans la fonction init via
   * l'appel glEnable(GL_DEPTH_TEST). */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant ou "courante") la matrice vue cr��e dans
   * init */
  gl4duBindMatrix("viewMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* Composer la matrice vue courante en simulant une "cam�ra" �
   * l'aide de la fonction LookAt(xyz_position_cam,
   * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
  gl4duLookAtf(0, 0, 3, 0, 0, 0, 0, 1, 0);
  /* lier (mettre en avant ou "courante") la matrice mod�le cr��e dans
   * init */
  gl4duBindMatrix("modelMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* on transmet la variable d'angle en l'int�grant dans la matrice
   * mod�le. Soit composer la matrice courante avec une rotation
   * "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 1, 0, 0);
  /* on incr�mente angle d'un 1/60 de 1/4 de tour soit (360� x 1/60). Tester
   * l'application en activant/d�sactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que r�sultat soit toujours le m�me. */
  angle += 0.25f * (1.0f / 60.0f) * 360.0f;
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
  /* activer l'�tage de textures 0, plusieurs �tages sont disponibles,
   * nous pouvons lier une texture par type et par �tage */
  glActiveTexture(GL_TEXTURE0);
  /* lier la texture _texId comme texture 2D */
  glBindTexture(GL_TEXTURE_2D, _texId);
  /* envoyer une info au program shader indiquant que tex est une
   * texture d'�tage 0, voir le type (sampler2D) de la variable tex
   * dans le shader */
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  /* envoi d'un bool�en pour inverser l'axe y des coordonn�es de
   * textures (plus efficace � faire dans le vertex shader */
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1); 
  /* Lier le VAO-machine-GL � l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme une bande d'un triangle avec 4 sommets
   * commen�ant � 0
   *
   * Attention ! Maintenant nous dessinons avec DrawElement qui
   * utilise les indices des sommets poass�s pour mailler */
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (const GLvoid *)0);
  /* d�-lier le VAO */
  glBindVertexArray(0);
  /* d�sactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appel�e au moment de sortir du programme (atexit), elle
 *  lib�re les �l�ments OpenGL utilis�s.*/
 void quit(void) {
  /* suppression de la texture _texId en GPU */
  if(_texId)
    glDeleteTextures(1, &_texId);
  /* suppression du VAO _vao en GPU */
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  /* suppression du VBO _buffer en GPU, maintenant il y en a deux */
  if(_buffer[0])
    glDeleteBuffers(2, _buffer);
  /* nettoyage des �l�ments utilis�s par la biblioth�que GL4Dummies */
  gl4duClean(GL4DU_ALL);
}

/*
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  initSign();
  atexit(quit);
  gl4duwDisplayFunc(drawSign);
  gl4duwMainLoop();
  return 0;
}
*/
