#pragma once

#include "Platform.h"

class Address
{
public:
	Address();
	Address(const std::string& address);
	~Address();
	
private:
	uint32_t address_;
};

