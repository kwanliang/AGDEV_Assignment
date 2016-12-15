#include "GOAP.h"

CGOAP::CGOAP()
{

}

CGOAP::~CGOAP()
{
    //for (auto it : m_StateList)
    //{
    //    m_StateList.pop_back();
    //}
}

void CGOAP::Init()
{

}

void CGOAP::Update()
{

}

void CGOAP::ClearList()
{
    std::vector<unsigned>::iterator it = m_StateList.begin();
    while (it != m_StateList.end())
    {
        it = m_StateList.erase(it);
    }
}