//
//  fmt.cpp
//  fmt
//
//  Created by apple on 2019/6/3.
//  Copyright © 2019 apple. All rights reserved.
//

#include <iostream>
#include "fmt.hpp"
#include "fmtPriv.hpp"

void fmt::HelloWorld(const char * s)
{
    fmtPriv *theObj = new fmtPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void fmtPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

