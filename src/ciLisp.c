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
    if(typeNum == false) {
        symTabNode->val_type = DOUBLE_TYPE;
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

    op->next = opList;

    return op;
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

    RET_VAL result = {INT_TYPE, 0};

    if(funcNode->opList == false)
    {
        printf("ERROR: too few parameters for the function \n");
        return (RET_VAL) {INT_TYPE, NAN};
    }

    RET_VAL op1 = eval(funcNode->opList);
    RET_VAL op2 = eval(funcNode->opList->next);


    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE
    switch(funcNode->oper)
    {
        case NEG_OPER:
            if(singleOp("neg", funcNode).type == DOUBLE_TYPE)
            {
                if(op1.type == INT_TYPE)
                {
                    result.type = INT_TYPE;
                    result.value = -(op1.value);
                }
                else{
                    result.type = DOUBLE_TYPE;
                    result.value = -(op1.value);
                }
            }
            else
                singleOp("neg", funcNode);
            break;
        case ABS_OPER:
            if(singleOp("abs", funcNode).type == DOUBLE_TYPE)
            {
                if(op1.type == INT_TYPE)
                {
                    result.type = INT_TYPE;
                    result.value = fabs(op1.value);
                }
                else{
                    result.type = DOUBLE_TYPE;
                    result.value = fabs(op1.value);
                }
            }
            else
                singleOp("abs", funcNode);
            break;
        case EXP_OPER:
            if(singleOp("exp", funcNode).type == DOUBLE_TYPE)
            {
                if(op1.type == INT_TYPE)
                {
                    result.type = INT_TYPE;
                    result.value = exp(op1.value);
                }
                else{
                    result.type = DOUBLE_TYPE;
                    result.value = exp(op1.value);
                }
            }
            else
                singleOp("exp", funcNode);

            break;
        case SQRT_OPER:
            if(singleOp("sqrt", funcNode).type == DOUBLE_TYPE)
            {
                if(op1.type == INT_TYPE)
                {
                    result.type = INT_TYPE;
                    result.value = sqrt(op1.value);
                }
                else{
                    result.type = DOUBLE_TYPE;
                    result.value = sqrt(op1.value);
                }
            }
            else
                singleOp("sqrt", funcNode);
            break;
        case ADD_OPER:
            //nOps("add", funcNode);
            return addHelperFunc(funcNode);
        case SUB_OPER:
            doubleOps("sub", funcNode);
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
            //nOps("mult", funcNode);
            return multHelperFunc(funcNode);
        case DIV_OPER:
            doubleOps("div", funcNode);
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
            doubleOps("remainder", funcNode);
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
            if(singleOp("log", funcNode).type == DOUBLE_TYPE)
            {
                if(op1.type == INT_TYPE)
                {
                    result.type = INT_TYPE;
                    result.value = log(op1.value);
                }
                else{
                    result.type = DOUBLE_TYPE;
                    result.value = log(op1.value);
                }
            }
            else
                singleOp("log", funcNode);
            break;
        case POW_OPER:
            doubleOps("pow", funcNode);
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
            return maxHelperFunc(funcNode);
        case MIN_OPER:
            return minHelperFunc(funcNode);
        case EXP2_OPER:
            if(singleOp("exp2", funcNode).type == DOUBLE_TYPE)
            {
                if(op1.type == INT_TYPE)
                {
                    result.type = INT_TYPE;
                    result.value = exp2(op1.value);
                }
                else{
                    result.type = DOUBLE_TYPE;
                    result.value = exp2(op1.value);
                }
            }
            else
                singleOp("exp2", funcNode);
            break;
        case CBRT_OPER:
            if(singleOp("cbrt", funcNode).type == DOUBLE_TYPE)
            {
                if(op1.type == INT_TYPE)
                {
                    result.type = INT_TYPE;
                    result.value = cbrt(op1.value);
                }
                else{
                    result.type = DOUBLE_TYPE;
                    result.value = cbrt(op1.value);
                }
            }
            else
                singleOp("cbrt", funcNode);
            break;
        case HYPOT_OPER:
            return hypotHelperFunc(funcNode);
        case PRINT_OPER:
            result.type = eval(funcNode->opList).type;
            result.value = eval(funcNode->opList).value;
            if((funcNode->opList))
            {
                result.value = eval(funcNode->opList).value;
                funcNode->opList = funcNode->opList->next;
                return result;
            }
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

int evalOpList (AST_NODE *opList)
{
    int numOps = 0;

    while(opList != NULL)
    {
        numOps++;
        opList = opList->next;
    }
    return numOps;
}

RET_VAL singleOp (char *funcName, FUNC_AST_NODE *funcNode)
{
    int numOps = evalOpList(funcNode->opList);
    if(numOps == 0) {
        printf("ERROR: too few parameters for the functions <%s>\n", funcName);
        return (RET_VAL) {INT_TYPE, NAN};
    }
    if(numOps < 1)
    {
        printf("ERROR: too few parameters for the functions <%s>\n", funcName);
        return (RET_VAL) {INT_TYPE, NAN};
    }
    else if (numOps > 1)
    {
        printf("WARNING: too many parameters for the function <%s>\n", funcName);
    }
    return (RET_VAL) {DOUBLE_TYPE, NAN};
}

RET_VAL doubleOps (char *funcName, FUNC_AST_NODE *funcNode)
{
    int numOps = evalOpList(funcNode->opList);
    if( numOps < 2)
    {
        RET_VAL result = {INT_TYPE, NAN};
        printf("ERROR: too few parameters for the functions <%s>\n", funcName);
        return result;
    }
    else if (numOps > 2)
    {
        printf("WARNING: too many parameters for the function <%s>\n", funcName);
    }
    return (RET_VAL) {DOUBLE_TYPE, NAN};
}


RET_VAL nOps (char *funcName, FUNC_AST_NODE *funcNode)
{
    int numOps = evalOpList(funcNode->opList);
    if(numOps <= 1) {
        RET_VAL result = {INT_TYPE, NAN};
        printf("ERROR: too few parameters for the function <%s>\n", funcName);
        return result;
    }
    return (RET_VAL) {DOUBLE_TYPE, NAN};
}

RET_VAL addHelperFunc(FUNC_AST_NODE *funcNode)
{
    if(nOps("add", funcNode).type == INT_TYPE)
    {
        return (RET_VAL){INT_TYPE, NAN};
    }
    else
    {
        RET_VAL result = (RET_VAL) {INT_TYPE, 0};
        result = addRecursive(funcNode->opList, result);
        return result;
    }
}

RET_VAL addRecursive(AST_NODE *opList, RET_VAL result)
{
    RET_VAL currNode = eval(opList);

    switch(result.type)
    {
        case INT_TYPE:
            if(currNode.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = currNode.value + result.value;
            }
            else
            {
                 result.value = (long) currNode.value + (long) result.value;
            }
            break;
        case DOUBLE_TYPE:
            result.value = currNode.value + result.value;
            break;
    }
    if(opList->next == NULL)
        return result;

    return addRecursive(opList->next, result);
}

RET_VAL multHelperFunc(FUNC_AST_NODE *funcNode)
{
    if(nOps("mult", funcNode).type == INT_TYPE)
    {
        return (RET_VAL){INT_TYPE, NAN};
    }
    else
    {
        RET_VAL result = (RET_VAL) {INT_TYPE, 1};
        result = multRecursive(funcNode->opList, result);
        return result;
    }
}

RET_VAL multRecursive(AST_NODE *opList, RET_VAL result)
{
    RET_VAL currNode = eval(opList);

    switch(result.type)
    {
        case INT_TYPE:
            if(currNode.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = currNode.value * result.value;
            }
            else
            {
                result.value = (long) currNode.value * (long) result.value;
            }
            break;
        case DOUBLE_TYPE:
            result.value = currNode.value * result.value;
            break;
    }
    if(opList->next == NULL)
        return result;

    return multRecursive(opList->next, result);
}

RET_VAL minHelperFunc(FUNC_AST_NODE *funcNode)
{
    if(nOps("min", funcNode).type == INT_TYPE)
    {
        return (RET_VAL){INT_TYPE, NAN};
    }
    else
    {
        RET_VAL result = minRecur(funcNode->opList->next, eval(funcNode->opList));
        return result;
    }
}

RET_VAL minRecur(AST_NODE *opList, RET_VAL result)
{
    RET_VAL currNode = eval(opList);

    switch(result.type)
    {
        case INT_TYPE:
            if(currNode.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = fmin(currNode.value, result.value);
            }
            else
            {
                result.value = fmin((long) currNode.value, (long) result.value);
            }
            break;
        case DOUBLE_TYPE:
            result.value = fmin(currNode.value,result.value);
            break;
    }
    if(opList->next == NULL)
        return result;

    return minRecur(opList->next, result);
}

RET_VAL maxHelperFunc(FUNC_AST_NODE *funcNode)
{
    if(nOps("max", funcNode).type == INT_TYPE)
    {
        return (RET_VAL){INT_TYPE, NAN};
    }
    else
    {
        RET_VAL result = maxRecur(funcNode->opList->next, eval(funcNode->opList));
        return result;
    }
}

RET_VAL maxRecur(AST_NODE *opList, RET_VAL result)
{
    RET_VAL currNode = eval(opList);

    switch(result.type)
    {
        case INT_TYPE:
            if(currNode.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = fmax(currNode.value, result.value);
            }
            else
            {
                result.value = fmax((long) currNode.value, (long) result.value);
            }
            break;
        case DOUBLE_TYPE:
            result.value = fmax(currNode.value,result.value);
            break;
    }
    if(opList->next == NULL)
        return result;

    return maxRecur(opList->next, result);
}

RET_VAL hypotHelperFunc(FUNC_AST_NODE *funcNode)
{
    if(nOps("hypot", funcNode).type == INT_TYPE)
    {
        return (RET_VAL){INT_TYPE, NAN};
    }
    else
    {
        RET_VAL result = hypotRecur(funcNode->opList->next, eval(funcNode->opList));
        return result;
    }
}
RET_VAL hypotRecur(AST_NODE *opList, RET_VAL result)
{
    RET_VAL currNode = eval(opList);

    switch(result.type)
    {
        case INT_TYPE:
            if(currNode.type == DOUBLE_TYPE)
            {
                result.type = DOUBLE_TYPE;
                result.value = hypot(currNode.value, result.value);
            }
            else
            {
                result.value = hypot((long) currNode.value, (long) result.value);
            }
            break;
        case DOUBLE_TYPE:
            result.value = hypot(currNode.value,result.value);
            break;
    }
    if(opList->next == NULL)
        return result;

    return hypotRecur(opList->next, result);
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO print the type and value of the value passed in.
    switch(val.type)
    {
        case INT_TYPE:
            printf("<INT>: ");
            printf("%.0lf", round(val.value));
            break;
        case DOUBLE_TYPE:
            printf("<DOUBLE>: ");
            printf("%lf", (val.value));
            break;
        default:
            yyerror("ERROR IN PrintRetVal, NOT DETECTING CASE TYPE");
    }
}
