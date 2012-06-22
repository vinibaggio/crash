#include "../libs/str_utils.h"

int main (int argc, char const *argv[]) {
	char test[2][200] = {{"  \n  testetestet teste teste \n teste teste \n\n"}, "test"};
	int numoftests = 2;
	int size = 0;
	int i = 0;

	for(i = 1; i <= numoftests; i++) {
		printf("------ Test number %d:\n", i);
		printf("%s\n", test[i-1]);
		printf("------\n");

		size = cleanstring(test[i-1], 200);

		printf("%s\n", test[i-1]);
		printf("------ End of Test number %d\n", i);
		printf("Number of characters, counting with null: %d\n\n", size);
	}
	return 0;
}

