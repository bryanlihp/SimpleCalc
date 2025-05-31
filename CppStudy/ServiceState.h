#pragma once

#define USER_NAME_WAIT 1
#define PASSWORD_WAIT  2
#define ECHO_SVC	   3
#define EVAL_SVC	   4

struct ServiceDescriptor;

typedef char* (*pfnGetPrompt)();
typedef bool (*pfnHandleInput)(ServiceDescriptor* pSvcDesc, char* pInputText);

struct ServiceState
{
	int serviceState = USER_NAME_WAIT;
	pfnGetPrompt GetPrompt;
};

ServiceState* GetServiceState(int state);