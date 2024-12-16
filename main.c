#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>
#include <dirent.h>
#include <sys/stat.h>

#include "project_macro.h"

// directory의 내용을  display
// display.c에서 제공하는 함수 선언,  현재 directory의 file 수를 return
int display_files(const char *directory, const int startline_idx, const int highlighted_idx, char *selected_filename, size_t filename_size);

// 스페이스 바를 누르면 할 행동 - highlited 된 directory로 이동, 실행파일이면 실행, 일반 파일이면 내용 보기 등
// execute.c에서 제공하는 함수;//directory인 경우  current_dr 바뀌어 돌아옴, return 은 99, 실패  return 0, 기타 return 1 
int execute_command(char current_dir[1024], const char selected_filename[256]);

char current_dir[MAX_DIR_LENGTH] = "~"; // 초기 디렉토리 설정
int highlighted_idx = 0;     // 파일 리스트 중 선택된 파일 인덱스, 0 - filecount-1
int print_start_idx = 0;     // 파일 리스트 중 출력 시작 줄 인덱스 0 - filecount-1

int main() {
    setlocale(LC_ALL, "");
    // ncurses 초기화
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    // 홈 디렉토리 가져오기  // 없어도 될 지 아닐지
    char *home_dir = getenv("HOME");
    if (home_dir != NULL) {
        strncpy(current_dir, home_dir, sizeof(current_dir) - 1);
    }

    
    // display.o로 넘겨서 현재 highligthed 된  file 의 이름을 받아 옴
    char selected_filename[MAX_FILENAME_LENGTH];

    // 메인 루프
    while (1) {
        clear();

        // 파일 출력 및 파일 개수 반환
        int file_count = display_files(current_dir, print_start_idx, highlighted_idx, selected_filename, sizeof(selected_filename) );

        // 키 입력 처리
        int ch = getch();
        int screen_height;
        int screen_width;
        // getmaxyx는 현재 terminal의 높이와 넓이 얻어옴, 다음과 같은 MACRO라서 &y, &x 가 필요 없음  
        // #define getmaxyx(win, y,x)  do { (y)=getmaxy(win);  (x)=getmaxx(win)}
        // 따라서
        getmaxyx(stdscr, screen_height, screen_width);

        switch (ch) {
            case KEY_UP:
                if (highlighted_idx > 0) {
                    highlighted_idx--;
                    if (highlighted_idx < print_start_idx) {
                        print_start_idx--;
                    }
                }
                break;
            case KEY_DOWN:
                if (highlighted_idx < file_count - 1) {
                    highlighted_idx++;
                    if (highlighted_idx - print_start_idx == screen_height - RESERVED_LINE_NO - 1) { // higlighted_idx가 라인 끝이면
                        print_start_idx++;
                    }
                }
                break;
            case ' ': 
               int return_value = execute_command(current_dir, selected_filename);
               if(return_value == 99) {
                    highlighted_idx =0;
                    print_start_idx = 0;
               }
               break; 
            case 'q': // 종료
                endwin();
                return 0;
            default:
                break;
        }
    }
}
