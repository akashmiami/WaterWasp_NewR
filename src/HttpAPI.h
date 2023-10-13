#ifndef __HTTP_API_H
#define __HTTP_API_H

bool HttpAPIGetUserId(char *email, char *userid);

bool HttpAPIGetToken(char *body, char *token);

bool HttpAPISendData(char *body);

bool HttpAPIPollCommand(char *email, char *token, char *body);

bool HttpAPIPollConfig(char *email, char *token, char *body);

bool HttpIFTTTget(char *url);

#endif /* __HTTP_API_H */
