#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "..\CalcLib\Utility.h"
#include "EchoService.h"
#include "ConsoleIo.h"

void OnUserNameRcvd(ServiceDescriptor* pSvcDesc, const char* pInputText);
void AuthenticateUser(ServiceDescriptor* pSvcDesc, const char* pInputText);
void EchoText(ServiceDescriptor* pSvcDesc, const char* pInputText);
void EvalSvc(ServiceDescriptor* pSvcDesc, char* pInputText);
bool HandleInput(ServiceDescriptor* pSvcDesc, char* pInputText);
const char* GetPrompter(int state);

void EchoService()
{
    ServiceDescriptor svcDesc;
    InitService(ConsoleInput, ConsoleOutput, &svcDesc);
    StartService(&svcDesc);
}


bool InitService(pfnSvcInput read, pfnSvcOutput write, ServiceDescriptor* pSvcDesc)
{
    if (pSvcDesc == nullptr)
    {
        return false;
    }

    pSvcDesc->read = read;
    pSvcDesc->write = write;
    pSvcDesc->state = EVAL_SVC;
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
    static const char* Prompters[] = { "USR>","PWD>","ECHO>","EXP>" };
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
    case EVAL_SVC:
        pText = Prompters[3];
        break;
    }
    return pText;
}

bool HandleInput(ServiceDescriptor* pSvcDesc, char* pInputText)
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
    case EVAL_SVC:
        EvalSvc(pSvcDesc, pInputText);
        break;
    }
    return true;
}


bool SetUsername(ServiceDescriptor* pSvcDesc, const char* pInputText)
{
    size_t n = strlen(pInputText);
    if (n == 0)
    {
        return false;
    }
    if (pSvcDesc->pLoggedInUserName)
    {
        free(pSvcDesc->pLoggedInUserName);
        pSvcDesc->pLoggedInUserName = nullptr;
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
    if (pInputText)
    {
        if (_stricmp(pInputText, "logout") == 0 )
        {
            pSvcDesc->state = USER_NAME_WAIT;
        }
        if (_stricmp(pInputText, "expval") == 0)
        {
            pSvcDesc->state = EVAL_SVC;
        }
        else
        {
            pSvcDesc->write(pInputText, true);
        }
    }
}

#include "..\CalcLib\CalcLib.h"

void EvalSvc(ServiceDescriptor* pSvcDesc, char* pInputText)
{
    if (pInputText)
    {
        if (_stricmp(pInputText, "logout") == 0)
        {
            pSvcDesc->state = USER_NAME_WAIT;
        }
        if (_stricmp(pInputText, "echo") == 0)
        {
            pSvcDesc->state = ECHO_SVC;
        }
        else
        {
            LexerDescriptor desc;
            InitLexer(&desc);

            int nInfexIndex = AddTokenSink(&desc, ResultType::TEXT, InFixExpBuilder);
            int nRpnIndex = AddTokenSink(&desc, ResultType::TEXT, RpnExpBuilder);
            int nEvalIndex = AddTokenSink(&desc, ResultType::NUMBER, EvalExpBuilder);
            
            bool bSucceeded = Parse(&desc, pInputText);
            if (bSucceeded)
            {
                LexTokenSink* pInFixSink = desc.ppTokenSinks[nInfexIndex];
                pSvcDesc->write(pInFixSink->result.resultValue.pResultText, true);
                LexTokenSink* pRpnSink = desc.ppTokenSinks[nRpnIndex];
                pSvcDesc->write(pRpnSink->result.resultValue.pResultText, true);
                LexTokenSink* pEvlSink = desc.ppTokenSinks[nEvalIndex];
                char buf[32];
                snprintf(buf, 32, "%d", pEvlSink->result.resultValue.nValue);
                pSvcDesc->write(buf, true);
            }
            else
            {
                pSvcDesc->write(desc.pErrorText, true);
            }
            DestroyLexer(&desc);
        }
    }
}