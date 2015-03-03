#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "tokenizer.h"
 
int main(int argc, char **argv) {

	/* BEGIN PROGRAM*/
	TokenizerT *tok;
	argv[1] = "I have a little ... 0x34fa bir dy t0 pl4y with 3.4e-62f \\*/\n";
	if(argv[1] == NULL){
		userGuide();
		exit(0);
	}
	if(argc > 2){
		printf("\nOnly one string allowed. Be sure to put quotation marks on your strings.\n");
		printf("The program will only tokenize the first argument string.\n");
		userGuide();
		exit(0);
	}
	
	tok = TKCreate(argv[1]);
	
	/* IF YOU WANT TO SEE MY DESIGN OF THE TOKENIZER HERE IT IS:*/
	printf("\n%s", tok->capturedString);
	printf("\n%s\n", tok->state);
	
	
	char *word;
	while((word = TKGetNextToken(tok)) != NULL){
		printToken(tok, word);
		free(word);
		
	}
	TKDestroy(tok);
	return 0;
}

TokenizerT *TKCreate(char *inputString) {
	TokenizerT *tok = malloc(sizeof(TokenizerT));
	tok->capturedString = (char *)calloc(strlen(inputString)+1,1);
	tok->state = (char *)calloc(strlen(inputString)+1,1);
	
	stateMachine(tok, inputString);
	tok->currentIndex = 0;
	return tok;
}

void TKDestroy(TokenizerT *tok) {
	free(tok->state);
	free(tok->capturedString);
	free(tok);
}

char *TKGetNextToken(TokenizerT *tok) {
	char *word = (char *)calloc(strlen(tok->capturedString)+1, 1);
	if(tok->state[tok->currentIndex] == '\0'){
		return NULL;
	}else if(tok->state[tok->currentIndex] == 'x'){
		tok->currentIndex++;
		free(word);
		return TKGetNextToken(tok);
	}else{
		int start, finish;
		start = tok->currentIndex;
		while(tok->state[tok->currentIndex] == '0'){
			tok->currentIndex++;
		}
		finish = tok->currentIndex;
		int x;
		int y = start;
		for(x=0 ; x<=finish-start; x++, y++){
			word[x] = tok->capturedString[y];
		}
		word[x] = '\0';
		tok->currentIndex++;
		return word;
	}
}
 
void printToken(TokenizerT *tok, char *word){
	int noError = 0;
	switch(tok->state[tok->currentIndex - 1]){
		case 'W':
			if(strcmp(word, "if") == 0 || strcmp(word, "else") == 0 || strcmp(word, "while") == 0 ||
			   strcmp(word, "switch") == 0 || strcmp(word, "do") == 0 || strcmp(word, "for") == 0 ||
			   strcmp(word, "case") == 0 || strcmp(word, "break") == 0 || strcmp(word, "continue") == 0 ||
	           strcmp(word, "goto") == 0 || strcmp(word, "default") == 0 || strcmp(word, "printf") == 0 ||
			   strcmp(word, "return") == 0 || strcmp(word, "exit") == 0){
				printf("C keyword: ");
				noError++;
				break;
			}else{
				printf("word ");
				noError++;
				break;
			}
		case 'D':
			printf("decimal integer constant ");
			noError++;
			break;
		case 'F':
			printf("floating-point constant ");
			noError++;
			break;
		case 'H':
			printf("hexadecimal integer constant ");
			noError++;
			break;
		case 'O':
			printf("octal integer constant ");
			noError++;
			break;
		case 'P':
			noError = determineOperator(tok, word);
			break;
		case 'Q':
			printf("quotation ");
			noError++;
			break;
		default:
			printf("ERROR: something went terribly wrong...");
			break;
	}
	if(noError){
		printf("%c%s%c\n", '"', word, '"');
	}
}

void stateMachine(TokenizerT *tok, char *string){
	/* sets state to W / D / F / H / O / P / Q */
	int index, counter;
	int Qindex, Qcounter = 0;
	for(index=0; index<strlen(string)+1; index++){
		tok->stringPtr = &string[index];
		counter = 0;
		if(isalpha(string[index])){
			counter += alphaLoop(tok, string,index+counter);
			index += counter-1;
			tok->state[index] = 'W'; /* for word token*/
			continue;
		}else if(isdigit(string[index])){
			if(string[index] != '0'){
				/*1-9 **********************************************/
				counter += numLoop(tok, string, index);
				if(string[index + counter] == '.'){
					if(string[index + counter + 1] != '\0' && isdigit(string[index + counter + 1])){
						counter += setTokenizer(tok, string, index, counter);
						counter += numLoop(tok, string, index+counter);
					}else{
						index += counter-1;
						tok->state[index] = 'D'; /* decimal */
						continue;
					}
					if(string[index + counter] == 'e' || string[index + counter] == 'E'){
						if(string[index + counter + 1] != '\0' && isdigit(string[index + counter + 1])){
							counter += setTokenizer(tok, string, index, counter);
							counter += numLoop(tok, string, index+counter);
						}
						if(string[index + counter + 1] != '\0' || string[index + counter + 1] == '-' || string[index + counter + 1] == '+'){
							if(string[index + counter + 2] != '\0' || isdigit(string[index + counter + 2])){
								counter += setTokenizer(tok, string, index, counter);
								counter += setTokenizer(tok, string, index, counter);
								counter += numLoop(tok, string, index+counter);
							}
						}
					}
					index += counter-1;
					tok->state[index] = 'F'; /* float */
					continue;
				}else if(string[index + counter] == 'e' || string[index + counter] == 'E'){
					if(string[index + counter + 1] != '\0' && isdigit(string[index + counter + 1])){
						counter += setTokenizer(tok, string, index, counter);
						counter += numLoop(tok, string, index+counter);
					}
					if(string[index + counter + 1] != '\0' && (string[index + counter + 1] == '-' || string[index + counter + 1] == '+')){
						if(string[index + counter + 2] != '\0' || isdigit(string[index + counter + 2])){
							counter += setTokenizer(tok, string, index, counter);
							counter += setTokenizer(tok, string, index, counter);
							counter += numLoop(tok, string, index+counter);
						}
					}
					index += counter-1;
					tok->state[index] = 'F'; /* float */
					continue;
				}else{
					index += counter-1;
					tok->state[index] = 'D'; /* decimal */
				}
			}else{
				/*0 *************************************************/
				counter += octLoop(tok, string, index);
				if(counter >= 2){
					index += counter-1;
					tok->state[index] = 'O'; /* octal */
					continue;
				}
				if(string[index + counter] == '.'){
					if(string[index + counter + 1] != '\0' && isdigit(string[index + counter + 1])){
						counter += setTokenizer(tok, string, index, counter);
						counter += numLoop(tok, string, index+counter);
					}else{
						index += counter - 1;
						tok->state[index] = 'D'; /* decimal */
						continue;
					}
					if(string[index + counter] == 'e' || string[index + counter] == 'E'){
						if(string[index + counter + 1] != '\0' && isdigit(string[index + counter + 1])){
							counter += setTokenizer(tok, string, index, counter);
							counter += numLoop(tok, string, index+counter);
						}else{
							index += counter-1;
							tok->state[index] = 'F'; /* float */
							continue;
						}
						if(string[index + counter + 1] != '\0' || string[index + counter + 1] == '-' || string[index + counter + 1] == '+'){
							if(string[index + counter + 2] != '\0' || isdigit(string[index + counter + 2])){
								counter += setTokenizer(tok, string, index, counter);
								counter += setTokenizer(tok, string, index, counter);
								counter += numLoop(tok, string, index+counter);
							}
						}
					}
					index += counter-1;
					tok->state[index] = 'F'; /* float */
				}else if(string[index + counter] == 'x' || string[index + counter] == 'X'){
					if(string[index + counter + 1] != '\0' && isdigit(string[index + counter + 1])){
						counter += setTokenizer(tok, string, index, counter);
						counter += hexLoop(tok, string, index+counter);
					}else{
						index += counter-1;
						tok->state[index] = 'D'; /* decimal */
						continue;
					}
					index += counter-1;
					tok->state[index] = 'H'; /* hex */
				}else{
					index += counter-1;
					tok->state[index] = 'O'; /* octal */
				}
			}
		}else if(isspace(string[index])){
			tok->capturedString[index] = string[index];
			tok->state[index] = 'x'; /* for skipping spaces.*/
			continue;
		}else{
			if(ispunct(string[index])){
				tok->capturedString[index] = string[index];
				if(tok->capturedString[index] == '\"'){		/*EXTRA CREDIT QUOTATION TOKEN*/
					Qcounter++;
					if(Qcounter == 2){
						for(;Qindex < index; Qindex++){
							tok->state[Qindex] = '0';
						}
						tok->state[index] = 'Q'; /* for quotation token*/
						Qcounter = 0;
					}else{
						Qindex = index;
						tok->state[index] = 'P'; /*if there is only one Q instead of set, it will count as a 'P'*/
					}
				}else{
					tok->state[index] = 'P'; /* for operator token*/
				}
			}
			else
				break;
		}
	}
	tok->currentIndex = 0;
}

int alphaLoop(TokenizerT *tok, char *string, int index){
	int counter = 0;
	for(; isalpha(*tok->stringPtr); tok->stringPtr++){
		tok->capturedString[index + counter] = string[index + counter];
		tok->state[index + counter] = '0';
		counter++;
	}
	return counter;
}

int numLoop(TokenizerT *tok, char *string, int index){
	int counter = 0;
	for(; isdigit(*tok->stringPtr); tok->stringPtr++){
		tok->capturedString[index + counter] = string[index + counter];
		tok->state[index + counter] = '0';
		counter++;
	}
	return counter;
}

int hexLoop(TokenizerT *tok, char *string, int index){
	int counter = 0;
	for(; isxdigit(*tok->stringPtr); tok->stringPtr++){
		tok->capturedString[index + counter] = string[index + counter];
		tok->state[index + counter] = '0';
		counter++;
	}
	return counter;
}

int octLoop(TokenizerT *tok, char *string, int index){
	int counter = 0;
	
	for(; isdigit(*tok->stringPtr) && (*tok->stringPtr != '8' && *tok->stringPtr != '9'); tok->stringPtr++){
		tok->capturedString[index + counter] = string[index + counter];
		tok->state[index + counter] = '0';
		counter++;
	}
	return counter;
}

int setTokenizer(TokenizerT *tok, char *string, int index, int counter){
	tok->capturedString[index + counter] = string[index + counter];
	tok->state[index + counter] = '0';
	tok->stringPtr++;
	return 1;
}

int determineOperator(TokenizerT *tok, char *word){
	/* list of operator state machine*/
	switch(tok->capturedString[tok->currentIndex - 1]){
		case '(':
			printf("left parenthesis ");
			break;
		case ')':
			printf("right parenthesis ");
			break;
		case '[':
			printf("left bracket ");
			break;
		case ']':
			printf("right bracket ");
			break;
		case '.':
			printf("period ");
			break;
		case ',':
			printf("comma ");
			break;
		case '~':
			printf("tilde ");
			break;
		case '?':
			printf("question mark ");
			break;
		case ':':
			printf("colon ");
			break;
		case '-':
			switch(tok->capturedString[tok->currentIndex]){
				case '>':
					printf("structure pointer ");
					addtoWord(tok, word, ">");
					break;
				case '=':
					printf("minus-equals ");
					addtoWord(tok, word, "=");
					break;
				case '-':
					printf("decrement ");
					addtoWord(tok, word, "-");
					break;
				default:
					printf("minus ");
					break;
			}
			break;
		case '+':
			switch(tok->capturedString[tok->currentIndex]){
				case '=':
					printf("plus-equals ");
					addtoWord(tok, word, "=");
					break;
				case '+':
					printf("increment ");
					addtoWord(tok, word, "+");
					break;
				default:
					printf("plus ");
					break;
			}
			break;
		case '*':
			if(tok->capturedString[tok->currentIndex] == '='){
				printf("multiply-equals ");
				addtoWord(tok, word, "=");
			}else{
				printf("multiply ");
			}
			break;
		case '/':
			switch(tok->capturedString[tok->currentIndex]){
				case '=':
					printf("divide-equals ");
					addtoWord(tok, word, "=");
					break;
				case '/':	/*******************************************************************************EXTRA CREDIT*/
					printf("comment ");
					addtoWord(tok, word, "/");
					break;
				default:
					printf("divide ");
					break;
			}
			break;
		case '%':
			if(tok->capturedString[tok->currentIndex] == '='){
				printf("modulus-equals ");
				addtoWord(tok, word, "=");
			}else{
				printf("modulus ");
			}
			break;
		case '>':
			switch(tok->capturedString[tok->currentIndex]){
				case '>':
					addtoWord(tok, word, ">");
					if(tok->capturedString[tok->currentIndex + 1] == '='){
						printf("shift right-equals ");
						addtoWord(tok, word, "=");
					}else{
						printf("shift-right ");
					}
					break;
				case '=':
					printf("greater or equal ");
					addtoWord(tok, word, "=");
					break;
				default:
					printf("greater than ");
					break;
			}
			break;
		case '<':
			switch(tok->capturedString[tok->currentIndex]){
				case '<':
					addtoWord(tok, word, "<");
					if(tok->capturedString[tok->currentIndex + 1] == '='){
						printf("shift left-equals ");
						addtoWord(tok, word, "=");
					}else{
						printf("shift-left ");
					}
					break;
				case '=':
					printf("less or equal ");
					addtoWord(tok, word, "=");
					break;
				default:
					printf("less than ");
					break;
			}
			break;
		case '=':
			if(tok->capturedString[tok->currentIndex] == '='){
				printf("equals ");
				addtoWord(tok, word, "=");
			}else{
				printf("assign ");
			}
			break;
		case '!':
			if(tok->capturedString[tok->currentIndex] == '='){
				printf("not equals ");
				addtoWord(tok, word, "=");
			}else{
				printf("negate ");
			}
			break;
		case '&':
			switch(tok->capturedString[tok->currentIndex]){
				case '&':
					printf("AND comparison ");
					addtoWord(tok, word, "&");
					break;
				case '=':
					printf("bitwise AND-equals ");
					addtoWord(tok, word, "=");
					break;
				default:
					printf("bitwise AND ");
					break;
			}
			break;
		case '^':
			if(tok->capturedString[tok->currentIndex] == '='){
				printf("bitwise exclusive OR-equals ");
				addtoWord(tok, word, "=");
			}else{
				printf("bitwise exclusive OR ");
			}
			break;
		case '|':
			switch(tok->capturedString[tok->currentIndex]){
				case '|':
					printf("OR comparison ");
					addtoWord(tok, word, "|");
					break;
				case '=':
					printf("bitwise OR-equals ");
					addtoWord(tok, word, "=");
					break;
				default:
					printf("bitwise OR ");
					break;
			}
			break;
		case '\\':
			switch(tok->capturedString[tok->currentIndex]){
				case 'n':
					printf("new-line character ");
					addtoWord(tok, word, "n");
					break;
				case 't':
					printf("tab character ");
					addtoWord(tok, word, "t");
					break;
				case 'b':
					printf("backspace character ");
					addtoWord(tok, word, "b");
					break;
				case '\'':
					printf("single quote character ");
					addtoWord(tok, word, "'");
					break;
				case 'r':
					printf("carriage return character ");
					addtoWord(tok, word, "r");
					break;
				case 'f':
					printf("form feed character ");
					addtoWord(tok, word, "f");
					break;
				case '\\':
					printf("backslash character ");
					addtoWord(tok, word, "\\");
					break;
				case 'd':
					if(tok->capturedString[tok->currentIndex + 1] == 'd'){
						if(tok->capturedString[tok->currentIndex + 2] == 'd'){
							printf("octal constant character ");
							addtoWord(tok, word, "d");
							addtoWord(tok, word, "d");
							addtoWord(tok, word, "d");
						}
					}else{
						printf("single backslash ");
						break;
					}
				default:
					printf("single backslash ");
					break;
			}
			break;
		case ';':
			printf("semi-colon ");
			break;
		case '#':
			printf("hash-tag ");
			break;
		default:
			printf("ERROR: [0x");
			hexError(word[0]);
			printf("]\n");
			return 0;
	}
	return 1;
}

void addtoWord(TokenizerT *tok, char *word, char *op){
	strcat(word, op);
	tok->currentIndex++;
}

void hexError(int x){
	int y = x/16;
	switch(y){
		case 10:
			printf("a");
			break;
		case 11:
			printf("b");
			break;
		case 12:
			printf("c");
			break;
		case 13:
			printf("d");
			break;
		case 14:
			printf("e");
			break;
		case 15:
			printf("f");
			break;
		default:
			printf("%d", y);
			break;
	}
	x = x-(y*16);
	switch(x){
		case 10:
			printf("a");
			break;
		case 11:
			printf("b");
			break;
		case 12:
			printf("c");
			break;
		case 13:
			printf("d");
			break;
		case 14:
			printf("e");
			break;
		case 15:
			printf("f");
			break;
		default:
			printf("%d", x);
			break;
	}
}

void userGuide(){
	printf("\n\nThe tokenizer program accepts one string argument:\n");
	printf("./tokenizer \"INSERT_STRING_HERE\"\n");
	printf("be sure to add the quotation marks!\n");
}
