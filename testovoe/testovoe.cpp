#include <iostream>
#include <Windows.h>
#include <vector>
#include <queue>
#include <deque>
#define charToInt(c) (c-'0')

int Sum(const char* s) {
    int value = 0;
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
        if (isdigit(s[i]))
            value += charToInt(s[i]);
    }

    return value;
}


//for debug output purposes
int unscanned = 0;
int current_index = 0;
int total_size = 0;


struct pt {
    int x, y;
    bool resimulated;
};


class C_Ant {
    int m_X;
    int m_Y;

    std::vector<pt> m_Visited;

    bool Visited(const int x, const int y) {
        for (auto& point : m_Visited) {

            if (x == point.x && y == point.y) {
                return true;
            }
        }

        return false;
    }

    bool ValidateStep(int new_x, int new_y, std::queue<pt> & new_points) {

        bool duplicate = Visited(new_x, new_y);

        if (duplicate)
        {
            m_X = new_x;
            m_Y = new_y;
            return true;
        }

        char s1[32] = { 0 };
        char s2[32] = { 0 };

        _itoa_s(new_x, s1, 10);
        int sum_x = Sum(s1);


        _itoa_s(new_y, s2, 10);
        int sum_y = Sum(s2);

        
        //printf("%d (%s) + %d (%s) = %d <= 25 -> %d\n", sum_x, s1, sum_y, s2, sum_x + sum_y, (sum_x + sum_y) <= 25);
        if ((sum_x + sum_y) <= 25) {
           
            m_X = new_x;
            m_Y = new_y;
            new_points.push({m_X, m_Y, false});
            return true;
        }

        return false;
    }

public :
    C_Ant(int x, int y) {
        m_X = x;
        m_Y = y;
        m_Visited.clear();
        m_Visited.push_back({ x,y, true });
    }

    bool MoveLeft(std::queue<pt>& new_points) {
        //printf("Moving left\n");
        if (ValidateStep(m_X - 1, m_Y, new_points)) {
            return true;
        }
        return false;
    }

    bool MoveRight(std::queue<pt>& new_points) {
        //printf("Moving right\n");
        if (ValidateStep(m_X + 1, m_Y, new_points)) {
            return true;
        }
        return false;
    }

    bool MoveUp(std::queue<pt>& new_points) {
        //printf("Moving up\n");
        if (ValidateStep(m_X, m_Y + 1, new_points)) {
            return true;
        }
        return false;
    } 

    bool MoveDown(std::queue<pt>& new_points) {
        //printf("Moving down\n");
        if (ValidateStep(m_X, m_Y - 1, new_points)) {
            return true;
        }
        return false;
    }

    void DoMovement(int cached_x, int cached_y, std::queue<pt>& new_points) {
        m_X = cached_x; m_Y = cached_y;
        while (MoveRight(new_points));

        m_X = cached_x; m_Y = cached_y;
        while (MoveDown(new_points));

         m_X = cached_x; m_Y = cached_y;
        while (MoveLeft(new_points));

        m_X = cached_x; m_Y = cached_y;
        while (MoveUp(new_points));
    }



    void Move(int x, int y) {
       
        std::queue<pt> start_path;
        DoMovement(1000, 1000, start_path);

        while (!start_path.empty()) {
            m_Visited.push_back(start_path.front());
            start_path.pop();
        }
        
        printf("First %d path built\n", m_Visited.size());

        unscanned = m_Visited.size() - 1;

        do {
            std::queue<pt> new_path;
            current_index = 0;
            total_size = m_Visited.size();
            for (auto& point : m_Visited) {
                if (!point.resimulated) {
                    
                    DoMovement(point.x, point.y, new_path);
                    point.resimulated = true;
                    unscanned--;
                    //break;
                }
                current_index++;
            }
            
            if (new_path.size())
                printf("Adding %d points\n", new_path.size());

            while (!new_path.empty()) {
                m_Visited.push_back(new_path.front());
                new_path.pop();
                unscanned++;
            }

            //Sleep(1);
        } while (GetUnscanned());

        printf("The ant can reach %d points\n", m_Visited.size());

    }

    bool GetUnscanned() {
        int ttl = 0;
        for (auto& point : m_Visited) {
            if (!point.resimulated) return true;
        }
        return false;
    }

};

DWORD __stdcall Info(LPVOID p) {

    while (true) {
        char dbg[512] = { 0 };
        sprintf_s(dbg, "Left: %d | Working %d of %d", unscanned, current_index, total_size);
        SetConsoleTitleA(dbg);
        Sleep(50);
    }

    return 1;
}

int main()
{
    std::cout << "Hello World! " << charToInt('1') << std::endl;

    C_Ant ant(1000, 1000);

    CreateThread(0, 0, Info, 0, 0, 0);

    ant.Move(1000,1000);

    std::cin.get();

    return 0;
}
