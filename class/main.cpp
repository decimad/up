#include "events_test.h"
#include "class_test.h"
#include "observer_test.h"

int main(int argc, char* argv[])
{
	events_test();
	typetest();
	observer_test();
	
	return 0;
}