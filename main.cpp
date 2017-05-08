//
//  main.cpp
//  Enhenced_mdfind
//
//  Created by Wayne on 2017/5/1.
//  Copyright © 2017年 Wayne. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <string>
#include <ncursesw/ncurses.h>
#include <vector>
#include <array>
#include <locale.h>
#include <regex>

using namespace std;

class fobject{
private:
    string _fullpath;
    string _name;
    string _path;
    regex re;
public:
    fobject( string );
    void open_me();
    void open_path();
    string get_name();
    string get_path();
    string get_fullpath();
};
fobject::fobject( string p ){
    _fullpath = p;
    size_t pos = p.find_last_of("/\\");
    re = regex(R"(\/Users\/[^\/]+\/)");
    _path = p.substr(0,pos+1);
    _name = p.substr(pos+1);
}
void fobject::open_me(){
    string cmd = "open ";
    cmd.append(regex_replace (_fullpath,regex(R"( )"),"\\ "));
    shared_ptr<FILE> pipe(popen((char*)cmd.c_str(), "r"), pclose);
}
void fobject::open_path(){
    string cmd = "open ";
    cmd.append(_path);
    shared_ptr<FILE> pipe(popen((char*)cmd.c_str(), "r"), pclose);
}
string fobject::get_name(){
    return _name;
}
string fobject::get_path(){
    return regex_replace(_path,re,"~/");;
}
string fobject::get_fullpath(){
    return regex_replace (_fullpath,re,"~/");;
}


int init_x = 0;
int init_y = 0;
vector<fobject> result_stream;
string args;
void initial_cmode();
void cmode();
void mdfind( string );
void open_file( string );

int main(int argc, const char * argv[]) {
    cout << *(localeconv())->currency_symbol <<endl;
    string cmd = "mdfind";
    for(int i = 1 ; i < argc ; i++){
        cmd.append(" ");
        cmd.append(argv[i]);
    }
    shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    array<char, 128> buffer;
    string result;
    while(!feof(pipe.get())){
        if(fgets(buffer.data(), 128, pipe.get())!=NULL)
            result += buffer.data();
    }
    if(argc<=1)
        cout << result;
    else{
        stringstream ss;
        ss << result.c_str();
        string t;
        while( getline(ss, t) ){
            result_stream.push_back( fobject( t ) ) ;
        }
        initial_cmode();
        cmode();
    }

    return 0;
}

void initial_cmode(){       // call many ncursres func to initial cmode(curses mode)
    setlocale(LC_ALL,"");
    initscr();              // start cmode
    cbreak();               // All char will be load to buffer except DELETE and CTRL until new line or return in older version use crmode() & nocrmode()
                            // close with nocbreak()
    nonl();                 // wheather "\n" or NEWLINE will be RETURN or not
                            // open with nl()
    noecho();               // any input char display on terminal or not
                            // open with echo()
    intrflush(stdscr,FALSE);// intrflush( WIN, bf) that WIN is type "WINDOW"'s ptr, bf is bool.
                            // if true, when input break it will be faster but with a little screen mess
    keypad(stdscr,TRUE);    // keypad( WIN, bf)
                            // trans special key (e.g. arrow key) into special key in function with "KEY_" ahead.
    //refresh();              // refresh treminal. at first it clean the screen.
}
void cmode(){
    //char ch = 0;
    vector<char> vc;
    int head_item = 0;
    int select_item = 0;
    bool quit = false;
    char num[10];
    do{
        mvaddstr(init_y, init_x, args.c_str());
        char s[10];
        sprintf(s, "%d", (int)result_stream.size());
        mvaddstr(init_y+1, init_x, s);
        addstr(" things found");
        if( LINES < result_stream.size()) {
            for( int i = 0 ; i < LINES-2 ; i++){
                if(head_item+i>result_stream.size()-1){
                    break;
                }
                move(init_y+(i+2), init_x);
                sprintf(num, "%d ", head_item+i);
                addstr(num);
                if( head_item+i == select_item){
                    attron(A_REVERSE);
                    addstr( (char*)result_stream[head_item+i].get_fullpath().c_str() );
                    attroff(A_REVERSE);
                }else{
                    addstr( (char*)result_stream[head_item+i].get_fullpath().c_str() );
                }
            }
        }else{
            for( int i = 0 ; i < result_stream.size() ; i++){
                move(init_y+(i+2), init_x);
                sprintf(num, "%d ", head_item+i);
                addstr(num);
                if(i==select_item){
                    attron(A_REVERSE);
                    addstr( (char*)result_stream[i].get_fullpath().c_str());
                    attroff(A_REVERSE);
                }else{
                    addstr( (char*)result_stream[i].get_fullpath().c_str());
                }/*
                if(result_stream[i].size()>COLS){
                    i++;
                }*/
            }
        }
        do{
            char ch = getch();
            vc.push_back(ch);
        }while(feof(stdin));
        //char t[10];
        //sprintf(t,"%d",(int)vc[0]);
        //addstr(t);
        for(int i = 0 ; i < vc.size() ; i ++){
            switch (vc[i]) {
                case 3:
                    select_item=max(min(--select_item,(int)result_stream.size()-1),0);
                    if(select_item - head_item <=2){
                        head_item=max(min(head_item-1,(int)result_stream.size()-1),0);
                    }
                    break;
                case 2:
                    select_item=max(min(++select_item,(int)result_stream.size()-1),0);
                    if(select_item - head_item >=LINES-5){
                        head_item=max(min(head_item+1,(int)result_stream.size()-1),0);
                    }
                    break;
                case 13:
                    result_stream[select_item].open_me();
                    quit = true;
                    break;
                case 'q':
                    quit = true;
                    break;
                default:
                    //addstr(&vc[i]);
                    //addstr(", ");
                    vc.clear();
                    break;
            }
        }
        vc.clear();
        clear();
        if(quit)
            break;
    }while(1);
    move(0,0);
    endwin();
}
int show_mes(){
    return 1;
}
