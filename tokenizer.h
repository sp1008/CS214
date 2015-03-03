#ifndef TOKENIZER_H
#define TOKENIZER_H

struct TokenizerT_ {
	char *capturedString;
	int currentIndex;
	char *stringPtr;
	char *state;
};

typedef struct TokenizerT_ TokenizerT;

TokenizerT *TKCreate(char *);
void TKDestroy(TokenizerT *);
char *TKGetNextToken(TokenizerT *);
void printToken(TokenizerT *, char *);
void stateMachine(TokenizerT *, char *);
int alphaLoop(TokenizerT *, char *, int);
int numLoop(TokenizerT *, char *, int);
int hexLoop(TokenizerT *, char *, int);
int octLoop(TokenizerT *, char *, int);
int setTokenizer(TokenizerT *, char *, int, int);
int determineOperator(TokenizerT *, char *);
void addtoWord(TokenizerT *, char *, char *);
void hexError(int);
void userGuide();

#endif
