#pragma once

#include <vector>
#include <string>
#include <cctype>
#include <math.h>
#include <functional>
#include <memory>
#include "interval.h"
#include "intervalmath.h"

class FormulaInterval
{
public:
    std::string s;
    Interval(*p) (Interval d);
    FormulaInterval(std::string s, Interval(*p) (Interval d))
    {
        this->s = s;
        this->p = p;
    }
};



class KeyValueInterval
{
public:
    std::string s;
    Interval d;

    KeyValueInterval()
    {
        this->s = "x";
        this->d = 0;
    }

    KeyValueInterval(std::string s, Interval d)
    {
        this->s = s;
        this->d = d;
    }

    KeyValueInterval(const KeyValueInterval& other)
    {
        this->s = other.s;
        this->d = other.d;
    }

    KeyValueInterval operator= (const KeyValueInterval &other)
    {
        this->s = other.s;
        this->d = other.d;

        return (*this);
    }

    KeyValueInterval operator+(Interval other)
    {
        KeyValueInterval t(this->s, this->d + other);
        return t;
    }

    KeyValueInterval operator-(Interval other)
    {
        KeyValueInterval t(this->s, this->d - other);
        return t;
    }

    KeyValueInterval operator*(Interval other)
    {
        KeyValueInterval t(this->s, this->d * other);
        return t;
    }

    KeyValueInterval operator/(Interval other)
    {
        KeyValueInterval t(this->s, this->d / other);
        return t;
    }

    KeyValueInterval operator+(KeyValueInterval &other)
    {
        if (this->s != other.s) throw 0;
        KeyValueInterval t(this->s, this->d + other.d);
        return t;
    }

    KeyValueInterval operator-(KeyValueInterval &other)
    {
        if (this->s != other.s) throw 0;
        KeyValueInterval t(this->s, this->d - other.d);
        return t;
    }

    KeyValueInterval operator*(KeyValueInterval &other)
    {
        if (this->s != other.s) throw 0;
        KeyValueInterval t(this->s, this->d * other.d);
        return t;
    }

    KeyValueInterval operator/(KeyValueInterval &other)
    {
        if (this->s != other.s) throw 0;
        KeyValueInterval t(this->s, this->d / other.d);
        return t;
    }

    KeyValueInterval& operator+= (KeyValueInterval &other)
    {
        if (this->s != other.s) throw 0;
        this->d = this->d + other.d;
        return *this;
    }

    KeyValueInterval& operator-= (KeyValueInterval &other)
    {
        if (this->s != other.s) throw 0;
        this->d = this->d - other.d;
        return *this;
    }

    KeyValueInterval& operator+= (Interval other)
    {
        this->d = this->d + other;
        return *this;
    }

    KeyValueInterval& operator-= (Interval other)
    {
        this->d = this->d - other;
        return *this;
    }

    bool operator== (KeyValueInterval &other)
    {
        if (this->s == other.s) {
            return true;
        }
        else {
            return false;
        }
    }

};












class ParserNodeInterval
{
    enum NodeType { Binary, Unary, Const, Variable, Empty };

    ParserNodeInterval *left = nullptr;
    ParserNodeInterval *right = nullptr;

    NodeType nodeType = NodeType::Empty;

    std::function<Interval(Interval left, Interval right)> binaryFunction;
    std::function<Interval(Interval left)> unaryFunction;
    Interval constFunction = 0;
    KeyValueInterval variableFunction;

public:
    ParserNodeInterval()
    {
        this->left = nullptr;
        this->right = nullptr;
    }

    ParserNodeInterval(const ParserNodeInterval &other)
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

    ParserNodeInterval operator=(const ParserNodeInterval &other)
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

    void convert(std::function<Interval(Interval left, Interval right)> function)
    {
        nodeType = NodeType::Binary;
        binaryFunction = function;
    }

    void convert(std::function<Interval(Interval left)> function)
    {
        nodeType = NodeType::Unary;
        unaryFunction = function;
    }

    void convert(Interval function)
    {
        nodeType = NodeType::Const;
        constFunction = function;
    }

    void convert(KeyValueInterval function)
    {
        nodeType = NodeType::Variable;
        variableFunction = function;
    }

    ~ParserNodeInterval()
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

    Interval calc(std::vector<KeyValueInterval> varmap)
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
            //throw std::string("ParserInterval::ERROR_PARSE");
        }
        return 0;
    }

    bool isAlive()
    {
        return this->nodeType != NodeType::Empty;
    }

    ParserNodeInterval* addLeft()
    {
        if (this->left) {
            return this->left;
        }
        this->left = new ParserNodeInterval();

        return this->left;
    }

    ParserNodeInterval* addRight()
    {
        this->right = new ParserNodeInterval;

        return this->right;
    }

    ParserNodeInterval* getLeft()
    {
        return this->left;
    }

    ParserNodeInterval* getRight()
    {
        return this->right;
    }

    static Interval Addition(Interval left, Interval right)
    {
        return left + right;
    }

    static Interval Subtraction(Interval left, Interval right)
    {
        return left - right;
    }

    static Interval Multiplication(Interval left, Interval right)
    {
        return left * right;
    }

    static Interval Division(Interval left, Interval right)
    {
        if (right == 0)
        {
            throw 2;
        }
        return left / right;
    }

    static Interval Power(Interval left, Interval right)
    {
        return IntervalMath::pow(left, right);
    }

    static Interval Abs(Interval left)
    {
        return IntervalMath::abs(left);
    }
};



































class ParserInterval
{
    static std::vector<FormulaInterval> functions()
    {
        return{
            FormulaInterval("abs", ParserNodeInterval::Abs),
            FormulaInterval("sin", IntervalMath::sin),
            FormulaInterval("cos", IntervalMath::cos),
            FormulaInterval("sqrt", IntervalMath::sqrt)
        };
    }

    std::string fString = "";

    bool isCompiled = false;
    ParserNodeInterval *rootNode;

    std::vector<ParserInterval> bracesList;

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

    ParserInterval()
    {
        rootNode = new ParserNodeInterval();
    }

    ParserInterval(std::string s)
    {
        rootNode = new ParserNodeInterval();
        this->fString = s;
    }

    ParserInterval(const ParserInterval& other)
    {
        this->rootNode = new ParserNodeInterval();
        *this->rootNode = *other.rootNode;
        this->fString = other.fString;
    }

    ~ParserInterval()
    {
        this->rootNode->destroy();
    }

    ParserInterval operator=(const ParserInterval& other)
    {
        this->rootNode = new ParserNodeInterval();
        *this->rootNode = *other.rootNode;
        this->fString = other.fString;

        return (*this);
    }

    void setBracesList(std::vector<ParserInterval> bracesList)
    {
        this->bracesList = bracesList;
    }


private:
    Interval calc(std::vector<KeyValueInterval> varmap, std::string f, ParserNodeInterval *currentNode)
    {
        if (f.size() == 0) throw ERROR_PARSE;
        f = trim(f);
        double d;
        if (tryParse(f, d)) {
            currentNode->convert(d);
            return d;
        }

        Interval interval;
        if (Interval::TryParse(f, interval)){
            currentNode->convert(interval);

            return interval;
        }

        size_t index;
        if ((index = f.find('[')) != std::string::npos)
        {
            size_t index2;
            if ((index2 = f.find(']')) != std::string::npos)
            {
                std::string g = f.substr(index, index2-index+1);
                bracesList.push_back(ParserInterval(g));
                bracesList[bracesList.size() - 1].setBracesList(bracesList);

                return calc(varmap, (f.substr(0, index) + "{" + std::to_string(bracesList.size() - 1) + "}" + f.substr(index2 + 1)), currentNode);

            }
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
        }


        if (f == ("pi")) {
            currentNode->convert(ParserInterval::PI);

            return ParserInterval::PI;
        }
        else if (f == ("-pi")) {
            currentNode->convert(-ParserInterval::PI);

            return -ParserInterval::PI;
        }
        if (f == ("e")) {
            currentNode->convert(ParserInterval::E);

            return ParserInterval::E;
        }
        else if (f == ("-e")) {
            currentNode->convert(-ParserInterval::E);

            return -ParserInterval::E;
        }


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

                bracesList.push_back(ParserInterval(g.substr(1, i - 1)));

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
            bracesList.push_back(ParserInterval(g.substr(0, i)));
            bracesList[bracesList.size() - 1].setBracesList(bracesList);

            return calc(varmap, (f.substr(0, index) + "{" + std::to_string(bracesList.size() - 1) + "}" + g.substr(i + 1)), currentNode);
        }

        if (((f[0] == '{') && (f[f.size() - 1] == '}'))) {
            double d;
            if (tryParse(f.substr(1, f.size() - 2), d)) {
                Interval value = bracesList[(int)d].calc(varmap);
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

                        Interval value = functions()[(int)funcFloat].p(bracesList[(int)braceFloat].calc(varmap));
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
                currentNode->convert(ParserNodeInterval::Addition);

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

                currentNode->convert(ParserNodeInterval::Subtraction);

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
            currentNode->convert(ParserNodeInterval::Multiplication);

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
            currentNode->convert(ParserNodeInterval::Division);

            return calc(varmap, f.substr(0, index), currentNode->addLeft()) / calc(varmap, f.substr(index + 1), currentNode->addRight());
        }
        if ((index = f.find('^')) != std::string::npos)
        {
            currentNode->convert(ParserNodeInterval::Power);

            return IntervalMath::pow(calc(varmap, f.substr(0, index), currentNode->addLeft()), calc(varmap, f.substr(index + 1), currentNode->addRight()));
        }

        throw f;

    }


public:
    Interval calc(std::vector<KeyValueInterval> varmap)
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
            Interval result = calc(varmap, fString, rootNode);
            this->isCompiled = true;
            this->bracesList.clear();
            return result;
        }
    }


};

