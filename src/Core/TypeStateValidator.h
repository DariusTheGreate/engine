#pragma once 

struct TypeStateValidator
{
	virtual bool validate() = 0;	
};

struct ValidChecker
{
protected:
	bool is_valid = false;
public:
	bool validate()
	{
		return is_valid;
	}
}
