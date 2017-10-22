/*
 * BrewBot: API tester for devices using the platform Qualcomm BREW.
 * Jonas Raoni Soares da Silva <http://raoni.org>
 * https://github.com/jonasraoni/brew-bot
 */

#ifndef __COMMON__
#define __COMMON__

#include "memory-block.c"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>

//-- consts -------------------------------------------------
//ctags types from "a..x"
const char* const typeMap[] = {
	"#", "#",
	"class",
	"macro definition",
	"enumerator",
	"function",
	"enumeration", "#", "#", "#", "#",
	"local variable",
	"member of class/structure/union",
	"namespace", "#",
	"function prototype", "#", "#",
	"structure",
	"type definition",
	"union name",
	"global variable", "#",
	"external variable"
};

//-- data types ---------------------------------------------
typedef struct {
	int	line;
	char *name, *path, type;
} Token;

typedef struct {
	Token *token;
	MemoryBlock lines;
} TokenMatch;

//-- functions ----------------------------------------------
//returns the folder
char *getFilePath( char *filename ){
	int x;
	char *pos = filename + strlen( filename );

	while( *pos != '\\' && *pos != '/' && --pos != filename );

	x = ( pos += filename != pos ) - filename;

	assert( pos = (char*)malloc( x+1 ) );
	memcpy( pos, filename, x );
	pos[x] = 0;
	return pos; 
}

//reads a line and attributes to "buffer" in the format "\0CONTENT\0"
//returns -1 if it's a binary file
//returns 0 if it's not EOF
//the buffer is static and shared across calls to improve performance
int readLine( FILE *fp, char **buffer ){
	static MemoryBlock line = getMemoryBlock( sizeof( char ), 1024 );
	static char tempChar;
	static char *curChar;

	*(char*)line->data = !(line->used = 1);
	while( fread( curChar = (char*)line->data + line->used++, sizeof( char ), 1, fp ) ) {
		tempChar = getc( fp );

		if( !tempChar )
			return -1;
		/*
		if( tempChar == EOF || ( ungetc( tempChar, fp ) && *curChar == '\n' ) ){
			*(curChar + (tempChar == EOF) ) = 0;
			*buffer = (char*)line->data + 1;
			return line->used - (tempChar != EOF);
		}
		*/

		if( tempChar == '\n' || ( *curChar == '\n' && ungetc( tempChar, fp ) && !(*curChar=0) ) || tempChar == EOF || !ungetc( tempChar, fp ) ){
			/********************************************************
			buffer = (char*)malloc( line->used - (tempChar != EOF) );
			strcpy( *buffer, (char*)line->data+1 );
			********************************************************/
			assert( incMemoryBlock( line ) );
			*(curChar + 1 ) = 0;
			*buffer = (char*)line->data + 1;
			return line->used;

		}
		assert( incMemoryBlock( line ) );
	}
	return 0;
}

//returns 1 when the character is a whitespace or code delimiter
int isWhiteSpace( register const char c ){
	static const char* const whiteSpace = " \t\r\t!\"#$%&'()*+-/:;<>=?@[]\\^`{}|~.\0";
	static const int len = strlen( whiteSpace )+1;
	return memchr( whiteSpace, c, len ) != NULL;
}

//given a mask/wildcard, search files and add their paths to files
void addFiles( char *mask, MemoryBlock files ){
    struct _finddata_t findInfo;
    long int curFile;
	if ( ( curFile = _findfirst( mask, &findInfo ) ) + 1 ) {
		char *basePath = getFilePath( mask );
		do
			if( *findInfo.name != '.' && !( findInfo.attrib & _A_SUBDIR ) ) {
				strcat( strcpy( *((char**)files->data+files->used++) = (char*)malloc( strlen( basePath ) + strlen( findInfo.name ) + 1 ), basePath ), findInfo.name );
				assert( incMemoryBlock( files ) );
			}
		while ( _findnext( curFile, &findInfo ) == 0 );
		free( basePath );
	}
	_findclose( curFile );
};

//orders the tokens
int sortMatchedTokens( const void *a, const void*b ){
	int i = (*(TokenMatch**)b)->token->type - (*(TokenMatch**)a)->token->type;
	return i ? i : strcmp( (*(TokenMatch**)b)->token->name, (*(TokenMatch**)a)->token->name );
}

//generates a token list from a file and saves them on the parameter tokens
int processTokens( char *filename, MemoryBlock tokens ){
	int x;
	Token *curToken;
	FILE *fp;
	char *line;
	if( !( fp = fopen( filename, "r" ) ) )
		return -1;
	while( readLine( fp, &line ) ){
		if( *line != '!' && strtok( line, "\t" ) ) {
			x = 0;
			curToken = *((Token**)tokens->data + tokens->used++) = (Token*)malloc( sizeof( Token ) );
			curToken->name = strcpy( (char*)malloc( strlen( line ) + 1 ), line );
			while( ( line = strtok( NULL, "\t" ) ) ){
				switch( ++x ){
					case 1: curToken->path = strcpy( (char*)malloc( strlen( line ) + 1 ), line ); break;
					case 2: curToken->line = atoi( line ); break;
					case 3: curToken->type = *line-'a'; break;
				}
			}
			assert( incMemoryBlock( tokens ) );
		}
	}
	fclose( fp );
	return tokens->used;
}

#endif
