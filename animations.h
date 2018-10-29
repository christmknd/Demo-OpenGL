/*!\file animations.h
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
#ifndef _ANIMATIONS_H

#define _ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

  extern void transition_fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void animation_flash(int state);
  extern void animation_vide(int state);
  extern void animation_damier(int state);


//  extern void animation_base(int state);

extern void animation_earth4(int state);
extern void animation_earthF(int state);
extern void animation_earthR(int state);

//extern void animation_earth6(int state);


extern void mugi_one(int state);
extern void roger(int state);//gold d roger
extern void dessin();//transition tunel 

//EQUIPAGE CHAPEAU DE PAILLE 

//luffy
extern void luffy_normal(int state);
extern void gear_second(int state);
extern void luffy_attack(int state);
extern void luffy_shock(int state);
extern void gear_fourth(int state);
extern void tankman(int state);
extern void snakeman(int state);

//zoro
extern void zoro_img(int state);
extern void zoro_attack(int state);
extern void zoro_shock(int state);

//sanji
extern void sanji_img(int state);
extern void sanji_attack(int state);

//jimbei
extern void jimbei_img(int state);
extern void water(int state);

//usopp
extern void usopp_img(int state);
extern void sogeking_img(int state);
extern void herbe(int state);

//franky 
extern void franky_img(int state);


//reste de l'equipage 
extern void robin_img(int state);
extern void brook_img(int state);
extern void nami_img(int state);
extern void chopper_img(int state);

//marine 
extern void corsaires_one(int state);
extern void ao_img(int state);
extern void ak_img(int state);
extern void aovsak_img(int state);
extern void akainu_img(int state);




//autres pirates

extern void law_img(int state);
extern void marco_img(int state);
extern void dofla_img(int state);



//yonkou
extern void equipage_bm(int state);
extern void bigmom_img(int state);

extern void equipage_shanks(int state);
extern void shanks_img(int state);

extern void equipage_kd(int state);
extern void kaido_img(int state);

extern void equipage_bn(int state);
extern void barbenoir_img(int state);

extern void yonkou_img(int state);


//fin 
extern void animation_fin(int state);

extern void animationsInit(void);



 /* Dans earth4.c */
  extern void earth4_init(void);
  extern void earth4_draw(void);


  extern void earthF_init(void);
  extern void earthF_draw(void);


extern void earthR_init(void);
  extern void earthR_draw(void);

/* Dans earth5.c */
//extern void animation_earth5(int state);
  //extern void earth5_init(void);
  //extern void earth5_draw(void);



/* Dans earth6.c */
 // extern void earth6_init(void);
  //extern void earth6_draw(void);

/* Dans darkhole */
/*extern void initGL_dark(void);
extern void draw_dark(void);
extern void animation_dark(int state);*/


/* signature fin.c */ 

extern void initSign(void);
extern void drawSign(void);
extern void animation_sign(int state);

//extern void animation_sign(int state);

extern void dessin_tunnel(void);
extern void animation_tunnel(int state);

//rosace
extern void diagInit(void);
extern void dessin_rosace(void);
extern void animation_rosace(int state);

//missile franky
/*
extern void initGL_F(void);
extern void draw_F(void);
extern void animation_missile(int state);*/

#ifdef __cplusplus
}
#endif

#endif
