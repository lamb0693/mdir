#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

#define RESERVED_LINE_NO 7 // UI를 위해 남겨 둬야 할 라인 수

// display.c에서 제공하는 함수 선언
int display_files(const char *directory, int startline_idx, int highlighted_idx);

char current_dir[1024] = "~"; // 초기 디렉토리 설정
int highlighted_idx = 0;     // 파일 리스트 중 선택된 파일 인덱스, 0 - filecount-1
int print_start_idx = 0;     // 파일 리스트 중 출력 시작 줄 인덱스 0 - filecount-1

int main() {
    setlocale(LC_ALL, "");
    // ncurses 초기화
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    // 홈 디렉토리 가져오기
    char *home_dir = getenv("HOME");
    if (home_dir != NULL) {
        strncpy(current_dir, home_dir, sizeof(current_dir) - 1);
    }

    // 메인 루프
    while (1) {
        clear();

        // 파일 출력 및 파일 개수 반환
        int file_count = display_files(current_dir, print_start_idx, highlighted_idx);

        // 키 입력 처리
        int ch = getch();
        int screen_height;
        int screen_width;
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
            case 'q': // 종료
                endwin();
                return 0;
            default:
                break;
        }
    }

    // ncurses 종료
    endwin();
    return 0;
}
