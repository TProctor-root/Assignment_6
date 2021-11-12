#pragma once
/*
 * assign6.hpp
 * Header file template for assignment 6.
 *
 * You can modify the classes here. You will also need to add a .cpp containing
 * the implementations of the two required functions.
 * 
 * You can add other methods/members to the classes if you need to, but you
 * have to keep the existing ones.
 */

#include <string>
#include <vector>

struct exp { 
    // You will have to implement the print method on all the subclasses.
    virtual void print() = 0;
};

// Precedence 100
struct exp_num : public exp {
    int value;

    exp_num(int v) : value(v) { }

    void print()  {
        std::cout << value;
    }
};


// Precedence 100
struct exp_var : public exp {
    std::string name;

    exp_var(std::string n) : name(n) { }
    
    void print() {
        std::cout << name;
    }
};


// Precdence 100
struct exp_paren : public exp {
    exp* inner;

    exp_paren(exp* i) : inner(i) { }

    void print() {
        std::cout << "(";
        inner -> print();
        std::cout << ")";
    }
};

// Precedence 100 
struct exp_minus : public exp {
    exp* inner;

    exp_minus(exp* i) : inner(i) { }

    void print() {
        std::cout << "-";
        inner -> print();
    }
};

// Precedence 100
struct exp_func : public exp {
    std::string name;
    exp* arg;

    exp_func(std::string n, exp* a) : name(n), arg(a) { }

    void print() {
        std::cout << name;
        std::cout << " -> ";
        std::cout << "(";
        arg -> print();
        std::cout << ")";
    }
};

// Precedence:
// +  50
// -  50
// *  75
// /  75
struct exp_op : public exp {
    char op;
    exp* left;
    exp* right;

    exp_op(char o, exp* l, exp* r) : op(o), left(l), right(r) { }

    void print() {;
        left -> print();
        std::cout  << op;
        right -> print();
    }
};


bool is_var(std::string s) {
	return s.front() >= 'a' && s.front() <= 'z' &&
	       s.find('_') >= s.length();
}

bool is_num(std::string s) {
	try {
		stoi(s);
		return true;
	}
	catch(std::invalid_argument& e) {
		return false;
	}
}

bool is_op(std::string s) {
	return s == "+" || s == "-" || s == "*" || s == "/";
}

int op_prec(std::string op) {
    if(op == "+" || op == "-") {
        return 50;
    }
    else if(op == "*" || op == "/") {
        return 75;
    }
    else {
        return 0;
    }
}

bool is_fname(std::string s) { 
   return s.front() >= 'a' && s.front() <= 'z'; 
}

int find_char_state(char c) {
  const std::string token_chars[6] = {
    " \t\n",
    "0123456789",
    "+-*/",
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_",
    "(",
    ")"
  };
  for(int i = 0; i < 6; i++)
    if(token_chars[i].find_first_of(c) != std::string::npos)
      return i;

  return -1;
}

// *** You need to implement the following two functions ***

std::vector<std::string> tokenize(std::string s) {
    std::vector<std::string> tokens;
    const int space = 0;
    const int op = 2;
    int mode = space;
    std::string token;
    for(char c : s) {
        if(mode == space) {
            int new_mode = find_char_state(c);
            //mode = space;
            if(new_mode != mode) {
                token.push_back(c);
                mode = new_mode;
            }
            else {
                continue;
            }
        }
        else {
            int new_mode = find_char_state(c);
            if(new_mode != mode || mode == op) {
                tokens.push_back(token);
                token.clear();
                if((new_mode != mode && new_mode != space) || new_mode == op) {
                    token.push_back(c);
                }
                mode = new_mode;
            }
            else {
                token.push_back(c);
            }
        }
    }
    if(!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

exp* parse(std::vector<std::string>::iterator start, std::vector<std::string>::iterator finish, int& prec) {
    if(0  == finish - start || start > finish || start == finish) {
       return nullptr; 
    }
    else if(1  == finish - start) {
        if(is_num(*start)) {
            prec = 100;
            return new exp_num{stoi(*start)};
        }
        else if(is_var(*start)) {
            prec = 100;
            return new exp_var{*start};
        }
        else {
            return nullptr;
        }
    }
    else {
        if(*start == "(" && *(finish - 1) == ")") {
            exp* e = parse(start + 1, finish - 1, prec);
            if(e) {
                prec = 100;
                return new exp_paren{e};
            }
        }
        if (is_fname(*start) && *(start + 1) == "(" && *(finish - 1) == ")") {
            exp* e = parse(start + 2, finish - 1, prec);
            if(e) {
                prec = 100;
                return new exp_func{*start, e};
            }
        }
        if(*start == "-") {
            int iprec;
            exp* e = parse(start + 1, finish, iprec);
            if(e && iprec >= 90) {
                prec = 90;
                return new exp_minus{e};
            }
        }
        for(std::vector<std::string>::iterator op = start + 1; op <= finish - 2; ++op) {
            int lprec, rprec;
            exp* l = parse(start, op, lprec);
            exp* r = parse(op + 1, finish, rprec);
            if(is_op(*op) && l != nullptr && r != nullptr && op_prec(*op) <= lprec && op_prec(*op) <= rprec) {
                prec = op_prec(*op);
                return new exp_op{(*op)[0], l, r};
            }
        }
        return nullptr;
    }
}