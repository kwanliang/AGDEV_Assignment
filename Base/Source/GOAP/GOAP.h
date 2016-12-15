#pragma once

#include <vector>

class CGOAP
{
public:
    CGOAP();
    ~CGOAP();

    virtual void Init();
    virtual void Update();
    virtual void Preconditions(double dt = 0.0333f) = 0;

    void ClearList();

    std::vector<unsigned> m_StateList;
};