//
//  jsoncpp.cpp
//  jsoncpp
//
//  Created by apple on 2019/6/3.
//  Copyright © 2019 apple. All rights reserved.
//

#include <iostream>
#include "jsoncpp.hpp"
#include "jsoncppPriv.hpp"

void jsoncpp::HelloWorld(const char * s)
{
    jsoncppPriv *theObj = new jsoncppPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void jsoncppPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

