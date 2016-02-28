#include <Company/CCompany.h>


CCompany::~CCompany() 
{
	for (auto &m : Members)
		delete m;

	for (auto &r : Ranks)
		delete r;
}
