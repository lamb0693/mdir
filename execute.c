
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_DIR_LENGTH 1024
#define MAX_FILENAME_LENGTH 256


// return 99 -  directory moved
int execute_command(char* current_dir, const char* selected_filename) {
    char new_path[1024];

    // current_dir 와 selected_filename을 이용 현재 file의 fullpath 만들기
    // (1) 예상 길이 확인
    if (strlen(current_dir) + strlen(selected_filename) + 1 >= sizeof(new_path)) {
        mvprintw(1, 0, "Path 가 너무 길어 사용 불가능합니다");
        return 0;
    }

    // (2) snprintf 를 이용해 file full path 생성
    int result = snprintf(new_path, sizeof(new_path), "%s/%s", current_dir, selected_filename);
    if (result < 0 || result >= sizeof(new_path)) {
        mvprintw(1, 0, "Error constructing new path!");
        return 0;
    }


    struct stat file_stat;
    if (stat(new_path, &file_stat) == 0) {
        switch (file_stat.st_mode & S_IFMT) {
            case S_IFDIR: // 디렉토리인 경우
            
                if (strcmp(selected_filename, ".") == 0) {
                    // 현재 디렉토리 무시
                    break;
                } else if (strcmp(selected_filename, "..") == 0) {
                    // 상위 디렉토리로 이동
                    char *last_slash = strrchr(current_dir, '/');
                    if (last_slash != NULL) {
                        *last_slash = '\0';
                    } else {
                        // 루트 디렉토리
                        strncpy(current_dir, "/", MAX_DIR_LENGTH - 1);
                        current_dir[MAX_DIR_LENGTH - 1] = '\0';
                    }
                } else {
                    // 하위 디렉토리로 이동
                    strncpy(current_dir, new_path, MAX_DIR_LENGTH - 1);
                    current_dir[MAX_DIR_LENGTH - 1] = '\0';
                }

                // 인덱스 초기화  -- return 99ㅣ면 idx 둘다 초기화
                // highlighted_idx = 0;
                // print_start_idx = 0화
                return 99;

            case S_IFREG: // 일반 파일인 경우
                mvprintw(1, 0, "Selected file: %s", selected_filename);
                getch();
                // 여기서 일반 파일에 대한 처리 추가 가능
                return 1;

            case S_IFLNK: // 심볼릭 링크
                mvprintw(1, 0, "Selected symbolic link: %s", selected_filename);
                // 심볼릭 링크 처리 추가 가능
                getch();
                return 1;

            default:
                if (file_stat.st_mode & S_IXUSR) {
                    mvprintw(1, 0, "Executable file: %s", selected_filename);
                    getch();
                    // 실행 파일 처리 추가 가능
                } else {
                    mvprintw(1, 0, "Unknown file type: %s", selected_filename);
                    getch();
                }
                return 1;
        }
    } else {
        mvprintw(1, 0, "Error reading file information: %s", selected_filename);
        getch();
    }
}


