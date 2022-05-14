// parser.h - Parser package
//
// Author:  Tim Stark
// Date:    May 2, 2022

#pragma once

enum tokenState
{
    stkStart,
    stkString,
    stkEscapeString,
    stkNumber,
    stkFraction,
    stkFirstExponent,
    stkExponent,
    stkName,
    stkBeginGroup,
    stkEndGroup,
    stkBeginArray,
    stkEndArray,
    stkComment,
    stkError
};

enum tokenType
{
    tkNull = 0,
    tkBegin = 1,
    tkEnd = 2,
    tkError = 3,
    tkName = 4,
    tkString = 5,
    tkNumber = 6,
    tkArray,
    tkGroup,
    tkBeginGroup,
    tkEndGroup,
    tkBeginArray,
    tkEndArray
};

class Value
{
public:
    enum valueType
    {
        vtNull      = 0,
        vtNumber    = 1,
        vtBoolean   = 2,
        vtString    = 3,
        vtArray     = 4,
        vtHash      = 5
    };

    Value() = default;
    ~Value() = default;

    Value(bool val) : type(vtBoolean) { data.b = val; }
    Value(double val) :  type(vtNumber) { data.d = val; }

    Value(const str_t &val) : type(vtString) { data.s = new std::string(val); }

    inline valueType getType() const    { return type; }
    inline bool isNull() const          { return type == vtNull; }

    inline double getNumber() const     { assert(type == vtNumber);  return data.d; }
    inline double getBoolean() const    { assert(type == vtBoolean); return data.b; }
    inline str_t getString() const      { assert(type == vtString);  return *data.s; }

private:
    valueType type = vtNull;

    union
    {
        str_t   *s;
        double  d;
        bool    b;
    } data;
};

typedef std::vector<Value *> ValueArray;
typedef std::map<std::string, Value *> hash_t;

class Group
{
public:
    Group() = default;
    ~Group();

    Value *getValue(std::string key);
    void addValue(std::string key, Value &val);

private:
    hash_t data;
};

class Parser
{
public:

    Parser(std::istream &in);

    tokenType getNextToken();

    Value *getValue();
    ValueArray *getArray();
    Group *getGroup();

    inline std::string getText() const      { return text; }
    inline double getNumber() const         { return value; }
    inline void pushBack()                  { pushedBack = true; }

    static void analyze(std::istream &in);

private:
    char readChar();
    bool issep(char ch) const           { return !isdigit(ch) && !isalpha(ch) && ch != '.'; }

private:
    std::istream &inFile;

    tokenType tkType = tkNull;

    std::string text;
    double value = 0.0;

    int lineno    = 0;
    char nextChar = '\0';

    bool pushedBack = false;
    bool haveValidName = false;
    bool haveValidString = false;
    bool haveValidNumber = false;
};