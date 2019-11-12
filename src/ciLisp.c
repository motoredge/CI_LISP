#include "ciLisp.h"

void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

// Array of string values for operations.
// Must be in sync with funcs in the OPER_TYPE enum in order for resolveFunc to work.
char *funcNames[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "read",
        "rand",
        "print",
        "equal",
        "less",
        "greater",
        ""
};

OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (funcNames[i][0] != '\0')
    {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(double value, NUM_TYPE type)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE
    node->type = NUM_NODE_TYPE;
    switch(node->type)
    {
        case INT_TYPE:
            node->data.number.value.ival = (long) value;
            break;
        case DOUBLE_TYPE:
            node->data.number.value.dval = value;
            break;
        default:
            yyerror("AST_NODE in createNumberNode unable to discern NUM_TYPE")
            break;
    }

    return node;
}

// Called when an f_expr is created (see ciLisp.y).
// Creates an AST_NODE for a function call.
// Sets the created AST_NODE's type to function.
// Populates the contained FUNC_AST_NODE with:
//      - An OPER_TYPE (the enum identifying the specific function being called)
//      - 2 AST_NODEs, the operands
// SEE: AST_NODE, FUNC_AST_NODE, AST_NODE_TYPE.
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, populate contained FUNC_AST_NODE
    // NOTE: you do not need to populate the "ident" field unless the function is type CUSTOM_OPER.
    // When you do have a CUSTOM_OPER, you do NOT need to allocate and strcpy here.
    // The funcName will be a string identifier for which space should be allocated in the tokenizer.
    // For CUSTOM_OPER functions, you should simply assign the "ident" pointer to the passed in funcName.
    // For functions other than CUSTOM_OPER, you should free the funcName after you're assigned the OPER_TYPE.
    node->data.function.oper = resolveFunc(funcName);
    node->data.function.op1 = op1;
    node->data.function.op2 = op2;

    switch(node->type)
    {
        case CUSTOM_OPER:
            node->type = CUSTOM_OPER;
            node->data.function.ident = funcName;
            break;
        default:
            free(funcName);
            break;
    }

    return node;
}

// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node)
{
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE)
    {
        // Recursive calls to free child nodes
        freeNode(node->data.function.op1);
        freeNode(node->data.function.op2);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER)
        {
            free(node->data.function.ident);
        }
    }

    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch.
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type)
    {
        case FUNC_NODE_TYPE:
            result = evalFuncNode(&node->data.function);
            break;
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}

// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(NUM_AST_NODE *numNode)
{
    if (!numNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the values stored in the node.
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE
    switch(numNode->type)
    {
        case INT_TYPE:
            result = (RET_VAL) (numNode->value.ival);
            break;
        case DOUBLE_TYPE:
            result = (RET_VAL) (numNode->value.dval);
            break;
        default:
            yyerror("ERROR IN EVALNUMNODE, POSSIBLE WRONG VALUE IN VALUE");
            break;
    }

    return result;
}


RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode)
{
    if (!funcNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE
    }
    switch(funcNode->oper)
    {
        case NEG_OPER:
            result = (- (funcNode->op1));
            break;
        case ABS_OPER:
            result = abs(funcNode->op1);
        case EXP_OPER:
            result = exp(funcNode->op1);
            break;
        case SQRT_OPER:
            result = sqrt(funcNode->op1);
            break;
        case ADD_OPER:
            result = (funcNode->op1) + (funcNode->op2);
            break;
        case SUB_OPER:
            result = (funcNode->op1) - (funcNode->op2);
            break;
        case MULT_OPER:
            result = (funcNode->op1) * (funcNode->op2);
            break;
        case DIV_OPER:
            result = (funcNode->op1) / (funcNode->op2);
            break;
        case REMAINDER_OPER:
            result = fmod(funcNode->op1, funcNode->op2);
            break;
        case LOG_OPER:
            result = log10(funcNode->op1);
            break;
        case POW_OPER:
            result = pow(funcNode->op1, funcNode->op2);
            break;
        case MAX_OPER:
            result = fmax(funcNode->op1, funcNode->op2);
            break;
        case MIN_OPER:
            result = fmin(funcNode->op1, funcNode->op2);
            break;
        case EXP2_OPER:
            result = exp2(funcNode->op1);
            break;
        case CBRT_OPER:
            result = cbrt(funcNode->op1);
            break;
        case HYPOT_OPER:
            result = hypot(funcNode->op1, funcNode->op2);
            break;
        default:
            yyerror("IN EVALFUNCNODE, THERE IS NO CASE TO POPULATE RESULT");
    }

    return result;
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO print the type and value of the value passed in.
    switch(val.type)
    {
        case DOUBLE_TYPE:
            printf("%s", val.type);
            printf("%d", val.value.dval);
            break;
        case INT_TYPE:
            printf("%s", val.type);
            printf("%l", val.value.ival);
        default:
            yyerror("ERROR IN PRINTRETVAL, NOT DETECTING CASE TYPE");
            break;
    }

}
