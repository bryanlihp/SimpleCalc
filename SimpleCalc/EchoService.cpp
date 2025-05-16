#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "EchoService.h"

void OnUserNameRcvd(ServiceDescriptor* pSvcDesc, const char* pInputText);
void SetUsername(const char* pInputText, ServiceDescriptor* pSvcDesc);
void AuthenticateUser(ServiceDescriptor* pSvcDesc, const char* pInputText);
void EchoText(ServiceDescriptor* pSvcDesc, const char* pInputText);
bool HandleInput(ServiceDescriptor* pSvcDesc, const char* pInputText);
const char* GetPrompter(int state);

bool InitService(pfnSvcInput read, pfnSvcOutput write, ServiceDescriptor* pSvcDesc)
{
    if (pSvcDesc == nullptr)
    {
        return false;
    }

    pSvcDesc->read = read;
    pSvcDesc->write = write;
    pSvcDesc->state = USER_NAME_WAIT;
    pSvcDesc->pLoggedInUserName = nullptr;
    pSvcDesc->nUsers = 2;
    pSvcDesc->serviceUsers[0] = {1,"user1","password1"};
    pSvcDesc->serviceUsers[1] = {2,"user2","password2"};
    return true;
}

void StartService(ServiceDescriptor* pSvcDesc)
{
    if (pSvcDesc == nullptr)
    {
        printf("Sercice not initialized.\n");
        return;
    }
    pSvcDesc->write("Echo service.",true);
    bool bContinue = true;
    while (bContinue)
    {
        const char* pPrompter = GetPrompter(pSvcDesc->state);
        pSvcDesc->write(pPrompter,false);
        char* pInputText = pSvcDesc->read();
        if (pInputText != nullptr)
        {
            if (_stricmp(pInputText, "exit") == 0)
            {
                break;
            }
            bContinue = HandleInput(pSvcDesc, pInputText);
            free(pInputText);
        }
        
    }

    if (pSvcDesc->pLoggedInUserName != nullptr)
    {
        free(pSvcDesc->pLoggedInUserName);
        pSvcDesc->pLoggedInUserName = nullptr;
    }
}

const char* GetPrompter(int state)
{
    static const char* Prompters[] = { "USR>","PWD>","ECHO>" };
    const char* pText = nullptr;
    switch (state)
    {
    case USER_NAME_WAIT:
        pText = Prompters[0];
        break;
    case PASSWORD_WAIT:
        pText = Prompters[1];
        break;
    case ECHO_SVC:
        pText = Prompters[2];
        break;
    }
    return pText;
}

bool HandleInput(ServiceDescriptor* pSvcDesc, const char* pInputText)
{
    switch (pSvcDesc->state)
    {
    case USER_NAME_WAIT:
        OnUserNameRcvd(pSvcDesc, pInputText);
        break;
    case PASSWORD_WAIT:
        AuthenticateUser(pSvcDesc, pInputText);
        break;
    case ECHO_SVC:
        EchoText(pSvcDesc, pInputText);
        break;
    }
    return true;
}

char* Concat(const char* pFirst, const char* pSecond)
{
    char* pBuffer = nullptr;
    int size1 = pFirst? strlen(pFirst) : 0;
    int size2 = pSecond ? strlen(pSecond) : 0;
    int size = size1 + size2;
    pBuffer = (char*)malloc(size+1);
    if (size != 0)
    {
        if (size1>0)
        {
            memcpy((void*)pBuffer, pFirst, size1);
        }
        if (size2 > 0)
        {
            memcpy((void*)(pBuffer+size1), pSecond, size2);
        }
        pBuffer[size] = '\0';
    }
    return pBuffer;
}

bool SetUsername(ServiceDescriptor* pSvcDesc, const char* pInputText)
{
    int n = strlen(pInputText);
    if (n == 0)
    {
        return false;
    }
    if (pSvcDesc->pLoggedInUserName)
    {
        free(pSvcDesc->pLoggedInUserName);
    }
    pSvcDesc->pLoggedInUserName = (char*)malloc(n + 1);
    memcpy(pSvcDesc->pLoggedInUserName, pInputText, n);
    pSvcDesc->pLoggedInUserName[n] = '\0';
    return true;
}


void OnUserNameRcvd(ServiceDescriptor* pSvcDesc, const char* pInputText)
{
    if(SetUsername(pSvcDesc, pInputText))
    {
        char* pEchoText = Concat("Username ", pInputText);
        pSvcDesc->write(pEchoText,true);
        free(pEchoText);
        pEchoText = nullptr;
        pSvcDesc->state = PASSWORD_WAIT;
    }
}


void AuthenticateUser(ServiceDescriptor* pSvcDesc, const char* pInputText)
{
    bool bAuthenticated = false;
    for (int i = 0; i < pSvcDesc->nUsers; i++)
    {
        if (_stricmp(pSvcDesc->pLoggedInUserName, pSvcDesc->serviceUsers[i].userName) == 0 &&
            strcmp(pInputText, pSvcDesc->serviceUsers[i].password) == 0)
        {
            bAuthenticated = true;
            break;
        }
    }
    char* pEchoText = nullptr;
    if (bAuthenticated)
    {
        pEchoText = Concat("Welcome ", pSvcDesc->pLoggedInUserName);
        pSvcDesc->state = ECHO_SVC;
    }
    else
    {
        pEchoText = Concat("Bad username or password", nullptr);
        pSvcDesc->state = USER_NAME_WAIT;
    }
    if (pEchoText)
    {
        pSvcDesc->write(pEchoText, true);
        free(pEchoText);
    }
}

void EchoText(ServiceDescriptor* pSvcDesc, const char* pInputText)
{
    if (pInputText && _stricmp(pInputText, "logout") == 0)
    {
        pSvcDesc->state = USER_NAME_WAIT;
    }
    else
    {
        pSvcDesc->write(pInputText, true);
    }
}
