//CiLisp
//Edgar Ramirez

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
    node->data.number.type = type;
    node->data.number.value = value;

    return node;
}


void setParent(AST_NODE *parent, AST_NODE *child)
{
    if(child != NULL)
        child->parent = parent;
}


AST_NODE *createFunctionNode(char *funcName, AST_NODE *opList)
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
    node->type = FUNC_NODE_TYPE;
    node->data.function.oper = resolveFunc(funcName);
    node->data.function.opList = opList;
    setParent(node, opList);
    return node;
}

AST_NODE *createSymbolNode(char *ident)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    node->type = SYM_NODE_TYPE;
    node->data.symbol.ident = ident;

    return node;
}


SYMBOL_TABLE_NODE *createSymbolTableNode(char *ident, AST_NODE *val, NUM_TYPE typeNum)
{
    SYMBOL_TABLE_NODE *symTabNode = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    if(symTabNode == NULL)
    {
        exit(EXIT_FAILURE+1);
    }

    symTabNode->val_type = typeNum;
    symTabNode->ident = ident;
    symTabNode->val = val;

    return symTabNode;
}

AST_NODE *setSymbolTable(SYMBOL_TABLE_NODE *symbolTable, AST_NODE * node)
{
    SYMBOL_TABLE_NODE **scope = &(node->symbolTable);
    if(*scope != NULL)
        scope = &((**scope).next);
    *scope = symbolTable;

    return node;
}

SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *let_list, SYMBOL_TABLE_NODE *let_element)
{
    if(let_list == NULL)
        return let_element;

    SYMBOL_TABLE_NODE *iter = let_list;
    while (iter->next != NULL)
    {
        iter = iter->next;
    }
    iter->next = let_element;

    return let_list;
}

AST_NODE *addOpToList (AST_NODE *op, AST_NODE *opList)
{
    if(opList == NULL)
        return op;

    AST_NODE *iter = opList;
    while (iter->next != NULL)
    {
        iter = iter->next;
    }
    iter->next = op;

    return opList;
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
        freeNode(node->data.function.opList);

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
            break;
        case SYM_NODE_TYPE:
            result = evalSymNode(node);
            break;
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
            result.type = INT_TYPE;
            result.value = numNode->value;
            break;
        case DOUBLE_TYPE:
            result.type = DOUBLE_TYPE;
            result.value = numNode->value;
            break;
        default:
            yyerror("ERROR IN EvalNumNode, POSSIBLE WRONG VALUE IN VALUE");
            break;
    }

    return result;
}


RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode)
{
    if (!funcNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};
    RET_VAL op1 = eval(funcNode->op1);
    RET_VAL op2 = eval(funcNode->op2);

    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE
    switch(funcNode->oper)
    {
        case NEG_OPER:
            result.type = op1.type;
            result.value = -(op1.value);
            break;
        case ABS_OPER:
            result.type = op1.type;
            result.value = fabs(op1.value);
            break;
        case EXP_OPER:
            result.type = op1.type;
            result.value = exp(op1.value);
            break;
        case SQRT_OPER:
            result.type = op1.type;
            result.value = sqrt(op1.value);
            break;
        case ADD_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = (op1.value + op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = (op1.value + op2.value);
            }
            break;
        case SUB_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = (op1.value - op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = (op1.value - op2.value);
            }
            break;
        case MULT_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = (op1.value * op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = (op1.value * op2.value);
            }
            break;
        case DIV_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = (op1.value / op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = (op1.value / op2.value);
            }
            break;
        case REMAINDER_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = fmod(op1.value,op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = fmod(op1.value,op2.value);
            }
            break;
        case LOG_OPER:
            result.type = op1.type;
            result.value = log(op1.value);
            break;
        case POW_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = pow(op1.value,op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = pow(op1.value,op2.value);
            }
            break;
        case MAX_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = fmax(op1.value,op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = fmax(op1.value,op2.value);
            }
            break;
        case MIN_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = fmin(op1.value,op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = fmin(op1.value,op2.value);
            }
            break;
        case EXP2_OPER:
            result.type = op1.type;
            result.value = sqrt(op1.value);
            break;
        case CBRT_OPER:
            result.type = op1.type;
            result.value = cbrt(op1.value);
            break;
        case HYPOT_OPER:
            if(op1.type == INT_TYPE && op2.type == INT_TYPE)
            {
                result.type = INT_TYPE;
                result.value = hypot(op1.value,op2.value);
            }

            if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = hypot(op1.value,op2.value);
            }
            break;
        case PRINT_OPER:
            result.value = eval(funcNode->opList).value;
            break;
        default:
            yyerror("In EvalFuncNode, THERE IS NO CASE TO POPULATE RESULT");
    }
    return result;
}

RET_VAL evalSymNode(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, NAN};

    SYMBOL_TABLE_NODE *iter = findSymbol(node->data.symbol.ident, node);

    if(iter == NULL)
    {
        printf("ERROR");
        return (RET_VAL) {INT_TYPE, NAN};
    }

    RET_VAL result = eval(iter->val);

    if(iter->val_type == INT_TYPE && iter->val->data.number.type == DOUBLE_TYPE)
    {
        result.value = floor(result.value);
        printf("WARNING: precision loss in the assignment for variable <%s> \n", node->data.symbol.ident);
    }

    return result;

}

SYMBOL_TABLE_NODE * findSymbol(char *ident, AST_NODE *symNode)
{
    if(symNode == NULL)
        return NULL;

    SYMBOL_TABLE_NODE *iter = symNode->symbolTable;

    while(iter!= NULL)
    {
        if(strcmp(ident, iter->ident) == 0)
        {
            return iter;
        }
        iter = iter->next;
    }

    return findSymbol(ident, symNode->parent);
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO print the type and value of the value passed in.
    switch(val.type)
    {
        case INT_TYPE:
            printf("%.0lf", round(val.value));
            break;
        case DOUBLE_TYPE:
            printf("%lf", (val.value));
            break;
        default:
            yyerror("ERROR IN PrintRetVal, NOT DETECTING CASE TYPE");
    }
}
