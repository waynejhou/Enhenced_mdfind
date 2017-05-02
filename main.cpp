//
//  main.cpp
//  Enhenced_mdfind
//
//  Created by Wayne on 2017/5/1.
//  Copyright © 2017年 Wayne. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>
#include <ncurses.h>
#include <vector>
#include <array>


using namespace std;

int init_x = 0;
int init_y = 0;
vector<string> result_stream;
string args;
void initial_cmode();
void cmode();
void mdfind( string );
void open_file( string );

int main(int argc, const char * argv[]) {
    if(argc==1){
        char ccc[] = "mdfind";
        char* cmd = strcat(ccc, args.c_str());
        array<char, 128> buffer;
        string result;
        shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
        if (!pipe) throw runtime_error("popen() failed!");
        while (!feof(pipe.get())) {
            if (fgets(buffer.data(), 128, pipe.get()) != NULL)
                result += buffer.data();
        }
        cout << result ;
        return 0;
    }
    for( int i = 1 ; i < argc ; i++){
        args.append(" ");
        args.append(argv[i]);
    }
    mdfind(args);
    /*cout << result_stream.size() << endl;
    for( int i = 0 ; i < result_stream.size() ; i++){
        cout << result_stream[i] << endl;
    }*/
    if( result_stream.size()==0 ){
        cout << "Nothing Found!!\n";
    }else if( result_stream.size()==1 ){
        open_file(result_stream[0]);
    }else{
        initial_cmode();
        cmode();
    }
    return 0;
}

void initial_cmode(){       // call many ncursres func to initial cmode(curses mode)
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
        //box(stdscr, '|','-');
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
                    addstr( result_stream[head_item+i].c_str());
                    attroff(A_REVERSE);
                }else{
                    addstr( result_stream[head_item+i].c_str());
                }/*
                if(result_stream[head_item+i].size()>COLS){
                    i++;
                }*/
            }
        }else{
            for( int i = 0 ; i < result_stream.size() ; i++){
                if(i==select_item){
                    attron(A_REVERSE);
                    mvaddstr(init_y+(i+2), init_x, result_stream[i].c_str());
                    attroff(A_REVERSE);
                }else{
                    mvaddstr(init_y+(i+2), init_x, result_stream[i].c_str());
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
                    open_file(result_stream[select_item]);
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
void mdfind( string args){
    char ccc[] = "mdfind";
    char* cmd = strcat(ccc, args.c_str());
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    string t;
    for(int i = 0 ; i < result.length() ; i++){
        if(result[i]=='\n'){
            //if(result[i]=='\n')
                //cout << "[" << "\\n:"<<(int)'\n' << "]\n\n";
            i++;
            result_stream.push_back(t);
            t.clear();
        }{
            //cout << "[" << result[i]<<":"<<(int)result[i] << "]";
            t.push_back(result[i]);
        }
    }
    /*
    stringstream ss;
    ss << result << EOF;
    string t;
    while(getline(ss,t)){
        cout << "mdssl\n";
        result_stream.push_back(t);
    }
    cout << ss.str();
     */
}
void open_file( string file ){
    char sss[4096] = "open \"";
    char ssse[] = "\"";
    strcat(sss, file.c_str());
    strcat(sss, ssse);
    //addstr( sss );
    //getch();
    shared_ptr<FILE> pipe(popen(sss, "r"), pclose);
}
