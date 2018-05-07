#pragma once

#include <vector>
#include <string>
#include <cctype>
#include <math.h>

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

};

class Parser
{
    static std::vector<Formula> functions()
    {
        return{	Formula("exp", exp),
            Formula("ln", log),
            Formula("sin", sin),
            Formula("cos", cos),
            Formula("arctg", atan),
            Formula("tg", tan),
            Formula("sqrt", sqrt)
        };
    }

    std::string fString;

public:

    static const int ERROR_PARENTHESIS = 0;
    static const int ERROR_PARSE = 1;
    static const int ERROR_DIVISION_BY_ZERO = 2;

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
            it++;

        std::string::const_reverse_iterator rit = s.rbegin();
        while (rit.base() != it && isspace(*rit))
            rit++;

        return std::string(it, rit.base());
    }

    static std::string addSpace(std::string source, char original)
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

    static std::string replace(std::string in)
    {
        std::string result="";
        for (unsigned int i = 0; i < in.size(); i++)
        {
            if (in[i]!=' ')
                result.push_back(in[i]);
        }
        return result;
    }

public:

    Parser(std::string s)
    {
        fString = s;
    }

    Parser(Parser& other)
    {
        this->fString = other.fString;
    }

private:
    double calc(std::vector<KeyValue> varmap, std::string f) //varmap - ?????? ??????????, f - ??????, ??????? ?????????? ?????????? (?????? ??? ?????? ??? ??????????? ???????)
    {
        if (f.size() == 0) throw ERROR_PARSE;
        f = trim(f);
        double d;
        if (tryParse(f, d))
            return d;
        if (f=="nan") return nan("");
        for (unsigned int i = 0; i < varmap.size(); i++)
        {
            if (f == varmap[i].s)
            {
                return varmap[i].d; //???????? ??????????
            }
            else if (f == ("-" + varmap[i].s))
            {
                return -varmap[i].d; //???????? ??????????
            }
        }

        if (f == ("pi"))
            return 3.14159265358979;//??
        else if (f == ("-pi"))
            return -3.14159265358979;//??
        if (f == ("e"))
            return exp(1);//????? e
        else if (f == ("-e"))
            return -exp(1);//????? e
        size_t index;
        for (unsigned int func = 0; func < functions().size(); func++) //??????? ??????? ? ??????? ??????
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
                //if (functions()[func].s=="cos")
                return calc(varmap, (f.substr(0, static_cast<int>(index)) + std::to_string(functions()[func].p(calc(varmap, g.substr(1, i - 1)))) + g.substr(i + 1))); // ????????? ???????, ??????????? ???????? ??????????? ?????? ???, ????? ??? ?????? ????????
            }
        }
        if ((index = f.find("(")) != std::string::npos) //??????? ??????
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
            return calc(varmap, (f.substr(0, index) + std::to_string(calc(varmap, g.substr(0, i))) + g.substr(i + 1)));
        }
        if ((index = f.find("+")) != std::string::npos)
        {
            if (index == 0) throw ERROR_PARSE;
            if ((f[index - 1] == 'e') || (f[index - 1] == 'E'))
            {

            } //?? ???????? ? scientific notation
            else
            {
                return calc(varmap, f.substr(0, index)) + calc(varmap, f.substr(index + 1));
            }
        }
        if (((index = inverse(f).find("-")) != std::string::npos) && (index) && ((inverse(f)[index + 1] == ' ')||(inverse(f)[index + 1] == '-'))) //????? 5-3-2 ???????????? ? 5-(3-2)
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

                if ((f[index-2]=='-' && f[index-1]==' '))
                {
                    std::string temp = f.substr(0, index-3);
                    if (temp.size()==0) temp = "0";
                    return calc(varmap, temp) + calc(varmap, f.substr(index + 1));
                }
                else if ((index < f.size()) && (f[index+1]=='-'))
                {
                    std::string temp = f.substr(0, index-1);
                    if (temp.size()==0) temp = "0";
                    return calc(varmap, temp) + calc(varmap, f.substr(index + 2));
                }
                else if ((index>1) && (f[index-1]=='-'))
                {
                    std::string temp = f.substr(0, index-2);
                    if (temp.size()==0) temp = "0";
                    return calc(varmap, temp) + calc(varmap, f.substr(index + 1));
                }
                else
                {
                    std::string temp = f.substr(0, index-1);
                    if (temp.size()==0) temp = "0";
                    return calc(varmap,temp) - calc(varmap, f.substr(index + 1));
                }
            }
        }
        if ((index = f.find("*")) != std::string::npos)
        {
            return calc(varmap, f.substr(0, index)) * calc(varmap, f.substr(index + 1));
        }
        if ((index = inverse(f).find("/")) != std::string::npos)
        {
            index = f.size() - index - 1;
            if (index <= 0) throw ERROR_PARSE;

            if (calc(varmap, f.substr(index + 1)) == 0)
            {
                throw ERROR_DIVISION_BY_ZERO;
            }
            return calc(varmap, f.substr(0, index)) / calc(varmap, f.substr(index + 1));
        }
        if ((index = f.find("^")) != std::string::npos)
        {
            return pow(calc(varmap, f.substr(0, index)), calc(varmap, f.substr(index + 1)));
        }

        throw f;

    }


public:
    double calc(std::vector<KeyValue> varmap)
    {
        fString = replace(fString);
        fString = addSpace(fString, '-');
        if (fString.size() == 0)
        {
            throw ERROR_PARSE;
        }

        return calc(varmap, fString);
    }


};

