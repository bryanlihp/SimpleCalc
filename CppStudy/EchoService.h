#pragma once
#include "ServiceIo.h"
#include "ServiceState.h"


struct User
{
	int userId;
	char userName[64];
	char password[32];
};

struct ServiceDescriptor
{
	pfnSvcInput read;
	pfnSvcOutput write;
	int state;
	char* pLoggedInUserName;
	int nUsers;
	User serviceUsers[2];
};

void EchoService();
bool InitService(pfnSvcInput read, pfnSvcOutput write, ServiceDescriptor * pSvcDesc);
void StartService(ServiceDescriptor * pSvcDesc);
