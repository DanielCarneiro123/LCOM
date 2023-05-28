#include "sprite.h"

extern MouseInfo mouse_info;

/**
 * @brief Create a sprite xpm object
 * Allocates memory for a sprite
 * @param sprite xpm of sprite to create
 * @return Sprite* Pointer to new sprite
 */
Sprite *create_sprite_xpm(xpm_map_t sprite){

  Sprite *sp = (Sprite *) malloc (sizeof(Sprite));
  if( sp == NULL ) return NULL;

  xpm_image_t img;
  sp->colors = (uint32_t *) xpm_load(sprite, XPM_8_8_8_8, &img);
  sp->height = img.height;
  sp->width = img.width;

  if( sp->colors == NULL ) {
    free(sp);
    return NULL;
  }
  return sp;
}

/**
 * @brief Deallocates sprite memory
 * @param sprite Sprite to delete
 */
void destroy_sprite(Sprite *sprite) {
    if (sprite == NULL) return;
    if (sprite->colors != NULL) free(sprite->colors);
    free(sprite);
    sprite = NULL;
}

bool is_mouse_in_sprite(Sprite *sprite) {
  return mouse_info.x >= sprite->x && mouse_info.x <= (sprite->x + sprite->width) && mouse_info.y >= sprite->y && mouse_info.y <= (sprite->y + sprite->height);
}
