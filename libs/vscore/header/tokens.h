typedef union {
    int				int32;	
    int				id;           /* a Hashtable key entry or vector position corresponding to a symbol added from the lexer */
    Node			*node;        /* e.g. an SFNdode */
    NodeList		*nodeList;    /* an MFNode (NodeList is derived from Array<Node*>) */
    Element			*element;     /* base abstract class of Field,EventIn,EventOut,ExposedField */
    FieldValue		*value;       /* base abstract class of SFFloat, SFString, SFBool, MFString etc..*/
    float			sffloat;      /* an SFFloat,float */
    StringArray		*stringArray; /* MFString (Array<String>)*/
    IntArray		*intArray;    /* MFInt32 (Array<int>*/
    FloatArray		*floatArray;  /* MFFloat (Array<float>*/
} YYSTYPE;
#define	ID	258
#define	STRING	259
#define	INT	260
#define	FLOAT	261
#define	SCRIPT	262
#define	DEF	263
#define	EXTERNPROTO	264
#define	FALSE_TOK	265
#define	IS	266
#define	NULL_TOK	267
#define	PROTO	268
#define	ROUTE	269
#define	TO	270
#define	TRUE_TOK	271
#define	USE	272
#define	EVENT_IN	273
#define	EVENT_OUT	274
#define	EXPOSED_FIELD	275
#define	FIELD	276


extern YYSTYPE yylval;
