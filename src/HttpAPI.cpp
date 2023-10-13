#include "AppConfig.h"
#include "Log.h"
#include "Utils.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "HttpAPI.h"

#define URI_COMMAND_GET "%s/user/api/cmd?Email=%s&Tkn=%s&Flag=%d"
#define URI_COMMAND_POST_DATA "%s/user/api/cmd/postdata"

#define FLAG_GET_USERID -1
#define FLAG_GET_COMMAND 0
#define FLAG_GET_CONFIG 1

bool HttpAPIGetUserId(char *email, char *userid)
{
    bool result = false;
    char base_uri[URI_MAX_LENGTH];
    char cmd_uri[URI_MAX_LENGTH];
    HTTPClient request;
    int response_code;

    memset(base_uri, 0, sizeof(base_uri));
    memset(cmd_uri, 0, sizeof(cmd_uri));
    ConfigGetStr(CONF_BASE_URI, base_uri);
    sprintf(cmd_uri, URI_COMMAND_GET, base_uri, email, "00:00:00:00:00:00", FLAG_GET_USERID);
    LOG_DBG("HTTP ->> %s", cmd_uri);
    request.begin(cmd_uri);
    response_code = request.GET();
    if (response_code == 200)
    {
        StaticJsonDocument<512> jsitem;
        String response = request.getString();
        if (response.charAt(0) == '"')
        {
            response.remove(0, 1);
        }
        if (response.charAt(response.length() - 1) == '"')
        {
            response.remove(response.length() - 1, 1);
        }

        for (int i = response.length() - 1; i >= 0; i--)
        {
            if (response.charAt(i) == '\\')
            {
                response.remove(i, 1);
            }
        }

        LOG_DBG("Response user id %s", response.c_str());
        if (!deserializeJson(jsitem, response))
        {
            if (jsitem.containsKey("userid"))
            {
                int uid = jsitem["userid"];
                sprintf(userid, "%d", uid);
                result = true;
            }
        }
    }
    else
    {
        LOG_ERR("Request user id error %d %s", response_code, request.getString().c_str());
    }

lb_exit:
    request.end();

    return result;
}

bool HttpAPIGetToken(char *body, char *token)
{
    bool result = false;
    char base_uri[URI_MAX_LENGTH];
    char cmd_uri[URI_MAX_LENGTH];
    HTTPClient request;
    int response_code;

    memset(base_uri, 0, sizeof(base_uri));
    memset(cmd_uri, 0, sizeof(cmd_uri));
    ConfigGetStr(CONF_BASE_URI, base_uri);
    sprintf(cmd_uri, URI_COMMAND_POST_DATA, base_uri);
    LOG_DBG("HTTP ->> [%s] body [%s]", cmd_uri, body);
    request.begin(cmd_uri);
    request.addHeader("Content-Type", "application/json");
    response_code = request.POST(body);
    if (response_code == 200)
    {
        String response = request.getString();
        if (response.charAt(0) == '"')
        {
            response.remove(0, 1);
        }
        if (response.charAt(response.length() - 1) == '"')
        {
            response.remove(response.length() - 1, 1);
        }
        LOG_DBG("Response token id %s", response.c_str());
        sprintf(token, "%s", response.c_str());
        result = true;
    }
    else
    {
        LOG_ERR("Request token id error %d %s", response_code, request.getString().c_str());
    }

lb_exit:
    request.end();

    return result;
}

bool HttpAPISendData(char *body)
{
    bool result = false;
    char base_uri[URI_MAX_LENGTH];
    char cmd_uri[URI_MAX_LENGTH];
    HTTPClient request;
    int response_code;

    memset(base_uri, 0, sizeof(base_uri));
    memset(cmd_uri, 0, sizeof(cmd_uri));
    ConfigGetStr(CONF_BASE_URI, base_uri);
    sprintf(cmd_uri, URI_COMMAND_POST_DATA, base_uri);
    LOG_DBG("HTTP ->> [%s] body %s", cmd_uri, body);
    request.begin(cmd_uri);
    request.addHeader("Content-Type", "application/json");
    response_code = request.POST(body);
    if (response_code == 200)
    {
        String response = request.getString();
        LOG_DBG("Response str %s", response.c_str());
        result = true;
    }
    else
    {
        LOG_ERR("Send data error %d %s", response_code, request.getString().c_str());
    }

lb_exit:
    request.end();

    return result;
}

bool HttpAPIPoll(int flag, char *email, char *token, char *body)
{
    bool result = false;
    char base_uri[URI_MAX_LENGTH];
    char cmd_uri[URI_MAX_LENGTH];
    HTTPClient request;
    int response_code;

    memset(base_uri, 0, sizeof(base_uri));
    memset(cmd_uri, 0, sizeof(cmd_uri));
    ConfigGetStr(CONF_BASE_URI, base_uri);
    sprintf(cmd_uri, URI_COMMAND_GET, base_uri, email, token, flag);
    LOG_DBG("HTTP ->> %s", cmd_uri);
    request.begin(cmd_uri);
    response_code = request.GET();
    if (response_code == 200)
    {
        StaticJsonDocument<512> jsitem;
        String response = request.getString();
        if (response.charAt(0) == '"')
        {
            response.remove(0, 1);
        }
        if (response.charAt(response.length() - 1) == '"')
        {
            response.remove(response.length() - 1, 1);
        }

        for (int i = response.length() - 1; i >= 0; i--)
        {
            if (response.charAt(i) == '\\')
            {
                response.remove(i, 1);
            }
        }

        LOG_DBG("Response poll %s %s", flag == FLAG_GET_COMMAND ? "command" : "config", response.c_str());
        strcpy(body, response.c_str());
        result = true;
    }
    else
    {
        LOG_ERR("Request error %d %s", response_code, request.getString().c_str());
    }

lb_exit:
    request.end();

    return result;
}

bool HttpAPIPollCommand(char *email, char *token, char *body)
{
    return HttpAPIPoll(FLAG_GET_COMMAND, email, token, body);
}

bool HttpAPIPollConfig(char *email, char *token, char *body)
{
    return HttpAPIPoll(FLAG_GET_CONFIG, email, token, body);
}

bool HttpIFTTTget(char *url)
{
    bool result = false;
    HTTPClient request;
    int response_code;

    LOG_DBG("GET ->> [%s]", url);
    request.begin(url);
    request.addHeader("Content-Type", "application/x-www-form-urlencoded");
    response_code = request.GET();
    if (response_code == 200)
    {
        String response = request.getString();
        LOG_DBG("Response str %s", response.c_str());
        result = true;
    }
    else
    {
        LOG_ERR("Send data error %d %s", response_code, request.getString().c_str());
    }

lb_exit:
    request.end();

    return result;
}
