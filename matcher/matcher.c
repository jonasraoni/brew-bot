/*
 * BrewBot: API tester for devices using the platform Qualcomm BREW.
 * Jonas Raoni Soares da Silva <http://raoni.org>
 * https://github.com/jonasraoni/brew-bot
 */

//-- includes -----------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <io.h>
#include "memory-block.c"
#include "common.c"


#define GENERATE_OCURRENCES_FOR_EACH_FILE


//-- main ---------------------------------------------------
int main(int argc, char* argv[]){

//-- vars ---------------------------------------------------
	char
		curType,
		*curChar,
		*line,
		*inFilename,
		*outFilename;

	MemoryBlock
		tokens = getMemoryBlock( sizeof( Token* ), 1024 ),
		files = getMemoryBlock( sizeof( char* ), 32 ),
		matchedTokens = getMemoryBlock( sizeof( TokenMatch* ), 128 ),
		curTokenMatchLines;

	Token *curToken;
	TokenMatch *curTokenMatch;

	FILE *fp, *htmlFile;

	int x, tokenLen, lineCount, tokenCount, matched;


//-- body----------------------------------------------------
	if( argc < 3 ){
		printf( "\n\nmatcher tokens_filename file_mask [file_mask file_mask ...]\n\n" );
		return 0;
	}

	printf( "\n\n############################ INITIALIZING PROCESS ############################\n\n" );
	printf( "# Creating list of files\n" );


	argc -= 2;
	while( argc-- )
		addFiles( argv[argc+2], files );
	printf( "> Found files: %d\n", files->used );


	printf( "\n# Processing tokens file\n" );

	switch( processTokens( argv[1], tokens ) ){
		case -1: return printf( "$ Error while opening tokens file\a" );
		case  0: return printf( "$ There's no tokens to process\a" );
		default: printf( "> Processed tokens: %d\n", tokens->used );
	}

	while( files->used ){

		lineCount = 0;

		if( !( fp = fopen( inFilename = *((char**)files->data + --files->used), "r" ) ) ) {
			printf( "$ Failed to open: %s\n\a", inFilename );
			free( inFilename );
			system( "pause" );
			continue;
		}

#ifdef GENERATE_OCURRENCES_FOR_EACH_FILE
		curChar = ( curChar = strrchr( inFilename, '\\' ) ) || ( curChar = strrchr( inFilename, '/' ) ) ? ++curChar : inFilename;
		assert( outFilename = (char*)malloc( strlen( curChar ) + 5 ) );
		strcat( strcpy( outFilename, curChar ), ".txt" );

		if( !( htmlFile = fopen( outFilename, "w" ) ) ) {
			printf( "$ Failed to create: %s\n\a", outFilename );
			free( outFilename );
			free( inFilename );
			fclose( fp );
			continue;
		}		
#endif

		printf( "\n\n# Processing: \"%s\"\n"
				"> Remaining: %d file(s)\n"
				"> Matching tokens\n", inFilename, files->used );

		while( x = readLine( fp, &line ) ) {

			if( x == -1 ) {
				printf( "$ SKIPPING BINARY FILE\n\a" );
				break;
			}
			++lineCount;

			tokenCount = tokens->used;

			while( tokenCount ) {

				matched = false;
				curTokenMatchLines = NULL;
				curChar = line;

   				tokenLen = strlen( ( curToken = *((Token**)tokens->data + --tokenCount) )->name );

				while( curChar = strstr( curChar, curToken->name ) ) {

					if( isWhiteSpace( curChar[tokenLen] ) && isWhiteSpace( *(curChar-1) ) ) {
						if( !matched ){

							curTokenMatch = NULL;
							x = matchedTokens->used;

							while( x )
								if( !strcmp( ((TokenMatch**)matchedTokens->data)[--x]->token->name, curToken->name ) )
									curTokenMatch = ((TokenMatch**)matchedTokens->data)[x];
							if( !curTokenMatch ){
								curTokenMatch = *((TokenMatch**)matchedTokens->data + matchedTokens->used++) = (TokenMatch*)malloc( sizeof( TokenMatch ) );
								curTokenMatch->token = curToken;
#ifdef GENERATE_OCURRENCES_FOR_EACH_FILE
								curTokenMatch->lines = getMemoryBlock( sizeof( int ), 10 );
#endif
								assert( incMemoryBlock( matchedTokens ) );
							}
							matched = true;
#ifdef GENERATE_OCURRENCES_FOR_EACH_FILE
							curTokenMatchLines = curTokenMatch->lines;
#endif
						}
#ifdef GENERATE_OCURRENCES_FOR_EACH_FILE
						*((int*)curTokenMatchLines->data+curTokenMatchLines->used++) = lineCount;
						assert( incMemoryBlock( curTokenMatchLines ) );
#endif
					}
					curChar = curChar + tokenLen;
				}
			}
		}

		printf( "> Processed lines: %d\n", lineCount );

#ifdef GENERATE_OCURRENCES_FOR_EACH_FILE
		x = matchedTokens->used;		

		printf( "> Found tokens: %d\n", x );

		printf( "> Ordering tokens\n" );
		qsort( matchedTokens->data, x, sizeof( TokenMatch* ), sortMatchedTokens );

		curType = -1;

		while( x ){
			curTokenMatch = *((TokenMatch**)matchedTokens->data+--x);
			int j = curTokenMatch->lines->used;

			if( curType != curTokenMatch->token->type ){
				curType = curTokenMatch->token->type;
				fprintf( htmlFile, "\n%s\n", typeMap[curType] );
			}
			fprintf( htmlFile, "%s\t%s\t%d\t", curTokenMatch->token->name, curTokenMatch->token->path, curTokenMatch->token->line );

			for( ; --j; fprintf( htmlFile, "%d, ", *((int*)curTokenMatch->lines->data+j) ) );

			if( !j )
				fprintf( htmlFile, "%d", *((int*)curTokenMatch->lines->data) );
			fprintf( htmlFile, "\n" );
			freeMemoryBlock( curTokenMatch->lines );
			free( curTokenMatch );
		}
		printf( "> Creating output\n");

		fclose( htmlFile );
		fclose( fp );

		if( !matchedTokens->used ) {
			printf( "> Removing output file" );
			unlink( outFilename );
		}

		free( inFilename );
		free( outFilename );
        matchedTokens->used = 0;
#else
		fclose( fp );
		free( inFilename );
#endif
	}

#ifndef GENERATE_OCURRENCES_FOR_EACH_FILE
	if( !( htmlFile = fopen( "output.txt", "w" ) ) ) {
		printf( "$ Failed to create output file.\n\a" );
		return 1;
	}

	x = matchedTokens->used;
	printf( "> Ordering tokens\n" );
	qsort( matchedTokens->data, x, sizeof( TokenMatch* ), sortMatchedTokens );

	curType = -1;

	printf( "\n\n> Creating output\n");
	while( x ){
		curTokenMatch = *((TokenMatch**)matchedTokens->data+--x);

		if( curType != curTokenMatch->token->type ){
			curType = curTokenMatch->token->type;
			fprintf( htmlFile, "\n%s\n", typeMap[curType] );
		}
		fprintf( htmlFile, "%s\t%s\t%d\n", curTokenMatch->token->name, curTokenMatch->token->path, curTokenMatch->token->line );

		free( curTokenMatch );
	}

	fclose( htmlFile );

#endif

	printf( "\n\n# Freeing up resources\n");

	while( tokens->used ){
		curToken = ((Token**)tokens->data)[--tokens->used];
		free( curToken->name );
		free( curToken->path );
		free( curToken );
	}

	freeMemoryBlock( matchedTokens );
	freeMemoryBlock( files );
	freeMemoryBlock( tokens );

	printf( "\n\n############################## PROCESS FINISHED ##############################\n\n" );

	//printf( "\a\a\a" );

	return 0;
}
