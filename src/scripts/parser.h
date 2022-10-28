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

class Value;
class Group;

typedef std::vector<Value *> Array;
typedef std::map<std::string, Value *, CompareIgnoreCasePredicate> hash_t;
using ValueArray = Array;

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
        vtGroup     = 5
    };

    Value() = default;
    ~Value() = default;

    Value(bool val) : type(vtBoolean)       { data.b = val; }
    Value(double val) :  type(vtNumber)     { data.d = val; }
    Value(Array *array) : type(vtArray)     { data.a = array; }
    Value(Group *group) : type(vtGroup)     { data.g = group; }

    Value(const str_t &val) : type(vtString) { data.s = new std::string(val); }
    
    inline valueType getType() const    { return type; }
    inline bool isNull() const          { return type == vtNull; }

    inline double getNumber() const     { assert(type == vtNumber);  return data.d; }
    inline double getBoolean() const    { assert(type == vtBoolean); return data.b; }
    inline str_t getString() const      { assert(type == vtString);  return *data.s; }
    inline Array *getArray() const      { assert(type == vtArray);   return data.a; }
    inline Group *getGroup() const      { assert(type == vtGroup);   return data.g; }

private:
    valueType type = vtNull;

    union
    {
        str_t   *s;
        double  d;
        bool    b;
        Array  *a;
        Group  *g;
    } data;
};

class Group
{
public:
    Group() = default;
    ~Group();

    void addValue(std::string key, Value &val);
    Value *getValue(cstr_t &key) const;

    bool getNumber(cstr_t &key, double &val) const;
    bool getNumber(cstr_t &key, float &val) const;
    bool getBoolean(cstr_t &key, bool &val) const;
    bool getString(cstr_t &key, str_t &val) const;   
    bool getVector(cstr_t &key, glm::dvec3 &val) const;
    bool getVector(cstr_t &key, glm::dvec4 &val) const;

    bool getPath(cstr_t &key, fs::path &path) const;
    bool getColor(cstr_t &key, color_t &color) const;

private:
    hash_t data;
};

class Parser
{
public:

    Parser(std::istream &in);

    tokenType getNextToken();

    Value *getValue();
    Array *getArray();
    Group *getGroup();

    inline std::string getText() const      { return text; }
    inline double getNumber() const         { return value; }
    inline int getLineNumber() const        { return lineno; }
    inline tokenType getTokenType() const   { return tkType; }
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