#include<stdlib.h>
#include<stdio.h>

int main()
{
	int r;
	r = system("ls -l");
	printf("%d" , r);
	return 0;

}
