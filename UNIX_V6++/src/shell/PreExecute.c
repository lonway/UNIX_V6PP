#include "globe.h"
#include "stdio.h"
#include "PreExecute.h"

#define IsKeyToken( ch ) \
		( (ch) == ';' || \
		 (ch) == '|'  || \
		 (ch) == '&'  || \
		 (ch) == '>'  || \
		 (ch) == '<'  || \
		 (ch) == '('  || \
		 (ch) == ')' )

#define IsSpace( ch ) (ch) == ' '

//将命令分解到args数组, keyToken也保存到args
int SpiltCommand( char* input )
{
	char* pline = line;

	int nargs = 0;

	char* pinput = input;
	char* pKeyToken = input;

	while ( *pKeyToken )
	{
		pinput = TrimLeft( pinput );

		pKeyToken = NextKeyToken(pinput);
		if ( pKeyToken > pinput ) 
		{	
			args[ nargs++ ] = pline;
			while ( pinput < pKeyToken )
			{
				if ( *pinput == '\\' && ( IsKeyToken( *(pinput+1) ) || *(pinput+1) == '\\' ) )
					++pinput;
				*pline++ = *pinput++;
			}
			*pline++ = 0;
		}
		
		if ( IsKeyToken( *pKeyToken ) )
		{
			args[ nargs++ ] = pline;
			*pline++ = *pKeyToken;
			*pline++ = 0;
		}
		pinput = pKeyToken + 1;
	}
	return nargs;
}

//获取下一个词条的首地址
char* NextKeyToken(char* input)
{
	char* p = input;
	while ( *p )
	{
		//遇到keyToken和空格则当前词条结束
		if ( IsKeyToken(*p) )
		{
			if ( *(p-1) == '\\' ) 
			{
				p += 2;	//??????
				continue;
			}
			else break;
		}
		else if ( IsSpace(*p) ) break;
		p++;
	}
	return p;
}

/* 去掉左边多余的空格 */
char* TrimLeft( char* tp )
{
	char* p = tp;
	while ( IsSpace(*p) && *p ) ++p;	/* 去掉多余的空格 */
	return p;
}
