#ifndef __PORTAL_H
#define __PORTAL_H

#include "Utils.h"

void PortalInit(void);

void PortalExit(void);

const timezone_t *PortalGetConfigTZ();

bool PortalGetConfigEmail(char *email);

bool PortalGetConfigWIFI(char *ssid, char *passwd);

bool PortalExistConfig(void);

#endif /* __PORTAL_H */
