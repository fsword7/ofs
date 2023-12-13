// parser.cpp - Parser package
//
// Author:  Tim Stark
// Date:    May 2, 2022

#include "main/core.h"
#include "scripts/parser.h"

// ******** Value ********


// ******** Value Group ********

Group::~Group()
{
    for (hash_t::iterator iter = data.begin(); iter != data.end(); iter++)
        delete iter->second;
}

void Group::addValue(std::string key, Value &val)
{
    data.insert(hash_t::value_type(key, &val));
}

Value *Group::getValue(cstr_t &key) const
{
    hash_t::const_iterator iter = data.find(key);
    if (iter == data.end())
        return nullptr;
    return iter->second;
}

bool Group::getNumber(cstr_t &key, double &val) const
{
    Value *v = getValue(key);
    if (v == nullptr || v->getType() != Value::vtNumber)
        return false;
    val = v->getNumber();
    return true;
}

bool Group::getNumber(cstr_t &key, float &val) const
{
    Value *v = getValue(key);
    if (v == nullptr || v->getType() != Value::vtNumber)
        return false;
    val = v->getNumber();
    return true;
}

bool Group::getBoolean(cstr_t &key, bool &val) const
{
    Value *v = getValue(key);
    if (v == nullptr || v->getType() != Value::vtBoolean)
        return false;
    val = v->getBoolean();
    return true;
}

bool Group::getString(cstr_t &key, str_t &val) const
{
    Value *v = getValue(key);
    if (v == nullptr || v->getType() != Value::vtString)
        return false;
    val = v->getString();
    return true;
}

bool Group::getVector(cstr_t &key, glm::dvec3 &val) const
{
    Value *v = getValue(key);
    if (v == nullptr || v->getType() != Value::vtArray)
        return false;

    Array *arr = v->getArray();
    if (arr->size() != 3)
        return false;
    Value *x = (*arr)[0];
    Value *y = (*arr)[1];
    Value *z = (*arr)[2];

    if (x->getType() != Value::vtNumber ||
        y->getType() != Value::vtNumber ||
        z->getType() != Value::vtNumber)
        return false;
    
    val = glm::dvec3(x->getNumber(), y->getNumber(), z->getNumber());

    return true;
}

bool Group::getVector(cstr_t &key, glm::dvec4 &val) const
{
    Value *v = getValue(key);
    if (v == nullptr || v->getType() != Value::vtArray)
        return false;

    Array *arr = v->getArray();
    if (arr->size() != 4)
        return false;
    Value *x = (*arr)[0];
    Value *y = (*arr)[1];
    Value *z = (*arr)[2];
    Value *w = (*arr)[3];

    if (x->getType() != Value::vtNumber ||
        y->getType() != Value::vtNumber ||
        z->getType() != Value::vtNumber ||
        w->getType() != Value::vtNumber)
        return false;
    
    val = glm::dvec4(x->getNumber(), y->getNumber(),
        z->getNumber(), w->getNumber());

    return true;
}

bool Group::getPath(cstr_t &key, fs::path &path) const
{
    std::string v;
    if (getString(key, v))
    {
        path = v;
        return true;
    }
    return false;
}

bool Group::getColor(cstr_t &key, color_t &color) const
{
    glm::dvec4 vec4;
    if (getVector(key, vec4))
    {
        color = color_t(vec4.x, vec4.y, vec4.z, vec4.w);
        return true;
    }

    glm::dvec3 vec3;
    if (getVector(key, vec3))
    {
        color = color_t(vec3.x, vec3.y, vec3.z);
        return true;
    }

    return false;
}

// ******** Parser ********

Parser::Parser(std::istream &in)
: inFile(in)
{

}

Value *Parser::getValue()
{
    tokenType token = getNextToken();
    Array *array;
    Group *group;

    switch (token)
    {
    case tkNumber:
        return new Value(getNumber());

    case tkString:
        return new Value(getText());

    case tkName:
        if (getText() == "true")
            return new Value(true);
        else if (getText() == "false")
            return new Value(false);
        pushBack();
        return nullptr;

    case tkBeginArray:
        pushBack();
        array = getArray();
        if (array == nullptr)
            return nullptr;
        return new Value(array);

    case tkBeginGroup:
        pushBack();
        group = getGroup();
        if (group == nullptr)
            return nullptr;
        return new Value(group);

    default:
        pushBack();
        return nullptr;
    }
}

ValueArray *Parser::getArray()
{
    tokenType token = getNextToken();

    if (token != tkBeginArray)
    {
        pushBack();
        return nullptr;
    }

    ValueArray *array = new ValueArray();

    token = getNextToken();
    while (token != tkEndArray)
    {
        pushBack();
        Value *val = getValue();
        if (val == nullptr)
        {
            delete array;
            return nullptr;
        }

        array->insert(array->end(), val);
        token = getNextToken();
    }

    return array;
}

Group *Parser::getGroup()
{
    tokenType token = getNextToken();

    if (token != tkBeginGroup)
    {
        pushBack();
        return nullptr;
    }

    Group *group = new Group();

    token = getNextToken();
    while (token != tkEndGroup)
    {
        if (token != tkName)
        {
            pushBack();
            delete group;
            return nullptr;
        }

        std::string name = getText();
        Value *val = getValue();
        if (val == nullptr)
        {
            delete group;
            return nullptr;
        }

        group->addValue(name, *val);
        token = getNextToken();
    }

    return group;
}

char Parser::readChar()
{
    char ch = inFile.get();
    if (ch == '\r' || ch == '\n')
        lineno++;
    return ch;
}

tokenType Parser::getNextToken()
{
    if (pushedBack == true)
    {
        pushedBack = false;
        return tkType;
    }

    tokenState state = stkStart;
    tokenType newToken = tkNull;
    bool isEof = false;

    text = "";
    value = 0.0;
    haveValidName = false;
    haveValidString = false;
    haveValidNumber = false;

    // Reset values for new token
    double intSign   = +1;
    double expSign   = +1;
    double intValue  = 0;
    double fracValue = 0;
    double fracExp   = 1;
    double expValue  = 0;

    while(newToken == tkNull)
    {
        nextChar = readChar();
        if (inFile.eof())
            isEof = true;
        else if (inFile.fail())
        {
            newToken = tkError;
        }

        switch (state)
        {
        case stkStart:
            if (isEof == true)
            {
                newToken = tkEnd;
            }
            if (std::isspace(nextChar))
            {
                // Do nothing
            }
            else if (nextChar == '#')
            {
                state = stkComment;
            }
            else if (std::isdigit(nextChar))
            {
                state = stkNumber;
                intValue = nextChar - '0';
            }
            else if (nextChar == '-')
            {
                state = stkNumber;
                intSign = -1;
            }
            else if (nextChar == '+')
            {
                state = stkNumber;
            }
            else if (isalpha(nextChar) || nextChar == '_')
            {
                state = stkName;
                text += nextChar;
                haveValidName = true;
            }
            else if (nextChar == '"')
            {
                state = stkString;
            }
            else if (nextChar == '{')
            {
                newToken = tkBeginGroup;
                nextChar = readChar();
            }
            else if (nextChar == '}')
            {
                newToken = tkEndGroup;
                nextChar = readChar();
            }
            else if (nextChar == '[')
            {
                newToken = tkBeginArray;
                nextChar = readChar();
            }
            else if (nextChar == ']')
            {
                newToken = tkEndArray;
                nextChar = readChar();
            }
            else if (nextChar == std::char_traits<char>::eof())
            {
                newToken = tkEnd;
            }
            else
                newToken = tkError;
            break;

        case stkComment:
            if (nextChar == '\r' || nextChar == '\n' ||
                nextChar == std::char_traits<char>::eof())
                state = stkStart;
            break;

        case stkName:
            if (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_')
                text += nextChar;
            else
            {
                newToken = tkName;
            }
            break;
        
        case stkString:
            if (nextChar == '"')
            {
                haveValidString = true;
                newToken = tkString;
                nextChar = readChar();
            }
            else if (nextChar == std::char_traits<char>::eof())
                newToken = tkError;
            else
                text += nextChar;
            break;

        case stkEscapeString:
            break;

        case stkNumber:
            if (isdigit(nextChar))
                intValue = (intValue * 10) + (char(nextChar) - '0');
            else if (nextChar == '.')
                state = stkFraction;
            else if (nextChar == 'e' || nextChar == 'E')
                state = stkFirstExponent;
            else if (issep(nextChar))
            {
                newToken = tkNumber;
                haveValidNumber = true;
            }
            else
                newToken = tkError;
            break;
        case stkFraction:
             if (isdigit(nextChar))
             {
                fracValue = (fracValue * 10) + (char(nextChar) - '0');
                fracExp *= 10;
            }
            else if (nextChar == 'e' || nextChar == 'E')
                state = stkFirstExponent;
            else if (issep(nextChar))
            {
                newToken = tkNumber;
                haveValidNumber = true;
            }
            else
                newToken = tkError;
            break;
        case stkFirstExponent:
             if (isdigit(nextChar))
             {
                expValue = char(nextChar) - '0';
                state = stkExponent;
            }
            else if (nextChar == '-')
            {
                expSign = -1;
                state = stkExponent;
            }
            else if (nextChar == '+')
                state = stkExponent;
            else
                newToken = tkError;
            break;
        case stkExponent:
            if (isdigit(nextChar))
                expValue = (expValue * 10) + (char(nextChar) - '0');
            else if (issep(nextChar))
            {
                newToken = tkNumber;
                haveValidNumber = true;
            }
            else
                newToken = tkError;
            break;

        case stkError:
            break;           
        }
    }

    if (haveValidNumber)
    {
        value = intValue + (fracValue / fracExp);
        if (expValue != 0.0)
            value *= pow(10.0, expValue * expSign);
        value *= intSign;
    }

    tkType = newToken;
    return tkType;
}

void Parser::analyze(std::istream &in)
{
    Parser parser(in);

    tokenType token = tkBegin;
    while (token != tkEnd)
    {
        token = parser.getNextToken();
        switch (token)
        {
        case tkBegin:
            logger->info("Begin\n");
            break;
        case tkEnd:
            logger->info("End\n");
            break;
        case tkName:
            logger->info("Name = {}\n", parser.getText());
            break;
        case tkString:
            logger->info("String = \"{}\"\n", parser.getText());
            break;
        case tkNumber:
            logger->info("Number = {}\n", parser.getNumber());
            break;
        case tkGroup:
            logger->info("{ ... }\n");
            break;
        case tkArray:
            logger->info("[ ... ]\n");
            break;
        default:
            // Logger::getLogger()->info("Unknown token = {}\n", token);
            break;
        }
    }
}