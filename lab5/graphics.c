#include <lcom/lcf.h>
#include "VBE.h"

int set_graphic_mode(uint16_t submode) {
    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86)); 
    reg86.intno = BIOS_VIDEOCARD_SERV;                                
    reg86.ax = VBE_MODE_SET;             
    reg86.bx = submode | VBE_LINEAR_FB;     
    if (sys_int86(&reg86) != 0) {     
        printf("Set graphic mode failed\n");
        return 1;
    }
    return 0;
}

int set_text_mode() {
    reg86_t reg86;                       
    memset(&reg86, 0, sizeof(reg86));  
    reg86.intno = BIOS_VIDEOCARD_SERV;                 
    reg86.ah = BIOS_SET_VIDEOMODE;                    
    reg86.al = 0x03;                                                   
    if(sys_int86(&reg86) != 0) {        
        printf("Set text mode failed\n");
        return 1;
    }
    return 0;
}

int get_mode_info(vbe_mode_info_t *mode_info, uint16_t mode) {
    memset(mode_info, 0, sizeof(*mode_info));
    if(vbe_get_mode_info(mode, mode_info)) return 1;
    return 0;
}