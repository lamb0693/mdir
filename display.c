#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <locale.h>
#include <ncurses.h>

// 문자열을 지정된 길이로 자르고 출력
void print_trimmed(const char *str, int max_width) {
    setlocale(LC_ALL, ""); // UTF-8 지원 설정
    int width = 0;         // 현재 출력 너비
    const char *ptr = str; // 문자열 포인터

    while (*ptr && width < max_width) {
        wchar_t wc;
        int char_len = mbtowc(&wc, ptr, MB_CUR_MAX); // UTF-8 문자 읽기
        if (char_len <= 0) {
            break; // 잘못된 문자 처리
        }

        int char_width = wcwidth(wc); // 문자의 출력 너비 계산
        if (char_width < 0) {
            char_width = 0; // 제어 문자 등은 너비 0으로 처리
        }

        if (width + char_width > max_width) {
            break; // 너비를 초과하면 중단
        }

        addnstr(ptr, char_len); // 문자를 출력
        width += char_width;
        ptr += char_len;       // 다음 문자로 이동
    }

    // 남은 공간을 공백으로 채우기
    for (int i = width; i < max_width; i++) {
        addch(' ');
    }
}

int custom_alphasort(const struct dirent **a, const struct dirent **b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}

// 디렉토리의 파일 정보를 출력하는 함수
int display_files(const char *directory, int startline_idx, int highlighted_idx) {
    struct dirent **filelist = NULL;
    int file_count = scandir(directory, &filelist, NULL, custom_alphasort);

    if (file_count < 0) {
        mvprintw(1, 0, "Error reading directory: %s", directory);
        return 0; // 파일 없음
    }

    // 터미널 크기 확인
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);

    // 상단 라인 출력
    mvprintw(0, 0, "======================");
    mvprintw(1, 0, "current directory :  %s", directory); 
    mvprintw(2, 0, "----------------------");
    mvprintw(3, 0, "%-30s %-10s %-10s %-20s", "Filename", "Kind", "Size", "Modified");
    mvprintw(4, 0, "----------------------");

    // 파일 리스트 출력
    int list_start = 5; // 파일 리스트 시작 줄 (헤더 이후)
    int list_end = screen_height - 6; // 하단 메뉴와 구분선 고려
    int j = list_start;

    for (int i = startline_idx; i < file_count && j < list_end; ++i, ++j) {
        struct stat file_stat;
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, filelist[i]->d_name);

        // 파일 정보 가져오기
        if (stat(filepath, &file_stat) == 0) {
            char *kind = (S_ISDIR(file_stat.st_mode)) ? "DIR" : "FILE";

            // 수정 시간 가져오기
            char mod_time[20];
            strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M", localtime(&file_stat.st_mtime));

            // 하이라이트 처리
            if (i == highlighted_idx) {
                attron(A_REVERSE);
            }

            mvprintw(j, 0, ""); // 줄 시작 위치로 이동 출력
            print_trimmed(filelist[i]->d_name, 30); // 파일 이름 (최대 30칸)
            mvprintw(j, 30, "%-10s %-10ld %-20s", kind, file_stat.st_size, mod_time); // 나머지 출력

            if (i == highlighted_idx) {
                attroff(A_REVERSE);
            }
        }
    }

    // 하단 라인 출력
    mvprintw(screen_height - 3, 0, "-------------------------------------------");
    mvprintw(screen_height - 2, 0, " Copy(C)  Cut(X) Paste(V) View(L) Edit(E) Execute(R)");
    mvprintw(screen_height - 1, 0, "======================");

    // 메모리 해제
    for (int i = 0; i < file_count; ++i) {
        free(filelist[i]);
    }
    free(filelist);

    return file_count; // 파일 개수 반환
}