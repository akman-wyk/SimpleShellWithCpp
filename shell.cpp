#include <string>
#include <vector>
#include <cstdio>
#ifdef _WIN32
#include <conio.h>

#define PREFIX -32
#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

#elif (__unix__) || (__APPLE__)
#include <termios.h>
#include <unistd.h>

#define putch(x) putc((x), stdout)

#define PREFIX 27
#define NEXT_PREFIX '['
#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'

#endif

void MoveLeftToEndAndClear(int length) {
    putch('\r');
    for (int i = 0; i < length; i++) {
        putch(' ');
    }
    putch('\r');
}

void MoveLeftOneAndClear() {
    putch('\b');
    putch(' ');
    putch('\b');
}

class History {
private:
    static std::string empty_str;

public:
    History() = default;

    void reset() {
        index = lines.size();
    }

    std::string& get() {
        if (0 <= index && index < lines.size()) {
            return lines[index];
        }
        return empty_str;
    }

    void forward() {
        if (index > 0) {
            index--;
        }
    }

    void backward() {
        if (index < lines.size()) {
            index++;
        }
    }

    void append(std::string line) {
        lines.emplace_back(std::move(line));
    }

    bool forward_end() {
        return index == 0;
    }

    bool backward_end() {
        return index == lines.size();
    }

private:
    std::vector<std::string> lines;
    int index = 0;
};

std::string History::empty_str = "";

History history;

#ifdef _WIN32
std::string readLine() {
    std::string line = "";
    int pos = 0;
    char c;
    c = getch();
    while (c != '\n' && c != '\r') {
        if (c == PREFIX) {
            c = getch();
            if (c == UP && !history.forward_end()) {
                if (!line.empty()) {
                    MoveLeftToEndAndClear(line.size());
                }
                history.forward();
                line.clear();
                line = history.get();
                printf("%s", line.c_str());
                pos = line.size();
            } else if (c == DOWN && !history.backward_end()) {
                if (!line.empty()) {
                    MoveLeftToEndAndClear(line.size());
                }
                history.backward();
                line.clear();
                line = history.get();
                printf("%s", line.c_str());
                pos = line.size();
            } else if (c == LEFT && pos > 0) {
                putch('\b');
                pos--;
            } else if (c == RIGHT && pos < line.size()) {
                putch(line[pos]);
                pos++;
            }
            c = getch();
            continue;
        }

        if (c == '\b') {
            if (!line.empty() && pos > 0) {
                // process variables
                pos--;
                line.erase(pos, 1);
                // process console output
                putch('\b');
                for (int i = pos; i < line.size(); i++) {
                    putch(line[i]);
                }
                putch(' ');
                for (int i = 0; i < line.size() - pos + 1; i++) {
                    putch('\b');
                }
            }
            c = getch();
            continue;
        }
        if (c == '\t') {
            // process variables
            line.insert(pos, 4, ' ');
            pos += 4;
            // process console output
            for (int i = 0; i < 4; i++) {
                putch(' ');
            }
            for (int i = pos; i < line.size(); i++) {
                putch(line[i]);
            }
            for (int i = 0; i < line.size() - pos; i++) {
                putch('\b');
            }

            c = getch();
            continue;
        }

        // process variables
        line.insert(pos, 1, c);
        pos++;
        // process console output
        putch(c);
        for (int i = pos; i < line.size(); i++) {
            putch(line[i]);
        }
        for (int i = 0; i < line.size() - pos; i++) {
            putch('\b');
        }
        // printf("%d\n", c);

        c = getch();
    }

    if (line != "") {
        history.append(line);
    }
    history.reset();
    return std::move(line);
}
#elif (__unix__) || (__APPLE__)
int getch(void) {
    int ret;
    struct termios term,term_old;
    if(tcgetattr(STDIN_FILENO,&term)==-1) {
        printf("Error!");
        return 0;
    }
    term_old=term;
    cfmakeraw(&term);
    tcsetattr(STDIN_FILENO,TCSANOW,&term);
    ret=fgetc(stdin);
    tcsetattr(STDIN_FILENO,TCSANOW,&term_old);
    return ret;
}

std::string readLine() {
    std::string line = "";
    int pos = 0;
    char c;
    c = getch();
    while (c != '\n' && c != '\r') {
        if (c == PREFIX) {
            c = getch();
            if (c == NEXT_PREFIX) {
                c = getch();
                if (c == UP && !history.forward_end()) {
                    if (!line.empty()) {
                        MoveLeftToEndAndClear(line.size());
                    }
                    history.forward();
                    line.clear();
                    line = history.get();
                    printf("%s", line.c_str());
                    pos = line.size();
                } else if (c == DOWN && !history.backward_end()) {
                    if (!line.empty()) {
                        MoveLeftToEndAndClear(line.size());
                    }
                    history.backward();
                    line.clear();
                    line = history.get();
                    printf("%s", line.c_str());
                    pos = line.size();
                } else if (c == LEFT && pos > 0) {
                    putch('\b');
                    pos--;
                } else if (c == RIGHT && pos < line.size()) {
                    putch(line[pos]);
                    pos++;
                }
            }
            c = getch();
            continue;
        }

        if (c == 127) {
            if (!line.empty() && pos > 0) {
                // process variables
                pos--;
                line.erase(pos, 1);
                // process console output
                putch('\b');
                for (int i = pos; i < line.size(); i++) {
                    putch(line[i]);
                }
                putch(' ');
                for (int i = 0; i < line.size() - pos + 1; i++) {
                    putch('\b');
                }
            }
            c = getch();
            continue;
        }

        if (c == '\t') {
            // process variables
            line.insert(pos, 4, ' ');
            pos += 4;
            // process console output
            for (int i = 0; i < 4; i++) {
                putch(' ');
            }
            for (int i = pos; i < line.size(); i++) {
                putch(line[i]);
            }
            for (int i = 0; i < line.size() - pos; i++) {
                putch('\b');
            }

            c = getch();
            continue;
        }

        // process variables
        line.insert(pos, 1, c);
        pos++;
        // process console output
        putch(c);
        for (int i = pos; i < line.size(); i++) {
            putch(line[i]);
        }
        for (int i = 0; i < line.size() - pos; i++) {
            putch('\b');
        }
        // printf("%d\n", c);

        c = getch();
    }

    if (line != "") {
        history.append(line);
    }
    history.reset();
    return std::move(line);
}
#endif

void shell() {
    int index = 1;
    std::string line = readLine();
    while (line != "quit") {
        printf("\n%d %s\n", index, line.c_str());
        index++;
        line = readLine();
    }
#if (__unix__) || (__APPLE__)
    printf("\n");
#endif
}

void test() {
    char c;
    c = getch();
#ifdef _WIN32
    if (c == PREFIX) {
        c = getch();
        printf("%d\n", c);
    }
#elif (__unix__) || (__APPLE__)
    if (c == PREFIX) {
        c = getch();
        if (c == NEXT_PREFIX) {
            c = getch();
            printf("%c\n", c);
        }
    }
#endif
}

int main() {
    shell();
    // test();
    return 0;
}