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

class FileInfo{
private:
    string _fullpath;
    string _name;
    string _path;
    regex re;
public:
    FileInfo( string );
    void OpenMe();
    void OpenPath();
    string GetName();
    string GetPath();
    string GetFullPath();
};
FileInfo::FileInfo( string path ){
    _fullpath = path;
    size_t pos = path.find_last_of("/\\");
    re = regex(R"(\/Users\/[^\/]+\/)");
    _path = path.substr(0,pos+1);
    _name = path.substr(pos+1);
}
void FileInfo::OpenMe(){
    string cmd = "open ";
    cmd.append(regex_replace (_fullpath,regex(R"( )"),"\\ "));
    shared_ptr<FILE> pipe(popen((char*)cmd.c_str(), "r"), pclose);
}
void FileInfo::OpenPath(){
    string cmd = "open ";
    cmd.append(_path);
    shared_ptr<FILE> pipe(popen((char*)cmd.c_str(), "r"), pclose);
}
string FileInfo::GetName(){
    return _name;
}
string FileInfo::GetPath(){
    return regex_replace(_path,re,"~/");;
}
string FileInfo::GetFullPath(){
    return regex_replace (_fullpath,re,"~/");;
}


void initial_cmode();
void cmode();
vector<FileInfo> mdfind( string );
vector<FileInfo> ResultFiles;

string arguments="";

int main(int argc, const char * argv[]) {
    if(argc<=1){
        system("mdfind");
        return 0;
    }

    for(int i = 1 ; i < argc ; i++){
        arguments.append(argv[i]);
        if(i!=argc-1)
            arguments.append(" ");
    }
    ResultFiles = mdfind(arguments);
    initial_cmode();
    cmode();
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
const int init_x = 0;
const int init_y = 0;
int offset_y = 0;
void cmode(){
    vector<char> pressedKey;
    int head_item = 0;
    int select_item = 0;
    bool quit = false;
    char num[10];
    do{
        string resultText =
        "search [" + arguments + "] " +
        "got "+to_string((int)ResultFiles.size()) + " results.";
        mvaddstr(init_y, init_x, resultText.c_str());
        offset_y=1;
        if( LINES < ResultFiles.size()) {
            for( int i = 0 ; i < LINES-1 ; i++){
                if(head_item + i >= ResultFiles.size()){
                    break;
                }
                move(init_y+(i+offset_y), init_x);
                addstr((to_string(head_item+i)+" ").c_str());
                if( head_item+i == select_item){
                    attron(A_REVERSE);
                    addstr( (char*)ResultFiles[head_item+i].GetFullPath().c_str() );
                    attroff(A_REVERSE);
                }else{
                    addstr( (char*)ResultFiles[head_item+i].GetFullPath().c_str() );
                }
            }
        }else{
            for( int i = 0 ; i < ResultFiles.size() ; i++){
                move(init_y+(i+offset_y), init_x);
                addstr((to_string(head_item+i)+" ").c_str());
                if(i==select_item){
                    attron(A_REVERSE);
                    addstr( (char*)ResultFiles[i].GetFullPath().c_str() );
                    attroff(A_REVERSE);
                }else{
                    addstr( (char*)ResultFiles[i].GetFullPath().c_str());
                }
            }
        }
        do{
            char ch = getch();
            pressedKey.push_back(ch);
        }while(feof(stdin));
        
        for(int i = 0 ; i < pressedKey.size() ; i ++){
            switch (pressedKey[i]) {
                case 3: //down
                    select_item=max(min(--select_item,(int)ResultFiles.size()-1),0);
                    if(select_item - head_item <=1){
                        head_item=max(min(head_item-1,(int)ResultFiles.size()-1),0);
                    }
                    break;
                case 2: //up
                    select_item=max(min(++select_item,(int)ResultFiles.size()-1),0);
                    if(select_item - head_item >=LINES-(LINES/5)) {
                        head_item=max(min(head_item+1,(int)ResultFiles.size()-1),0);
                    }
                    break;
                case 13:
                    ResultFiles[select_item].OpenMe();
                    quit = true;
                    break;
                case 'q':
                    quit = true;
                    break;
                default:
                    pressedKey.clear();
                    break;
            }
        }
        pressedKey.clear();
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
vector<FileInfo> mdfind( string arguments ){
    string cmd = "mdfind ";
    cmd.append(arguments);
    shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    array<char, 128> buffer;
    string result;
    while(!feof(pipe.get())){
        if(fgets(buffer.data(), 128, pipe.get())!=NULL)
            result += buffer.data();
    }
    stringstream ss;
    ss << result.c_str();
    string t;
    vector<FileInfo> ret;
    while( getline(ss, t) ){
        ret.push_back( FileInfo( t ) ) ;
    }
    return ret;
}