#include "pch.h"
#include <stdlib.h>
#include <string.h>
#include "Utility.h"

char* Concat(const char* pFirst, const char* pSecond)
{
    char* pBuffer = nullptr;
    size_t size1 = pFirst ? strlen(pFirst) : 0;
    size_t size2 = pSecond ? strlen(pSecond) : 0;
    size_t size = size1 + size2;
    pBuffer = (char*)malloc(size + 1);
    if (size != 0)
    {
        if (size1 > 0)
        {
            memcpy((void*)pBuffer, pFirst, size1);
        }
        if (size2 > 0)
        {
            memcpy((void*)(pBuffer + size1), pSecond, size2);
        }
        pBuffer[size] = '\0';
    }
    return pBuffer;
}

char* Append(char* pFirst, const char* pSecond)
{
    size_t size2 = pSecond ? strlen(pSecond) : 0;
    if (0== size2)
    {
        return pFirst;
    }
    size_t size1 = pFirst ? strlen(pFirst) : 0;
    size_t newSize = size1 + size2;
    
    char* pNew  = (char*)realloc(pFirst, newSize + 1);
    if (pNew)
    {
        memcpy(pNew + size1, pSecond, size2);
        pNew[newSize] = '\0';
        return pNew;
    }
    else
    {
        return nullptr;
    }
}

