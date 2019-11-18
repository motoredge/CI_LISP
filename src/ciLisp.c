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
    node->type = FUNC_NODE_TYPE;
    node->data.function.oper = resolveFunc(funcName);
    node->data.function.op1 = op1;
    node->data.function.op2 = op2;

//    switch(node->type)
//    {
//        case CUSTOM_OPER:
//            node->type = CUSTOM_OPER;
//            node->data.function.ident = funcName;
//            break;
//        default:
//            free(funcName);
//            break;
//    }

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

    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE
    //RET_VAL op1 = eval(funcNode->op1);
    //RET_VAL op2 = eval(funcNode->op2);
    switch(funcNode->oper)
    {
        case NEG_OPER:
            result = NegOperHelp(funcNode);
            break;
        case ABS_OPER:
            result = AbsOperHelp(funcNode);
            break;
        case EXP_OPER:
            result = ExpOperHelp(funcNode);
            break;
        case SQRT_OPER:
            result = SqrtOperHelp(funcNode);
            break;
        case ADD_OPER:
            result = AddOperHelp(funcNode);
            break;
        case SUB_OPER:
            result = SubOperHelp(funcNode);
            break;
        case MULT_OPER:
            result = MultOperHelp(funcNode);
            break;
        case DIV_OPER:
            result = DivOperHelp(funcNode);
            break;
        case REMAINDER_OPER:
            result = RemainOperHelp(funcNode);
            break;
        case LOG_OPER:
            result = LogOperHelp(funcNode);
            break;
        case POW_OPER:
            result = PowOperHelp(funcNode);
            break;
        case MAX_OPER:
            result = MaxOperHelp(funcNode);
            break;
        case MIN_OPER:
            result = MinOperHelp(funcNode);
            break;
        case EXP2_OPER:
            result = Exp2OperHelp(funcNode);
            break;
        case CBRT_OPER:
            result = CbrtOperHelp(funcNode);
            break;
        case HYPOT_OPER:
            result = HyptOperHelp(funcNode);
            break;
        default:
            yyerror("IN EvalFuncNode, THERE IS NO CASE TO POPULATE RESULT");
    }
    return result;
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO print the type and value of the value passed in.
    switch(val.type)
    {
        case INT_TYPE:
            printf("%.0lf", round(val.value));
            printf(" hello");
            break;
        case DOUBLE_TYPE:
            printf("%lf", (val.value));
            printf(" bye");
            break;
        default:
            yyerror("ERROR IN PrintRetVal, NOT DETECTING CASE TYPE");
    }
}

RET_VAL NegOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = funcNode->op1->data.number.type;
    result.value = -(funcNode->op1->data.number.value);
    return result;
}

RET_VAL AbsOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = funcNode->op1->data.number.type;
    result.value = fabs(funcNode->op1->data.number.value);
    return result;
}

RET_VAL ExpOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = funcNode->op1->data.number.type;
    result.value = exp(funcNode->op1->data.number.value);
    return result;
}

RET_VAL SqrtOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = funcNode->op1->data.number.type;
    result.value = sqrt(funcNode->op1->data.number.value);
    return result;
}

RET_VAL AddOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) + (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) + (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) + (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) + (funcNode->op2->data.number.value));
    }
    return result;
}

RET_VAL SubOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) - (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op2->data.number.type;
        result.value = ((funcNode->op1->data.number.value) - (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) - (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) - (funcNode->op2->data.number.value));
    }
    return result;
}

RET_VAL MultOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) * (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op2->data.number.type;
        result.value = ((funcNode->op1->data.number.value) * (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) * (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) * (funcNode->op2->data.number.value));
    }
    return result;
}

RET_VAL DivOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) / (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op2->data.number.type;
        result.value = ((funcNode->op1->data.number.value) / (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) / (funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = ((funcNode->op1->data.number.value) / (funcNode->op2->data.number.value));
    }
    return result;
}

RET_VAL RemainOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmod((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op2->data.number.type;
        result.value = fmod((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmod((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmod((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    return result;
}

RET_VAL LogOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = log((funcNode->op1->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = log((funcNode->op1->data.number.value));
    }
    return result;
}

RET_VAL PowOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = pow((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op2->data.number.type;
        result.value = pow((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = pow((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = pow((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    return result;
}

RET_VAL MaxOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmax((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op2->data.number.type;
        result.value = fmax((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmax((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmax((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    return result;
}

RET_VAL MinOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmin((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op2->data.number.type;
        result.value = fmin((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmin((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = fmin((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    return result;
}

RET_VAL Exp2OperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = exp2((funcNode->op1->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = exp2((funcNode->op1->data.number.value));
    }
    return result;
}

RET_VAL CbrtOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = cbrt((funcNode->op1->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = cbrt((funcNode->op1->data.number.value));
    }
    return result;
}

RET_VAL HyptOperHelp(FUNC_AST_NODE *funcNode)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = hypot((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == INT_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op2->data.number.type;
        result.value = hypot((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == INT_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = hypot((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    if(funcNode->op1->type == DOUBLE_TYPE && funcNode->op2->type == DOUBLE_TYPE)
    {
        result.type = funcNode->op1->data.number.type;
        result.value = hypot((funcNode->op1->data.number.value),(funcNode->op2->data.number.value));
    }
    return result;
}