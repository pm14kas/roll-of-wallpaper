#pragma once

#include <vector>
#include <string>
#include <cctype>
#include <math.h>
#include <functional>
#include <memory>

class Formula
{
public:
    std::string s;
    double(*p) (double d);
    Formula(std::string s, double(*p) (double d))
    {
        this->s = s;
        this->p = p;
    }
};



class KeyValue
{
public:
    std::string s;
    double d;

    KeyValue()
    {
        this->s = "x";
        this->d = 0;
    }

    KeyValue(std::string s, double d)
    {
        this->s = s;
        this->d = d;
    }

    KeyValue(const KeyValue& other)
    {
        this->s = other.s;
        this->d = other.d;
    }

    KeyValue operator= (const KeyValue &other)
    {
        this->s = other.s;
        this->d = other.d;

        return (*this);
    }

    KeyValue operator-()
    {
        KeyValue t(this->s, -this->d);
        return t;
    }

    KeyValue operator+(double other)
    {
        KeyValue t(this->s, this->d + other);
        return t;
    }

    KeyValue operator-(double other)
    {
        KeyValue t(this->s, this->d - other);
        return t;
    }

    KeyValue operator*(double other)
    {
        KeyValue t(this->s, this->d * other);
        return t;
    }

    KeyValue operator/(double other)
    {
        KeyValue t(this->s, this->d / other);
        return t;
    }

    KeyValue operator+(KeyValue &other)
    {
        if (this->s != other.s) throw 0;
        KeyValue t(this->s, this->d + other.d);
        return t;
    }

    KeyValue operator-(KeyValue &other)
    {
        if (this->s != other.s) throw 0;
        KeyValue t(this->s, this->d - other.d);
        return t;
    }

    KeyValue operator*(KeyValue &other)
    {
        if (this->s != other.s) throw 0;
        KeyValue t(this->s, this->d * other.d);
        return t;
    }

    KeyValue operator/(KeyValue &other)
    {
        if (this->s != other.s) throw 0;
        KeyValue t(this->s, this->d / other.d);
        return t;
    }

    KeyValue& operator+= (KeyValue &other)
    {
        if (this->s != other.s) throw 0;
        this->d += other.d;
        return *this;
    }

    KeyValue& operator-= (KeyValue &other)
    {
        if (this->s != other.s) throw 0;
        this->d -= other.d;
        return *this;
    }

    KeyValue& operator+= (double other)
    {
        this->d += other;
        return *this;
    }

    KeyValue& operator-= (double other)
    {
        this->d -= other;
        return *this;
    }

    bool operator== (KeyValue &other)
    {
        if (this->s == other.s) {
            return true;
        }
        else {
            return false;
        }
    }

};












class ParserNode
{
    enum NodeType { Binary, Unary, Const, Variable, Empty };

    ParserNode *left = nullptr;
    ParserNode *right = nullptr;

    NodeType nodeType = NodeType::Empty;

    std::function<double(double left, double right)> binaryFunction;
    std::function<double(double left)> unaryFunction;
    double constFunction = 0;
    KeyValue variableFunction;

public:
    ParserNode()
    {
        this->left = nullptr;
        this->right = nullptr;
    }

    ParserNode(const ParserNode &other)
    {
        this->binaryFunction = other.binaryFunction;
        this->unaryFunction = other.unaryFunction;
        this->constFunction = other.constFunction;
        this->variableFunction = other.variableFunction;

        this->nodeType = other.nodeType;

        if (other.left) {
            if (!this->left) {
                this->addLeft();
            }
            *this->left = *other.left;
        }

        if (other.right) {
            if (!this->right) {
                this->addRight();
            }
            *this->right = *other.right;
        }
    }

    ParserNode operator=(const ParserNode &other)
    {
        this->binaryFunction = other.binaryFunction;
        this->unaryFunction = other.unaryFunction;
        this->constFunction = other.constFunction;
        this->variableFunction = other.variableFunction;

        this->nodeType = other.nodeType;

        if (other.left) {
            if (!this->left) {
                this->addLeft();
            }
            *this->left = *other.left;
        }

        if (other.right) {
            if (!this->right) {
                this->addRight();
            }
            *this->right = *other.right;
        }
        return (*this);
    }

    void convert(std::function<double(double left, double right)> function)
    {
        nodeType = NodeType::Binary;
        binaryFunction = function;
    }

    void convert(std::function<double(double left)> function)
    {
        nodeType = NodeType::Unary;
        unaryFunction = function;
    }

    void convert(double function)
    {
        nodeType = NodeType::Const;
        constFunction = function;
    }

    void convert(KeyValue function)
    {
        nodeType = NodeType::Variable;
        variableFunction = function;
    }

    ~ParserNode()
    {
        //this->destroy();
    }

    void destroy()
    {
        if (this->left) {
            this->left->destroy();
            this->left = nullptr;
        }

        if (this->right) {
            this->right->destroy();
            this->right = nullptr;
        }

        delete this;

    }

    double calc(std::vector<KeyValue> varmap)
    {
        if ((this->nodeType == NodeType::Binary) && (this->left) && (this->right)) {
            return binaryFunction(this->left->calc(varmap), this->right->calc(varmap));
        }
        else if ((this->nodeType == NodeType::Unary) && (this->left)) {
            return unaryFunction(this->left->calc(varmap));
        }
        else if (this->nodeType == NodeType::Const) {
            return constFunction;
        }
        else if (this->nodeType == NodeType::Variable) {
            for (unsigned int i = 0; i < varmap.size(); i++)
            {
                if (varmap[i] == this->variableFunction) {
                    return varmap[i].d;
                }
            }
        }
        else {
            return nan("");
            //throw std::string("Parser::ERROR_PARSE");
        }
        return 0;
    }

    bool isAlive()
    {
        return this->nodeType != NodeType::Empty;
    }

    ParserNode* addLeft()
    {
        if (this->left) {
            return this->left;
        }
        this->left = new ParserNode();

        return this->left;
    }

    ParserNode* addRight()
    {
        this->right = new ParserNode;

        return this->right;
    }

    ParserNode* getLeft()
    {
        return this->left;
    }

    ParserNode* getRight()
    {
        return this->right;
    }

    static double Addition(double left, double right)
    {
        return left + right;
    }

    static double Subtraction(double left, double right)
    {
        return left - right;
    }

    static double Multiplication(double left, double right)
    {
        return left * right;
    }

    static double Division(double left, double right)
    {
        if (right == 0)
        {
            throw 2;
        }
        return left / right;
    }

    static double Power(double left, double right)
    {
        return pow(left, right);
    }

    static double Abs(double left)
    {
        return abs(left);
    }
};



































class Parser
{
    static std::vector<Formula> functions()
    {
        return{ Formula("exp", exp),
            Formula("ln", log),
            Formula("abs", ParserNode::Abs),
            Formula("sin", sin),
            Formula("cos", cos),
            Formula("arctg", atan),
            Formula("tg", tan),
            Formula("sqrt", sqrt)
        };
    }

    std::string fString = "";

    bool isCompiled = false;
    ParserNode *rootNode;

    std::vector<Parser> bracesList;

public:

    static const int ERROR_PARENTHESIS = 0;
    static const int ERROR_PARSE = 1;
    static const int ERROR_DIVISION_BY_ZERO = 2;

    static constexpr double E = 2.71828182845904523536;

    static constexpr double PI = 3.14159265358979323846;


    static std::string exception(int id)
    {
        switch (id)
        {
        case 0:
            return "Parenthesis mismatch";
            break;
        case 1:
            return "Invalid argument";
            break;
        case 2:
            return "Division by zero";
            break;
        default:
            return "Unknown error";
        }
    }

    static bool tryParse(std::string param, double &result)
    {
        try
        {
            std::string::size_type sz;
            result = stod(param, &sz);
            if (param.substr(sz) != "\0")
            {
                return false;
            }
        }
        catch (...)
        {
            return false;
        }
        return true;
    }

private:

    static std::string trim(const std::string &s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isspace(*it))
            ++it;

        std::string::const_reverse_iterator rit = s.rbegin();
        while (rit.base() != it && isspace(*rit))
            ++rit;

        return std::string(it, rit.base());
    }

    static std::string addSpace(std::string &source, char original)
    {
        std::string result = "";
        for (unsigned int i = 0; i < source.size(); i++)
        {
            if (source[i] == original)
            {
                result.push_back(' ');
            }
            result.push_back(source[i]);
        }
        return result;
    }

    static std::string inverse(std::string in)
    {
        int first = 0, last = in.size();
        while ((first != last) && (first != --last))
        {
            std::swap(in[first], in[last]);
            ++first;
        }
        return in;
    }

    static std::string replace(std::string &in)
    {
        std::string result = "";
        for (unsigned int i = 0; i < in.size(); i++)
        {
            if (in[i] != ' ')
                result.push_back(in[i]);
        }
        return result;
    }

public:

    Parser()
    {
        rootNode = new ParserNode();
    }

    Parser(std::string s)
    {
        rootNode = new ParserNode();
        this->fString = s;
    }

    Parser(const Parser& other)
    {
        this->rootNode = new ParserNode();
        *this->rootNode = *other.rootNode;
        this->fString = other.fString;
    }

    ~Parser()
    {
        this->rootNode->destroy();
    }

    Parser operator=(const Parser& other)
    {
        this->rootNode = new ParserNode();
        *this->rootNode = *other.rootNode;
        this->fString = other.fString;

        return (*this);
    }

    void setBracesList(std::vector<Parser> bracesList)
    {
        this->bracesList = bracesList;
    }


private:
    double calc(std::vector<KeyValue> varmap, std::string f, ParserNode *currentNode)
    {
        if (f.size() == 0) throw ERROR_PARSE;
        f = trim(f);
        double d;
        if (tryParse(f, d)) {
            currentNode->convert(d);

            return d;
        }
        if (f == "nan") {
            currentNode->convert(nan(""));

            return nan("");
        }

        for (unsigned int i = 0; i < varmap.size(); i++)
        {
            if (f == varmap[i].s)
            {
                currentNode->convert(varmap[i]);

                return varmap[i].d;
            }
            else if (f == ("-" + varmap[i].s))
            {
                currentNode->convert(-varmap[i]);

                return -varmap[i].d;
            }
        }


        if (f == ("pi")) {
            currentNode->convert(Parser::PI);

            return Parser::PI;
        }
        else if (f == ("-pi")) {
            currentNode->convert(-Parser::PI);

            return -Parser::PI;
        }
        if (f == ("e")) {
            currentNode->convert(Parser::E);

            return Parser::E;
        }
        else if (f == ("-e")) {
            currentNode->convert(-Parser::E);

            return -Parser::E;
        }

        size_t index;
        for (unsigned int func = 0; func < functions().size(); func++)
        {
            std::string name = functions()[func].s;
            if ((index = f.find(name)) != std::string::npos)
            {
                std::string g = f.substr(index + name.size());
                g = trim(g);
                if (g.substr(0, 1) != "(")
                {
                    throw ERROR_PARENTHESIS;
                }
                unsigned int j = 1, i;
                for (i = 1; i < g.size(); i++)
                {
                    if (g.substr(i, 1) == "(")
                    {
                        j++;
                    }
                    else if (g.substr(i, 1) == ")")
                    {
                        j--;
                    }
                    if (!j)
                    {
                        break;
                    }
                }
                if (j)
                {
                    throw ERROR_PARENTHESIS;
                }

                bracesList.push_back(Parser(g.substr(1, i - 1)));

                bracesList[bracesList.size() - 1].setBracesList(bracesList);


                std::string replacement = "{" + std::to_string(func) + "{" + std::to_string(bracesList.size() - 1) + "}}";
                replacement = replacement + "";
                return calc(varmap, (f.substr(0, static_cast<int>(index)) + replacement + g.substr(i + 1)), currentNode); // ????????? ???????, ??????????? ???????? ??????????? ?????? ???, ????? ??? ?????? ????????
            }
        }

        if ((index = f.find('(')) != std::string::npos)
        {
            std::string g = f.substr(index + 1);
            unsigned int j = 1, i;
            for (i = 0; i < g.size(); i++)
            {
                if (g.substr(i, 1) == "(")
                {
                    j++;
                }
                else if (g.substr(i, 1) == ")")
                {
                    j--;
                }
                if (!j)
                {
                    break;
                }
            }
            if (j)
            {
                throw ERROR_PARENTHESIS;
            }
            bracesList.push_back(Parser(g.substr(0, i)));
            bracesList[bracesList.size() - 1].setBracesList(bracesList);

            return calc(varmap, (f.substr(0, index) + "{" + std::to_string(bracesList.size() - 1) + "}" + g.substr(i + 1)), currentNode);
        }

        if (((f[0] == '{') && (f[f.size() - 1] == '}'))) {
            double d;
            if (tryParse(f.substr(1, f.size() - 2), d)) {
                double value = bracesList[(int)d].calc(varmap);
                if (bracesList[(int)d].isCompiled) {
                    *currentNode = *bracesList[(int)d].rootNode;
                    //*currentNode = *bracesList[(int)d].rootNode;
                }
                return value;
            }
            else {
                size_t indexBrace;
                if ((indexBrace = f.find('{', 1)) != std::string::npos) {
                    double funcFloat = -1;
                    double braceFloat = -1;

                    if (tryParse(f.substr(1, indexBrace - 1), funcFloat) &&
                        tryParse(f.substr(indexBrace + 1, f.size() - indexBrace - 3), braceFloat))
                    {
                        //fells
                        if (bracesList.size() < braceFloat)
                        {
                            throw "string: " + f + " bracefloat:" + std::to_string(braceFloat) + " size:" + std::to_string(bracesList.size());
                        }

                        double value = functions()[(int)funcFloat].p(bracesList[(int)braceFloat].calc(varmap));
                        if (bracesList[(int)braceFloat].isCompiled)
                        {
                            currentNode->convert(functions()[(int)funcFloat].p);
                            *currentNode->addLeft() = *bracesList[(int)braceFloat].rootNode;
                        }
                        return value;
                    }
                }
            }
        }

        if ((index = f.find('+')) != std::string::npos)
        {
            if (index == 0) throw ERROR_PARSE;
            if ((f[index - 1] == 'e') || (f[index - 1] == 'E'))
            {

            }
            else
            {
                currentNode->convert(ParserNode::Addition);

                return calc(varmap, f.substr(0, index), currentNode->addLeft()) + calc(varmap, f.substr(index + 1), currentNode->addRight());
            }
        }
        if (((index = inverse(f).find('-')) != std::string::npos) && (index) && ((inverse(f)[index + 1] == ' ') || (inverse(f)[index + 1] == '-'))) //????? 5-3-2 ???????????? ? 5-(3-2)
        {
            index = f.size() - index - 1;
            //if (index <= 0) throw ERROR_PARSE;;

            if ((f[index - 2] == 'e') || (f[index - 2] == 'E'))
            {
                f = f.substr(0, index - 1) + f.substr(index);//??????? ?????, ??????? ???????????? ??? ?????? ???????? ??????
            }
            else
            {
                /*if (f[index-1]==' ')
                return calc(varmap, f.substr(0, index-1)) - calc(varmap, f.substr(index + 1));*/

                currentNode->convert(ParserNode::Subtraction);

                if ((f[index - 2] == '-' && f[index - 1] == ' '))
                {
                    std::string temp = f.substr(0, index - 3);
                    if (temp.size() == 0) temp = "0";
                    return calc(varmap, temp, currentNode->addLeft()) + calc(varmap, f.substr(index + 1), currentNode->addRight());
                }
                else if ((index < f.size()) && (f[index + 1] == '-'))
                {
                    std::string temp = f.substr(0, index - 1);
                    if (temp.size() == 0) temp = "0";
                    return calc(varmap, temp, currentNode->addLeft()) + calc(varmap, f.substr(index + 2), currentNode->addRight());
                }
                else if ((index>1) && (f[index - 1] == '-'))
                {
                    std::string temp = f.substr(0, index - 2);
                    if (temp.size() == 0) temp = "0";
                    return calc(varmap, temp, currentNode->addLeft()) + calc(varmap, f.substr(index + 1), currentNode->addRight());
                }
                else
                {
                    std::string temp = f.substr(0, index - 1);
                    if (temp.size() == 0) temp = "0";
                    return calc(varmap, temp, currentNode->addLeft()) - calc(varmap, f.substr(index + 1), currentNode->addRight());
                }
            }
        }

        if ((index = f.find('*')) != std::string::npos)
        {
            currentNode->convert(ParserNode::Multiplication);

            return calc(varmap, f.substr(0, index), currentNode->addLeft()) * calc(varmap, f.substr(index + 1), currentNode->addRight());
        }
        if ((index = inverse(f).find('/')) != std::string::npos)
        {
            index = f.size() - index - 1;
            if (index <= 0) throw ERROR_PARSE;

            /*if (rawCalc(varmap, f.substr(index + 1)) == 0)
            {
            throw ERROR_DIVISION_BY_ZERO;
            }*/
            currentNode->convert(ParserNode::Division);

            return calc(varmap, f.substr(0, index), currentNode->addLeft()) / calc(varmap, f.substr(index + 1), currentNode->addRight());
        }
        if ((index = f.find('^')) != std::string::npos)
        {
            currentNode->convert(ParserNode::Power);

            return pow(calc(varmap, f.substr(0, index), currentNode->addLeft()), calc(varmap, f.substr(index + 1), currentNode->addRight()));
        }

        throw f;

    }


public:
    double calc(std::vector<KeyValue> varmap)
    {
        if (fString.size() == 0)
        {
            throw ERROR_PARSE;
        }
        fString = replace(fString);
        fString = addSpace(fString, '-');
        if (fString.size() == 0)
        {
            throw ERROR_PARSE;
        }

        if (this->isCompiled) {
            if (!rootNode->isAlive())
            {
                throw "test";
            }
            return rootNode->calc(varmap);
        }
        else {
            double result = calc(varmap, fString, rootNode);
            this->isCompiled = true;
            this->bracesList.clear();
            return result;
        }
    }


};

