main(){
	char c = 127;
	unsigned uC = 255;
	
	printf("========== Antes ========== \n");
	printf("Char: %i \nUnsigned Char: %i \n", c, uC);
	
	c = c + 1;
	uC = uC + 1;
	
	printf("========= Despues ========= \n");
	printf("Char: %i \nUnsigned Char: %i \n", c, uC);
}
